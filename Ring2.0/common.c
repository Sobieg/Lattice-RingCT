#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#endif

void mem_cleanse(void *ptr, size_t len) {
#if defined(_WIN32)
	SecureZeroMemory(ptr, len);
#elif defined(HAVE_MEMSET_S)
	if (0U < len && memset_s(ptr, (rsize_t) len, 0, (rsize_t) len) != 0) {
		abort();
	}
#else
	typedef void *(*memset_t)(void *, int, size_t); //is it defends?
	static volatile memset_t memset_func = memset;
	memset_func(ptr, 0, len);
#endif
}
void mem_secure_free(void *ptr, size_t len) {
	if (ptr != NULL) {
        mem_cleanse(ptr, len);
		free(ptr); // IGNORE free-check
	}
}

void mem_insecure_free(void *ptr) {
	free(ptr); // IGNORE free-check
}

void print_bytes(unsigned char *buf, size_t len)
{
	size_t i = 0;
	for (i = 0; i < len; i++)
	{
		printf("%02X", buf[i]);
	}
	printf("\n");
}
