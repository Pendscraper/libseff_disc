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

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "mem/seff_mem.h"
#include "seff.h"
#include "seff_types.h"

#ifndef NDEBUG
#define DEBUG_INFO(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_INFO(fmt, ...)
#endif

extern __thread seff_coroutine_t *_seff_current_coroutine;

effect_id_generative_cell __seff_starting_struct = {NULL, NULL, NULL};
effect_id_generative_cell *_seff_generated_ids = &__seff_starting_struct;

seff_request_t seff_resume(seff_coroutine_t *k, void *arg, effect_set handled) {
	return seff_resume_unwrapped(k, arg, handled);
}

seff_request_t seff_resume_handling_all(seff_coroutine_t *k, void *arg) {
    return seff_resume(k, arg, HANDLES_ALL);
}

void frame_push(seff_cont_t *cont, void *elt) {
    cont->rsp = (char *)cont->rsp - sizeof(void *);
    *(void **)cont->rsp = elt;
}

seff_coroutine_t *seff_current_coroutine(void) { return _seff_current_coroutine; }
#ifdef STACK_POLICY_SEGMENTED
__asm__("seff_current_stack_top:"
        "movq %fs:0x70,%rax;"
        "ret;");
#endif

void seff_coroutine_reset(seff_coroutine_t *k) {}

void seff_coroutine_delete(seff_coroutine_t *k) {
    seff_coroutine_release(k);
    free(k);
}

void seff_coroutine_release(seff_coroutine_t *k) {
    STACK_POLICY_SWITCH(
        {
            seff_stack_segment_t *segment = k->frame_ptr;
            while (segment && segment->prev) {
                segment = segment->prev;
            }
            while (segment) {
                seff_stack_segment_t *old = segment;
                segment = segment->next;
                free(old);
            }
        },
        { free(k->frame_ptr); }, {_Pragma("not implemented")});
}

seff_coroutine_t *seff_coroutine_new(seff_start_fun_t *fn, void *arg) {
    seff_coroutine_t *k = (seff_coroutine_t *)malloc(sizeof(seff_coroutine_t));
    seff_coroutine_init(k, fn, arg);
    return k;
}

seff_coroutine_t *seff_coroutine_new_sized(seff_start_fun_t *fn, void *arg, size_t frame_size) {
    seff_coroutine_t *k = (seff_coroutine_t *)malloc(sizeof(seff_coroutine_t));
    seff_coroutine_init_sized(k, fn, arg, frame_size);
    return k;
}

// TODO: We're getting this from object file seff_mem.o, this seems dirty
extern size_t default_frame_size;
bool seff_coroutine_init(seff_coroutine_t *k, seff_start_fun_t *fn, void *arg) {
    return seff_coroutine_init_sized(k, fn, arg, default_frame_size);
}

E __attribute__((noreturn, no_split_stack)) void coroutine_prelude(void);
bool seff_coroutine_init_sized(
    seff_coroutine_t *k, seff_start_fun_t *fn, void *arg, size_t frame_size) {
    char *rsp;
    // Overhead needed by coroutine_prelude (since they make a call)
    size_t overhead = 16;
    seff_frame_ptr_t stack = init_stack_frame(frame_size + overhead, &rsp);
    if (!stack) {
        return false;
    }
    k->frame_ptr = stack;
    k->resume_point.rsp = (void *)rsp;
    k->resume_point.rbp = NULL;
    k->resume_point.ip = (void *)coroutine_prelude;
#ifdef STACK_POLICY_SEGMENTED
    void *stack_top = rsp - frame_size - overhead;
    k->resume_point.stack_top = stack_top;
#endif
    k->resume_point.current_coroutine = k;
    k->resume_point.rbx = (void *)0xcacabbbb;
    k->resume_point.r12 = (void *)0xcaca1212;
    k->resume_point.r13 = (void *)k;
    k->resume_point.r14 = (void *)arg;
    k->resume_point.r15 = (void *)fn;
    k->state = PAUSED;

    /*
     * Padding to ensure the stack address is 16-byte aligned
     * This is necessary depending on the stack layout because the
     * x86-64 ABI mandates a 16-byte aligned stack
     */
    size_t pad = ((uintptr_t)k->resume_point.rsp) % 16;
    k->resume_point.rsp = ((char *)k->resume_point.rsp) - pad;

    assert(((uintptr_t)k->resume_point.rsp) % 16 == 0);

    return true;
}

bool seff_find_effect_in(effect_id effect, effect_set handled) {
	if (handled == HANDLES_ALL) {
		return true;
	}
	if (handled == NULL) { // this check exists solely because syscall wrappers seem to make the effect set null
		return false;
	}
	for (int i = 1; i <= handled[0]; i++) {
		if (handled[i] == effect)
			return true;
	}
	return false;
}

seff_coroutine_t *seff_locate_handler(effect_id effect) {
    seff_coroutine_t *k = _seff_current_coroutine;
    if (effect != EFF_ID(return)) {
        // The special 'return' effect is implicitly handled by every coroutine
        while (k && !seff_find_effect_in(effect, k->handled_effects)) {
            k = (seff_coroutine_t *)k->parent_coroutine;
        }
    }
    return k;
}

default_handler_t *seff_set_default_handler(effect_id effect, default_handler_t *handler) {
	// effect IDs are also a pointer to the pointer to the default handler
	default_handler_t **handler_loc = (default_handler_t **)effect;
	default_handler_t *prev = *handler_loc;
	*handler_loc = handler;
    return prev;
}

default_handler_t *seff_get_default_handler(effect_id effect) { return *((default_handler_t **)effect); }

effect_id seff_alloc_gen_id(void) {
	effect_id_generative_cell *newcell = malloc(sizeof(effect_id_generative_cell));
	newcell -> previous = NULL;
	newcell -> handler = NULL;
	newcell -> next = _seff_generated_ids;
	_seff_generated_ids -> previous = newcell;
	_seff_generated_ids = newcell;
	return (effect_id)newcell;
}

void seff_dealloc_gen_id(effect_id id) {
	effect_id_generative_cell *remcell = (effect_id_generative_cell *)id;
	effect_id_generative_cell *back = remcell -> previous;
	effect_id_generative_cell *forward = remcell -> next;
	if (forward != NULL) {
		forward -> previous = back;
	}
	if (back != NULL) {
		back -> next = forward;
	} else {
		_seff_generated_ids = forward;
	}
	// do we dealloc the default handler here? naah
	free(remcell);
	return;
}

void seff_throw(effect_id eff_id, void *payload) {
    // The state code is set by seff_exit
    seff_coroutine_t *handler = seff_locate_handler(eff_id);
    if (handler) {
        seff_exit(handler, eff_id, payload);
    } else {
        /* Execute the handler in-place, since default handlers are not allowed to pause the
         * coroutine */
        (*seff_get_default_handler(eff_id))(payload);
        exit(-1);
    }
    // TODO: add an error message when there is no handler (rather than crashing)
    // This is a bit complicated since we don't want to have a call to fprintf directly here
}
