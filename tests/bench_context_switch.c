#include "seff.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET 0
#define PUT 1

DEFINE_EFFECT(get, int64_t, {});
DEFINE_EFFECT(put, void, { int64_t value; });

int64_t get(void) { return PERFORM(get); }
void put(int64_t arg) { PERFORM(put, arg); }

void *coroutine(void *arg) {
    while (true) {
        put(get() + 1);
    }
}

int main(void) {
    size_t requests = 0;

    seff_coroutine_t *k = seff_coroutine_new(coroutine, NULL);
    seff_request_t request = seff_resume(k, NULL, HANDLES(EFF_ID(get), EFF_ID(put)));

    int64_t state = 0;
    while (requests < 100 * 1000 * 1000) { // 100.000.000
        CASE_SWITCH(request, {
            CASE_EFFECT(get, {
                request = seff_resume(k, (void *)state, HANDLES(EFF_ID(get), EFF_ID(put)));
                break;
            })
            CASE_EFFECT(put, {
                state = payload.value;
                request = seff_resume(k, NULL, HANDLES(EFF_ID(get), EFF_ID(put)));
                break;
            })
		    CASE_DEFAULT(
		        assert(false);
            )
        })
        requests++;
    }
    printf("Final state: %ld\n", state);
    printf("Handled %ld requests\n", requests);
    return 0;
}
