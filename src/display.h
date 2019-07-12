#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

struct DisplayPoint {
	double x;    // pixels, 0 means left, more means right
	double y;    // pixels, 0 means top, more means down
};

bool display_pointisonscreen(struct DisplayPoint dp);
void display_line(SDL_Renderer *rnd, struct DisplayPoint dp1, struct DisplayPoint dp2);



#endif   // DISPLAY_H
