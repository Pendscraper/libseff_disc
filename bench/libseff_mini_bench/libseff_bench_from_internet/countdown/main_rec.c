#include "seff.h"
#include <stdio.h>

DEFINE_EFFECT(get, int64_t, {});
DEFINE_EFFECT(put, void, { int64_t new_value; });

int64_t* evalState(seff_coroutine_t *k, int64_t param, int64_t *state) {
  seff_request_t req = seff_resume(k, (void*) param, HANDLES(EFF_ID(get), EFF_ID(put)));
  CASE_SWITCH(req, {
    CASE_EFFECT(get, {
      return evalState(k, *state, state);
    })
    CASE_EFFECT(put, {
      *state = payload.new_value;
      printf("New value: %ld\n", *state);
      return evalState(k, 0, state);
    })
    CASE_RETURN({
      return (int64_t*) payload.result;
    })
  });
}

void* countdown(void* parameter) {
  int64_t state = PERFORM (get);
  while (state > 0) {
    PERFORM (put, state - 1);
    state = PERFORM (get);
  }
  return &state;
}

int64_t run(int64_t n) {
  int64_t state = n;
  seff_coroutine_t *k = seff_coroutine_new(countdown, &state);
  return *evalState(k, 0, &state);
}

int main(int argc, char** argv) { 
  int64_t n = argc != 2 ? 5 : atoi(argv[1]);
  int64_t result = run(n);
  printf("%ld\n", result);
  return 0;   
}
