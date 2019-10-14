#include <math.h>

#include "vecmat.h"
#include "display.h"


// make sure that the partial derivatives are right!
double floor_y(double x, double z) {              return sin(x)+sin(z); }
static double partial_wrt_z(double x, double z) { return        cos(z); }
static double partial_wrt_x(double x, double z) { return cos(x)       ; }
//double floor_y(double x, double z) { return exp(-(x*x+z*z)); }
//static double partial_wrt_z(double x, double z) { return -2*z*exp(-(x*x+z*z)); }
//static double partial_wrt_x(double x, double z) { return -2*x*exp(-(x*x+z*z)); }

struct Vec3 floor_perp_vector(double x, double z)
{
	// create two vectors that go along the surface of the floor
	struct Vec3 a = { 0, partial_wrt_z(x, z), 1 };
	struct Vec3 b = { 1, partial_wrt_x(x, z), 0 };

	// cross product is perpendicular
	return vec3_cross(a, b);
}


#define SIZE 8
#define LINES_PER_UNIT 10

void floor_draw(struct DisplayBuf *buf, const struct DisplayCamera *cam)
{
	int sz = SIZE * LINES_PER_UNIT;
	for (int rawx = -sz; rawx < sz; rawx++) {
		for (int rawz = -sz; rawz < sz; rawz++) {
			double x = rawx, z = rawz, xx = rawx+1, zz = rawz+1;
			x /= LINES_PER_UNIT;
			z /= LINES_PER_UNIT;
			xx /= LINES_PER_UNIT;
			zz /= LINES_PER_UNIT;

			// TODO: cache floor_y values?
			struct Vec3 point   = { x, floor_y(x,z), z };
			struct Vec3 xxpoint = { xx, floor_y(xx,z), z };
			struct Vec3 zzpoint = { x, floor_y(x,zz), zz };

			display_line(buf, cam, point, xxpoint);
			display_line(buf, cam, point, zzpoint);

			// uncomment to see perpendicular vectors
			// useful for checking that you got derivatives right in floor.c
			//display_line(buf, cam, point, vec3_add(point, vec3_mul_scalar(floor_perp_vector(x, z), 0.1)));
		}
	}
}
