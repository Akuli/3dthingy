#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PI_FLOAT 3.14159265358979323846f

const char *argv0;


static void fatal_sdl_error(const char *funcname)
{
	fprintf(stderr, "%s: %s failed: %s\n", argv0, funcname, SDL_GetError());
	abort();
}


#define NUMBER_OF_FUNNY_POINTS_Z_DIRECTION 10
#define VIEW_WIDTH_ANGLE 1.0f

struct GamePoint {
	float x;   // more means right
	float y;   // more means up
	float z;   // more means back, less means forward
};

struct ScreenPoint {
	// pixels
	unsigned int x;
	unsigned int y;
};

// linear_map(3, 7, 0, 100, 4) == 25
static float linear_map(float srcmin, float srcmax, float dstmin, float dstmax, float val)
{
	float relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

bool get_screen_location(struct GamePoint p, struct ScreenPoint *res)
{
	// example angles: 0 means right, pi/2 means forward, pi means left, -pi means left, -pi/2 means back
	float angle = atan2f(-p.z, p.x);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < PI_FLOAT/2);
	float x = linear_map(PI_FLOAT/2 - VIEW_WIDTH_ANGLE, PI_FLOAT/2 + VIEW_WIDTH_ANGLE, 0, SCREEN_WIDTH, angle);
	if (!(0 <= x && x < SCREEN_WIDTH))
		return false;

	res->x = (unsigned int)x;
	// TODO: y
	return true;
}

int main(int argc, char **argv)
{
	printf("hello world %f\n", linear_map(3, 7, 0, 100, 4));
	argv0 = argv[0];

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal_sdl_error("SDL_Init");

	SDL_Window *win = SDL_CreateWindow("hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!win)
		fatal_sdl_error("SDL_CreateWindow");

	SDL_Surface *sur = SDL_GetWindowSurface(win);
	SDL_FillRect(sur, NULL, SDL_MapRGB(sur->format, 0xff, 0, 0xff));
	SDL_UpdateWindowSurface(win);

	struct GamePoint points[2*NUMBER_OF_FUNNY_POINTS_Z_DIRECTION];
	for (size_t i = 0; i < NUMBER_OF_FUNNY_POINTS_Z_DIRECTION; i++) {
		float z = -(float)i;
		points[2*i]     = (struct GamePoint){ .x =  1.0, .y = 0, .z = z };
		points[2*i + 1] = (struct GamePoint){ .x = -1.0, .y = 0, .z = z };
	}

	for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); i++) {
		struct ScreenPoint sp;
		if (get_screen_location(points[i], &sp)) {
			for (unsigned int i = 0; i < sp.x; i += 10)
				putchar(' ');
			putchar('|');
			putchar('\n');
		}
	}

	//SDL_Delay(2000);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
