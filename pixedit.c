#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "str.h"

const int DEFAULT_ISO_TILE_WIDTH_PIXELS = 16;
const int DEFAULT_MAX_SPRITE_HEIGHT_PIXELS = 128;
const int DEFAULT_MAX_SPRITE_WIDTH_TILES = 3;
const int DEFAULT_ZOOM_FACTOR = 16;

struct args {
        int iso_tile_width_pixels;
        int max_sprite_height_pixels;
        int max_sprite_width_tiles;
        int zoom_factor;
        const char *filename;
};

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


int main(int argc, char **argv)
{
        struct args args;


        parse_args(argc, argv, &args);

        /* Init SDL */
        if (SDL_Init(SDL_INIT_VIDEO)) {
                printf("SDL_Init: %s\n", SDL_GetError());
                return -1;
        }

        /* Cleanup SDL on exit. */
        atexit(SDL_Quit);

        
}
