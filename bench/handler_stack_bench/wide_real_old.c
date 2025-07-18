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
		effect_set handles = HANDLES(effIntermediate01) | HANDLES(effIntermediate02) |
									HANDLES(effIntermediate03) | HANDLES(effIntermediate04) |
									HANDLES(effIntermediate05) | HANDLES(effIntermediate06) |
									HANDLES(effIntermediate07) | HANDLES(effIntermediate08) |
									HANDLES(effIntermediate09) | HANDLES(effIntermediate10) |
									HANDLES(effIntermediate11) | HANDLES(effIntermediate12) |
									HANDLES(effIntermediate13) | HANDLES(effIntermediate14) |
									HANDLES(effIntermediate15) | HANDLES(effIntermediate16) |
									HANDLES(effIntermediate17) | HANDLES(effIntermediate18) |
									HANDLES(effIntermediate19) | HANDLES(effIntermediate20) |
									HANDLES(effIntermediate21) | HANDLES(effIntermediate22) |
									HANDLES(effIntermediate23) | HANDLES(effIntermediate24) |
									HANDLES(effIntermediate25) | HANDLES(effIntermediate26) |
									HANDLES(effIntermediate27) | HANDLES(effIntermediate28) |
									HANDLES(effIntermediate29) | HANDLES(effIntermediate30) |
									HANDLES(effIntermediate31) | HANDLES(effIntermediate32);
		seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(count - 1));
		seff_resume(k, NULL, handles);
	} else {
		uint64_t times = (uint64_t)PERFORM(effBase, count);
		for (int i = 0; i < times; i++) {
			PERFORM(effBase, count);
		}
	}
	return (void*)76;
}

int main(int argc, char **argv) {
	if (argc > 3) return 1;
	uint64_t rotations = (argc >= 2) ? (uint64_t)argv[1] : 10000;
	uint64_t depth = (argc == 3) ? (uint64_t)argv[2] : 600;
    if (rotations == 0) return 2;
    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(effBase);
    seff_resume(k, NULL, handler);
    seff_request_t req = seff_resume(k, (void*)rotations, handler);
    for (int i = 0; i < rotations; i++) {
	switch (req.effect) {
            CASE_EFFECT(req, effBase, { break;});
	    CASE_RETURN(req, {return 1;})
	    default:
		assert(false);
        }
	req = seff_resume(k, (void*)rotations, handler);
    }
    if (req.effect == EFF_ID(return)) return 0;
    return 1;
}
