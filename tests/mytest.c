#include "seff.h"
#include <stdio.h>


const int PLAYER_COUNT = 4;
typedef enum {
	NONE,
	SKIP,
	DOUBLE,
	ROTATE
} Item;

DEFINE_EFFECT(end_turn, 0, void, { Item used; });

typedef struct {
	int health;
	Item items[10];
} Setup;

void *turn(void *setup) {
	
	int health = ((Setup*)setup)->health;
	for (int i = 0; i < 10; i++) {
		Item current = ((Setup*)setup)->items[i];
		if (current == DOUBLE) {
			health = health + 1;
		}
		PERFORM(end_turn, current);
	};
	
	int *hth = malloc(sizeof(int));
	hth[0] = health;
	return (void *)hth;
}

int nextPlayer(int current, bool forward) {
	if (forward) {
		return (current + 1) % PLAYER_COUNT;
	} else {
		return (current + PLAYER_COUNT - 1) % PLAYER_COUNT;
	}
}

int main(void) {
	seff_coroutine_t *players[PLAYER_COUNT];
	Setup default_setup = {2, {NONE, NONE, SKIP, DOUBLE, NONE, ROTATE, SKIP, ROTATE, DOUBLE, SKIP}};
	for (int i = 0; i < PLAYER_COUNT; i++) {
		players[i] = seff_coroutine_new(turn, &default_setup);
	}
	int currentPlayer = 0;
	bool forward = true;
	
	while (true) {
    		seff_request_t request = seff_resume(players[currentPlayer], NULL, HANDLES(end_turn));
    		switch (request.effect) {
    		
		    CASE_EFFECT(request, end_turn, {
		        switch (payload.used) {
		        	case SKIP:
		        		currentPlayer = nextPlayer(currentPlayer, forward);
		        		break;
		        	case DOUBLE:
		        		break;
		        	case ROTATE:
		        		forward = !forward;
		        		break;
		        	default:
		        		break;
		        }
		        currentPlayer = nextPlayer(currentPlayer, forward);
		    });
		    CASE_RETURN(request, {
		    	/* add killing the other players here*/int *final = (int *)payload.result;
		    	printf("fastest has %d health", *final);
		    	free(final);
		    	return 0; 
		    });
		}
	}
}
