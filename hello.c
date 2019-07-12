#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PI_DOUBLE 3.14159265358979323846

const char *argv0;


#define CALL_SDL(f, ...) do{ \
	if ((f)(__VA_ARGS__) < 0) { \
		fprintf(stderr, "%s: %s failed: %s\n", argv0, #f, SDL_GetError()); \
		abort(); \
	} \
} while(0)


#define GRID_SIZE 10
#define VIEW_WIDTH_ANGLE 1.0
#define VIEW_HEIGHT_ANGLE (VIEW_WIDTH_ANGLE * SCREEN_HEIGHT / SCREEN_WIDTH)
#define FPS 60
#define PLAYER_SPEED 0.1
#define ROTATION_SPEED 0.1

struct Vec3 {
	double x;   // more means right
	double y;   // more means up
	double z;   // more means back, less means forward
};

static struct Vec3 vec3_add(struct Vec3 a, struct Vec3 b)
{
	return (struct Vec3){ a.x+b.x, a.y+b.y, a.z+b.z };
}

static struct Vec3 vec3_add3(struct Vec3 a, struct Vec3 b, struct Vec3 c)
{
	return vec3_add(a, vec3_add(b, c));
}

static struct Vec3 vec3_neg(struct Vec3 a)
{
	return (struct Vec3){ -a.x, -a.y, -a.z };
}

static struct Vec3 vec3_sub(struct Vec3 a, struct Vec3 b)
{
	return vec3_add(a, vec3_neg(b));
}

