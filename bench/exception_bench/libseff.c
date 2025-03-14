#include "seff.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

DEFINE_EFFECT(runtime_error, void, { char *msg; });

void *computation(void *_arg) { THROW(runtime_error, "error"); }

int main(void) {
    size_t caught = 0;
    seff_coroutine_t k;
    seff_coroutine_init(&k, computation, NULL);
    for (size_t i = 0; i < 1000000; i++) {
        seff_request_t exn = seff_resume(&k, NULL, HANDLES(runtime_error));
        CASE_SWITCH(exn, {
            CASE_EFFECT(runtime_error, { caught++; break;});
		    CASE_DEFAULT(
		        assert(false);
		    )
        })
        seff_coroutine_init(&k, computation, NULL);
    }
    seff_coroutine_release(&k);
    printf("Caught %lu exceptions\n", caught);
    return 0;
}
