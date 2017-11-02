#ifndef ID_H
#define ID_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


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

typedef char Color[4];

void draw_circle(int h, int k, int r, int filled, Color c, Image *image);
void draw_cross(int h, int k, int r,  Color c, Image *image);
void draw_elipse(int h, int k, int r, int filled, Color c, Image *image);
void draw_rect(int x, int y, int w, int h, Color c, Image *image);
int  filter(int x, int y, Image *image, Color color, int cv);
void filter_circle(int h, int k, int r, int filled, int cv, Color c, Image *image);
int  plot(int x, int y, Image *image, Color color);
void scan(Image *image, char colors[][3], int numc);
#endif
