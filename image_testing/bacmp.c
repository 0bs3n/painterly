#include "id.h"
#include <stb_image.h>
#include <stb_image_write.h>


int
diff(char* a, char* b, size_t size)
{
    size_t i;
    int diff = 0;
    for (i = 0; i < (size); i++) {
        if (a[i] != b[i]) diff++;
    }

    return diff;
}

int 
main() 
{
    Image image;
    image.data = stbi_load("../test.png", &image.width, &image.height, &image.bpp, 0);
    int sb = image.width * image.height * image.bpp;
    Color c = {0xff, 0, 0, 0};
    Color c2 = {0, 0, 0x99, 0};
    char colors[32][3];
    unsigned char* empty = malloc(image.width * image.height * image.bpp);
    scan(&image, colors, 32);
    // unsigned char* empty2 = malloc(image.width * image.height * image.bpp);
    memset(empty, 0x00, sb);
    // memset(empty2, 0x00, sb);
    Image copy1 = { .data = empty, .height = image.height, .width = image.width, .bpp = image.bpp };
    // Image copy2 = { .data = empty2, .height = image.height, .width = image.width, .bpp = image.bpp };
    int i;
    for (i = 0; i < 32; i++) {
        //draw_circle((i + 1) * 30, 500, 15, 1, colors[i], &copy1);
    }

    filter_circle(200, 600, 50, 1, 0, c, &image);
    filter_circle(220, 600, 50, 1, 2, c2, &image);
    filter_circle(900, 600, 100, 1, 2, c2, &image);

    midpoint_circle(600, 600, 20, c, &copy1);
    draw_circle(560, 600, 20, 0, c, &copy1);

    draw_rect( 900, 900, 100, 100, c, &image);

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\n",
            image.width, image.height, image.bpp, sb);

    stbi_write_bmp("test.bmp", image.width, image.height, image.bpp, image.data);
    stbi_write_bmp("copy1.bmp", copy1.width, copy1.height, copy1.bpp, copy1.data);
    
    return 0;
}

