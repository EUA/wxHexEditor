/* sha512_sha384.c
 *
 * The sha512 and sha384 secure hash functions.
 * Copyright (C) 2004 B. Poettering
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This code heavily borrows from the sha256/sha1 code written by 
 * Niels Möller and Peter Gutmann (see sha256_sha224.c of the mhash library)
 */ 

#include "libdefs.h"

#ifdef ENABLE_SHA512_SHA384

#include "mhash_sha512_sha384.h"

/* A block, treated as a sequence of 64-bit words. */
#define SHA512_SHA384_DATA_LENGTH 16

#define ROTR(n,x) ((x)>>(n) | ((x)<<(64-(n))))
#define SHR(n,x) ((x)>>(n))

/* The SHA512/384 functions. The Choice function is the same as the SHA1
   function f1, and the majority function is the same as the SHA1 f3
   function. They can be optimized to save one boolean operation each
   - thanks to Rich Schroeppel, rcs@cs.arizona.edu for discovering
   this */

/* #define Choice(x,y,z) ( ( (x) & (y) ) | ( ~(x) & (z) ) ) */
#define Choice(x,y,z)   ( (z) ^ ( (x) & ( (y) ^ (z) ) ) )
/* #define Majority(x,y,z) ( ((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)) ) */
#define Majority(x,y,z) ( ((x) & (y)) ^ ((z) & ((x) ^ (y))) )

#define S0(x) (ROTR(28,(x)) ^ ROTR(34,(x)) ^ ROTR(39,(x)))
#define S1(x) (ROTR(14,(x)) ^ ROTR(18,(x)) ^ ROTR(41,(x)))

#define s0(x) (ROTR(1,(x)) ^ ROTR(8,(x)) ^ SHR(7,(x)))
#define s1(x) (ROTR(19,(x)) ^ ROTR(61,(x)) ^ SHR(6,(x)))

/* The sha512/384 round __constants. */
static __const mutils_word64 K[80] = {
  0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 
  0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 
  0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 
  0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 
  0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 
  0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 
  0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL, 
  0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 
  0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 
  0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 
  0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 
  0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL, 
  0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 
  0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 
  0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL, 
  0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 
  0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 
  0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 
  0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 
  0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL, 
  0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 
  0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 
  0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
  0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 
  0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 
  0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 
  0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL 
};

/* The initial expanding function.  The hash function is defined over an
   80-word expanded input array W, where the first 16 are copies of the input
   data, and the remaining 64 are defined by

        W[ t ] = s1(W[t-2] + W[t-7] + s0(W[i-15] + W[i-16]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.
*/

#define EXPAND(W,i) \
( W[(i) & 15 ] += (s1(W[((i)-2) & 15]) + W[((i)-7) & 15] + s0(W[((i)-15) & 15])) )

/* The prototype SHA sub-round.  The fundamental sub-round is:

        T1 = h + S1(e) + Choice(e,f,g) + K[t] + W[t]
	T2 = S0(a) + Majority(a,b,c)
	a' = T1+T2
	b' = a
	c' = b
	d' = c
	e' = d + T1
	f' = e
	g' = f
	h' = g

   but this is implemented by unrolling the loop 8 times and renaming
   the variables
   ( h, a, b, c, d, e, f, g ) = ( a, b, c, d, e, f, g, h ) each
   iteration. This code is then replicated 8, using the next 8 values
   from the W[] array each time */

#define ROUND(a,b,c,d,e,f,g,h,k,data) do {		\
  mutils_word64 T1 = h + S1(e) + Choice(e,f,g) + k + data;	\
  d += T1;						\
  h = T1 + S0(a) + Majority(a,b,c);		        \
} while (0)


/* Helper macros derived from those in mhash_sha1.h */

#ifndef EXTRACT_UCHAR
#define EXTRACT_UCHAR(p)  (*(unsigned char *)(p))
#endif

#define STRING2INT64(s) ((((((((((((((mutils_word64)(EXTRACT_UCHAR(s) << 8)    \
			 | EXTRACT_UCHAR(s+1)) << 8)  \
			 | EXTRACT_UCHAR(s+2)) << 8)  \
			 | EXTRACT_UCHAR(s+3)) << 8)  \
			 | EXTRACT_UCHAR(s+4)) << 8)  \
			 | EXTRACT_UCHAR(s+5)) << 8)  \
			 | EXTRACT_UCHAR(s+6)) << 8)  \
			 | EXTRACT_UCHAR(s+7))

/* Initialize the SHA512/384 values */

