#include <stdio.h>
#include "rand.h"
#if defined(_WIN32)
// windows way to use random
#else
#if defined(__APPLE__)
#include <sys/random.h>
unsigned long long int randombytes(unsigned char* ptr, unsigned long long num) {
    unsigned long long tail = num;
    unsigned long long multiplicator = 0;
    while(tail != 0) {
        getentropy(ptr + multiplicator * 256, tail > 256 ? 256 : tail);
        multiplicator++;
        tail -= tail > 256 ? 256 : tail;
    }
    return num;
}
#else
#include <unistd.h>
int randombytes(unsigned char* ptr, unsigned long long num) {
    return getrandom(ptr, num);
}
#endif
#endif
