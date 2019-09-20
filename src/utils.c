#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

char const *progname = NULL;

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "%s: Virtual memory exhausted\n", progname);
        abort();
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t newsize) {
    ptr = realloc(ptr, newsize);
    if (ptr == NULL) {
        fprintf(stderr, "%s: Virtual memory exhausted\n", progname);
        abort();
    }
    return ptr;
}
