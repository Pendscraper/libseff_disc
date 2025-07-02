#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, int, {int count; });

DEFINE_EFFECT(effIntermediate, void, {});

void* tracer(void* arg) {
	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		effect_set handles = HANDLES(EFF_ID(effIntermediate), EFF_ID(effIntermediate),
									EFF_ID(effIntermediate), EFF_ID(effIntermediate),
									EFF_ID(effIntermediate), EFF_ID(effIntermediate),
									EFF_ID(effIntermediate), EFF_ID(effIntermediate));
		seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(count - 1));
		seff_resume(k, NULL, handles);
	} else {
		PERFORM(effBase, count);
	}
	return (void*)76;
}

int main(int argc, char **argv) {
	if (argc > 2) return 1;
	uint64_t depth = (argc == 2) ? (uint64_t)argv[1] : 600;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    return 0;
}
