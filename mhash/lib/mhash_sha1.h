#if defined(ENABLE_SHA1)

#if !defined(__MHASH_SHA1_H)
#define __MHASH_SHA1_H

#include <libdefs.h>

#define sha_init 		mhash_sha_init
#define sha_update 		mhash_sha_update
#define sha_final 		mhash_sha_final
#define sha_digest 		mhash_sha_digest
#define sha_copy 		mhash_sha_copy

/* The SHA block size and message digest sizes, in bytes */

#define SHA_DATASIZE    64
#define SHA_DATALEN     16
#define SHA_DIGESTSIZE  20
#define SHA_DIGESTLEN    5
/* The structure for storing SHA info */

typedef struct sha_ctx {
  mutils_word32 digest[SHA_DIGESTLEN];  /* Message digest */
  mutils_word32 count_l, count_h;       /* 64-bit block count */
  mutils_word8 block[SHA_DATASIZE];     /* SHA data buffer */
  mutils_word32 index;                  /* index into buffer */
} SHA_CTX;

void sha_init(struct sha_ctx *ctx);
void sha_update(struct sha_ctx *ctx, mutils_word8 *buffer, mutils_word32 len);
void sha_final(struct sha_ctx *ctx);
void sha_digest(struct sha_ctx *ctx, mutils_word8 *s);
void sha_copy(struct sha_ctx *dest, struct sha_ctx *src);

#if 1

#ifndef EXTRACT_UCHAR
#define EXTRACT_UCHAR(p)  (*(mutils_word8 *)(p))
#endif

#define STRING2INT(s) ((((((EXTRACT_UCHAR(s) << 8)    \
			 | EXTRACT_UCHAR(s+1)) << 8)  \
			 | EXTRACT_UCHAR(s+2)) << 8)  \
			 | EXTRACT_UCHAR(s+3))
#else
mutils_word32 STRING2INT(mutils_word8 *s)
{
  mutils_word32 r;
  mutils_word32 i;
  
  for (i = 0, r = 0; i < 4; i++, s++)
    r = (r << 8) | *s;
  return r;
}
#endif

#endif

#endif
