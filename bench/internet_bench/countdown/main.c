#include "seff.h"
#include <stdio.h>

DEFINE_EFFECT(get, int64_t, {});
DEFINE_EFFECT(put, void, { int64_t new_value; });

static int64_t evalState(seff_coroutine_t *k, int64_t initialState) {
  int64_t state = initialState;
  effect_set handles_state = HANDLES(EFF_ID(get), EFF_ID(put)); 
  seff_request_t req = seff_resume(k, NULL, handles_state);

  int64_t result = -1;
  bool done = false;
  while (!done) {   
    CASE_SWITCH(req, {
      CASE_EFFECT(get, {
        req = seff_resume(k, (void*) state, handles_state);
        break;
      })
      CASE_EFFECT(put, {
        state = payload.new_value;
        req = seff_resume(k, 0, handles_state);
        break;
      })
      CASE_RETURN({
        result = (int64_t) payload.result;
        done = true;
        break;
      })
    });
  }
  return result;
}

static void* countdown(void* parameter) {
  int64_t state = PERFORM (get);
  while (state > 0) {
    PERFORM (put, state - 1);
    state = PERFORM (get);
  }
  return (void*) state;
}

static int64_t run(int64_t n) {
  seff_coroutine_t *k = seff_coroutine_new(countdown, (void*) n);
  int64_t result = evalState(k, n);
  seff_coroutine_delete(k);
  return result;
}

int main(int argc, char** argv) { 
  int64_t n = argc != 2 ? 5 : atoi(argv[1]);
  int64_t result = run(n); 
  
  // Increase output buffer size to increase performance
  char buffer[8192];
  setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
  printf("%ld\n", result);
  return 0;   
}
