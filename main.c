#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "app.h"

/**
 * Print a command-line usage message.
 */
static void print_usage(app_t *app)
{
        printf("Usage:  ibp [options] <file>\n"
               "Options: \n"
               "    -H <pixels>: max sprite height [%d]\n"
               "    -h: print this help\n"
               "    -i <pixels>: iso tile width [%d]\n"
               "    -w <tiles>: max sprite width [%d]\n"
               "    -z <integer>: zoom factor [%d]\n",
               app->iso_tile_width_pixels,
               app->max_sprite_height_pixels,
               app->max_sprite_width_tiles,
               app->zoom_factor
                );
}


/**
 * Parse command-line args.
 */
static void parse_args(int argc, char **argv, app_t *app)
{
        int c = 0;

        while ((c = getopt(argc, argv, "H:i:w:z:")) != -1) {
                switch (c) {
                case 'H':
                        app->max_sprite_height_pixels = atoi(optarg);
                        break;
                case 'i':
                        app->iso_tile_width_pixels = atoi(optarg);
                        break;
                case 'w':
                        app->max_sprite_width_tiles = atoi(optarg);
                        break;
                case 'z':
                        app->zoom_factor = atoi(optarg);
                        break;
                case '?':
                default:
                        print_usage(app);
                        exit(-1);
                        break;
                }
        }

        if (optind < argc) {
                app->filename = argv[optind];
        } else {
                printf("No filename given\n");
                print_usage(app);
                exit(-1);
        }
}


int main(int argc, char **argv)
{
        static app_t app;
        Uint32 start_ticks, end_ticks;
        SDL_Event event;

        app_init(&app);

        parse_args(argc, argv, &app);

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
