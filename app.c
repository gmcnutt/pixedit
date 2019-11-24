#include "app.h"


static void app_render(app_t *app)
{
        /* Clear screen */
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(app->renderer);

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
        app->iso_tile_width_pixels = 16;
        app->max_sprite_height_pixels = 128;
        app->max_sprite_width_tiles = 3;
        app->zoom_factor = 16;
                
        app->dispatch = app_dispatch;
        app->render = app_render;
}
