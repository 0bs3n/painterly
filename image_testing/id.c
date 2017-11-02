#include "id.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

int
align(int num, int alignment)
{
    return alignment * (num / alignment);
}

int
plot(int x, int y, Image *image, Color color) 
{
    if (x > 0 && y > 0 && x < image->width && y < image->height) {
        int i;
        x *= image->bpp;
        int target_pixel = x + (y * (image->width * image->bpp));
        for (i = 0; i < image->bpp; i++)
            image->data[target_pixel + i] = color[i];
        return 0;
    }
    else return 1;
}

int
filter(int x, int y, Image *image, Color color, int cv)
{
    if (x > 0 && y > 0 && x < image->width && y < image->height) {
        x *= image->bpp; 
        int target_pixel = x + (y * (image->width * image->bpp));
        image->data[target_pixel + cv] = color[cv];
        return 0;
    }
    else return 1;
}

void
midpoint_circle(int x0, int y0, int radius, Color c, Image *image)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
    printf("x: %d\n"
            "y: %d\n"
            "dx: %d\n"
            "dy: %d\n"
            "err: %d\n\n",
            x, y, dx, dy, err);
        plot(x0 + x, y0 + y, image, c);
        plot(x0 + y, y0 + x, image, c);
        plot(x0 - y, y0 + x, image, c);
        plot(x0 - x, y0 + y, image, c);
        plot(x0 - x, y0 - y, image, c);
        plot(x0 - y, y0 - x, image, c);
        plot(x0 + y, y0 - x, image, c);
        plot(x0 + x, y0 - y, image, c);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2; 
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }
}

void
scan(Image *image, char colors[][3], int numc) 
{
    int x, y;
    int size = image->width * image->height;
    int i, j;
    for (x = 0, j = 0, y = 0; 
            j < numc; 
            x += (3 * ((size / 64) / 3)) * image->bpp, 
            y += image->height / numc, j++) {

        printf("%d: x: %d, y: %d\n", j, (x / image->bpp) / image->width, y);

        for (i = 0; i < 3; i++) {
            colors[j][i] = image->data[(x + (y * image->width)) + i];
            printf("/%02x", colors[j][i]);
        }

        printf("\n");
        printf("/%02x", image->data[x + (y * image->width)]);
        printf("/%02x", image->data[x + (y * image->width) + 1]);
        printf("/%02x ", image->data[x + (y * image->width) + 2]);
        printf("\n");
    }
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
filter_circle(int h, int k, int r, int filled, int cv, Color c, Image *image)
{
    int x, y;
    int px, npx;
    for (x = h - r, y = k - r; y <= k + r; x++) {
        px = sqrt((r*r) - ((y - k) * (y - k))) + h;
        npx = -sqrt((r*r) - ((y - k) * (y - k))) + h;
        if (filled) {
            for (; px >= h && npx <= h; px--, npx++) {
                filter(px, y, image, c, cv);
                filter(npx, y, image, c, cv);
            }
        } else {
            filter(px, y, image, c, cv);
            filter(npx, y, image, c, cv);
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

/*
int
main()
{
    Image image;
    image.data = stbi_load("../test.png", &image.width, &image.height, &image.bpp, 0);
    int sb = image.width * image.height * image.bpp;
    Color c = {0xff, 0, 0, 0};
    Color c2 = {0xff, 0, 0, 0};

    filter_circle(220, 100, 50, 1, 0, c2, &image);

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\n",
            image.width, image.height, image.bpp, sb);

    stbi_write_bmp("test.bmp", image.width, image.height, image.bpp, image.data);
    
    return 0;
}
*/
