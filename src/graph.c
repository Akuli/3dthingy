#include <math.h>

#include "graph.h"
#include "vecmat.h"


// make sure that the partial derivatives are right!
double graph_y(double x, double z) {              return sin(x)*sin(z); }
static double partial_wrt_z(double x, double z) { return sin(x)*cos(z); }
static double partial_wrt_x(double x, double z) { return cos(x)*sin(z); }

struct Vec3 graph_perp_vector(double x, double z)
{
	// create two vectors that go along the surface of the graph
	struct Vec3 a = { 0, partial_wrt_z(x, z), 1 };
	struct Vec3 b = { 1, partial_wrt_x(x, z), 0 };

	// cross product is perpendicular
	return vec3_cross(a, b);
}
