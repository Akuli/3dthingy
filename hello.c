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


#define GRID_SIZE 10
#define VIEW_WIDTH_ANGLE 1.0f
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * SCREEN_HEIGHT / SCREEN_WIDTH)
#define FPS 60
#define PLAYER_SPEED 0.1f

struct Vec3 {
	float x;   // more means right
	float y;   // more means up
	float z;   // more means back, less means forward
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

struct ScreenPoint {
	// pixels
	float x;     // 0 means left, more means right
	float y;     // 0 means top, more means down
};

static bool screenpoint_isonscreen(struct ScreenPoint sp)
{
	return (0 <= (int)sp.x && (int)sp.x < SCREEN_WIDTH &&
			0 <= (int)sp.y && (int)sp.y < SCREEN_HEIGHT);
}


// linear_map(3, 7, 0, 100, 4) == 25
static float linear_map(float srcmin, float srcmax, float dstmin, float dstmax, float val)
{
	float relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

// plrloc should be location of the player
struct ScreenPoint vec3_to_screenpoint(struct Vec3 plrloc, struct Vec3 pnt)
{
	struct Vec3 rel = vec3_sub(pnt, plrloc);

	// examples: 0 means right, pi/2 means forward, pi means left, -pi means left, -pi/2 means back
	float xzangle = atan2f(-rel.z, -rel.x);

	// examples: pi means down, pi/2 means forward, 0 means up
	float yzangle = atan2f(-rel.z, rel.y);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < PI_FLOAT/2);
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < PI_FLOAT/2);

	float x = linear_map(
		PI_FLOAT/2 - VIEW_WIDTH_ANGLE, PI_FLOAT/2 + VIEW_WIDTH_ANGLE,
		0, SCREEN_WIDTH,
		xzangle);
	float y = linear_map(
		PI_FLOAT/2 - VIEW_HEIGHT_ANGLE, PI_FLOAT/2 + VIEW_HEIGHT_ANGLE,
		0, SCREEN_HEIGHT,
		yzangle);

	return (struct ScreenPoint){ x, y };
}


struct Line {
	struct Vec3 start;
	struct Vec3 end;
};

// finds intersections of infinitely long lines a and b
struct ScreenPoint intersect_lines(
	struct ScreenPoint astart, struct ScreenPoint aend,
	struct ScreenPoint bstart, struct ScreenPoint bend)
{
	// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
	float x1 = astart.x;
	float y1 = astart.y;
	float x2 = aend.x;
	float y2 = aend.y;
	float x3 = bstart.x;
	float y3 = bstart.y;
	float x4 = bend.x;
	float y4 = bend.y;

	float denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
	assert(fabs(denom) > 0.001);   // TODO
	return (struct ScreenPoint){
		( (x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) )/denom,
		( (x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) )/denom,
	};
}

static bool fit_point_of_line_to_screen(struct ScreenPoint *pnt, struct ScreenPoint otherpnt)
{
	// e.g. tl = top left
	struct ScreenPoint tl = {0,0};
	struct ScreenPoint tr = {SCREEN_WIDTH-1,0};
	struct ScreenPoint bl = {0,SCREEN_HEIGHT-1};
	struct ScreenPoint br = {SCREEN_WIDTH-1,SCREEN_HEIGHT-1};

	if (pnt->x < 0)
		*pnt = intersect_lines(*pnt, otherpnt, tl, bl);
	if (pnt->x >= SCREEN_WIDTH)
		*pnt = intersect_lines(*pnt, otherpnt, tr, br);
	if (pnt->y < 0)
		*pnt = intersect_lines(*pnt, otherpnt, tl, tr);
	if (pnt->y >= SCREEN_HEIGHT)
		*pnt = intersect_lines(*pnt, otherpnt, bl, br);

	// FIXME: prevent the weird thing from happening correctly instead of this hack
	return (pnt->y > SCREEN_HEIGHT/4);
}

static void line_draw(struct Vec3 plrloc, SDL_Renderer *rnd, struct Line ln)
{
	struct ScreenPoint sp1 = vec3_to_screenpoint(plrloc, ln.start);
	struct ScreenPoint sp2 = vec3_to_screenpoint(plrloc, ln.end);

	bool scr1 = screenpoint_isonscreen(sp1);
	bool scr2 = screenpoint_isonscreen(sp2);

	if (!scr1 && !scr2)
		return;
	if (!scr1 && !fit_point_of_line_to_screen(&sp1, sp2))
		return;
	if (!scr2 && !fit_point_of_line_to_screen(&sp2, sp1))
		return;

	if (!screenpoint_isonscreen(sp1) || !screenpoint_isonscreen(sp2))
		return;

	SDL_RenderDrawLine(rnd, (int)sp1.x, (int)sp1.y, (int)sp2.x, (int)sp2.y);
}


struct Player {
	struct Vec3 loc;
};

void handle_key(struct Player *plr, SDL_Keysym k)
{
	switch(k.sym) {
	// TODO: figure out constants for arrow keys
	case SDLK_LEFT:
		plr->loc.x -= PLAYER_SPEED;
		break;
	case SDLK_RIGHT:
		plr->loc.x += PLAYER_SPEED;
		break;
	case SDLK_UP:
		plr->loc.y += PLAYER_SPEED;
		break;
	case SDLK_DOWN:
		plr->loc.y -= PLAYER_SPEED;
		break;
	case 'f':   // forward
		plr->loc.z -= PLAYER_SPEED;
		break;
	case 'b':   // back
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

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		abort();
	}

	SDL_Window *win;
	SDL_Renderer *rnd;
	if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &win, &rnd) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		abort();
	}

	struct Player plr;
	plr.loc = (struct Vec3){0.0, 0.5, 0.0};

	struct Line lines[2*(2*GRID_SIZE)*(2*GRID_SIZE)];
	size_t i = 0;
	for (int x = -GRID_SIZE; x < GRID_SIZE; x++) {
		for (int z = -GRID_SIZE; z < GRID_SIZE; z++) {
			lines[i++] = (struct Line){
				(struct Vec3){ (float)x, 0, (float)z },
				(struct Vec3){ (float)(x+1), 0, (float)z },
			};
			lines[i++] = (struct Line){
				(struct Vec3){ (float)x, 0, (float)z },
				(struct Vec3){ (float)x, 0, (float)(z+1) },
			};
		}
	}

	while (true) {
		uint32_t start = SDL_GetTicks();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch(evt.type) {
			case SDL_KEYDOWN:
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

		SDL_RenderClear(rnd);

		for (size_t i = 0; i < sizeof(lines)/sizeof(lines[0]); i++)
			line_draw(plr.loc, rnd, lines[i]);

		SDL_RenderPresent(rnd);

		uint32_t sleep2 = start + (uint32_t)(1000/FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
