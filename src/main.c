#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "grid.h"
#include "player.h"
#include "vecmat.h"

#define FPS 60


void handle_key(struct Player *plr, SDL_Keysym k)
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
}

struct Line {
	struct Vec3 start;
	struct Vec3 end;
};

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		abort();
	}

	SDL_Window *win;
	SDL_Renderer *rnd;
	if(SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, &win, &rnd) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		abort();
	}

	struct Player plr;
	player_init(&plr);

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
		grid_draw(rnd, &plr);
		SDL_RenderPresent(rnd);

		uint32_t sleep2 = start + (uint32_t)(1000/FPS);
		uint32_t end = SDL_GetTicks();
		if (end < sleep2)
			SDL_Delay(sleep2 - end);
	}
}
