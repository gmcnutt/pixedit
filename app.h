/**
 * App definitions
 */

#ifndef app_h
#define app_h

#include <SDL2/SDL.h>

#include "iso.h"

typedef struct app {
        SDL_Window *window;
        SDL_Renderer *renderer;
        void (*dispatch)(struct app *, SDL_Event *event);
        void (*render)(struct app *);

        struct {
                size_t frames;
                size_t events_seen;
                size_t events_handled;
        } perf;

        iso_grid_t grid;
        int max_sprite_h;  /* pixels */
        int zoom;          /* factor */

        const char *filename;

        unsigned char done: 1;
} app_t;

void app_init(app_t *app);

#endif
