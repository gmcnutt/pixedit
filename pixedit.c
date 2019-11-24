#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct args {
        int iso_tile_width_pixels;
        int max_sprite_height_pixels;
        int max_sprite_width_tiles;
        int zoom_factor;
        const char *filename;
} args_t;

typedef struct app {
        args_t args;
        struct {
                size_t frames;
                size_t events_seen;
                size_t events_handled;
        } perf;
        SDL_Window *window;
        SDL_Renderer *renderer;
        void (*dispatch)(struct app *, SDL_Event *event);
        void (*render)(struct app *);
        char done: 1;
} app_t;

const int DEFAULT_ISO_TILE_WIDTH_PIXELS = 16;
const int DEFAULT_MAX_SPRITE_HEIGHT_PIXELS = 128;
const int DEFAULT_MAX_SPRITE_WIDTH_TILES = 3;
const int DEFAULT_ZOOM_FACTOR = 16;

/**
 * Print a command-line usage message.
 */
static void print_usage(void)
{
        printf("Usage:  ibp [options] <file>\n"
               "Options: \n"
               "    -H <pixels>: max sprite height [%d]\n"
               "    -h: print this help\n"
               "    -i <pixels>: iso tile width [%d]\n"
               "    -w <tiles>: max sprite width [%d]\n"
               "    -z <integer>: zoom factor [%d]\n",
               DEFAULT_ISO_TILE_WIDTH_PIXELS,
               DEFAULT_MAX_SPRITE_HEIGHT_PIXELS,
               DEFAULT_MAX_SPRITE_WIDTH_TILES,
               DEFAULT_ZOOM_FACTOR
                );
}


/**
 * Parse command-line args.
 */
static void parse_args(int argc, char **argv, struct args *args)
{
        int c = 0;

        memset(args, 0, sizeof(*args));

        while ((c = getopt(argc, argv, "H:i:w:z:")) != -1) {
                switch (c) {
                case 'H':
                        args->max_sprite_height_pixels = atoi(optarg);
                        break;
                case 'i':
                        args->iso_tile_width_pixels = atoi(optarg);
                        break;
                case 'w':
                        args->max_sprite_width_tiles = atoi(optarg);
                        break;
                case 'z':
                        args->zoom_factor = atoi(optarg);
                        break;
                case '?':
                default:
                        print_usage();
                        exit(-1);
                        break;
                }
        }

        if (optind < argc) {
                args->filename = argv[optind];
        } else {
                printf("No filename given\n");
                print_usage();
                exit(-1);
        }
}

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

int main(int argc, char **argv)
{
        static app_t app;
        Uint32 start_ticks, end_ticks;
        SDL_Event event;

        app.dispatch = app_dispatch;
        app.render = app_render;

        parse_args(argc, argv, &app.args);

        /* Init SDL */
        if (SDL_Init(SDL_INIT_VIDEO)) {
                printf("SDL_Init: %s\n", SDL_GetError());
                return -1;
        }

        /* Cleanup SDL on exit. */
        atexit(SDL_Quit);

        /* Create the main window */
        if (! (app.window = SDL_CreateWindow(
                       "Demo", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN))) {
                printf("SDL_CreateWindow: %s\n", SDL_GetError());
                return -1;
        }

        /* Create the renderer. */
        if (! (app.renderer = SDL_CreateRenderer(app.window, -1, 0))) {
                printf("SDL_CreateRenderer: %s\n", SDL_GetError());
                goto destroy_window;
        }

        start_ticks = SDL_GetTicks();

        /* Run the event loop until done. */
        while (!app.done && SDL_WaitEvent(&event)) {
                app.perf.events_seen++;
                app.dispatch(&app, &event);
        }

        end_ticks = SDL_GetTicks();

        /* Print some performance statistics */
        printf(
                "Events seen/handled: %zu/%zu\n",
                app.perf.events_seen,
                app.perf.events_handled
                );
        printf("Frames: %zu\n", app.perf.frames);
        if (end_ticks > start_ticks) {
                printf(
                        "%2.2f FPS\n",
                        ((double)app.perf.frames * 1000) / (end_ticks - start_ticks)
                        );
        }

        SDL_DestroyRenderer(app.renderer);
destroy_window:
        SDL_DestroyWindow(app.window);
        
}
