#include <simg.h>
#include <stb_image.h>
#include <stb_image_write.h>
#define WEBM_OUTPUT 0

int 
main(int argc, char **argv) 
{
    if (argc < 3) {
        printf("need arguments\n");
        exit(1);
    }
    int color_depth = atoi(argv[2]);
    srand(time(NULL));
    Image image;
    char colors[color_depth][3];
    // char colors2[color_depth][3];

    image.data = stbi_load(
            argv[1], 
            &image.width, 
            &image.height, 
            &image.bpp, 0);

    image.size = image.width * image.height * image.bpp;

    unsigned char* data_working = malloc(image.size);
    unsigned char* data_output = malloc(image.size);
    unsigned char* data_colorscheme = malloc(image.size);

    memset(data_working, 0xff, image.size);
    memset(data_output, 0xff, image.size);
    memset(data_colorscheme, 0xff, image.size);

    Image working = { 
        .data = data_working, 
        .height = image.height, 
        .width = image.width, 
        .bpp = image.bpp 
    };

    Image output = { 
        .data = data_output, 
        .height = image.height, 
        .width = image.width, 
        .bpp = image.bpp 
    };

    Image colorscheme = { 
        .data = data_colorscheme, 
        .height = image.height, 
        .width = image.width, 
        .bpp = image.bpp 
    };

    working.size = working.bpp * working.width * working.height;
    output.size = output.bpp * output.width * output.height;
    colorscheme.size = colorscheme.bpp * colorscheme.width * colorscheme.height;

    scan(&image, colors, color_depth);
    // scan_2(&image, colors2, color_depth);
    
    int x, y;
    int c;
    int wd, bd;
    int i = 0;
    int r = 10;
    int iter = atoi(argv[3]);
    int num_images = 100;
    int portion = iter / num_images;
    int j = 0;
    while (i < iter) {
        x = rand() % image.width;
        y = rand() % image.height;
        c = rand() % color_depth;
        
        midpoint_circle(x, y, r, 1, colors[c], &working);

        wd = circ_diff(x, y, r, &image, &working, 0);
        bd = circ_diff(x, y, r, &image, &output, 0);

        if (wd < bd) 
            circ_diff(x, y, r, &working, &output, 1);
        else
            circ_diff(x, y, r, &output, &working, 1);
        ++i; 

        if (i % portion == 0) {
            if (WEBM_OUTPUT) {
                char fn[100];
                sprintf(fn, "output/%03d.bmp", j);
                stbi_write_bmp(fn, output.width, output.height, output.bpp, output.data);
            }
            printf("\r%d/%d", j, num_images);
            fflush(stdout);
            j++;
        }
    }
    int it_count = i;

    int dx, dy;
    dx = 8;
    dy = 4;
    for (x = 20 + image.width / (2 * dx), y = 20 + image.height / (2 * dy), i = 0, j = 0; 
         x + (y * image.width) < image.size / image.bpp && j < color_depth; 
         x += image.width / dx, j++) {

        if (i == dx) {
            y+= image.height / dy;
            x = 20 + image.width / (2 * dx);
            i = 0;
        }

        midpoint_circle(x, y, 20, 1, colors[j], &colorscheme);
        i++;
    }

    // Currently, and initially unintentionally, circ_diff in mode=1 copies only slivers of circles,
    // rather than the whole circle, as intended. It looks pretty nice but I'd like to figure out why.
    // also, because output is drawn of slivers, while working is made of circles, the calls to
    // circ_diff() attempting to copy from output to working (to correct erroneous placements of circles)
    // have little effected, reverting only a sliver of the image to its previous state. 
    // This results in working.bmp being untirely unrecognizeable compared to 
    // the original image, but output comes out correctly. Once I figure out 
    // how to copy whole circles back and forth, I can determine whether this 
    // is actually desirable behaviour.

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\nIterations: %d\n",
            image.width, image.height, image.bpp, image.size, it_count);

    stbi_write_bmp("colorscheme.bmp", colorscheme.width, colorscheme.height, colorscheme.bpp, colorscheme.data);
    stbi_write_bmp("output.bmp", output.width, output.height, output.bpp, output.data);

    stbi_image_free(image.data);
    free(colorscheme.data);
    free(output.data);
    
    return 0;
}

