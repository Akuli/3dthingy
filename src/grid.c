#include <math.h>

#include "display.h"
#include "player.h"

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

static double f(double x, double z)
{
	//return 0.;
	//return exp(-(x*x + z*z));
	return sin(x) + sin(z);
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

			draw_line(rnd, plr, (struct Vec3){ x, f(x,z), z }, (struct Vec3){ xx, f(xx,z), z });
			draw_line(rnd, plr, (struct Vec3){ x, f(x,z), z }, (struct Vec3){ x, f(x,zz), zz });
		}
	}
}
