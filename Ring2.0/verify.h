#ifndef RINGCT_VERIFY_H
#define RINGCT_VERIFY_H

#include <stddef.h>

/* returns 0 for equal strings, 1 for non-equal strings */
int verify(const unsigned char *a, const unsigned char *b, size_t len);

/* b = 1 means mov, b = 0 means don't mov*/
void cmov(unsigned char *r, const unsigned char *x, size_t len, unsigned char b);

#endif
