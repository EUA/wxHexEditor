#if defined(ENABLE_ADLER32)

#if !defined(MHASH_ADLER32_H)
#define MHASH_ADLER32_H

#include "libdefs.h"

void mhash_clear_adler32(mutils_word32 * adler);
void mhash_get_adler32( __const mutils_word32 * adler, void* ret);
void mhash_adler32(mutils_word32 * adler, __const void *, mutils_word32 len);

#endif

#endif
