#include "display.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vecmat.h"

#define VIEW_WIDTH_SLOPE 1.5
#define VIEW_HEIGHT_SLOPE (VIEW_WIDTH_SLOPE * DISPLAY_HEIGHT / DISPLAY_WIDTH)

struct Point2D {
	double x;    // pixels, 0 means left, more means right
	double y;    // pixels, 0 means top, more means down
};

// maps x to slope*x + constant
struct LinearMap {
	double slope, constant;
};

// LinearMap "constructors" work so that (3, 7, 0, 100) maps 4 to 25

#define CREATE_LINEAR_MAP_COMPILE_TIME(SMIN, SMAX, DMIN, DMAX) { \
	.slope = ((DMAX)-(DMIN))/((SMAX)-(SMIN)), \
	.constant = (DMIN) - (SMIN)*((DMAX)-(DMIN))/((SMAX)-(SMIN)), \
}

static struct LinearMap
create_linear_map(double srcmin, double srcmax, double dstmin, double dstmax)
{
	return (struct LinearMap) CREATE_LINEAR_MAP_COMPILE_TIME(srcmin, srcmax, dstmin, dstmax);
}

// does da mapping
static inline double calculate_linear_map(struct LinearMap map, double val)
{
	return map.slope*val + map.constant;
}


struct DisplayBuf {
	char data[DISPLAY_HEIGHT][DISPLAY_WIDTH];

	// this is for drawing multiple points at once, helps with performance
	SDL_Point points[DISPLAY_WIDTH * DISPLAY_HEIGHT];
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
	memset(buf->data, 0, sizeof(buf->data));
}

void displaybuf_render(SDL_Renderer *rnd, struct DisplayBuf *buf)
{
	int n = 0;
	for (int x = 0; x < DISPLAY_WIDTH; x++)
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
			if (buf->data[y][x])
				buf->points[n++] = (SDL_Point){ x, y };

	SDL_RenderDrawPoints(rnd, buf->points, n);
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
	static const struct LinearMap viewmap_x = CREATE_LINEAR_MAP_COMPILE_TIME(
		-VIEW_WIDTH_SLOPE, VIEW_WIDTH_SLOPE,
		0, DISPLAY_WIDTH);
	static const struct LinearMap viewmap_y = CREATE_LINEAR_MAP_COMPILE_TIME(
		-VIEW_HEIGHT_SLOPE, VIEW_HEIGHT_SLOPE,
		0, DISPLAY_HEIGHT);

	struct Vec3 rel = mat3_mul_vec3(cam->world2player, vec3_sub(pnt, cam->location));

	if (rel.z > 0)   // object behind the camera
		return false;

	// positive means left, 0 means forward, negative means right
	double xzslope = rel.x / (-rel.z);

	// positive means down, 0 means forward, negative means up
	double yzslope = (-rel.y) / (-rel.z);

	ptr->x = calculate_linear_map(viewmap_x, xzslope);
	ptr->y = calculate_linear_map(viewmap_y, yzslope);
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
