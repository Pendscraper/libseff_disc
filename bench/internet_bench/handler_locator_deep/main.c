#include "seff.h"
#include <stdio.h>

#define DEPTH 1

#define CUST_ID 3

static effect_id EFF_ID(custid) = (effect_id)CUST_ID;
typedef void EFF_RET_T(custid);
typedef struct custparams EFF_PAYLOAD_T(custid);

typedef struct custparams {
	int64_t loops;
	int depth;
} custparams;

static void* rsub(void *params) {
	custparams pars = *(custparams *)params;
	if (pars.depth == 0) {
		for (int i = 0; i < pars.loops; i++) {
			printf("%p", seff_locate_handler(CUST_ID));
		}
	}
	custparams newpars = {
		.loops = pars.loops,
		.depth = pars.depth - 1
	};
	seff_coroutine_t *k = seff_coroutine_new(rsub, &newpars);
	seff_request_t req = seff_resume(k, NULL, HANDLES_NONE);
	CASE_SWITCH(req, {
		CASE_RETURN({
		  return NULL;
		})
		
		CASE_DEFAULT({
			assert(false);
		})
	});
}


int main(int argc, char** argv) {
	int64_t n = argc != 2 ? 5 : atoi(argv[1]);
	
	custparams newpars = {
		.loops = n,
		.depth = DEPTH
	};
	
	seff_coroutine_t *k = seff_coroutine_new(rsub, &newpars);
	seff_request_t req = seff_resume(k, NULL, HANDLES(CUST_ID));
	while (true) {
		CASE_SWITCH(req, {
			CASE_EFFECT(custid, {
				req = seff_resume(k, NULL, HANDLES(CUST_ID));
				break;
			})
			CASE_RETURN({
			  return 0;
			})
			
			CASE_DEFAULT({
				assert(false);
			})
		});
	}
}
