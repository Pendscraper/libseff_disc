/*
 *
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
 *
 * libseff is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * 	    http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 */

/*
 * The libseff library for shallow effects and handlers.
 * This is the only file that client code should include.
 */
#ifndef SEFF_H
#define SEFF_H

#include "seff_types.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
#define E extern "C"
#else
#define E
#endif

typedef void *(seff_start_fun_t)(void *);

E bool seff_coroutine_init(seff_coroutine_t *, seff_start_fun_t *, void *);
E bool seff_coroutine_init_sized(seff_coroutine_t *, seff_start_fun_t *, void *, size_t);
E void seff_coroutine_release(seff_coroutine_t *);
E void seff_coroutine_reset(seff_coroutine_t *);

E seff_coroutine_t *seff_coroutine_new(seff_start_fun_t *, void *);
E seff_coroutine_t *seff_coroutine_new_sized(seff_start_fun_t *, void *, size_t);
E void seff_coroutine_delete(seff_coroutine_t *);

E seff_coroutine_t *seff_locate_handler(effect_id effect);

E seff_coroutine_t *seff_current_coroutine(void);
// For debugging only. Do not use.
#ifdef STACK_POLICY_SEGMENTED
E __attribute__((no_split_stack)) void *seff_current_stack_top(void);
#endif

E __attribute__((no_split_stack)) seff_request_t seff_resume(
    seff_coroutine_t *k, void *arg, effect_set handled);
E seff_request_t seff_resume_handling_all(seff_coroutine_t *k, void *arg);

E default_handler_t *seff_set_default_handler(effect_id effect, default_handler_t *handler);
E default_handler_t *seff_get_default_handler(effect_id effect);

E __attribute__((no_split_stack)) void *seff_yield(
    seff_coroutine_t *self, effect_id effect, void *payload);
// Performance difference is massive between seff_perform being inlined or not
static inline void *seff_perform(effect_id eff_id, void *payload) {
    seff_coroutine_t *handler = seff_locate_handler(eff_id);
    if (handler) {
        return seff_yield(handler, eff_id, payload);
    } else {
        // Execute the handler in-place, since default handlers
        // are not allowed to pause the coroutine
        return seff_get_default_handler(eff_id)(payload);
    }
    // TODO: add an error message when there is no handler (rather than crashing)
    // This is a bit complicated since we don't want to have a call to fprintf directly
    // here, use a syscall wrapper
}

E __attribute__((noreturn, no_split_stack)) void seff_exit(
    seff_coroutine_t *k, effect_id eff_id, void *payload);
E __attribute__((noreturn)) void seff_throw(effect_id eff_id, void *payload);

extern effect_id_generative_cell *_seff_generated_ids;

E effect_id seff_alloc_gen_id(void);
E void seff_dealloc_gen_id(effect_id);

// TODO: this is architecture specific
#define MAKE_SYSCALL_WRAPPER(ret, fn, ...)                                                        \
    ret __attribute__((no_split_stack)) fn##_syscall_wrapper(__VA_ARGS__);                        \
    __asm__(#fn "_syscall_wrapper:"                                                               \
                "movq %fs:0x70, %rax;"                                                            \
                "testq %rax, %rax;"                                                               \
                "jz " #fn ";"                                                                     \
                "movq %rsp, %fs:_seff_paused_coroutine_stack@TPOFF;"                              \
                "movq %fs:_seff_system_stack@TPOFF, %rsp;" STACK_POLICY_SWITCH(                   \
                    "movq %rax, %fs:_seff_paused_coroutine_stack_top@TPOFF;", "",                 \
                    "") "movq $0, %fs:0x70;"                                                      \
                        "callq " #fn ";"                                                          \
                        "movq %fs:_seff_paused_coroutine_stack@TPOFF, %rsp;" STACK_POLICY_SWITCH( \
                            "movq %fs:_seff_paused_coroutine_stack_top@TPOFF, %rcx;", "",         \
                            "") "movq %rcx, %fs:0x70;"                                            \
                                "retq;")

#define EFF_ID(name) __##name##_eff_id
#define EFF_PAYLOAD_T(name) __##name##_eff_payload
#define EFF_RET_T(name) __##name##_eff_ret
#define EFF_DEF_HANDLER(name) __##name##_eff_def_handler
#define _HOW_MANY_ARGS(type, ...) (sizeof((type[]){__VA_ARGS__})/sizeof(type))
#define HANDLES(...) ((effect_set){_HOW_MANY_ARGS(effect_id, __VA_ARGS__), (effect_id[]){__VA_ARGS__}})
#define HANDLES_TOPLEVEL(...) HANDLES(__VA_ARGS__)

#define HANDLES_ALL _handles_all_set
#define HANDLES_NONE ((effect_set){0, NULL})

