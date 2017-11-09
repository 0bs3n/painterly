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
    if (x >= 0 && y >= 0 && x < image->width && y < image->height) {
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
copy_plot(Image *dest, Image *source, int x, int y)
{
    if (x >= 0 && y >= 0 && x < dest->width && y < dest->height) {
        int i;
        x *= dest->bpp;
        int target_pixel = x + (y * (dest->width * dest->bpp));
        for (i = 0; i < dest->bpp; i++)
            dest->data[target_pixel + i] = source->data[target_pixel + i];
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
midpoint_circle(int x0, int y0, int radius, int filled, Color c, Image *image)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        if (filled) {
            int i;
            for (i = x0 - x; i < x0 + x; i++) {
                plot(i, y0 - y, image, c);
                plot(i, y0 + y, image, c);
            }

            for (i = x0 - y; i < x0 + y; i++) {
                plot(i, y0 - x, image, c);
                plot(i, y0 + x, image, c);
            }
        }

        else {
            plot(x0 + x, y0 + y, image, c);
            plot(x0 + y, y0 + x, image, c);
            plot(x0 - y, y0 + x, image, c);
            plot(x0 - x, y0 + y, image, c);
            plot(x0 - x, y0 - y, image, c);
            plot(x0 - y, y0 - x, image, c);
            plot(x0 + y, y0 - x, image, c);
            plot(x0 + x, y0 - y, image, c);
        }

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

float 
calc(Image *source, Image *copy, int x, int y)
{
    int source_target = (x + (y * source->width)) * source->bpp;
    int copy_target   = (x + (y * source->width)) * source->bpp;

    source_target     = source_target < source->size ? source_target : source->size;
    copy_target       = copy_target < copy->size ? copy_target : copy->size;

    source_target     = source_target > 0 ? source_target : 0;
    copy_target       = copy_target > 0 ? copy_target : 0;

    unsigned char red_s   = source->data[source_target];
    unsigned char green_s = source->data[source_target + 1];
    unsigned char blue_s  = source->data[source_target + 2];
    unsigned char red_c   = copy->data[copy_target];
    unsigned char green_c = copy->data[copy_target + 1];
    unsigned char blue_c  = copy->data[copy_target + 2];
    int rd, gd, bd;
    rd = (red_s - red_c) * (red_s - red_c);
    gd = (green_s - green_c) * (green_s - green_c);
    bd = (blue_s - blue_c) * (blue_s - blue_c);
    int diff = sqrt(rd + gd + bd);
    return diff;
}

int
circ_diff(int x0, int y0, int radius, Image *source, Image * copy, int mode)
{
    // TODO The final piece of the puzzle. mode=1 Only draws horizontal slivers of circles,
    // 1px high, along the center of the circle. mode=0 works fine, only the
    // algorith to copy data (in the form of circles) from one image to the other
    // appears faulty.
    
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    int diff = 0;
// 
    while (x >= y) {
        int i;
        int target = (x + (y * source->width * source->height * source->bpp));
        // TODO the following scope appears to be the culprit interfering with the adjustment
        // of algorithm values.
        // However, if the condition is removed, the code hangs in the midlle
        // of this function.
        if (target < source->size) {
            for (i = x0 - x; i < x0 + x; i++) {
                if (mode) {
                        copy_plot(copy, source, i, y0 - y);
                        copy_plot(copy, source, i, y0 + y);
                                /*
                        if (i > 0 && y0 - y > 0) {
                            copy->data[(xv * source->bpp + (y0 - y) * source->width * source->bpp) + j] 
                                = source->data[(xv * source->bpp + (y0 - y) * source->width * source->bpp) + j];

                            copy->data[(xv * source->bpp + (y0 + y) * source->width * source->bpp) + j] 
                                = source->data[(xv * source->bpp + (y0 + y) * source->width * source->bpp) + j];
                        }
                        */
                }

                else {
                    diff += calc(source, copy, i, y0 - y);
                    diff += calc(source, copy, i, y0 + y);
                }
            }

            for (i = x0 - y; i < x0 + y; i++) {
                if (mode) {
                    copy_plot(copy, source, i, y0 - x);
                    copy_plot(copy, source, i, y0 + x);
                                /*
                        if (i > 0 && y0 - y > 0) {
                            copy->data[(xv * source->bpp + (y0 - x) * source->width * source->bpp) + j] 
                                = source->data[(xv * source->bpp + (y0 - x) * source->width * source->bpp) + j];

                            copy->data[(xv * source->bpp + (y0 + x) * source->width * source->bpp) + j] 
                                = source->data[(xv * source->bpp + (y0 + x) * source->width * source->bpp) + j];
                        }
                        */
                }
                else {
                    diff += calc(source, copy, i, y0 - x);
                    diff += calc(source, copy, i, y0 + x);
                }
            }
        }

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
    return diff;
}

void
scan_2(Image *image, char colors[][3], int numc) 
{
    int f1 = sqrt(numc);
    while (numc % f1 != 0) {
        f1--;
    }

    int f2 = numc/f1;
     
    int x, y, i, j, k;
    int dx, dy;
    dx = f1 > f2 ? f1 : f2;
    dy = f1 < f2 ? f1 : f2;
    for (x = (image->width / (2 * dx)),
         y = image->height / (2 * dy), 
         i = 0, j = 0; 
         x + (y * image->width) < image->size && j < numc && y < image->height; 
         x += (image->width / dx), j++) {

        for (k = 0; k < 3; k++) {
            colors[j][k] = image->data[(x + (y * image->width)) * image->bpp + k];
        }

        if (i == dx) {
            y+= (image->height / dy);
            x = image->width / (2 * dx);
            i = 0;
        }

        i++;
    }
}

void
scan(Image *image, char colors[][3], int numc) 
{
    int x, y;
    int i, j;
    for (x = 0, j = 0, y = 0; 
         j < numc; 
         x += image->width * 2/ numc,
         j++) {

        for (i = 0; i < 3; i++) {
            colors[j][i] = image->data[(x + (y * image->width)) + i];
        }

        // TODO: Cleanup up magic numbers below, in "j % 2" and "numc) * 4"
        // find a more universal way to appropriately vary the sample points
        if (j % 2 == 0 && j != 0)
            y += (image->height / numc) * 4;
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
