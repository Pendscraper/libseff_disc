#include "seff.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

DEFINE_EFFECT(runtime_error, void, { char *msg; });

#define MAX_DEPTH 100

#define KB (1024)

seff_coroutine_t children[MAX_DEPTH];

void *computation(void *_arg) {
    int64_t depth = (int64_t)_arg;
    if (depth == 0) {
        THROW(runtime_error, "error");
    } else {
        seff_coroutine_init(&children[depth], computation, (void *)(depth - 1));
        seff_resume(&children[depth], NULL, 0);
    }
    return NULL;
}

int main(void) {
    size_t caught = 0;
    seff_coroutine_t k;
    seff_coroutine_init_sized(&k, computation, (void *)(MAX_DEPTH - 1), 16 * KB);
    for (size_t depth = 0; depth < MAX_DEPTH; depth++) {
        seff_coroutine_init_sized(&children[depth], computation, NULL, 16 * KB);
    }

    for (size_t i = 0; i < 5000; i++) {
        seff_request_t exn = seff_resume(&k, NULL, HANDLES(EFF_ID(runtime_error)));
        CASE_SWITCH(exn, {
            CASE_EFFECT(runtime_error, { caught++; break;});
            
		    CASE_DEFAULT(
		        assert(false);
            )
        })
        seff_coroutine_init_sized(&k, computation, (void *)(MAX_DEPTH - 1), 16 * KB);
    }
    seff_coroutine_release(&k);
    for (size_t depth = 0; depth < MAX_DEPTH; depth++) {
        seff_coroutine_release(&children[depth]);
    }
    printf("Caught %lu exceptions\n", caught);
    return 0;
}
