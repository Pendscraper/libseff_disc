#include "seff.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

DEFINE_EFFECT(runtime_error, void, { char *msg; });

#define MAX_DEPTH 100

#define KB (1024)

size_t caught = 0;

void *computation(void *_arg) {
    int64_t depth = (int64_t)_arg;
    if (depth == 0) {
        THROW(runtime_error, "error");
    } else {
        seff_coroutine_t *k = seff_coroutine_new(computation, (void *)(depth - 1));
        seff_request_t exn = seff_resume(k, NULL, HANDLES(EFF_ID(runtime_error)));
        CASE_SWITCH(exn, {
            CASE_EFFECT(runtime_error, {
                caught++;
                seff_coroutine_delete(k);
                THROW(runtime_error, payload.msg);
                break;
            })
        	CASE_DEFAULT(
            	assert(false);
            )
        })
    }
    return NULL;
}

int main(void) {
    for (size_t i = 0; i < 5000; i++) {
        seff_coroutine_t *k = seff_coroutine_new(computation, (void *)(MAX_DEPTH - 1));
        seff_request_t exn = seff_resume(k, NULL, HANDLES(EFF_ID(runtime_error)));
        CASE_SWITCH(exn, {
            CASE_EFFECT(runtime_error, {
                caught++;
                break;
            })
        	CASE_DEFAULT(
            	assert(false);
            )
        })
        seff_coroutine_delete(k);
    }
    printf("Caught %lu exceptions\n", caught);
    return 0;
}
