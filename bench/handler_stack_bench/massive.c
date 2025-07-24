#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, 0, void*, {uint64_t count; });

static uint64_t width;

void* tracer(void* arg) {
	effect_set handles = (~((~(0UL)) << width)) ^ 1UL;

	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(count - 1));
		seff_resume(k, NULL, handles);
	} else {
		uint64_t times = (uint64_t)PERFORM(effBase, count);
		volatile seff_coroutine_t *original = NULL;
		for (int i = 0; i < times; i++) {
			original = seff_locate_handler(EFF_ID(effBase));
		}
		printf("%p found %d times\n", (void*)original, (int)times);
	}
	return (void*)76;
}

int main(int argc, char **argv) {
    if (argc > 4) return 1;
    uint64_t rotations = (argc >= 2) ? atoi(argv[1]) : 10000;
    uint64_t depth = (argc >= 3) ? atoi(argv[2]) : 600;
    width = (argc >= 4) ? atoi(argv[3]) : 30;

    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(effBase);
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
