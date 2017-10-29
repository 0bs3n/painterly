#include <stdlib.h>
#include <stdio.h>

int
diff(char* a, char* b, size_t size)
{
    size_t i;
    int diff = 0;
    for (i = 0; i < (size); i++) {
        if (a[i] != b[i]) diff++;
    }

    return diff;
}

int 
not_main() 
{
    char a[] = {88, 44, 18, 22, 98};
    char b[] = {88, 44, 18, 22, 98};
    char* c  = malloc(5);
    printf("%d\n", *(a + 8));
    printf("%d\n", *(c - 10));
    printf("%d\n", diff(a, b, 6));
    return 0;
}

