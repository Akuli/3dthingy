#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PI_FLOAT 3.14159265358979323846f

const char *argv0;


#define CALL_SDL(f, ...) do{ \
	if ((f)(__VA_ARGS__) < 0) { \
		fprintf(stderr, "%s: %s failed: %s\n", argv0, #f, SDL_GetError()); \
		abort(); \
	} \
} while(0)


#define NUMBER_OF_FUNNY_POINTS_Z_DIRECTION 10
#define VIEW_WIDTH_ANGLE 1.0f
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * SCREEN_HEIGHT / SCREEN_WIDTH)
#define FPS 60
#define PLAYER_SPEED 0.1f

struct Vec3 {
	float x;   // more means right
	float y;   // more means up
	float z;   // more means back, less means forward
};

struct ScreenPoint {
	// pixels
	int x;     // 0 means left, more means right
	int y;     // 0 means top, more means down
};


struct Player {
	struct Vec3 loc;
};


static inline struct Vec3 vec3_add(struct Vec3 a, struct Vec3 b)
{
	return (struct Vec3){ a.x+b.x, a.y+b.y, a.z+b.z };
}

static inline struct Vec3 vec3_neg(struct Vec3 a)
{
	return (struct Vec3){ -a.x, -a.y, -a.z };
}

static inline struct Vec3 vec3_sub(struct Vec3 a, struct Vec3 b)
{
	return vec3_add(a, vec3_neg(b));
}


// linear_map(3, 7, 0, 100, 4) == 25
static float linear_map(float srcmin, float srcmax, float dstmin, float dstmax, float val)
{
	float relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

// rel2plr should be relative to the player location
bool get_screen_location(struct Vec3 rel2plr, struct ScreenPoint *res)
{
	// examples: 0 means right, pi/2 means forward, pi means left, -pi means left, -pi/2 means back
	float xzangle = atan2f(-rel2plr.z, -rel2plr.x);

	// examples: pi means down, pi/2 means forward, 0 means up
	float yzangle = atan2f(-rel2plr.z, rel2plr.y);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < PI_FLOAT/2);
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < PI_FLOAT/2);

	float scrx = linear_map(PI_FLOAT/2 - VIEW_WIDTH_ANGLE, PI_FLOAT/2 + VIEW_WIDTH_ANGLE, 0, SCREEN_WIDTH, xzangle);
	float scry = linear_map(PI_FLOAT/2 - VIEW_HEIGHT_ANGLE, PI_FLOAT/2 + VIEW_HEIGHT_ANGLE, 0, SCREEN_HEIGHT, yzangle);

	res->x = (int)scrx;
	res->y = (int)scry;
	return (0 <= scrx && scrx < SCREEN_WIDTH &&
			0 <= scry && scry < SCREEN_HEIGHT);
}

void handle_key(struct Player *plr, SDL_Keysym k)
{
	switch(k.sym) {
	// TODO: figure out constants for arrow keys
	case 'a':
		plr->loc.x -= PLAYER_SPEED;
		break;
	case 'd':
		plr->loc.x += PLAYER_SPEED;
		break;
	case 'w':
		plr->loc.y += PLAYER_SPEED;
		break;
	case 's':
		plr->loc.y -= PLAYER_SPEED;
		break;
	case 'e':   // forward
		plr->loc.z -= PLAYER_SPEED;
		break;
	case 'q':   // back
		plr->loc.z += PLAYER_SPEED;
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	assert(linear_map(3, 7, 0, 100, 4) == 25);

	argv0 = argv[0];

	CALL_SDL(SDL_Init, SDL_INIT_VIDEO);

	SDL_Window *win;
	SDL_Renderer *rnd;
	CALL_SDL(SDL_CreateWindowAndRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, &win, &rnd);

	struct Player plr;
	plr.loc = (struct Vec3){0};

	struct Vec3 points[2*NUMBER_OF_FUNNY_POINTS_Z_DIRECTION];
	for (size_t i = 0; i < NUMBER_OF_FUNNY_POINTS_Z_DIRECTION; i++) {
		float z = -(float)i;
		points[2*i]     = (struct Vec3){ .x =  1.0, .y = -1.0, .z = z };
		points[2*i + 1] = (struct Vec3){ .x = -1.0, .y = -1.0, .z = z };
	}

	while (true) {
		uint32_t start = SDL_GetTicks();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch(evt.type) {
			case SDL_KEYDOWN:
				printf("key down\n");
				handle_key(&plr, evt.key.keysym);
				break;

			case SDL_QUIT:
				SDL_DestroyWindow(win);
				SDL_Quit();
				return 0;

			default:
				break;
			}
		}

		CALL_SDL(SDL_RenderClear, rnd);

		for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); i++) {
			struct ScreenPoint sp;
			if (get_screen_location(vec3_sub(points[i], plr.loc), &sp))
				CALL_SDL(SDL_RenderDrawLine, rnd, sp.x, sp.y, sp.x, sp.y+10);
		}

		SDL_RenderPresent(rnd);

		uint32_t sleep2 = start + (uint32_t)(1000/FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
