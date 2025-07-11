#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, void*, {uint64_t count; });

DEFINE_EFFECT(effIntermediate01, void *, {});
DEFINE_EFFECT(effIntermediate02, void *, {});
DEFINE_EFFECT(effIntermediate03, void *, {});
DEFINE_EFFECT(effIntermediate04, void *, {});
DEFINE_EFFECT(effIntermediate05, void *, {});
DEFINE_EFFECT(effIntermediate06, void *, {});
DEFINE_EFFECT(effIntermediate07, void *, {});
DEFINE_EFFECT(effIntermediate08, void *, {});
DEFINE_EFFECT(effIntermediate09, void *, {});
DEFINE_EFFECT(effIntermediate10, void *, {});
DEFINE_EFFECT(effIntermediate11, void *, {});
DEFINE_EFFECT(effIntermediate12, void *, {});
DEFINE_EFFECT(effIntermediate13, void *, {});
DEFINE_EFFECT(effIntermediate14, void *, {});
DEFINE_EFFECT(effIntermediate15, void *, {});
DEFINE_EFFECT(effIntermediate16, void *, {});
DEFINE_EFFECT(effIntermediate17, void *, {});
DEFINE_EFFECT(effIntermediate18, void *, {});
DEFINE_EFFECT(effIntermediate19, void *, {});
DEFINE_EFFECT(effIntermediate20, void *, {});
DEFINE_EFFECT(effIntermediate21, void *, {});
DEFINE_EFFECT(effIntermediate22, void *, {});
DEFINE_EFFECT(effIntermediate23, void *, {});
DEFINE_EFFECT(effIntermediate24, void *, {});
DEFINE_EFFECT(effIntermediate25, void *, {});
DEFINE_EFFECT(effIntermediate26, void *, {});
DEFINE_EFFECT(effIntermediate27, void *, {});
DEFINE_EFFECT(effIntermediate28, void *, {});
DEFINE_EFFECT(effIntermediate29, void *, {});
DEFINE_EFFECT(effIntermediate30, void *, {});
DEFINE_EFFECT(effIntermediate31, void *, {});
DEFINE_EFFECT(effIntermediate32, void *, {});

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
	uint64_t rotations = (argc >= 2) ? (uint64_t)argv[1] : 10000;
	uint64_t depth = (argc == 3) ? (uint64_t)argv[2] : 600;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
