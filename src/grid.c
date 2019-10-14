#include "display.h"
#include "player.h"
#include "graph.h"
#include "grid.h"

#define SIZE 8
#define LINES_PER_UNIT 10

void grid_draw(struct DisplayBuf *buf, const struct DisplayCamera *cam)
{
	int sz = SIZE * LINES_PER_UNIT;
	for (int rawx = -sz; rawx < sz; rawx++) {
		for (int rawz = -sz; rawz < sz; rawz++) {
			double x = rawx, z = rawz, xx = rawx+1, zz = rawz+1;
			x /= LINES_PER_UNIT;
			z /= LINES_PER_UNIT;
			xx /= LINES_PER_UNIT;
			zz /= LINES_PER_UNIT;

			// TODO: cache graph_y values?
			struct Vec3 point   = { x, graph_y(x,z), z };
			struct Vec3 xxpoint = { xx, graph_y(xx,z), z };
			struct Vec3 zzpoint = { x, graph_y(x,zz), zz };

			display_line(buf, cam, point, xxpoint);
			display_line(buf, cam, point, zzpoint);

			// uncomment to see perpendicular vectors
			// useful for checking that you got derivatives right in graph.c
			//display_line(buf, cam, point, vec3_add(point, vec3_mul_scalar(graph_perp_vector(x, z), 0.1)));
		}
	}
}