static double vec3_dot(struct Vec3 a, struct Vec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

static double vec3_len(struct Vec3 v)
{
	return sqrt(vec3_dot(v, v));
}

static struct Vec3 vec3_mul_scalar(struct Vec3 v, double c)
{
	return (struct Vec3){ v.x*c, v.y*c, v.z*c };
}

static struct Vec3 vec3_unit(struct Vec3 v)
{
	return vec3_mul_scalar(v, 1.0f/vec3_len(v));
}

struct Mat3 {
	double
		a, b, c,
		d, e, f,
		g, h, i;
};

// returned matrix rotates in xz plane clockwise by an angle w (viewed from above)
struct Mat3 mat3_rotation_xz(double w)
{
	// exercise for you: derive this formula with intuition of sin,cos of small angles
	return (struct Mat3){
		cos(w),  0, sin(w),
		0,       1, 0,
		-sin(w), 0, cos(w),
	};
}

struct Mat3 mat3_mul_scalar(struct Mat3 m, double c)
{
	return (struct Mat3){
		m.a*c, m.b*c, m.c*c,
		m.d*c, m.e*c, m.f*c,
		m.g*c, m.h*c, m.i*c,
	};
}

static struct Vec3 mat3_mul_vec3(struct Mat3 m, struct Vec3 v)
{
	struct Vec3 xcol = { m.a, m.d, m.g };
	struct Vec3 ycol = { m.b, m.e, m.h };
	struct Vec3 zcol = { m.c, m.f, m.i };
	return vec3_add3(
		vec3_mul_scalar(xcol, v.x),
		vec3_mul_scalar(ycol, v.y),
		vec3_mul_scalar(zcol, v.z));
}

double mat3_det(struct Mat3 m)
{
	// https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
	double
		a=m.a, b=m.b, c=m.c,
		d=m.d, e=m.e, f=m.f,
		g=m.g, h=m.h, i=m.i;
	return a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
}

struct Mat3 mat3_inverse(struct Mat3 m)
{
	// https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
	double
		a=m.a, b=m.b, c=m.c,
		d=m.d, e=m.e, f=m.f,
		g=m.g, h=m.h, i=m.i;

	return mat3_mul_scalar(
		(struct Mat3){
			e*i-f*h, c*h-b*i, f*b-c*e,
			f*g-d*i, a*i-c*g, c*d-a*f,
			d*h-e*g, b*g-a*h, a*e-b*d,
		},
		1.0/mat3_det(m)
	);
}

struct ScreenPoint {
	// pixels
	double x;     // 0 means left, more means right
	double y;     // 0 means top, more means down
};

static bool screenpoint_isonscreen(struct ScreenPoint sp)
{
	return (0 <= (int)sp.x && (int)sp.x < SCREEN_WIDTH &&
			0 <= (int)sp.y && (int)sp.y < SCREEN_HEIGHT);
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
	double x1 = astart.x;
	double y1 = astart.y;
	double x2 = aend.x;
	double y2 = aend.y;
	double x3 = bstart.x;
	double y3 = bstart.y;
	double x4 = bend.x;
	double y4 = bend.y;

	double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
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

static void draw_line(SDL_Renderer *rnd, struct ScreenPoint sp1, struct ScreenPoint sp2)
{
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
	double rot;   // rotation angle, more means counter-clockwise if viewed from above, 0 is default
};

// linear_map(3, 7, 0, 100, 4) == 25
static double linear_map(double srcmin, double srcmax, double dstmin, double dstmax, double val)
{
	double relative = (val - srcmin) / (srcmax - srcmin);
	return dstmin + relative*(dstmax - dstmin);
}

// plrloc should be location of the player
static struct ScreenPoint vec3_to_screenpoint(struct Player plr, struct Vec3 pnt)
{
	struct Vec3 rel = vec3_sub(pnt, plr.loc);

	struct Mat3 antirotate = mat3_rotation_xz(-plr.rot);
	rel = mat3_mul_vec3(antirotate, rel);

	// examples: 0 means right, pi/2 means forward, pi means left, -pi means left, -pi/2 means back
	double xzangle = atan2(-rel.z, -rel.x);

	// examples: pi means down, pi/2 means forward, 0 means up
	double yzangle = atan2(-rel.z, rel.y);

	assert(0 < VIEW_WIDTH_ANGLE && VIEW_WIDTH_ANGLE < PI_DOUBLE/2);
	assert(0 < VIEW_HEIGHT_ANGLE && VIEW_HEIGHT_ANGLE < PI_DOUBLE/2);

	double x = linear_map(
		PI_DOUBLE/2 - VIEW_WIDTH_ANGLE, PI_DOUBLE/2 + VIEW_WIDTH_ANGLE,
		0, SCREEN_WIDTH,
		xzangle);
	double y = linear_map(
		PI_DOUBLE/2 - VIEW_HEIGHT_ANGLE, PI_DOUBLE/2 + VIEW_HEIGHT_ANGLE,
		0, SCREEN_HEIGHT,
		yzangle);

	return (struct ScreenPoint){ x, y };
}


void handle_key(struct Player *plr, SDL_Keysym k)
{
	struct Vec3 mv;

	switch(k.sym) {
	// TODO: figure out constants for arrow keys
	case 'w':     // "fly" higher
		mv = (struct Vec3){0,1,0};
		break;
	case 's':     // "land" lower
		mv = (struct Vec3){0,-1,0};
		break;
	case SDLK_UP:   // forward
		mv = (struct Vec3){0,0,-1};
		break;
	case SDLK_DOWN:   // back
		mv = (struct Vec3){0,0,1};
		break;
	case SDLK_LEFT:   // rotate left
		plr->rot += ROTATION_SPEED;
		return;
	case SDLK_RIGHT:   // rotate right
		plr->rot -= ROTATION_SPEED;
		return;
	default:
		return;
	}

	mv = vec3_mul_scalar(mv, PLAYER_SPEED);
	plr->loc = vec3_add(plr->loc, mat3_mul_vec3(mat3_rotation_xz(plr->rot), mv));
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
				(struct Vec3){ (double)x, 0, (double)z },
				(struct Vec3){ (double)(x+1), 0, (double)z },
			};
			lines[i++] = (struct Line){
				(struct Vec3){ (double)x, 0, (double)z },
				(struct Vec3){ (double)x, 0, (double)(z+1) },
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

		for (size_t i = 0; i < sizeof(lines)/sizeof(lines[0]); i++) {
			struct ScreenPoint sp1 = vec3_to_screenpoint(plr, lines[i].start);
			struct ScreenPoint sp2 = vec3_to_screenpoint(plr, lines[i].end);
			draw_line(rnd, sp1, sp2);
		}

		SDL_RenderPresent(rnd);

		uint32_t sleep2 = start + (uint32_t)(1000/FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
