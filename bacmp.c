#include <stdlib.h>
#include <stdio.h>
#include "id.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
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
    unsigned char* empty = malloc(image.width * image.height * image.bpp);
    unsigned char* empty2 = malloc(image.width * image.height * image.bpp);
    memset(empty, 0, sb);
    memset(empty2, 0, sb);
    Image copy1 = { .data = empty, .height = image.height, .width = image.width, .bpp = image.bpp };
    Image copy2 = { .data = empty2, .height = image.height, .width = image.width, .bpp = image.bpp };


    filter_circle(220, 100, 50, 1, 0, c, &image);

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\n",
            image.width, image.height, image.bpp, sb);

    stbi_write_bmp("test.bmp", image.width, image.height, image.bpp, image.data);
    stbi_write_bmp("copy1.bmp", copy1.width, copy1.height, copy1.bpp, copy1.data);
    
    return 0;
}

