#include "siml.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

void
swap(void *a, void *b, size_t len) {
    unsigned char *pa = a;
    unsigned char *pb = b;
    unsigned char tmp;

    for (size_t i = 0; i != len; ++i) {
        tmp = pa[i];
        pa[i] = pb[i];
        pb[i] = tmp;
    }
}


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

    return 1;
}

void
get_dominant_color(Image *image, Color color)
{
    // FIXME: This function is disgusting and only works for bpp = 3,
    // aka RGB colorspaces, no alpha. Uses and ungodly amount of memory,
    // though only briefly. Also requires <string.h> and <stdio.h> to be pulled in.
    // does not seem to be particularly useful either, as it only grabs one color.
     
    unsigned char **colors = full_scan(image);
    unsigned long *count = malloc(0xffffff * sizeof(unsigned long));
    memset(count, 0, 0xffffff * sizeof(unsigned long));

    for (int i = 0; i < image->width * image->height; ++i) {
        char colorval[8]; 
        sprintf(colorval, "%02x%02x%02x", colors[i][0], colors[i][1], colors[i][2]);
        unsigned long color_id = strtol(colorval, NULL, 16);
        count[color_id]++;
    }
    unsigned long highest = 0;
    int highest_index = 0;
    for (int i = 0; i < 0xffffff; ++i) {
        if (count[i] > highest) {
            highest = count[i];
            highest_index = i;
        }
    }
    char cv[10];
    char* p = cv;
    sprintf(cv, "%x", highest_index);
    char r[3], g[3], b[3];

    memcpy(r, p, 2);
    r[2] = '\0';
    memcpy(g, p += 2, 2);
    g[2] = '\0';
    memcpy(b, p + 2, 2);
    b[2] = '\0';
    color[0] = (unsigned char)strtol(r, NULL, 16);
    color[1] = (unsigned char)strtol(g, NULL, 16);
    color[2] = (unsigned char)strtol(b, NULL, 16);
    free(count);
}

int
line_diff(Image *source, Image *copy, int x0, int y0, int x1, int y1, int copy_mode)
{
    // Account for line with negative direction
    if (x1 < x0) {
        swap(&x0, &x1, sizeof(int));
        swap(&y0, &y1, sizeof(int));
    } 

    int diff = 0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int err = 0;
    int y = y0;
    int x = x0;
    int stepx, stepy;
    stepx = stepy = 1;

    if (dx < 0) { dx = -dx; stepx = -1; }
    if (dy < 0) { dy = -dy; stepy = -1; }

    if (dy > dx) {
        if (stepy < 0) {
            for (y = y0; y >= y1; y+=stepy) {
                if (copy_mode)
                    copy_plot(source, copy, x, y);
                else
                    diff += calc(source, copy, x, y);
                err = err + dx;
                if ((err << 1) >= dy) {
                    x += stepx;
                    err = err - dy;
                }
            }
        } else {
            for (y = y0; y <= y1; y+=stepy) {
                if (copy_mode)
                    copy_plot(source, copy, x, y);
                else
                    diff += calc(source, copy, x, y);
                err += dx;
                if ((err << 1) >= dy) {
                    x += stepx;
                    err -= dy;
                }
            }
        }
    } 

    else {
        for (x = x0; x <= x1; x+=stepx) {
            if (copy_mode)
                copy_plot(source, copy, x, y);
            else
                diff += calc(source, copy, x, y);
            err += dy;
            if ((err << 1) >= dx) {
                y += stepy;
                err -= dx;
            }
        }
    }
    return diff;
}


void
draw_line(Image *image, Color color, 
          int x0, int y0, int x1, int y1)
{
    // Account for line with negative direction
    if (x1 < x0) {
        swap(&x0, &x1, sizeof(int));
        swap(&y0, &y1, sizeof(int));
    } 

    int dx = x1 - x0;
    int dy = y1 - y0;
    int err = 0;
    int y = y0;
    int x = x0;
    int stepx, stepy;
    stepx = stepy = 1;

    if (dx < 0) { dx = -dx; stepx = -1; }
    if (dy < 0) { dy = -dy; stepy = -1; }

    if (dy > dx) {
        if (stepy < 0) {
            for (y = y0; y >= y1; y+=stepy) {
                plot(x, y, image, color);
                err = err + dx;
                if ((err << 1) >= dy) {
                    x += stepx;
                    err = err - dy;
                }
            }
        } else {
            for (y = y0; y <= y1; y+=stepy) {
                plot(x, y, image, color);
                err += dx;
                if ((err << 1) >= dy) {
                    x += stepx;
                    err -= dy;
                }
            }
        }
    } 

    else {
        for (x = x0; x <= x1; x+=stepx) {
            plot(x, y, image, color);
            err += dy;
            if ((err << 1) >= dx) {
                y += stepy;
                err -= dx;
            }
        }
    }
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

    return 1;
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

    return 1;
}


