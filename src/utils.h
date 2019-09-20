#ifndef UTILS_H
#define UTILS_H

// Name of running program
char const *progname;

/**
 * Implementation of malloc which panics on error. Result is therefore
 * guaranteed of being non-null
 */
void *xmalloc(size_t size);

/**
 * Implementation of realloc which panics on error. Result is therefore
 * guaranteed of being non-null
 */
void *xrealloc(void *ptr, size_t newsize);

#endif /* UTILS_H */
