#ifndef SIML_H
#define SIML_H
#define MAX_COLOR_DEPTH_BYTES 4

typedef unsigned char Color[MAX_COLOR_DEPTH_BYTES];

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

typedef struct {
    Color color;
    int frequency;
} Scanned_color;


int   broken_line_diff      (int x0, int y0, int radius, Image *source, Image * copy, int mode);
int   calc                  (Image *source, Image *copy, int x, int y);
int   circ_diff             (int x0, int y0, int radius, Image *source, Image * copy, int mode);
int   copy_plot             (Image *dest, Image *source, int x, int y);
void  draw_circle           (int h, int k, int r, int filled, Color c, Image *image);
void  draw_cross            (int h, int k, int r,  Color c, Image *image);
void  draw_elipse           (int h, int k, int r, int filled, Color c, Image *image);
void  draw_line             (Image *image, Color color, int x0, int y0, int x1, int y1);
void  draw_rect             (int x, int y, int w, int h, Color c, Image *image);
int   filter                (int x, int y, Image *image, Color color, int cv);
void  filter_circle         (int h, int k, int r, int filled, int cv, Color c, Image *image);
unsigned char** full_scan   (Image *image);
void  grid_scan             (Image *image, unsigned char colors[][3], int numc);
int   line_diff             (Image *source, Image *copy, int x0, int y0, int x1, int y1, int copy_mode);
void  midpoint_circle       (int ux, int uy, int radius, int filled, Color c, Image *image);
void  midpoint_diff         (int x0, int y0, int radius, Image *source, Image *copy);
int   plot                  (int x, int y, Image *image, Color color);
void  reduce_color_pallete  (Image *image, unsigned char **new_pallete, int num_colors);
void  sample_point          (Image *image, unsigned char color[], int x, int y);
void  siml_free             (unsigned char **colors, unsigned long nmemb);
void  sloppy_scan           (Image *image, unsigned char colors[][3], int numc);
void  swap                  (void *a, void *b, unsigned long len);
#endif
