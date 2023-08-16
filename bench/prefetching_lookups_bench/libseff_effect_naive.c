#include "libseff_effect_common.h"
#include "seff.h"
#include <stdio.h>

long SegsEffectSingleLookup(int v[], size_t v_size, int lookups[], size_t lookups_size) {

    size_t found_count = 0;
    size_t not_found_count = 0;
    const size_t coro_size = 256;

    for (size_t i = 0; i < lookups_size; ++i) {
        seg_args args;
        args.first = v;
        args.len = v_size;
        args.key = lookups[i];

        seff_coroutine_t *coro =
            seff_coroutine_new_sized(SegEffectBinarySearch, (void *)&args, coro_size);
        bool finished = false;
        int64_t last_read = 0;
        while (!finished) {
            seff_eff_t *request =
                seff_handle(coro, (void *)last_read, HANDLES(deref));

            if (coro->state == FINISHED) {
                bool res = (bool) request ? 1 : 0;
                found_count += res;
                not_found_count += 1 - res;
                finished = true;
                seff_coroutine_delete(coro);
            } else {
                switch (request->id) {
                    CASE_EFFECT(request, deref, {
                        last_read = *payload.addr;
                        break;
                    });
                }
            }

        }
    }

    if (found_count + not_found_count != lookups_size)
        printf("BUG: found %zu, not-found: %zu total %zu, expected: %zu\n", found_count,
            not_found_count, found_count + not_found_count, lookups_size);

    return found_count;
}

long testSegsEffSingle(int v[], size_t v_size, int lookups[], size_t lookups_size, int streams) {
    return SegsEffectSingleLookup(v, v_size, lookups, lookups_size);
}

int runner_c(long (*testFn)(int[], size_t, int[], size_t, int), int streams, const char *algo_name);

int main(int argc, const char **argv) {
    int streams = 1;

    if (argc == 2) {
        streams = atoi(argv[1]);
    }

    // 8 seems to be optimal
    return runner_c(testSegsEffSingle, streams, "seff");
}