void
draw_circle(int x0, int y0, int radius, int filled, Color c, Image *image)
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

int 
calc(Image *source, Image *copy, int x, int y)
{
    int source_target     = (x + (y * source->width)) * source->bpp;
    int copy_target       = (x + (y * source->width)) * source->bpp;

    source_target         = source_target < source->size ? 
                            source_target : source->size;

    copy_target           = copy_target < copy->size ? 
                            copy_target : copy->size;

    source_target         = source_target > 0 ? source_target : 0;
    copy_target           = copy_target > 0 ? copy_target : 0;

    unsigned char red_s   = source->data[source_target];
    unsigned char green_s = source->data[source_target + 1];
    unsigned char blue_s  = source->data[source_target + 2];
    unsigned char red_c   = copy->data[copy_target];
    unsigned char green_c = copy->data[copy_target + 1];
    unsigned char blue_c  = copy->data[copy_target + 2];

    int rd, gd, bd;
    rd = abs(red_s - red_c);
    gd = abs(green_s - green_c);
    bd = abs(blue_s - blue_c);
    int diff = rd + gd + bd;
    return diff;
}

int
circ_diff(int x0, int y0, int radius, Image *source, Image * copy, int mode)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    int diff = 0;
    while (x >= y) {

        int i;
        if (mode) {
            for (i = x0 - x; i < x0 + x; i++) {
                copy_plot(copy, source, i, y0 + y);
                copy_plot(copy, source, i, y0 - y);
            }

            for (i = x0 - y; i < x0 + y; i++) {
                copy_plot(copy, source, i, y0 + x);
                copy_plot(copy, source, i, y0 - x);
            }

            /*
            for (i = x0 - radius; i < x0 + radius; ++i) {
                copy_plot(copy, source, i, y0 + y);
                copy_plot(copy, source, i, y0 - y);
                copy_plot(copy, source, i, y0 + x);
                copy_plot(copy, source, i, y0 - x);
            }
            */
        } 

        else {
            for (i = x0 - radius; i < x0 + radius; ++i) {
                // TODO below we are diffing based on the average color value
                // for the ENTIRE circle, it might be faster to diff on a single
                // line of pixels, as in the broken line function, and might not
                // impact the accuracy of the diff.
                
                diff += calc(copy, source, i, y0 + y);
                diff += calc(copy, source, i, y0 - y);
                diff += calc(copy, source, i, y0 + x);
                diff += calc(copy, source, i, y0 - x);
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

int
broken_line_diff(int x0, int y0, int radius, Image *source, Image * copy, int mode)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    int diff = 0;

    while (x >= y) {
        int i;
        int target = (x + (y * source->width * source->height * source->bpp));
        if (target < source->size) {
            for (i = x0 - x; i < x0 + x; i++) {
                if (mode) {
                        copy_plot(copy, source, i, y0 - y);
                        copy_plot(copy, source, i, y0 + y);
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

unsigned char**
full_scan(Image *image) {

    int i, j;

    int image_end = image->width * image->height;

    unsigned char **colors = 
        malloc(image->width * image->height * sizeof(char*));

    for (i = 0; i < image_end; ++i) {
        colors[i] = malloc(3);
        for (j = 0; j < image->bpp; ++j) {
            colors[i][j] = image->data[(i * image->bpp) + j];
        }
    }

    return colors;
}

void
siml_free(unsigned char** colors, size_t nmemb)
{
    size_t i;
    for (i = 0; i < nmemb; ++i) {
        free(colors[i]);
    }

    free(colors);
}


void
grid_scan(Image *image, unsigned char colors[][3], int numc) 
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
sloppy_scan(Image *image, unsigned char colors[][3], int numc) 
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

void
sample_point(Image *image, unsigned char color[], int x, int y)
{
    int j;
    x *= image->bpp;
    for (j = 0; j < image->bpp; ++j) {
        color[j] = image->data[x + (y * image->width * image->bpp) + j];
    }
}

