/**
 * Functions for rendering to an isometric grid.
 */
#ifndef iso_h
#define iso_h

#include <SDL2/SDL.h>

typedef struct iso_grid {
        int w, h; /* in tiles */
        struct {
                int w, h; /* in pixels */
        } tile;
} iso_grid_t;

/**
 * Render isometric grid lines.
 */
void iso_draw_grid(SDL_Renderer *renderer, iso_grid_t *grid);

#endif
