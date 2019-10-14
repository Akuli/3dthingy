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


static void handle_key(struct Player *plr, SDL_Keysym k, bool keydown)
{
	switch(k.sym) {
	case 'w':
		plr->movingup = 1*keydown;
		break;
	case 's':
		plr->movingup = -1*keydown;
		break;
	case SDLK_UP:
		plr->moving4ward = 1*keydown;
		break;
	case SDLK_DOWN:
		plr->moving4ward = -1*keydown;
		break;
	case SDLK_RIGHT:
		plr->rotating = 1*keydown;
		break;
	case SDLK_LEFT:
		plr->rotating = -1*keydown;
		break;
	default:
		return;
	}
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

	struct DisplayBuf *gridbuf = displaybuf_new();

	while (true) {
		uint32_t start = SDL_GetTicks();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch(evt.type) {
			case SDL_KEYDOWN:
				handle_key(&plr, evt.key.keysym, true);
				break;
			case SDL_KEYUP:
				handle_key(&plr, evt.key.keysym, false);
				break;

			case SDL_QUIT:
				displaybuf_free(gridbuf);
				SDL_DestroyWindow(win);
				SDL_DestroyRenderer(rnd);
				SDL_Quit();
				return 0;

			default:
				break;
			}
		}

		struct DisplayCamera cam = player_getcamera(&plr);

		SDL_RenderClear(rnd);

		SDL_SetRenderDrawColor(rnd, 0xff, 0xff, 0xff, 0xff);
		displaybuf_clear(gridbuf);
		grid_draw(gridbuf, &cam);   // this is the performance bottleneck
		displaybuf_render(rnd, gridbuf);

		SDL_SetRenderDrawColor(rnd, 0, 0, 0, 0xff);
		SDL_RenderPresent(rnd);

		player_move(&plr);

		uint32_t sleep2 = start + (uint32_t)(1000/PHYSICS_FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
