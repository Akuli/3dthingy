#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "grid.h"
#include "physics.h"
#include "player.h"
#include "vecmat.h"


static void handle_key(struct Player *plr, SDL_Keysym k, bool *changeflag)
{
	switch(k.sym) {
	case 'w':
		player_move(plr, PLAYERMOVE_HIGHER, 1);
		break;
	case 's':
		player_move(plr, PLAYERMOVE_HIGHER, -1);
		break;
	case SDLK_UP:
		player_move(plr, PLAYERMOVE_FORWARD, 1);
		break;
	case SDLK_DOWN:
		player_move(plr, PLAYERMOVE_FORWARD, -1);
		break;
	case SDLK_RIGHT:
		player_move(plr, PLAYERMOVE_ROTATE, 1);
		break;
	case SDLK_LEFT:
		player_move(plr, PLAYERMOVE_ROTATE, -1);
		break;
	default:
		return;
	}

	*changeflag = true;
}

static void draw_circle(SDL_Renderer *rnd, int centerx, int centery, int radius)
{
	for (int x = -radius; x < radius; x++) {
		int yminmax = (int) sqrt(radius*radius - x*x);
		for (int y = -yminmax + 1; y < yminmax; y++)
			SDL_RenderDrawPoint(rnd, centerx + x, centery + y);
	}
}

static void draw_physics_object_circle(
	SDL_Renderer *rnd, const struct Player *plr, const struct PhysicsObject *po)
{
	struct DisplayPoint dp;
	if (player_getdisplaypoint(*plr, po->location, &dp))
		draw_circle(rnd, (int)dp.x, (int)dp.y, 30);
}

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		abort();
	}

	SDL_Window *win;
	SDL_Renderer *rnd;
	if(SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, &win, &rnd) < 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
		abort();
	}

	struct Player plr;
	player_init(&plr);

	struct PhysicsObject po = {0};
	po.location.z = -3.0;
	po.location.y = 3.0;
	po.frictionness = 3.0;

	bool changeflag = true;

	while (true) {
		uint32_t start = SDL_GetTicks();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch(evt.type) {
			case SDL_KEYDOWN:
				handle_key(&plr, evt.key.keysym, &changeflag);
				break;

			case SDL_QUIT:
				SDL_DestroyWindow(win);
				SDL_Quit();
				return 0;

			default:
				break;
			}
		}

		// TODO: figure out how changeflag should be used with the physics stuff
		SDL_RenderClear(rnd);

		SDL_SetRenderDrawColor(rnd, 0xff, 0xff, 0xff, 0xff);
		grid_draw(rnd, &plr);    // slow, uses a lot of cpu

		SDL_SetRenderDrawColor(rnd, 0, 0xff, 0xff, 0xff);
		draw_physics_object_circle(rnd, &plr, &po);

		SDL_SetRenderDrawColor(rnd, 0, 0, 0, 0xff);
		SDL_RenderPresent(rnd);

		// this is here to make sure that stuff gets rendered when the program starts
		changeflag = false;

		physicsobject_move(&po);

		uint32_t sleep2 = start + (uint32_t)(1000/PHYSICS_FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
