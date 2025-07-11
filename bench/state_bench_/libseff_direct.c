#include "seff.h"

#include <stdio.h>

DEFINE_EFFECT(put, void, { int64_t value; });
DEFINE_EFFECT(get, int64_t, {});

void put(seff_coroutine_t *handler, int64_t number) { YIELD(handler, put, number); }
int64_t get(seff_coroutine_t *handler) { return YIELD(handler, get); }

void *stateful(void *_arg) {
    seff_coroutine_t *put_handler = seff_locate_handler(EFF_ID(put));
    seff_coroutine_t *get_handler = seff_locate_handler(EFF_ID(get));
    for (int i = 0; i < 10000000; i++) {
        put(put_handler, get(get_handler) + 1);
    }
    return NULL;
}

int main(int argc, char **argv) {
    seff_coroutine_t *k = seff_coroutine_new(stateful, NULL);

    int64_t value = 0;

    seff_request_t request = seff_resume(k, NULL, HANDLES(EFF_ID(put), EFF_ID(get)));
    while (!seff_finished(request)) {
        CASE_SWITCH(request, {
            CASE_EFFECT(put, {
                value = payload.value;
                request = seff_resume(k, NULL, HANDLES(EFF_ID(put), EFF_ID(get)));
                break;
            });
            CASE_EFFECT(get, {
                request = seff_resume(k, (void *)value, HANDLES(EFF_ID(put), EFF_ID(get)));
                break;
            });
        })
    }

    printf("Final value is %ld\n", value);
}
