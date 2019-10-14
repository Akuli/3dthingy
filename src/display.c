#include "display.h"
#include <stdbool.h>
#include <stdlib.h>

struct DisplayBuf {
	char data[DISPLAY_HEIGHT][DISPLAY_WIDTH];
};

struct DisplayBuf *displaybuf_new(void)
{
	struct DisplayBuf *buf = malloc(sizeof(struct DisplayBuf));
	if (!buf) {
		fprintf(stderr, "not enough memory\n");
		abort();
	}
	return buf;
}

void displaybuf_free(struct DisplayBuf *buf)
{
	free(buf);
}

void displaybuf_clear(struct DisplayBuf *buf)
{
	memset(buf->data, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
}


// not needed currently, but uncomment if you need this

/*
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
*/


static void swap(struct DisplayPoint *dp1, struct DisplayPoint *dp2)
{
	struct DisplayPoint tmp = *dp1;
	*dp1 = *dp2;
	*dp2 = tmp;
}

// TODO: don't copy/pasta this between files
// linear_map(3, 7, 0, 100, 4) == 25
static double linear_map(double srcmin, double srcmax, double dstmin, double dstmax, double val)
{
	double relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

#define max(a, b) ((a)>(b) ? (a) : (b))

void displaybuf_draw_line(struct DisplayBuf *buf, struct DisplayPoint dp1, struct DisplayPoint dp2)
{
	// avoid division by zero
	if (dp1.x == dp2.x && dp1.y == dp2.y)
		return;

	if (fabs(dp1.x - dp2.x) > fabs(dp1.y - dp2.y)) {
		if (dp1.x > dp2.x)
			swap(&dp1, &dp2);
		for (int x = max(0, (int)dp1.x); x < (int)dp2.x && x < DISPLAY_WIDTH; x++) {
			int y = (int) linear_map(dp1.x, dp2.x, dp1.y, dp2.y, x);
			if (0 <= y && y < DISPLAY_HEIGHT)
				buf->data[y][x] = 1;
		}
	} else {
		if (dp1.y > dp2.y)
			swap(&dp1, &dp2);
		for (int y = max(0, (int)dp1.y); y < (int)dp2.y && y < DISPLAY_HEIGHT; y++) {
			int x = (int) linear_map(dp1.y, dp2.y, dp1.x, dp2.x, y);
			if (0 <= x && x < DISPLAY_WIDTH)
				buf->data[y][x] = 1;
		}
	}
}


void displaybuf_render(SDL_Renderer *rnd, const struct DisplayBuf *buf)
{
	for (int x = 0; x < DISPLAY_WIDTH; x++)
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
			if (buf->data[y][x])
				SDL_RenderDrawPoint(rnd, x, y);
}