void sha512_init(struct sha512_sha384_ctx *ctx)
{
  /* Initial values */
	static __const mutils_word64 H0[_SHA512_SHA384_STATE_LENGTH] = {
	  0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 
	  0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 
	  0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
	};

	mutils_memcpy(ctx->state, H0, sizeof(H0));

	/* Initialize bit count */
	ctx->bitcount_low = ctx->bitcount_high = 0;

	/* Initialize buffer */
	ctx->index = 0;
}

void sha384_init(struct sha512_sha384_ctx *ctx)
{
  /* Initial values */
	static __const mutils_word64 H0[_SHA512_SHA384_STATE_LENGTH] = {
	  0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, 0x9159015a3070dd17ULL,
	  0x152fecd8f70e5939ULL, 0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
	  0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL,
	};

	mutils_memcpy(ctx->state, H0, sizeof(H0));

	/* Initialize bit count */
	ctx->bitcount_low = ctx->bitcount_high = 0;

	/* Initialize buffer */
	ctx->index = 0;
}

/* Perform the SHA transformation.  Note that this code, like MD5, seems to
   break some optimizing compilers due to the complexity of the expressions
   and the size of the basic block.  It may be necessary to split it into
   sections, e.g. based on the four subrounds

   Note that this function destroys the data area */

static void sha512_sha384_transform(mutils_word64 * state, mutils_word64 * data)
{
	mutils_word64 A, B, C, D, E, F, G, H;	/* Local vars */
	mutils_word8 i;
	__const mutils_word64 *k;
	mutils_word64 *d;

	/* Set up first buffer and local data buffer */
	A = state[0];
	B = state[1];
	C = state[2];
	D = state[3];
	E = state[4];
	F = state[5];
	G = state[6];
	H = state[7];

	/* Heavy mangling */
	/* First 16 subrounds that act on the original data */

	for (i = 0, k = K, d = data; i < 16; i += 8, k += 8, d += 8) {
		ROUND(A, B, C, D, E, F, G, H, k[0], d[0]);
		ROUND(H, A, B, C, D, E, F, G, k[1], d[1]);
		ROUND(G, H, A, B, C, D, E, F, k[2], d[2]);
		ROUND(F, G, H, A, B, C, D, E, k[3], d[3]);
		ROUND(E, F, G, H, A, B, C, D, k[4], d[4]);
		ROUND(D, E, F, G, H, A, B, C, k[5], d[5]);
		ROUND(C, D, E, F, G, H, A, B, k[6], d[6]);
		ROUND(B, C, D, E, F, G, H, A, k[7], d[7]);
	}

	for (; i < 80; i += 16, k += 16) {
		ROUND(A, B, C, D, E, F, G, H, k[0], EXPAND(data, 0));
		ROUND(H, A, B, C, D, E, F, G, k[1], EXPAND(data, 1));
		ROUND(G, H, A, B, C, D, E, F, k[2], EXPAND(data, 2));
		ROUND(F, G, H, A, B, C, D, E, k[3], EXPAND(data, 3));
		ROUND(E, F, G, H, A, B, C, D, k[4], EXPAND(data, 4));
		ROUND(D, E, F, G, H, A, B, C, k[5], EXPAND(data, 5));
		ROUND(C, D, E, F, G, H, A, B, k[6], EXPAND(data, 6));
		ROUND(B, C, D, E, F, G, H, A, k[7], EXPAND(data, 7));
		ROUND(A, B, C, D, E, F, G, H, k[8], EXPAND(data, 8));
		ROUND(H, A, B, C, D, E, F, G, k[9], EXPAND(data, 9));
		ROUND(G, H, A, B, C, D, E, F, k[10], EXPAND(data, 10));
		ROUND(F, G, H, A, B, C, D, E, k[11], EXPAND(data, 11));
		ROUND(E, F, G, H, A, B, C, D, k[12], EXPAND(data, 12));
		ROUND(D, E, F, G, H, A, B, C, k[13], EXPAND(data, 13));
		ROUND(C, D, E, F, G, H, A, B, k[14], EXPAND(data, 14));
		ROUND(B, C, D, E, F, G, H, A, k[15], EXPAND(data, 15));
	}

	/* Update state */
	state[0] += A;
	state[1] += B;
	state[2] += C;
	state[3] += D;
	state[4] += E;
	state[5] += F;
	state[6] += G;
	state[7] += H;
}

