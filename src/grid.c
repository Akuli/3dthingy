#include "display.h"
#include "player.h"
#include "graph.h"
#include "grid.h"

#define SIZE 8
#define LINES_PER_UNIT 10


static void draw_line(struct SDL_Renderer *rnd, const struct Player *plr,
		struct Vec3 start, struct Vec3 end)
{
	struct DisplayPoint dp1, dp2;
	if (player_getdisplaypoint(*plr, start, &dp1) &&
		player_getdisplaypoint(*plr, end, &dp2))
	{
		display_line(rnd, dp1, dp2);
	}
}

void grid_draw(struct SDL_Renderer *rnd, const struct Player *plr)
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

			draw_line(rnd, plr, point, xxpoint);
			draw_line(rnd, plr, point, zzpoint);

			// uncomment to see perpendicular vectors
			// useful for checking that you got derivatives right in graph.c
			//draw_line(rnd, plr, point, vec3_add(point, vec3_mul_scalar(graph_perp_vector(x, z), 0.1)));
		}
	}
}
