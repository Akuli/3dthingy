#include "player.h"
#include <assert.h>
#include <math.h>
#include "display.h"
#include "vecmat.h"

#define MOVING_SPEED 10
#define ROTATION_SPEED 3

static const struct Vec3 player_to_camera = { 0.0, 0.5, 0.0 };

void player_init(struct Player *plr)
{
	memset(plr, 0, sizeof *plr);
	plr->physics.location = (struct Vec3){ 3.0, 0.5, 4.0 };
	plr->physics.frictionness = 15.0;
	plr->rot = 0;
}


void player_move(struct Player *plr)
{
	bool touchesground = physicsobject_move(&plr->physics);

	plr->rot -= plr->rotating * ROTATION_SPEED / (double)PHYSICS_FPS;

	// it is touching the graph
	struct Vec3 diff = {
		0,
		plr->movingup * MOVING_SPEED / (double)PHYSICS_FPS,
		touchesground*( -plr->moving4ward * MOVING_SPEED / (double)PHYSICS_FPS ),
	};
	struct Mat3 rotate = mat3_rotation_xz(plr->rot);
	plr->physics.velocity = vec3_add(plr->physics.velocity, mat3_mul_vec3(rotate, diff));
}

struct DisplayCamera player_getcamera(const struct Player *plr)
{
	return (struct DisplayCamera){
		.location = vec3_add(plr->physics.location, player_to_camera),
		.world2player = mat3_rotation_xz(-plr->rot),
	};
}
