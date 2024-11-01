#include "seff.h"

const int PLAYER_COUNT = 4;
enum Item {
	NONE,
	SKIP,
	DOUBLE,
	ROTATE
};

DEFINE_EFFECT(end_turn, 0, void, { enum Item used; });

struct Setup {
	int health;
	enum Item items[10];
};

void *turn(void *setup) {
	int health = ((struct Setup)setup).health;
	enum Item[10] items = ((struct Setup)setup).items;
	for (int i = 0; i < 10; i++) {
		if (items[i] == DOUBLE) {
			health = health + 1;
		}
		PERFORM(end_turn, items[i]);
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
	seff_coroutine_t players[PLAYER_COUNT];
	for (int i = 0; i < PLAYER_COUNT; i++) {
		players[i] = seff_coroutine_new(turn, (struct Setup){2, {NONE, NONE, SKIP, DOUBLE, NONE, ROTATE, SKIP, ROTATE, DOUBLE, SKIP}}))
	}
	int currentPlayer = 0;
	bool forward = true;
	
	while (true) {
    		seff_request_t request = seff_resume(&players[currentPlayer], NULL, HANDLES(end_turn));
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
		    CASE_RETURN(request, { /* add killing the other players here*/return *payload.result; });
		}
	}
}
