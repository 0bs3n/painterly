#include "id.h"
#include <stb_image.h>
#include <stb_image_write.h>

int 
main(int argc, char **argv) 
{
    if (argc < 3) {
        printf("need arguments");
        exit(1);
    }
    int color_depth = atoi(argv[2]);
    srand(time(NULL));
    Image image;
    char colors[color_depth][3];

    image.data = stbi_load(
            argv[1], 
            &image.width, 
            &image.height, 
            &image.bpp, 0);

    image.size = image.width * image.height * image.bpp;

    unsigned char* data_working = malloc(image.size);
    unsigned char* data_backup = malloc(image.size);

    scan(&image, colors, color_depth);
    memset(data_working, 0xff, image.size);
    memset(data_backup, 0xff, image.size);

    Image working = { 
        .data = data_working, 
        .height = image.height, 
        .width = image.width, 
        .bpp = image.bpp 
    };

    Image backup = { 
        .data = data_backup, 
        .height = image.height, 
        .width = image.width, 
        .bpp = image.bpp 
    };

    working.size = working.bpp * working.width * working.height;
    backup.size = backup.bpp * backup.width * backup.height;

    int x, y;
    int c;
    int wd, bd;
    int i = 0;
    int r = 10;
    while (i < 1000000) {
        x = rand() % image.width;
        y = rand() % image.height;
        c = rand() % 64;
        
        midpoint_circle(x, y, r, 1, colors[c], &working);

        wd = circ_diff(x, y, r, &image, &working, 0);
        bd = circ_diff(x, y, r, &image, &backup, 0);

        if (wd < bd) 
            circ_diff(x, y, r, &working, &backup, 1);
        else
            circ_diff(x, y, r, &backup, &working, 1);

        ++i; 
    }

    // Currently, and initially unintentionally, circ_diff in mode=1 copies only slivers of circles,
    // rather than the whole circle, as intended. It looks pretty nice but I'd like to figure out why.
    // also, because backup is drawn of slivers, while working is made of circles, the calls to
    // circ_diff() attempting to copy from backup to working (to correct erroneous placements of circles)
    // have little effected, reverting only a sliver of the image to its previous state. 
    // This results in working.bmp being untirely unrecognizeable compared to 
    // the original image, but backup comes out correctly. Once I figure out 
    // how to copy whole circles back and forth, I can determine whether this 
    // is actually desirable behaviour.

    printf("Width: %d\nHeight %d\nBytes per pixels: %d\nSize: %d\nIterations: %d\n",
            image.width, image.height, image.bpp, image.size, i);

    stbi_write_bmp("working2.bmp", working.width, working.height, working.bpp, working.data);
    stbi_write_bmp("backup2.bmp", backup.width, backup.height, backup.bpp, backup.data);
    
    return 0;
}

