#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, 0, void*, {uint64_t count; });

DEFINE_EFFECT(effIntermediate01, 1, void *, {});
DEFINE_EFFECT(effIntermediate02, 2, void *, {});
DEFINE_EFFECT(effIntermediate03, 3, void *, {});
DEFINE_EFFECT(effIntermediate04, 4, void *, {});
DEFINE_EFFECT(effIntermediate05, 5, void *, {});
DEFINE_EFFECT(effIntermediate06, 6, void *, {});
DEFINE_EFFECT(effIntermediate07, 7, void *, {});
DEFINE_EFFECT(effIntermediate08, 8, void *, {});
DEFINE_EFFECT(effIntermediate09, 9, void *, {});
DEFINE_EFFECT(effIntermediate10, 10, void *, {});
DEFINE_EFFECT(effIntermediate11, 11, void *, {});
DEFINE_EFFECT(effIntermediate12, 12, void *, {});

void* tracer(void* arg) {
	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		effect_set handles = HANDLES(effIntermediate01) | HANDLES(effIntermediate02) |
									HANDLES(effIntermediate03) | HANDLES(effIntermediate04) |
									HANDLES(effIntermediate05) | HANDLES(effIntermediate06) |
									HANDLES(effIntermediate07) | HANDLES(effIntermediate08) |
									HANDLES(effIntermediate09) | HANDLES(effIntermediate10) |
									HANDLES(effIntermediate11) | HANDLES(effIntermediate12);
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
	uint64_t rotations = (argc >= 2) ? (uint64_t)argv[1] : 10000;
	uint64_t depth = (argc == 3) ? (uint64_t)argv[2] : 600;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(effBase);
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
