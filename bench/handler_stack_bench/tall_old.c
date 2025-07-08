#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, 60, void*, {uint64_t count; });

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
DEFINE_EFFECT(effIntermediate13, 13, void *, {});
DEFINE_EFFECT(effIntermediate14, 14, void *, {});
DEFINE_EFFECT(effIntermediate15, 15, void *, {});
DEFINE_EFFECT(effIntermediate16, 16, void *, {});
DEFINE_EFFECT(effIntermediate17, 17, void *, {});
DEFINE_EFFECT(effIntermediate18, 18, void *, {});
DEFINE_EFFECT(effIntermediate19, 19, void *, {});
DEFINE_EFFECT(effIntermediate20, 20, void *, {});
DEFINE_EFFECT(effIntermediate21, 21, void *, {});
DEFINE_EFFECT(effIntermediate22, 22, void *, {});
DEFINE_EFFECT(effIntermediate23, 23, void *, {});
DEFINE_EFFECT(effIntermediate24, 24, void *, {});
DEFINE_EFFECT(effIntermediate25, 25, void *, {});
DEFINE_EFFECT(effIntermediate26, 26, void *, {});
DEFINE_EFFECT(effIntermediate27, 27, void *, {});
DEFINE_EFFECT(effIntermediate28, 28, void *, {});
DEFINE_EFFECT(effIntermediate29, 29, void *, {});
DEFINE_EFFECT(effIntermediate30, 30, void *, {});
DEFINE_EFFECT(effIntermediate31, 31, void *, {});
DEFINE_EFFECT(effIntermediate32, 32, void *, {});

void* tracer(void* arg) {
	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		effect_set handles = HANDLES(EFF_ID(effIntermediate01), EFF_ID(effIntermediate02),
									EFF_ID(effIntermediate03), EFF_ID(effIntermediate04),
									EFF_ID(effIntermediate05), EFF_ID(effIntermediate06),
									EFF_ID(effIntermediate07), EFF_ID(effIntermediate08),
									EFF_ID(effIntermediate09), EFF_ID(effIntermediate10),
									EFF_ID(effIntermediate11), EFF_ID(effIntermediate12),
									EFF_ID(effIntermediate13), EFF_ID(effIntermediate14),
									EFF_ID(effIntermediate15), EFF_ID(effIntermediate16),
									EFF_ID(effIntermediate17), EFF_ID(effIntermediate18),
									EFF_ID(effIntermediate19), EFF_ID(effIntermediate20),
									EFF_ID(effIntermediate21), EFF_ID(effIntermediate22),
									EFF_ID(effIntermediate23), EFF_ID(effIntermediate24),
									EFF_ID(effIntermediate25), EFF_ID(effIntermediate26),
									EFF_ID(effIntermediate27), EFF_ID(effIntermediate28),
									EFF_ID(effIntermediate29), EFF_ID(effIntermediate30),
									EFF_ID(effIntermediate31), EFF_ID(effIntermediate32));
		seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(count - 1));
		seff_resume(k, NULL, handles);
	} else {
		uint64_t times = (uint64_t)PERFORM(effBase, count);
		volatile seff_coroutine_t *original = NULL;
		for (int i = 0; i < times; i++) {
			original = seff_locate_handler(EFF_ID(effBase));
		}
		printf("%p found %d times", (void*)original, (int)times);
	}
	return (void*)76;
}

int main(int argc, char **argv) {
	if (argc > 3) return 1;
	uint64_t rotations = (argc >= 2) ? (uint64_t)argv[1] : 100000;
	uint64_t depth = (argc == 3) ? (uint64_t)argv[2] : 6000;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
