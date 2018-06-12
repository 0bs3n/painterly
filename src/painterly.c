#include "siml.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <time.h>
#include <unistd.h>

#define MIN(x, y) (x < y ? x : y)

void
printf_usage(int exit_code, char *fn) {

  printf(
 " ____   _    ___ _   _ _____ _____ ____  _  __   __\n"
 "|  _ \\ / \\  |_ _| \\ | |_   _| ____|  _ \\| | \\ \\ / /\n"
 "| |_) / _ \\  | ||  \\| | | | |  _| | |_) | |  \\ V / \n"
 "|  __/ ___ \\ | || |\\  | | | | |___|  _ <| |___| |  \n"
 "|_| /_/   \\_\\___|_| \\_| |_| |_____|_| \\_\\_____|_|  \n\n\n");
  printf("Usage: %s -f/--input-file <input_image> \n"
         "         [-c <num_colors>] \n"
         "         [-i <iterations / 1000>] \n"
         "         [-s/--sample-method direct | average]\n"
         "         [-w/--video-output]\n", fn);
                                                    
    exit(exit_code);
}

enum sample_methods {
    DIRECT,
    AVERAGE
};

int 
main(int argc, char **argv) 
{
    int iter = 1000000;
    int num_colors = 64;
    int WEBM_OUTPUT = 0;
    int file_provided = 0;
    int sample_method = AVERAGE;
    char input_file[256];

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf_usage(0, argv[0]); // FIXME: Implement this
        } else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--video-output")) {
            WEBM_OUTPUT = 1;

        } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--sample-method")) {
            if (!strcmp(argv[++i], "direct")) { sample_method = DIRECT; }
        } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--iterations")) {
            iter = 1000 * atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--colors")) {
            num_colors = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--input-file")) {
            strncpy(input_file, argv[++i], 255);
            if (access(input_file, F_OK) == -1) {
                printf("File does not exist!\n");
                printf_usage(1, argv[0]);
            }
            file_provided = 1;
        }
    }

    if (!file_provided) {
        printf("Please provide an image file with the ``-f'' or ``--input-file'' option\n");
        printf_usage(1, argv[0]);
    }

    srand(time(NULL));
    Image image;

    image.data = stbi_load(
            input_file, 
            &image.width, 
            &image.height, 
            &image.bpp, 0);

    image.size = image.width * image.height * image.bpp;

    unsigned char* data_working     = malloc(image.size);
    unsigned char* data_output      = malloc(image.size);

    memset(data_working,     0xff, image.size);
    memset(data_output,      0xff, image.size);

    unsigned char **palette;
    Color color;

    if (sample_method == AVERAGE) {
        palette = siml_init_palette(num_colors, image.bpp);
        reduce_color_pallete(&image, palette, num_colors);
    } 
    

    Image working = { 
        .data   = data_working, 
        .height = image.height, 
        .width  = image.width, 
        .bpp    = image.bpp,
        .size   = image.size
    };

    Image output = { 
        .data   = data_output, 
        .height = image.height, 
        .width  = image.width, 
        .bpp    = image.bpp,
        .size   = image.size
    };

    int x0, y0, x1, y1, rx, ry, c;
    int wd, bd;
    int i = 0;
    int num_images = 1000;
    int portion = iter / num_images;
    int j = 0;
    int max_line_length = MIN(image.width, image.height) / 12;

    while (i < iter) {
        x0 =  rand() % image.width;
        y0 =  rand() % image.height;
        x1 = (rand() % (max_line_length << 1)) + (x0 - max_line_length);
        y1 = (rand() % (max_line_length << 1)) + (y0 - max_line_length);

        if (sample_method == AVERAGE) {
            c  =  rand() % num_colors;
            draw_line(&output, palette[c], x0, y0, x1, y1);
        } else {
            rx =  rand() % image.width;
            ry =  rand() % image.height;
            sample_point(&image, color, rx, ry);
            draw_line(&output, color, x0, y0, x1, y1);
        }

        wd = line_diff(&working, &image, x0, y0, x1, y1, 0);
        bd = line_diff(&output, &image, x0, y0, x1, y1, 0);

        if (wd < bd)
            line_diff(&output, &working, x0, y0, x1, y1, 1);
        else
            line_diff(&working, &output, x0, y0, x1, y1, 1);
        ++i;

        if ((i - 1) % portion == 0) {
            if (WEBM_OUTPUT) {
                char fn[100];
                sprintf(fn, "video/%03d.png", j);
                stbi_write_png(fn, 
                               output.width, 
                               output.height, 
                               output.bpp, 
                               output.data,
                               output.width * output.bpp); 
            }

            printf("\r%d/%d", j, num_images);
            fflush(stdout);
            j++;
        }
    }

    printf("\r");

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\nIterations: %d\n",
            image.width, image.height, image.bpp, image.size, i);

    if (sample_method == AVERAGE) {
        Image color_palette = gen_colorscheme(palette, num_colors);

        stbi_write_png("colorscheme.png", 
                        color_palette.width, 
                        color_palette.height, 
                        color_palette.bpp, 
                        color_palette.data, 
                        color_palette.width * color_palette.bpp);
        stbi_image_free(color_palette.data);
        siml_free_palette(palette, num_colors);
    }

    stbi_write_png("output.png", 
                    output.width, 
                    output.height, 
                    output.bpp, 
                    output.data, 
                    output.width * output.bpp);

    stbi_image_free(image.data);
    stbi_image_free(output.data);
    stbi_image_free(working.data);

    return 0;
}

