#include "seff.h"
#include "seff_types.h"

#include <stdio.h>

DEFINE_EFFECT(effBase, void*, {uint64_t count; });

static int width;

void* tracer(void* arg) {
	effect_id *handles = malloc((width + 1) * sizeof(effect_id));
	handles[0] = width;
	for (int i = 1; i <= width; i++) {
		handles[i] = (effect_id)malloc(sizeof(char));
	}

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
    width = (argc >= 4) ? atoi(argv[3]) : 350;

    seff_coroutine_t *k = seff_coroutine_new(tracer, (void*)(depth));
    effect_set handler = HANDLES(EFF_ID(effBase));
    seff_resume(k, NULL, handler);
    seff_resume(k, (void*)rotations, handler);
    return 0;
}
