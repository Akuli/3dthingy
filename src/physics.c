#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "floor.h"
#include "physics.h"
#include "vecmat.h"

static const struct Vec3 gravity = { 0.0, -6.0/PHYSICS_FPS, 0.0 };


bool physicsobject_move(struct PhysicsObject *po)
{
	po->location = vec3_add(po->location, vec3_mul_scalar(po->velocity, 1.0/PHYSICS_FPS));
	po->velocity = vec3_add(po->velocity, gravity);

	printf("velocity: %f %f %f\n", po->velocity.x, po->velocity.y, po->velocity.z);

	struct FloorInfo fi = floorinfo_get(po->location.x, po->location.z);
	if (po->location.y < fi.yvalue) {
		double friction = (fi.yvalue - po->location.y) * po->frictionness;
		assert(friction > 0);
		if (friction > 1)
			friction = 1;

		po->location.y = fi.yvalue;

		// remove all velocity except what goes in the direction of the floor
		struct Vec3 badcomponent = vec3_projection(po->velocity, floorinfo_perp(&fi));
		po->velocity = vec3_sub(po->velocity, badcomponent);

		// add friction
		po->velocity = vec3_mul_scalar(po->velocity, 1.0 - friction);

		return true;
	}
	return false;
}
