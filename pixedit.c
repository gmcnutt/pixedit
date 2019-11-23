#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "str.h"


struct args {
        char **filenames;
        char *destdir;
        Uint8 alpha;
        bool crop, debug, sort;
        int filecount;
};

struct imageinfo {
        SDL_Surface *image;
        char *filename;
        size_t size;
};

/**
 * Print a command-line usage message.
 */
static void print_usage(void)
{
        printf("Usage:  ibp [options] <files>\n"
               "Options: \n"
               "    -c <alpha>: crop out transparent pixels\n"
               "    -d: debug\n"
               "    -h:	help\n"
               "    -s: sort list output by image size\n"
               "    -D <dir>: write modified images as .png here\n"
               "The default is to list the image attributes.\n"
                );
}


/**
 * Parse command-line args.
 */
static void parse_args(int argc, char **argv, struct args *args)
{
        int c = 0;

        memset(args, 0, sizeof(*args));

        while ((c = getopt(argc, argv, "D:c:dhs")) != -1) {
                switch (c) {
                case 'c':
                        args->crop = true;
                        args->alpha = atoi(optarg) & 0xff;
                        break;
                case 'd':
                        args->debug = true;
                        break;
                case 'h':
                        print_usage();
                        exit(0);
                case 's':
                        args->sort = true;
                        break;
                case 'D':
                        args->destdir = optarg;
                        break;
                case '?':
                default:
                        print_usage();
                        exit(-1);
                        break;
                }
        }

        if (args->crop && !args->destdir) {
                printf("The -c option requires -D.");
                print_usage();
                exit(-1);
        }

        if (optind < argc) {
                args->filecount = argc - optind;
                args->filenames = &argv[optind];
        } else {
                print_usage();
                exit(-1);
        }
}

/**
 * Compare `a` to `b` for sorting.
 *
 * Both `a` and `b` should be pointers to `struct imageinfo` objects.
 *
 * Returns
 *     -1 if a < b
 *      0 if a == b
 *      1 if a > b
 */
static int compare(const void *pa, const void *pb)
{
        struct imageinfo *a = (struct imageinfo*)pa,
                *b = (struct imageinfo*)pb;
        return a->size - b->size;
}

/**
 * Print info about image to stdout.
 */
static void dump(struct imageinfo *info)
{
        SDL_PixelFormat *format = info->image->format;
        const char *fmtname = SDL_GetPixelFormatName(format->format);

        /* Basic info */
        printf(
                "%5d %5d %5d %10zu",
                info->image->w,
                info->image->h,
                info->image->pitch,
                info->size
                );

        /* Pixel format info */
        printf(" %10s", &fmtname[16]);


        /* File info */
        printf(" %s\n", info->filename);
}

/**
 * Convert the image to a supported format.
 */
static void normalize(struct imageinfo *info)
{
        if (info->image->format->format == SDL_PIXELFORMAT_ABGR8888) {
                return;
        }

        SDL_Surface *original = info->image;
        info->image = SDL_ConvertSurfaceFormat(
                original,
                SDL_PIXELFORMAT_ABGR8888,
                0
                );
        assert(info->image);

        SDL_FreeSurface(original);
}

/**
 * Crop off any transparency on the borders.
 *
 * Crop such that the image includes only pixels with an alpha > max_alpha
 * (where 0 is transparent and 255 is opaque).
 */
static void crop(struct imageinfo *info, Uint8 max_alpha)
{
        /* Assert image has a supported format. This format was chosen because
         * the pixels are a contiguous array of 32 bit values. */
        assert(info->image->format->format == SDL_PIXELFORMAT_ABGR8888);
        assert(info->image->pitch == info->image->w * 4);

        int right = 0, left = info->image->w, bottom = 0, top = info->image->h;
        Uint32 *pixel = info->image->pixels;
        Uint32 amask = info->image->format->Amask;
        Uint8 ashift = info->image->format->Ashift;

        for (int y = 0; y < info->image->h; y++) {
                for (int x = 0; x < info->image->w; x++) {
                        Uint8 alpha = ((*pixel & amask) >> ashift);
                        alpha = abs(alpha - SDL_ALPHA_TRANSPARENT);
                        if (alpha > max_alpha) {
                                if (x < left) {
                                        left = x;
                                }
                                if (x > right) {
                                        right = x;
                                }
                                if (y > bottom) {
                                        bottom = y;
                                }
                                if (y < top) {
                                        top = y;
                                }
                        }
                        pixel++;
                }
        }

        SDL_Rect clip = {left, top, right - left, bottom - top};
        SDL_Surface *copy = SDL_CreateRGBSurfaceWithFormat(
                0,  /* flags */
                clip.w,  /* width */
                clip.h,  /* height */
                32,  /* depth */
                info->image->format->format  /* format */
                );

        SDL_BlitSurface(info->image, &clip, copy, NULL);
        SDL_FreeSurface(info->image);
        info->image = copy;
        info->size = copy->w * copy->h;
}

/**
 * Save the image as .png to the directory.
 */
static void save(struct imageinfo *info, const char *destdir)
{
        char *last, *filename = info->filename;

        /* Get the filename part of the path. */
        while ((last = strchr(filename, '/'))) {
                filename = last + 1;
        }

        /* Replace the suffix with .png */
        last = strchr(filename, '.');
        if (last) {
                *last = '\0';
        }

        /* Build a new path to the dest dir. Note that I am not freeing the
         * original info->filename because it is probably from the argv
         * vector. */
        info->filename = str_printf("%s/%s.png", destdir, filename);
        IMG_SavePNG(info->image, info->filename);

        if (last) {
                *last = '.';
        }
}

int main(int argc, char **argv)
{
        struct args args;
        struct imageinfo *images;
        int count = 0;


        parse_args(argc, argv, &args);

        /* Init SDL */
        if (SDL_Init(SDL_INIT_VIDEO)) {
                printf("SDL_Init: %s\n", SDL_GetError());
                return -1;
        }

        /* Cleanup SDL on exit. */
        atexit(SDL_Quit);

        /* Allocate our image table */
        if (args.filecount) {
                if (! (images = calloc(args.filecount, sizeof(images[0])))) {
                        perror("calloc");
                        return -1;
                }
        }

        /* Load the image table */
        for (int i = 0; i < args.filecount; i++) {
                SDL_Surface *image;

                if (! (image = IMG_Load(args.filenames[i]))) {
                        if (args.debug) {
                                printf("%s: %s\n", args.filenames[i],
                                       SDL_GetError());
                        }
                        continue;
                }

                images[count].image = image;
                images[count].filename = args.filenames[i];
                images[count].size = image->w * image->h;
                count++;
        }

        if (args.crop) {
                for (int i = 0; i < count; i++) {
                        normalize(&images[i]);
                        crop(&images[i], args.alpha);
                }
        }

        if (args.destdir) {
                for (int i = 0; i < count; i++) {
                        save(&images[i], args.destdir);
                }
        }

        if (args.sort) {
                qsort(images, count, sizeof(images[0]), compare);
        }

        /* Print header */
        printf(
                "%5s %5s %5s %10s %10s %s\n",
                "w",
                "h",
                "pitch",
                "size",
                "pixfmt",
                "name"
                );

        /* Print image info for all images */
        for (int i = 0; i < count; i++) {
                dump(&images[i]);
        }
}
