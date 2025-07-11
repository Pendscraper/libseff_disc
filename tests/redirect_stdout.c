#include "seff.h"
#include "seff_types.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

DEFINE_EFFECT(print, void, { const char *str; });
void print(const char *str) { PERFORM(print, str); }

void *default_print(void *_arg) {
    EFF_PAYLOAD_T(print) payload = *(EFF_PAYLOAD_T(print) *)_arg;
    fputs(payload.str, stdout);
    return NULL;
}

extern size_t default_frame_size;
void *with_output_to_buffer(seff_start_fun_t *fn, void *arg, char *buffer) {
    seff_coroutine_t k;
    seff_coroutine_init(&k, fn, arg);

    char *buffptr = buffer;

    while (true) {
        seff_request_t res = seff_resume(&k, NULL, HANDLES(EFF_ID(print)));
        CASE_SWITCH(res, {
            CASE_EFFECT(print, {
                // Note we are NOT using strcpy!
                buffptr = stpcpy(buffptr, payload.str);
                break;
            });
            CASE_RETURN({ return payload.result; });
		    CASE_DEFAULT(
		        assert(false);
		    )
        })
    }

    return NULL;
}

void *with_output_to_default(seff_start_fun_t *fn, void *arg) {
    seff_coroutine_t k;
    seff_coroutine_init(&k, fn, arg);

    seff_request_t res = seff_resume(&k, NULL, HANDLES_NONE);
    assert(res.effect == EFF_ID(return));
    return res.payload;
}

void *printer(void *arg) {
    print("Hello, ");
    print("World!\n");
    return NULL;
}

int main(void) {
    seff_set_default_handler(EFF_ID(print), default_print);
    with_output_to_default(printer, NULL);
    char buffer[512] = {0};
    with_output_to_buffer(printer, NULL, buffer);
    printf("Coroutine said %s", buffer);
}
