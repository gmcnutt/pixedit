/**
 * App definitions
 */

#ifndef app_h
#define app_h

#include <SDL2/SDL.h>

typedef struct args {
} args_t;

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
        
        /* Commandline args */
        int iso_tile_width_pixels;
        int max_sprite_height_pixels;
        int max_sprite_width_tiles;
        int zoom_factor;
        const char *filename;
        
        char done: 1;
} app_t;

void app_init(app_t *app);

#endif
