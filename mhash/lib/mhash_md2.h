/* mhash_md2.h
 *
 * The MD2 hash function, described in RFC 1319.
 * 
 * This code was originally written by Niels Möller for libnettle. It
 * was altered by B. Poettering to fit the mhash interface. The original
 * copyright notice follows.
 */

/* Copyright (C) 2003 Niels Möller
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

#if defined(ENABLE_MD2)
 
#if !defined(__MHASH_MD2_H_INCLUDED)
#define __MHASH_MD2_H_INCLUDED

#define MD2_DIGEST_SIZE 16
#define MD2_DATA_SIZE 16

typedef struct md2_ctx
{
  mutils_word8 C[MD2_DATA_SIZE];
  mutils_word8 X[3 * MD2_DATA_SIZE];
  mutils_word8 buffer[MD2_DATA_SIZE]; /* block buffer */
  mutils_word32 index;                 /* pointer into buffer */
} MD2_CTX;

void md2_init(struct md2_ctx *ctx);

void md2_update(struct md2_ctx *ctx, __const mutils_word8 *data, mutils_word32 length);

void md2_digest(struct md2_ctx *ctx, mutils_word8 *digest);


#endif /* __MHASH_MD2_H_INCLUDED */

#endif
