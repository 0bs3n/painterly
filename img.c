#include <stdio.h>
#include <stdlib.h>
#include "bacmp.c"

int
main(void)
{
    size_t size;
    size_t checked;
    size_t i;
    char *fc;
    FILE *ep = fopen("./test.png", "rb");
    FILE *np = fopen("./out.png", "wb");

    fseek(ep, 0, SEEK_END);
    size = ftell(ep);
    rewind(ep);
    fc = malloc(size);

    if (!ep) {
        printf("No file\n");
        exit(1);
    }

    if ((checked = fread(fc, sizeof *fc, size, ep)) != size)  {
        printf("Sizing error: %lu != %lu\n", size, checked);
        exit(1);
    }

    for (i = 0; i < size; i++) {
        fprintf(np, "%c", *(fc + i));
    }

    return 0;
}
