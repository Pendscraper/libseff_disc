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

void* tracer(void* arg) {
	uint64_t count = (uint64_t)arg;
	if (count > 0) {
		effect_set handles = HANDLES(EFF_ID(effIntermediate01), EFF_ID(effIntermediate02),
									EFF_ID(effIntermediate03), EFF_ID(effIntermediate04),
									EFF_ID(effIntermediate05), EFF_ID(effIntermediate06),
									EFF_ID(effIntermediate07), EFF_ID(effIntermediate08),
									EFF_ID(effIntermediate09), EFF_ID(effIntermediate10),
									EFF_ID(effIntermediate11), EFF_ID(effIntermediate12));
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
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
