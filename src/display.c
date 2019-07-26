#include "display.h"
#include <stdbool.h>

bool display_pointisonscreen(struct DisplayPoint dp)
{
	int x = (int)dp.x;
	int y = (int)dp.y;
	return (0 <= x && x < DISPLAY_WIDTH &&
			0 <= y && y < DISPLAY_HEIGHT);
}


// finds intersection point of infinitely long lines a and b
static struct DisplayPoint intersect_lines(
	struct DisplayPoint astart, struct DisplayPoint aend,
	struct DisplayPoint bstart, struct DisplayPoint bend)
{
	// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
	double
		x1 = astart.x, y1 = astart.y,
		x2 = aend.x,   y2 = aend.y,
		x3 = bstart.x, y3 = bstart.y,
		x4 = bend.x,   y4 = bend.y;

	double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
	// TODO: handle the case where denom is very small
	return (struct DisplayPoint){
		( (x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) )/denom,
		( (x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) )/denom,
	};
}

static void fit_point_of_line_to_screen(struct DisplayPoint *pnt, struct DisplayPoint otherpnt)
{
	// e.g. tl = top left
	struct DisplayPoint tl = {0,0};
	struct DisplayPoint tr = {DISPLAY_WIDTH-1,0};
	struct DisplayPoint bl = {0,DISPLAY_HEIGHT-1};
	struct DisplayPoint br = {DISPLAY_WIDTH-1,DISPLAY_HEIGHT-1};

	if (pnt->x < 0)
		*pnt = intersect_lines(*pnt, otherpnt, tl, bl);
	if (pnt->x >= DISPLAY_WIDTH)
		*pnt = intersect_lines(*pnt, otherpnt, tr, br);
	if (pnt->y < 0)
		*pnt = intersect_lines(*pnt, otherpnt, tl, tr);
	if (pnt->y >= DISPLAY_HEIGHT)
		*pnt = intersect_lines(*pnt, otherpnt, bl, br);
}

void display_line(SDL_Renderer *rnd, struct DisplayPoint dp1, struct DisplayPoint dp2)
{
	bool scr1 = display_pointisonscreen(dp1);
	bool scr2 = display_pointisonscreen(dp2);

	if (!scr1 && !scr2)
		return;
	if (!scr1)
		fit_point_of_line_to_screen(&dp1, dp2);
	if (!scr2)
		fit_point_of_line_to_screen(&dp2, dp1);

	if (!display_pointisonscreen(dp1) || !display_pointisonscreen(dp2))
		return;

	SDL_RenderDrawLine(rnd, (int)dp1.x, (int)dp1.y, (int)dp2.x, (int)dp2.y);
}
