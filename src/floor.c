#include <math.h>
#include <stdbool.h>

#include "vecmat.h"
#include "display.h"
#include "floor.h"

#define SIZE 8
#define LINES_PER_UNIT 10


struct FloorInfo floorinfo_get(double x, double z)
{
	return (struct FloorInfo) {
		// sine wave valley
		.yvalue   = sin(x)+sin(z),
		.xpartial = cos(x)+0     ,
		.zpartial = 0     +cos(z),

		// gauss exponential function peak thingy
		/*
		.yvalue = exp(-x*x-z*z),
		.xpartial = -2*x*exp(-x*x-z*z),
		.zpartial = -2*z*exp(-x*x-z*z),
		*/

		// a rotation symmetric version of sin(1/x^2)
		/*
		.yvalue = sin(1/(x*x + z*z)),
		.xpartial = cos(1/(x*x + z*z)) * -1/pow(x*x + z*z, 2) * 2*x,
		.zpartial = cos(1/(x*x + z*z)) * -1/pow(x*x + z*z, 2) * 2*z,
		*/

		// something that i couldn't differentiate by hand, funny spiral
		/*
		.yvalue = cos(3*hypot(x,z) + atan2(x,z)),
		.xpartial = -(3*x/hypot(x, z) + z/(x*x + z*z))*sin(3*hypot(x,z) + atan2(x, z)),
		.zpartial = -(3*z/hypot(x, z) + x/(x*x + z*z))*sin(3*hypot(x,z) + atan2(x, z)),
		*/
	};
}


//double floor_y(double x, double z) { return cos(3*hypot(x,z) + atan2(x,z)); }


struct Vec3 floorinfo_perp(const struct FloorInfo *fi)
{
	// create two vectors that go along the surface of the floor
	struct Vec3 a = { 0, fi->zpartial, 1 };
	struct Vec3 b = { 1, fi->xpartial, 0 };

	// cross product is perpendicular
	return vec3_cross(a, b);
}


static double y_table[2*SIZE*LINES_PER_UNIT + 1][2*SIZE*LINES_PER_UNIT + 1];
static bool y_table_ready = false;

static void init_value_table(void)
{
	int sz = SIZE * LINES_PER_UNIT;
	for (int rawx = -sz; rawx < sz; rawx++) {
		for (int rawz = -sz; rawz < sz; rawz++) {
			y_table[rawx + sz][rawz + sz] = floorinfo_get(
				rawx / (double)LINES_PER_UNIT,
				rawz / (double)LINES_PER_UNIT
			).yvalue;
		}
	}
}

void floor_draw(struct DisplayBuf *buf, const struct DisplayCamera *cam)
{

	if (!y_table_ready) {
		init_value_table();
		y_table_ready = true;
	}

	int sz = SIZE * LINES_PER_UNIT;
	for (int rawx = -sz; rawx < sz; rawx++) {
		for (int rawz = -sz; rawz < sz; rawz++) {
			double x = rawx, z = rawz, xx = rawx+1, zz = rawz+1;
			x /= LINES_PER_UNIT;
			z /= LINES_PER_UNIT;
			xx /= LINES_PER_UNIT;
			zz /= LINES_PER_UNIT;

			struct Vec3 point   = { x, y_table[rawx + sz][rawz + sz], z };
			struct Vec3 xxpoint = { xx, y_table[rawx + sz + 1][rawz + sz], z };
			struct Vec3 zzpoint = { x, y_table[rawx + sz][rawz + sz + 1], zz };

			display_line(buf, cam, point, xxpoint);
			display_line(buf, cam, point, zzpoint);

			// uncomment to see perpendicular vectors
			// useful for checking that you got derivatives right in floor.c
			//display_line(buf, cam, point, vec3_add(point, vec3_mul_scalar(vec3_unit(floor_perp_vector(x, z)), 0.1)));
		}
	}
}
