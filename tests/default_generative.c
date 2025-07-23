#include "seff.h"
#include "seff_types.h"

#include <assert.h>
#include <stdio.h>

static effect_id division_by_zero_id;
static bool shouldnt_crash = true;

int safe_division(int dividend, int divisor) {
    if (divisor == 0) {
        seff_throw(division_by_zero_id, (void *)dividend);
    }
    return dividend / divisor;
}

void *safe_computation(void *_arg) {
	printf("resumed\n");
    for (int i = -5; i < 5; i++) {
        safe_division(100, i);
    }
    return NULL;
}

void *div_by_zero_crash(void *arg) {
    fprintf(stderr, "ERROR: dividing %d by zero\n", (int)arg);
    exit(shouldnt_crash);
}

extern size_t default_frame_size;
int main(void) {
    puts("hi");
    DEFINE_LOCAL_EFFECT(division_by_zero);
    division_by_zero_id = EFF_ID(division_by_zero);

    seff_set_default_handler(EFF_ID(division_by_zero), div_by_zero_crash);
    seff_coroutine_t k;
    seff_coroutine_init(&k, safe_computation, NULL);
    
    puts("startup safe");
    effect_set handled = HANDLES(EFF_ID(division_by_zero));
	printf("leng: %d, first: %lu, addr: %p\n", handled.length, handled.effects[0], handled.effects);

    seff_request_t exn = seff_resume(&k, NULL, handled);

    if (exn.effect == EFF_ID(division_by_zero)) {
	printf("Caught division (%d / 0) in coroutine, continuing main\n", (int)exn.payload);
    }

    shouldnt_crash = false;
    seff_coroutine_release(&k);
    safe_division(100, 0);
    UNDEF_LOCAL_EFFECT(division_by_zero);
    return 1;
}
