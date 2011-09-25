#if defined(ENABLE_TIGER)

#if !defined(__MHASH_TIGER_H)
#define __MHASH_TIGER_H

#include "libdefs.h"

#define TIGER_DIGESTSIZE 24
#define TIGER160_DIGESTSIZE 20
#define TIGER128_DIGESTSIZE 16
#define TIGER_DATASIZE 64

#ifdef TIGER_64BIT

#define h0init 0x0123456789ABCDEFLL
#define h1init 0xFEDCBA9876543210LL
#define h2init 0xF096A5B4C3B2E187LL

#define TIGER_DATALEN 8
#define TIGER_DIGESTLEN 3
#define TIGER128_DIGESTLEN 2
#define TIGER160_DIGESTLEN 2 /* 2.5 actually. */

typedef struct tiger_ctx {
  mutils_word64 digest[TIGER_DIGESTLEN];  /* Message digest */ 
  mutils_word64 count;	 /* 64-bit block count */
  mutils_word8 block[TIGER_DATASIZE];     /* RIPEMD data buffer */  
  mutils_word32 index;                             /* index into buffer */
} TIGER_CTX;


#else

#define h0init 0x89ABCDEF;
#define h1init 0x01234567;
#define h2init 0x76543210;
#define h3init 0xFEDCBA98;
#define h4init 0xC3B2E187;
#define h5init 0xF096A5B4;

#define TIGER_DATALEN 16
#define TIGER_DIGESTLEN 6
#define TIGER128_DIGESTLEN 4
#define TIGER160_DIGESTLEN 5

typedef struct tiger_ctx {
  mutils_word32 digest[TIGER_DIGESTLEN];  /* Message digest */ 
  mutils_word32 count_l, count_h;	 /* 64-bit block count */
  mutils_word8 block[TIGER_DATASIZE];     /* RIPEMD data buffer */  
  mutils_word32 index;                             /* index into buffer */
} TIGER_CTX;

#endif /* !TIGER_64BIT */

void tiger_digest(struct tiger_ctx *ctx, mutils_word8 * s);
void tiger_digest160(struct tiger_ctx *ctx, mutils_word8 * s);
void tiger_digest128(struct tiger_ctx *ctx, mutils_word8 * s);
void tiger_final(struct tiger_ctx *ctx);
void tiger_update(struct tiger_ctx *ctx, mutils_word8 * buffer, mutils_word32 len);
void tiger_init(struct tiger_ctx *ctx);
void tiger128_digest(struct tiger_ctx *ctx, mutils_word8 * s);
void tiger160_digest(struct tiger_ctx *ctx, mutils_word8 * s);

#endif

#endif
