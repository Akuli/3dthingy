#ifndef PLAYER_H
#define PLAYER_H

#include "display.h"
#include "physics.h"
#include "vecmat.h"

struct Player {
	struct PhysicsObject physics;
	double rot;    // rotation angle, more means counter-clockwise if viewed from above

	// -1 means e.g. back, 1 means e.g. forward, 0 means don't move in the given way
	int moving4ward;
	int movingup;
	int rotating;
};

void player_init(struct Player *plr);
void player_move(struct Player *plr);
struct DisplayCamera player_getcamera(const struct Player *plr);


#endif    // PLAYER_H
