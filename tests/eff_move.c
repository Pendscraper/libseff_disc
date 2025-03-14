#include "seff.h"
#include "seff_types.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFINE_EFFECT(get_name, char *, {});
DEFINE_EFFECT(print, void, { char *str; });

void *foo(void *arg) {
    char str[128] = "Hello from ";

    while (1) {
        char *back = str + sizeof("Hello from ") - 1;
        char *name = PERFORM(get_name);
        strcpy(back, name);
        PERFORM(print, str);
    }
}

void *bar(void *_child) {
    seff_coroutine_t *child = (seff_coroutine_t *)_child;

    char *response = NULL;
    seff_request_t request = seff_resume(child, response, HANDLES(EFF_ID(get_name)));
    CASE_SWITCH(request, {
        CASE_EFFECT(get_name, {
            response = "bar1";
            break;
        });
		CASE_DEFAULT(
		    assert(false);
        )
    })

    request = seff_resume(child, response, HANDLES(EFF_ID(get_name)));
    CASE_SWITCH(request, {
        CASE_EFFECT(get_name, {
            response = "bar2";
            break;
        });
		CASE_DEFAULT(
		    assert(false);
        )
    })

    request = seff_resume(child, response, HANDLES(EFF_ID(get_name)));
    CASE_SWITCH(request, {
        CASE_EFFECT(get_name, {
            response = "bar3";
            break;
        });
		CASE_DEFAULT(
		    assert(false);
        )
    })

    return NULL;
}

int main(void) {
    seff_coroutine_t *k = seff_coroutine_new(foo, NULL);

    seff_request_t request = seff_resume(k, NULL, HANDLES(EFF_ID(get_name), EFF_ID(print)));
    assert(request.effect == EFF_ID(get_name));

    char *main_name = "main";
    request = seff_resume(k, main_name, HANDLES(EFF_ID(get_name), EFF_ID(print)));
    assert(request.effect == EFF_ID(print));
    puts(((EFF_PAYLOAD_T(print) *)request.payload)->str);

    seff_coroutine_t *j = seff_coroutine_new(bar, k);

    while (1) {
        request = seff_resume(j, NULL, HANDLES(EFF_ID(print)));
        if (j->state == FINISHED)
            break;
        CASE_SWITCH(request, {
            CASE_EFFECT(print, {
                puts(payload.str);
                break;
            });
		    CASE_DEFAULT(
		        assert(false);
            )
        })
    }
}