#define ALLOC_NEW_STATIC_ID() ({static effect_id _____id = &_____id; _____id;})

#ifdef EFF_ID_POLICY_FIXED
#define CASE_SWITCH(effect_id, block) switch(effect_id) {					   \
	block																	   \
	}
#define CASE_EFFECT(request, name, block)										\
    case EFF_ID(name): {														\
        EFF_PAYLOAD_T(name) payload = *(EFF_PAYLOAD_T(name) *)request.payload; 	\
        (void)payload;															\
        block									    							\
    }
#define CASE_RETURN(request, block)       \
    case EFF_ID(return): {                \
        struct {                          \
            void *result;                 \
        } payload;                        \
        payload.result = request.payload; \
        (void)payload;                    \
        block           	              \
    }
#define CASE_DEFAULT(block) default: block

#else

#define CASE_SWITCH(request, block) { seff_request_t *__loc_request = &request;		\
	do {																				\
		block																			\
	} while (false);	/*this is added so breaks still work as you would expect*/		\
}
#define CASE_EFFECT(name, block)															\
    if (EFF_ID(name) == __loc_request->effect) {                    		                \
        EFF_PAYLOAD_T(name) payload = *(EFF_PAYLOAD_T(name) *)(*__loc_request).payload; 	\
        (void)payload;                                                         				\
        block                                                                  				\
}
#define CASE_RETURN(block)       									\
    if (EFF_ID(return) == __loc_request->effect) {        		    \
        struct {                          							\
            void *result;                 							\
        } payload;                        							\
        payload.result = __loc_request->payload; 					\
        (void)payload;                    							\
        block                             							\
}
#define CASE_DEFAULT(block)	{ block }

#endif


#ifdef EFF_ID_POLICY_COUNTER
effect_id _id_counter_libseff_internal = 0;
effect_id _get_new_id() {
	return _id_counter_libseff_internal++;
}
#endif


#define DEFINE_LOCAL_EFFECT(name)					\
	effect_id EFF_ID(name) = seff_alloc_gen_id();			\
	default_handler_t *EFF_DEF_HANDLER(name) = *EFF_ID(name);

#define UNDEF_LOCAL_EFFECT(name) seff_dealloc_gen_id(EFF_ID(name));

#define DEFINE_LOCAL_EFFECT_IN(name, block) {DEFINE_EFFECT_LOCAL(name)				\
					 	do {						\
							block;					\
					 	} while (0);					\
					 	UNDEF_EFFECT_LOCAL(name)			\
					    }

#define DEFINE_EFFECT(name, ret_val, payload)          												\
    typedef ret_val EFF_RET_T(name);                       											\
    static default_handler_t *EFF_DEF_HANDLER(name) = NULL;											\
    static const effect_id EFF_ID(name) = (effect_id) &EFF_DEF_HANDLER(name);						\
    typedef struct payload EFF_PAYLOAD_T(name)

// Note that return need not return a struct
typedef void EFF_RET_T(return);
static const effect_id EFF_ID(return) = (effect_id) RETURN_EFFECT_ID;
typedef void EFF_PAYLOAD_T(return);

static const effect_set _handles_all_set = {-1, NULL}; // we do a little lying

static inline bool seff_finished(seff_request_t req) { return req.effect == EFF_ID(return); }

#define PERFORM(name, ...)                                                   \
    ({                                                                       \
        EFF_PAYLOAD_T(name) __payload = (EFF_PAYLOAD_T(name)){__VA_ARGS__};  \
        (EFF_RET_T(name))(uintptr_t) seff_perform(EFF_ID(name), &__payload); \
    })

#ifndef NDEBUG
E bool seff_find_effect_in(effect_id, effect_set);
#define ASSERT_HANDLES(coroutine, name) \
    assert(seff_find_effect_in(EFF_ID(name), coroutine->handled_effects))
#else
#define ASSERT_HANDLES(coroutine, name)
#endif

#define YIELD(coroutine, name, ...)                                                   \
    ({                                                                                \
        EFF_PAYLOAD_T(name) __payload = (EFF_PAYLOAD_T(name)){__VA_ARGS__};           \
        ASSERT_HANDLES(coroutine, name);                                              \
        (EFF_RET_T(name))(uintptr_t) seff_yield(coroutine, EFF_ID(name), &__payload); \
    })

#define THROW(name, ...)                               \
    ({                                                 \
        EFF_PAYLOAD_T(name) __payload = {__VA_ARGS__}; \
        seff_throw(EFF_ID(name), &__payload);          \
    })

#define EXIT(coroutine, name, ...)                     \
    ({                                                 \
        EFF_PAYLOAD_T(name) __payload = {__VA_ARGS__}; \
        seff_exit(EFF_ID(name), &__payload);           \
    })

#endif
