#ifndef PHYSICS_H
#define PHYSICS_H

// this is needed for physics because changing this must not affect how fast objects move
#define PHYSICS_FPS 30

struct PhysicsObject {
	// no acceleration because currently all objects are affected by gravity and nothing else
	struct Vec3 location, velocity;

	// bigger value means more friction, 0 means no friction
	double frictionness;
};

void physicsobject_move(struct PhysicsObject *po);


#endif   // PHYSICS_H
