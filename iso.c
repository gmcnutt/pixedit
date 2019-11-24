/**
 * Functions for rendering to an isometric grid.
 */

#include "iso.h"


static inline int grid_to_screen_x(int grid_x, int grid_y, int half_w)
{
        return (grid_x - grid_y) * half_w;
}


static inline int grid_to_screen_y(int grid_x, int grid_y, int half_h)
{
        return (grid_x + grid_y) * half_h;
}

void iso_draw_grid(SDL_Renderer *renderer, iso_grid_t *grid)
{
        int hw = grid->tile.w / 2;
        int hh = grid->tile.h / 2;
        int offx = grid_to_screen_x(grid->h, 0, hw);


        for (int row = 0; row <= grid->h; row++) {
                SDL_RenderDrawLine(
                        renderer,
                        offx + grid_to_screen_x(0, row, hw),
                        grid_to_screen_y(0, row, hh),
                        offx + grid_to_screen_x(grid->w, row, hw),
                        grid_to_screen_y(grid->w, row, hh));
        }
        for (int col = 0; col <= grid->w; col++) {
                SDL_RenderDrawLine(
                        renderer,
                        offx + grid_to_screen_x(col, 0, hw),
                        grid_to_screen_y(col, 0, hh),
                        offx + grid_to_screen_x(col, grid->h, hw),
                        grid_to_screen_y(col, grid->h, hh));
        }
}
