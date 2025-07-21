#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, void*, {uint64_t count; });

DEFINE_EFFECT(effIntermediate01, void *, {});

void* tracer(void* arg) {
	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		effect_set handles = HANDLES(EFF_ID(effIntermediate01));
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
	if (argc > 3) return 1;
	uint64_t rotations = (argc >= 2) ? atoi(argv[1]) : 10000;
	uint64_t depth = (argc == 3) ? atoi(argv[2]) : 600;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
