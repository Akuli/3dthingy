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


// drawing lots of lines with SDL_RenderDrawLine uses more cpu than first putting
// information about the lines to a DisplayBuf, and then drawing all those
struct DisplayBuf;

struct DisplayBuf *displaybuf_new(void);
void displaybuf_free(struct DisplayBuf *buf);
void displaybuf_clear(struct DisplayBuf *buf);
void displaybuf_draw_line(struct DisplayBuf *buf, struct DisplayPoint dp1, struct DisplayPoint dp2);
void displaybuf_render(SDL_Renderer *rnd, const struct DisplayBuf *buf);


#endif   // DISPLAY_H
