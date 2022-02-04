#include <stdio.h>
#include <stdlib.h>

char *malloc_str(int len) {
    char *p = malloc(len + 1);
    p[len] = '\0';
    return p;
}