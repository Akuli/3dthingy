#include "player.h"
#include <assert.h>
#include <math.h>
#include "display.h"
#include "vecmat.h"

#define VIEW_WIDTH_ANGLE 1.0
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * DISPLAY_HEIGHT / DISPLAY_WIDTH)

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


static double pi=0, tan_view_width_angle=0, tan_view_height_angle=0;

static void init_constants(void)
{
	if (pi != 0)
		return;

	pi = 4*atan(1.0);
	tan_view_width_angle = tan(VIEW_WIDTH_ANGLE);
	tan_view_height_angle = tan(VIEW_HEIGHT_ANGLE);
}

// linear_map(3, 7, 0, 100, 4) == 25
static double linear_map(double srcmin, double srcmax, double dstmin, double dstmax, double val)
{
	double relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

bool player_getdisplaypoint(struct Player plr, struct Vec3 pnt, struct DisplayPoint *ptr)
{
	init_constants();

	struct Vec3 rel = vec3_sub(pnt, vec3_add(plr.physics.location, player_to_camera));

	struct Mat3 antirotate = mat3_rotation_xz(-plr.rot);
	rel = mat3_mul_vec3(antirotate, rel);

	if (rel.z > 0)   // object behind the player
		return false;

	// positive means left, 0 means forward, negative means right
	double xzslope = rel.x / (-rel.z);

	// positive means down, 0 means forward, negative means up
	double yzslope = (-rel.y) / (-rel.z);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < pi/2);
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < pi/2);

	ptr->x = linear_map(
		-tan_view_width_angle, tan_view_width_angle,
		0, DISPLAY_WIDTH,
		xzslope);
	ptr->y = linear_map(
		-tan_view_height_angle, tan_view_height_angle,
		0, DISPLAY_HEIGHT,
		yzslope);
	return true;
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
