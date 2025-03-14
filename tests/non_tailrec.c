#include "seff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFINE_EFFECT(put, void, { int x; });

void *effectful(void *arg) {
    size_t iters = (size_t)arg;
    for (size_t i = 0; i < iters; i++) {
        PERFORM(put, i);
    }
    return NULL;
}

void dummy(void) __attribute__((optnone)) { free(NULL); }

void handle(seff_coroutine_t *k) {
    seff_request_t req = seff_resume(k, NULL, HANDLES(EFF_ID(put)));
    CASE_SWITCH(req, {
        CASE_EFFECT(put, {
            dummy();
            handle(k);
            dummy();
            break;
        });
        CASE_RETURN({ return; });
    })
}

int main(int argc, char **argv) {
    size_t depth = 100;
    if (argc == 2) {
        sscanf(argv[1], "%lu", &depth);
    }
    printf("depth is %lu\n", depth);

    seff_coroutine_t *k = seff_coroutine_new(effectful, (void *)depth);

    handle(k);
    return 0;
}
