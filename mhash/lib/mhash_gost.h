/*
 *  gosthash.h 
 *  21 Apr 1998  Markku-Juhani Saarinen <mjos@ssh.fi>
 * 
 *  GOST R 34.11-94, Russian Standard Hash Function 
 *  header with function prototypes.
 *
 *  Copyright (c) 1998 SSH Communications Security, Finland
 *  All rights reserved.                    
 */

#if defined(ENABLE_GOST)

#if !defined(GOSTHASH_H)
#define GOSTHASH_H

/*
   State structure 
 */

#include "libdefs.h"

typedef struct {
	mutils_word32 sum[8];
	mutils_word32 hash[8];
	mutils_word32 len[8];
	mutils_word8 partial[32];
	mutils_word32 partial_bytes;
} GostHashCtx;

/*
   Compute some lookup-tables that are needed by all other functions. 
 */

#if 0
void gosthash_init(void);
#endif

/*
   Clear the state of the given context structure. 
 */

void gosthash_reset(GostHashCtx * ctx);

/*
   Mix in len bytes of data for the given buffer. 
 */

void gosthash_update(GostHashCtx * ctx, __const mutils_word8 * buf, mutils_word32 len);

/*
   Compute and save the 32-byte digest. 
 */

void gosthash_final(GostHashCtx * ctx, mutils_word8 * digest);

#endif /*
	      GOSTHASH_H 
	    */

#endif
