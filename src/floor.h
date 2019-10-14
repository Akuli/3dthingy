// the 3d function graph being presented in the program

#ifndef FLOOR_H
#define FLOOR_H

#include "vecmat.h"
#include "display.h"

// returns height of floor
double floor_y(double x, double z);

// returns a vector perpendicular to the floor at (x, floor_y(x,z), z)
struct Vec3 floor_perp_vector(double x, double z);

void floor_draw(struct DisplayBuf *buf, struct DisplayCamera *cam);


#endif   // FLOOR_H
