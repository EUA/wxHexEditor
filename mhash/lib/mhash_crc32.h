#if defined(ENABLE_CRC32)

#if !defined(__MHASH_CRC32_H)
#define __MHASH_CRC32_H

#include "libdefs.h"

void mhash_clear_crc32(mutils_word32 * crc);
void mhash_get_crc32( __const mutils_word32 *crc, void *ret);
void mhash_crc32(mutils_word32 *crc, __const void *given_buf, mutils_word32 len);
void mhash_crc32b(mutils_word32 *crc, __const void *given_buf, mutils_word32 len);

#endif

#endif
