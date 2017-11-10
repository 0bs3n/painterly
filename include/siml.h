#ifndef SIMG_H
#define SIMG_H
#include <math.h>


typedef struct {
    int height;
    int thickness;
    int x, y;
} Rect;

typedef struct {
    int width;
    int height;
    int bpp;
    int size;
    unsigned char* data;
} Image;

typedef char Color[4];

void draw_circle(int h, int k, int r, int filled, Color c, Image *image);
void draw_cross(int h, int k, int r,  Color c, Image *image);
void draw_elipse(int h, int k, int r, int filled, Color c, Image *image);
void draw_rect(int x, int y, int w, int h, Color c, Image *image);
float calc(Image *source, Image *copy, int x, int y);
int circ_diff(int x0, int y0, int radius, Image *source, Image * copy, int mode);
int  filter(int x, int y, Image *image, Color color, int cv);
void filter_circle(int h, int k, int r, int filled, int cv, Color c, Image *image);
void midpoint_circle(int ux, int uy, int radius, int filled, Color c, Image *image);
void midpoint_diff(int x0, int y0, int radius, Image *source, Image *copy);
int  plot(int x, int y, Image *image, Color color);
void scan(Image *image, char colors[][3], int numc);
void scan_2(Image *image, char colors[][3], int numc);
#endif
