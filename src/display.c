#include "display.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vecmat.h"

#define VIEW_WIDTH_ANGLE 1.0
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * DISPLAY_HEIGHT / DISPLAY_WIDTH)

struct Point2D {
	double x;    // pixels, 0 means left, more means right
	double y;    // pixels, 0 means top, more means down
};

struct DisplayBuf {
	char data[DISPLAY_HEIGHT][DISPLAY_WIDTH];
};

// maps x to slope*x + constant
struct LinearMap {
	double slope, constant;
};

// create_linear_map(3, 7, 0, 100) maps 4 to 25
static struct LinearMap
create_linear_map(double srcmin, double srcmax, double dstmin, double dstmax)
{
	double slope = (dstmax - dstmin)/(srcmax - srcmin);
	double constant = dstmin - srcmin*slope;
	return (struct LinearMap){ slope, constant };
}

// does da mapping
static inline double calculate_linear_map(struct LinearMap map, double val)
{
	return map.slope*val + map.constant;
}


// linear_map(3, 7, 0, 100, 4) == 25
static double linear_map(double srcmin, double srcmax, double dstmin, double dstmax, double val)
{
	return calculate_linear_map(create_linear_map(srcmin, srcmax, dstmin, dstmax), val);
}


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
	memset(buf, 0, sizeof *buf);
}

void displaybuf_render(SDL_Renderer *rnd, const struct DisplayBuf *buf)
{
	for (int x = 0; x < DISPLAY_WIDTH; x++)
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
			if (buf->data[y][x])
				SDL_RenderDrawPoint(rnd, x, y);
}


static void swap(struct Point2D *dp1, struct Point2D *dp2)
{
	struct Point2D tmp = *dp1;
	*dp1 = *dp2;
	*dp2 = tmp;
}

#define max(a, b) ((a)>(b) ? (a) : (b))

// this is faster than just using sdl renderer, but still a performance bottleneck
static void draw_2d_line(struct DisplayBuf *buf, struct Point2D dp1, struct Point2D dp2)
{
	// avoid division by zero
	if (dp1.x == dp2.x && dp1.y == dp2.y)
		return;

	if (fabs(dp1.x - dp2.x) > fabs(dp1.y - dp2.y)) {
		if (dp1.x > dp2.x)
			swap(&dp1, &dp2);

		struct LinearMap lin = create_linear_map(dp1.x, dp2.x, dp1.y, dp2.y);
		for (int x = max(0, (int)dp1.x); x < (int)dp2.x && x < DISPLAY_WIDTH; x++) {
			int y = (int) calculate_linear_map(lin, x);
			if (0 <= y && y < DISPLAY_HEIGHT)
				buf->data[y][x] = 1;
		}
	} else {
		if (dp1.y > dp2.y)
			swap(&dp1, &dp2);

		struct LinearMap lin = create_linear_map(dp1.y, dp2.y, dp1.x, dp2.x);
		for (int y = max(0, (int)dp1.y); y < (int)dp2.y && y < DISPLAY_HEIGHT; y++) {
			int x = (int) calculate_linear_map(lin, y);
			if (0 <= x && x < DISPLAY_WIDTH)
				buf->data[y][x] = 1;
		}
	}
}


static bool point_to_2d(
	const struct DisplayCamera *cam, struct Vec3 pnt, struct Point2D *ptr)
{
	struct Vec3 rel = mat3_mul_vec3(cam->world2player, vec3_sub(pnt, cam->location));

	if (rel.z > 0)   // object behind the camera
		return false;

	// positive means left, 0 means forward, negative means right
	double xzslope = rel.x / (-rel.z);

	// positive means down, 0 means forward, negative means up
	double yzslope = (-rel.y) / (-rel.z);

	// asin(1) = pi/2 = 1/4 of a turn
	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < asin(1));
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < asin(1));

	ptr->x = linear_map(
		-tan(VIEW_WIDTH_ANGLE), tan(VIEW_WIDTH_ANGLE),
		0, DISPLAY_WIDTH,
		xzslope);
	ptr->y = linear_map(
		-tan(VIEW_HEIGHT_ANGLE), tan(VIEW_HEIGHT_ANGLE),
		0, DISPLAY_HEIGHT,
		yzslope);
	return true;
}

void display_line(
	struct DisplayBuf *buf, const struct DisplayCamera *cam,
	struct Vec3 start, struct Vec3 end)
{
	struct Point2D start2d, end2d;
	if (point_to_2d(cam, start, &start2d) && point_to_2d(cam, end, &end2d))
		draw_2d_line(buf, start2d, end2d);
}
