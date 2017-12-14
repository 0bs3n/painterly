#include "siml.h"
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#define MIN(x, y) (x < y ? x : y)

int 
main(int argc, char **argv) 
{
    if (argc < 3) {
        // TODO print proper usage and implement proper command line arguments
        printf("need arguments\n");
        exit(1);
    }

    srand(time(NULL));
    Image image;

    image.data = stbi_load(
            argv[1], 
            &image.width, 
            &image.height, 
            &image.bpp, 0);

    image.size = image.width * image.height * image.bpp;

    unsigned char* data_working     = malloc(image.size);
    unsigned char* data_output      = malloc(image.size);

    memset(data_working,     0xff, image.size);
    memset(data_output,      0xff, image.size);

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

    Color color;
    
    // int x, y;
    int x0, y0, x1, y1, rx, ry;
    int wd, bd;
    int i = 0;
    // int r = atoi(argv[4]);
    int iter = atoi(argv[2]);
    int num_images = 1000;
    int portion = iter / num_images;
    int j = 0;
    int WEBM_OUTPUT = atoi(argv[3]);
    int max_line_length = MIN(image.width, image.height) / 12;
    if (argv[4]) {
        max_line_length = atoi(argv[4]);
    }
    printf("%d\n", max_line_length);


    while (i < iter) {
        x0 = rand() % image.width;
        y0 = rand() % image.height;
        x1 = (rand() % (max_line_length << 1)) + (x0 - max_line_length);
        y1 = (rand() % (max_line_length << 1)) + (y0 - max_line_length);
        rx = rand() % image.width;
        ry = rand() % image.height;
        sample_point(&image, color, rx, ry);
        draw_line(&output, color, x0, y0, x1, y1);

        wd = line_diff(&working, &image, x0, y0, x1, y1, 0);
        bd = line_diff(&output, &image, x0, y0, x1, y1, 0);

        if (wd < bd)
            line_diff(&output, &working, x0, y0, x1, y1, 1);
        else
            line_diff(&working, &output, x0, y0, x1, y1, 1);
        ++i;

        /* Section for using circles as the primitive to draw
        x = rand() % image.width;
        y = rand() % image.height;

        sample_point(&image, color, x, y);
        
        draw_circle(x, y, r, 1, color, &working);

        wd = circ_diff(x, y, r, &image, &working, 0);
        bd = circ_diff(x, y, r, &image, &output, 0);

        if (wd < bd) 
            circ_diff(x, y, r, &working, &output, 1);
        else
            circ_diff(x, y, r, &output, &working, 1);
        ++i; 
        */

        if ((i - 1) % portion == 0) {
            if (WEBM_OUTPUT) {
                char fn[100];
                sprintf(fn, "output/%03d.png", j);
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

    stbi_write_png("output.png", 
                    output.width, 
                    output.height, 
                    output.bpp, 
                    output.data, 
                    output.width * output.bpp);

    stbi_image_free(image.data);
    free(output.data);
    free(working.data);
    
    return 0;
}

