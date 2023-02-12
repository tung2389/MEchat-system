#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *malloc_str(int len) {
    char *p = malloc(len + 1);
    p[len] = '\0';
    return p;
}

char *fgets_str(char *str, int num , FILE *stream) {
    fgets(str, num + 1, stream);
    str[strlen(str) - 1] = '\0';
    return str;
}