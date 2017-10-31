#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

typedef int Color[4];

int
plot(int x, int y, Image *image, Color color) 
{
    if (x > 0 && y > 0 && x < image->width && y < image->height) {
        int i;
        x *= image->bpp;
        int target_pixel = x + (y * (image->width * image->bpp));
        printf("Current colorvalue: %02x%02x%02x\n", image->data[target_pixel], image->data[target_pixel+1], image->data[target_pixel+2]);
        for (i = 0; i < image->bpp; i++)
            image->data[target_pixel + i] = color[i];
        return 0;
    }
    else return 1;
}

void
draw_rect(int x, int y, int w, int h, Color c, Image *image)
{
    int startx = x - 1;
    int starty = y;
    for (; y - (starty) <= h; x++) {
        plot(x, y, image, c);
        if (x - startx == w) {
            x = startx; 
            y++;
        }
    }
}

void
draw_circle(int h, int k, int r, int filled, Color c, Image *image)
{
    int x, y;
    int px, npx;
    for (x = h - r, y = k - r; y <= k + r; x++) {
        px = sqrt((r*r) - ((y - k) * (y - k))) + h;
        npx = -sqrt((r*r) - ((y - k) * (y - k))) + h;
        if (filled) {
            for (; px >= h && npx <= h; px--, npx++) {
                plot(px, y, image, c);
                plot(npx, y, image, c);
            }
        } else {
            plot(px, y, image, c);
            plot(npx, y, image, c);
        }

        if (x == h + r + 1) {
            x = h - r;
            y++;
        }
    }
}

void
draw_cross(int h, int k, int r,  Color c, Image *image)
{
    int x, y;
    for ( x = h - r, y = k - r; y < k + r; x++) {
        if (x == h || y == k)
            plot(x, y, image, c);

        if (x == h + r) {
            x = h - r;
            y++;
        }
    }
}

int
main()
{
    Image image;
    image.data = stbi_load("../test.png", &image.width, &image.height, &image.bpp, 0);
    int sb = image.width * image.height * image.bpp;
    Color c = {0xff, 0, 0, 0};
    
    draw_circle(100, 100, 50, 0, c, &image);
    draw_circle(220, 100, 50, 1, c, &image);
    // draw_circle(400, 400, 300, c, &image);

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\n",
            image.width, image.height, image.bpp, sb);

    stbi_write_bmp("test.bmp", image.width, image.height, image.bpp, image.data);
    
    return 0;
}
