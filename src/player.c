#include "player.h"
#include <assert.h>
#include <math.h>
#include "display.h"
#include "vecmat.h"

#define VIEW_WIDTH_ANGLE 1.0
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * DISPLAY_HEIGHT / DISPLAY_WIDTH)

#define MOVING_SPEED 0.1
#define ROTATION_SPEED 0.1

void player_init(struct Player *plr)
{
	plr->loc = (struct Vec3){0.0, 0.5, 0.0};
	plr->rot = 0;
}


static double pi = 0;

// linear_map(3, 7, 0, 100, 4) == 25
static double linear_map(double srcmin, double srcmax, double dstmin, double dstmax, double val)
{
	double relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

struct DisplayPoint player_getdisplaypoint(struct Player plr, struct Vec3 pnt)
{
	if (pi == 0)
		pi = 4*atan(1.0);

	struct Vec3 rel = vec3_sub(pnt, plr.loc);

	struct Mat3 antirotate = mat3_rotation_xz(-plr.rot);
	rel = mat3_mul_vec3(antirotate, rel);

	// examples: 0 means right, pi/2 means forward, pi means left, -pi means left, -pi/2 means back
	double xzangle = atan2(-rel.z, -rel.x);

	// examples: pi means down, pi/2 means forward, 0 means up
	double yzangle = atan2(-rel.z, rel.y);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < pi/2);
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < pi/2);

	double x = linear_map(
		pi/2 - VIEW_WIDTH_ANGLE, pi/2 + VIEW_WIDTH_ANGLE,
		0, DISPLAY_WIDTH,
		xzangle);
	double y = linear_map(
		pi/2 - VIEW_HEIGHT_ANGLE, pi/2 + VIEW_HEIGHT_ANGLE,
		0, DISPLAY_HEIGHT,
		yzangle);

	return (struct DisplayPoint){ x, y };
}

void player_move(struct Player *plr, enum PlayerMove mv, int dir)
{
	assert(dir==1 || dir==-1);
	struct Vec3 diff;

	switch(mv) {
	case PLAYERMOVE_FORWARD:
		diff = (struct Vec3){0,0,-dir*MOVING_SPEED};
		break;
	case PLAYERMOVE_HIGHER:
		diff = (struct Vec3){0,dir*MOVING_SPEED,0};
		break;
	case PLAYERMOVE_ROTATE:
		plr->rot -= dir*ROTATION_SPEED;
		return;   // skip diff stuff
	}

	plr->loc = vec3_add(plr->loc, mat3_mul_vec3(mat3_rotation_xz(plr->rot), diff));
}
