#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "vecmat.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

// drawing lots of lines with SDL_RenderDrawLine uses more cpu than first putting
// information about the lines to a DisplayBuf, and then drawing all those
struct DisplayBuf;

struct DisplayBuf *displaybuf_new(void);
void displaybuf_free(struct DisplayBuf *buf);
void displaybuf_clear(struct DisplayBuf *buf);
void displaybuf_render(SDL_Renderer *rnd, const struct DisplayBuf *buf);


// contains all the information needed for drawing 3d things
struct DisplayCamera {
	struct Vec3 location;
	struct Mat3 world2player;  // multiplying rotates a vector for showing it on screen
};

void display_line(
	struct DisplayBuf *buf, const struct DisplayCamera *cam,
	struct Vec3 start, struct Vec3 end);


#endif   // DISPLAY_H
