/* mhash_sha256_sha224.h
 *
 * The sha256 and sha224 hash functions.
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001 Niels Möller
 *  
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 * 
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

/* The SHA224 support was added by B. Poettering in April 2004. */

#if defined(ENABLE_SHA256_SHA224)
 
#if !defined(__MHASH_SHA256_SHA224_H_INCLUDED)
#define __MHASH_SHA256_SHA224_H_INCLUDED


/* SHA256 and SHA224 */

#define SHA256_DIGEST_SIZE 32
#define SHA224_DIGEST_SIZE 28
#define SHA256_SHA224_DATA_SIZE 64

/* State is kept internally as 8 32-bit words. */
#define _SHA256_SHA224_DIGEST_LENGTH 8

typedef struct sha256_sha224_ctx
{
  mutils_word32 state[_SHA256_SHA224_DIGEST_LENGTH]; /* State variables */
  mutils_word64 bitcount;                            /* Bit counter */
  mutils_word8 block[SHA256_SHA224_DATA_SIZE];        /* SHA256/224 data buffer */
  mutils_word32 index;                         /* index into buffer */
} SHA256_SHA224_CTX;

void
sha256_init(struct sha256_sha224_ctx *ctx);

void
sha224_init(struct sha256_sha224_ctx *ctx);

void
sha256_sha224_update(struct sha256_sha224_ctx *ctx, __const mutils_word8 *data, mutils_word32 length);

void
sha256_sha224_final(struct sha256_sha224_ctx *ctx);

void
sha256_digest(__const struct sha256_sha224_ctx *ctx, mutils_word8 *digest);

void
sha224_digest(__const struct sha256_sha224_ctx *ctx, mutils_word8 *digest);


#endif /* __MHASH_SHA256_SHA224_H_INCLUDED */

#endif
