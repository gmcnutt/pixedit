#include "app.h"
#include "iso.h"

static void draw_pixel_grid(app_t *app)
{
        int top = 0, left = 0;
        int right = app->grid.tile.w * app->zoom;
        int bottom = app->max_sprite_h * app->zoom;

        for (int y = top; y <= bottom; y += app->zoom) {
                SDL_RenderDrawLine(app->renderer, left, y, right, y);
        }

        for (int x = left; x <= right; x += app->zoom) {
                SDL_RenderDrawLine(app->renderer, x, top, x, bottom);
        }

}

static void app_render(app_t *app)
{
        /* Clear screen */



        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(app->renderer);

        /* Show the pixel grid */
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        if (app->zoom > 2) {
                draw_pixel_grid(app);
        }

        /* Update view */
        SDL_RenderPresent(app->renderer);

        app->perf.frames++;
}

static void app_dispatch(app_t *app, SDL_Event *event)
{
        switch (event->type) {
        case SDL_QUIT:
                app->done = 1;
                app->perf.events_handled++;
                break;
        case SDL_KEYDOWN:
                switch (event->key.keysym.sym) {
                case SDLK_q:
                        app->done = 1;
                        app->perf.events_handled++;
                        break;
                default:
                        break;
                }
                break;
        case SDL_WINDOWEVENT:
                app->render(app);
                app->perf.events_handled++;
                break;
        default:
                break;
        }
}

void app_init(app_t *app)
{
        memset(app, 0, sizeof(*app));

        /* Default values */
        app->grid.tile.w  = 64;
        app->grid.tile.h = 32;
        app->max_sprite_h = 64;
        app->grid.w  = 1;
        app->grid.h  = 1;
        app->zoom = 8;

        app->dispatch = app_dispatch;
        app->render = app_render;
}
