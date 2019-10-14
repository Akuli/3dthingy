// the 3d function graph being presented in the program

#ifndef FLOOR_H
#define FLOOR_H

#include "vecmat.h"
#include "display.h"

struct FloorInfo {
	// how high is it
	double yvalue;

	// partial derivatives
	double xpartial;
	double zpartial;
};

struct FloorInfo floorinfo_get(double x, double z);

// returns a vector perpendicular to the floor, of undefined length
struct Vec3 floorinfo_perp(const struct FloorInfo *info);

// draws a grid of a part of the floor
void floor_draw(struct DisplayBuf *buf, const struct DisplayCamera *cam);


#endif   // FLOOR_H
