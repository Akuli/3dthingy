#ifndef PLAYER_H
#define PLAYER_H

#include "display.h"
#include "vecmat.h"

struct Player {
	struct Vec3 loc;
	double rot;    // rotation angle, more means counter-clockwise if viewed from above
};

void player_init(struct Player *plr);

// figure out where the player sees the given point
// returns false when the point is on the other side of the player and should just be completely ignored
bool player_getdisplaypoint(struct Player plr, struct Vec3 pnt, struct DisplayPoint *ptr);

enum PlayerMove {
	PLAYERMOVE_FORWARD,
	PLAYERMOVE_ROTATE,
	PLAYERMOVE_HIGHER,
};

// dir should be 1 or -1 to do the move as is or reversed (i.e. back instead of forward)
void player_move(struct Player *plr, enum PlayerMove mv, int dir);


#endif    // PLAYER_H