static void 
sha512_sha384_block(struct sha512_sha384_ctx *ctx, __const mutils_word8 * block)
{
	mutils_word64 data[SHA512_SHA384_DATA_LENGTH];
	mutils_word32 i;

	/* Update bit counter */
	if ((ctx->bitcount_low += 1024) < 1024)
	{
		ctx->bitcount_high++;
	}

	/* Endian independent conversion */
	for (i = 0; i < SHA512_SHA384_DATA_LENGTH; i++, block += 8)
	{
		data[i] = STRING2INT64(block);
	}

	sha512_sha384_transform(ctx->state, data);
}

void
sha512_sha384_update(struct sha512_sha384_ctx *ctx, __const mutils_word8 * buffer, 
		     mutils_word32 length)
{
	mutils_word32 left;

	if (ctx->index)
	{	/* Try to fill partial block */
		left = SHA512_SHA384_DATA_SIZE - ctx->index;
		if (length < left)
		{
			mutils_memcpy(ctx->block + ctx->index, buffer, length);
			ctx->index += length;
			return;	/* Finished */
		} else {
			mutils_memcpy(ctx->block + ctx->index, buffer, left);
			sha512_sha384_block(ctx, ctx->block);
			buffer += left;
			length -= left;
		}
	}
	while (length >= SHA512_SHA384_DATA_SIZE)
	{
		sha512_sha384_block(ctx, buffer);
		buffer += SHA512_SHA384_DATA_SIZE;
		length -= SHA512_SHA384_DATA_SIZE;
	}
/* Buffer leftovers */
/* NOTE: The corresponding sha1 code checks for the special case length == 0.
 * That seems supoptimal, as I suspect it increases the number of branches. */

	mutils_memcpy(ctx->block, buffer, length);
	ctx->index = length;
}

/* Final wrapup - pad to SHA512_SHA384_DATA_SIZE-byte boundary with the bit pattern
   1 0* (128-bit count of bits processed, MSB-first) */

void sha512_sha384_final(struct sha512_sha384_ctx *ctx)
{
	mutils_word64 data[SHA512_SHA384_DATA_LENGTH];
	mutils_word32 i;
	mutils_word32 words;

	i = ctx->index;

	/* Set the first char of padding to 0x80.  This is safe since there is
	   always at least one byte free */

/*  assert(i < SHA512_SHA384_DATA_SIZE);
 */
	ctx->block[i++] = 0x80;

	/* Fill rest of word */
	for (; i & 7; i++)
		ctx->block[i] = 0;

	/* i is now a multiple of the word size 8 */
	words = i >> 3;
	for (i = 0; i < words; i++)
		data[i] = STRING2INT64(ctx->block + 8 * i);

	if (words > (SHA512_SHA384_DATA_LENGTH - 2)) {	/* No room for 
                 length in this block. Process it and pad with another one */
		for (i = words; i < SHA512_SHA384_DATA_LENGTH; i++)
			data[i] = 0;
		sha512_sha384_transform(ctx->state, data);
		for (i = 0; i < (SHA512_SHA384_DATA_LENGTH - 2); i++)
			data[i] = 0;
	} else
		for (i = words; i < SHA512_SHA384_DATA_LENGTH - 2; i++)
			data[i] = 0;

	if ((ctx->bitcount_low += 8 * ctx->index) < 8 * ctx->index)
		ctx->bitcount_high++;
	data[SHA512_SHA384_DATA_LENGTH - 2] = ctx->bitcount_high;
	data[SHA512_SHA384_DATA_LENGTH - 1] = ctx->bitcount_low;
	sha512_sha384_transform(ctx->state, data);
}

static void 
sha512_sha384_digest(__const struct sha512_sha384_ctx *ctx, mutils_word8 * s, mutils_word32 len)
{
	mutils_word32 i;

	if (s != NULL)
        {
		for (i = 0; i < len; i++)
		{
			*s++ = ctx->state[i] >> 56;
			*s++ = 0xff & (ctx->state[i] >> 48);
			*s++ = 0xff & (ctx->state[i] >> 40);
			*s++ = 0xff & (ctx->state[i] >> 32);
			*s++ = 0xff & (ctx->state[i] >> 24);
			*s++ = 0xff & (ctx->state[i] >> 16);
			*s++ = 0xff & (ctx->state[i] >> 8);
			*s++ = 0xff & ctx->state[i];
		}
	}
}

void sha512_digest(__const struct sha512_sha384_ctx *ctx, mutils_word8 * s)
{
	sha512_sha384_digest(ctx, s, SHA512_DIGEST_SIZE / 8);
}

void sha384_digest(__const struct sha512_sha384_ctx *ctx, mutils_word8 * s)
{
	sha512_sha384_digest(ctx, s, SHA384_DIGEST_SIZE / 8);
}

#endif /* ENABLE_SHA512_SHA384 */
