#if defined(ENABLE_HAVAL)

#if !defined(__MHASH_HAVAL_H)

#define __MHASH_HAVAL_H

typedef struct {
	mutils_word16 passes, hashLength;	/* HAVAL parameters */
	mutils_word32 digest[8];		/* message digest (fingerprint) */
	mutils_word8  block[128];		/* context data block */
	mutils_word32 occupied;			/* number of occupied bytes in the data block */
	mutils_word32 bitCount[2];		/* 64-bit message bit count */
	mutils_word32 temp[8];			/* temporary buffer */
} havalContext;

mutils_error havalInit256( havalContext *hcp);
mutils_error havalInit224( havalContext *hcp);
mutils_error havalInit192( havalContext *hcp);
mutils_error havalInit160( havalContext *hcp);
mutils_error havalInit128( havalContext *hcp);

mutils_error havalInit (havalContext *hcp, mutils_word32 passes, mutils_word32 length);
	/* Initialize a HAVAL hashing context according to the desired	*/
	/* number of passes and hash length.  Returns:					*/
	/* 0: no error.													*/
	/* 1: hcp is NULL.												*/
	/* 2: invalid number of passes (must be 3, 4, or 5).			*/
	/* 3: invalid hash length (must be 128, 160, 192, 224, or 256).	*/
	
mutils_error havalUpdate (havalContext *hcp, __const mutils_word8 *dataBuffer, mutils_word32 dataLength);
	/* Updates a HAVAL hashing context with a data block dataBuffer	*/
	/* of length dataLength.  Returns:								*/
	/* 0: no error.													*/
	/* 1: hcp is NULL.												*/
	
mutils_error havalFinal (havalContext *hcp, mutils_word8 *digest);
	/* Finished evaluation of a HAVAL digest, clearing the context.	*/
	/* The digest buffer must be large enough to hold the desired	*/
	/* hash length.  Returns:										*/
	/* 0: no error.													*/
	/* 1: hcp is NULL.												*/
	/* 2: digest is NULL.											*/

#endif /* __MHASH_HAVAL_H */

#endif
