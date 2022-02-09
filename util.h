#ifndef UTIL
#define UTIL

// Dynamically allocate space for a string whose length is len
char *malloc_str(int len);
// fgets, but exclude the last newline character. This method is used to remove the \n character when reading from stdin
char *fgets_str(char *str, int num , FILE *stream);

#endif