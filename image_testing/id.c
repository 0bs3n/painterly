#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>


typedef struct {
    int height;
    int thickness;
    int x, y;
} Rect;

typedef struct {
    int width;
    int height;
    int bpp;
    unsigned char* data;
} Image;

typedef struct {
    int r, g, b;
} Color;


int
plot(int x, int y, Image *image, Color color) 
{
    x = x * image->bpp;
    int target_pixel = x + (y * (image->width * image->bpp));
    image->data[target_pixel] = color.r;
    image->data[target_pixel + 1] = color.g;
    image->data[target_pixel + 2] = color.b;
    return 0;
}

void
draw_rect(int x, int y, int w, int h, Color c, Image *image)
{
    int startx = x;
    int starty = y;
    for (; y - starty < h; x++) {
        if (x - startx == w) {
            x = startx; 
            y++;
        }
        plot(x, y, image, c);
    }
}

int
main()
{
    Image image;
    image.data = stbi_load("../test.png", &image.width, &image.height, &image.bpp, 0);
    int sb = image.width * image.height * image.bpp;
    Color c = { .r = 0xff, .g = 0xff, .b = 0xff };
    
    draw_rect(100, 100, 200, 200, c, &image);

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\n",
            image.width, image.height, image.bpp, sb);

    stbi_write_jpg("test.jpg", image.width, image.height, image.bpp, image.data, 0);
    stbi_write_bmp("test.bmp", image.width, image.height, image.bpp, image.data);
    
    return 0;
}
