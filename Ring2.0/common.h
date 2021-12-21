#ifndef RINGCT_COMMON_H
#define RINGCT_COMMON

#include <stdint.h>
#include <stdlib.h>

/**
 * Zeros out `len` bytes of memory starting at `ptr`.
 *
 * Designed to be protected against optimizing compilers which try to remove "unnecessary"
 * operations.  Should be used for all buffers containing secret data.
 *
 * @param[in] ptr The start of the memory to zero out.
 * @param[in] len The number of bytes to zero out.
 */
void mem_cleanse(void *ptr, size_t len);

/**
 * Zeros out `len` bytes of memory starting at `ptr`, then frees `ptr`.
 *
 * Can be called with `ptr = NULL`, in which case no operation is performed.
 *
 * Designed to be protected against optimizing compilers which try to remove "unnecessary"
 * operations.  Should be used for all buffers containing secret data.
 *
 * @param[in] ptr The start of the memory to zero out and free.
 * @param[in] len The number of bytes to zero out.
 */
void mem_secure_free(void *ptr, size_t len);

/**
 * Frees `ptr`.
 *
 * Can be called with `ptr = NULL`, in which case no operation is performed.
 *
 * Should only be used on non-secret data.
 *
 * @param[in] ptr The start of the memory to free.
 */
void mem_insecure_free(void *ptr);

/**
*function: print bBuff array
*@param[in] buf The array will be printed
*@param[in] len the number of bytes
*/
void print_bytes(unsigned char *buf, size_t len);

#endif // RINGCT_COMMON_H
