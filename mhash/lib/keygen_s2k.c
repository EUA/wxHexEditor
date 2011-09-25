/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
 *    Copyright (C) 2002 Nikos Mavroyanopoulos
 *
 *    This library is free software; you can redistribute it and/or modify it 
 *    under the terms of the GNU Library General Public License as published 
 *    by the Free Software Foundation; either version 2 of the License, or 
 *    (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the
 *    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *    Boston, MA 02111-1307, USA.
 */



#include "libdefs.h"
#include "keygen.h"
#include <mhash.h>

#define MAX_DIGEST_SIZE 40

/* Key generation using OpenPGP Simple S2K algorithm */
mutils_error _mhash_gen_key_s2k_simple(hashid algorithm,
				       void *keyword, mutils_word32 key_size,
				       mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8* key;
	mutils_word8 digest[MAX_DIGEST_SIZE];
	char null = '\0';
	mutils_word32 i,j, times, total;
	MHASH td;
	mutils_word32 block_size = mhash_get_block_size(algorithm);

	/* This should never happen, so naturally it is bound to. */

	if (block_size == 0)
	{
		errno = EINVAL;
		return(-MUTILS_INVALID_SIZE);
	}

	times = key_size / block_size;

	if (key_size % block_size != 0)
	{
		times++;
	}

	total = times * block_size;

	key = mutils_malloc(total);

	if (key == NULL)
	{
		return(-MUTILS_SYSTEM_RESOURCE_ERROR); /* or what? */
	}

	mutils_bzero(key, total);

	for (i = 0; i < times; i++) {
		td = mhash_init(algorithm);
		if (td == MHASH_FAILED) {
			mutils_free(key);
			return(-MUTILS_INVALID_FUNCTION);
		}
		
		for (j = 0; j < i; j++)
		{
			mhash(td, &null, 1);
		}
		mhash(td, password, plen);
		mhash_deinit(td, digest);

		mutils_memcpy(&key[i * block_size], digest, block_size);
	}
	mutils_memcpy(keyword, key, key_size);
	mutils_bzero(key, key_size);
	mutils_free(key);
	return(MUTILS_OK);
}


/* Key generation using OpenPGP Salted S2K algorithm */
mutils_error _mhash_gen_key_s2k_salted(hashid algorithm,
				       void *keyword, mutils_word32 key_size,
				       mutils_word8 *salt, mutils_word32 salt_size,
				       mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8 *key;
	mutils_word8 digest[MAX_DIGEST_SIZE];
	char null='\0';
	mutils_word32 i;
	mutils_word32 j;
	mutils_word32 times;
	MHASH td;
	mutils_word32 block_size = mhash_get_block_size(algorithm);
	mutils_word32 total;

	if (salt == NULL)
	{
		return(-MUTILS_INVALID_INPUT_BUFFER);
	}

	if (salt_size < 8)
	{
	  	return(-MUTILS_INVALID_SIZE);	/* This algorithm will use EXACTLY
						 * 8 bytes salt.
						 */
	}

	times = key_size / block_size;

	if (key_size % block_size != 0)
		times++;

	total = times * block_size;

	key = mutils_malloc(total);

#if defined(MHASH_ROBUST)
	if (key == NULL)
		return(-MUTILS_SYSTEM_RESOURCE_ERROR); /* or what? */
#endif

	for (i = 0; i < times; i++) {
		td = mhash_init(algorithm);
		if (td == MHASH_FAILED) {
			mutils_free(key);
			return(-MUTILS_INVALID_FUNCTION);
		}
		
		for (j = 0; j < i; j++)
			mhash(td, &null, 1);

		mhash(td, salt, 8);
		mhash(td, password, plen);
		mhash_deinit(td, digest);
		
		mutils_memcpy( &key[i*block_size], digest, block_size);
	}
	mutils_memcpy(keyword, key, key_size);
	mutils_bzero(key, key_size);
	mutils_free(key);
	return(MUTILS_OK);
}

#define EXPBIAS 6
           
/* Key generation using OpenPGP Iterated and Salted S2K algorithm */
mutils_error _mhash_gen_key_s2k_isalted(hashid algorithm, mutils_word64 _count, 
					void *keyword, mutils_word32 key_size,
					mutils_word8 *salt, mutils_word32 salt_size,
					mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8 *key;
	mutils_word8 digest[MAX_DIGEST_SIZE];
	mutils_word8 null='\0';
	mutils_word32 i;
	mutils_word32 j;
	mutils_word32 z;
	mutils_word32 times;
	MHASH td;
	mutils_word32 block_size = mhash_get_block_size(algorithm);
	mutils_word8 *saltpass;
	mutils_word32 saltpass_size;
	mutils_word32 bcount, rest;
	mutils_word32 count = _count;
	mutils_word32 total;

	if (salt == NULL)
		return(-MUTILS_INVALID_INPUT_BUFFER);

	if (salt_size < 8)
		return(-MUTILS_INVALID_SIZE);	/* This algorithm will use EXACTLY
						 * 8 bytes salt.
						 */

	if ((saltpass = mutils_malloc(8 + plen)) == NULL)
		return(-MUTILS_SYSTEM_RESOURCE_ERROR); /* hmm */

	mutils_memcpy( saltpass, salt, 8);
	mutils_memcpy( &saltpass[8], password, plen);
	saltpass_size = plen+8;

	times = key_size/block_size;
	if (key_size % block_size != 0)
		times++;

	total = times * block_size;

	key = mutils_malloc(total);

	if (key == NULL) {
	   mutils_bzero(saltpass, saltpass_size);
	   mutils_free(saltpass);
	   return(-MUTILS_SYSTEM_RESOURCE_ERROR);
        }

	mutils_bzero(key, total);

	/* Calculate the iterations
	 */
	bcount /*bytes */ = 
		((mutils_word32)16 + (count & 15)) << ((count >> 4) + EXPBIAS); 

	count /* iterations */ = 
		(bcount / saltpass_size);

	rest = bcount % saltpass_size;

	if (bcount < saltpass_size)
	{
		count++;
		rest = 0;
	}
	
	for (i = 0; i < times; i++)
	{
		td = mhash_init(algorithm);
		if (td == MHASH_FAILED)
		{
		        mutils_bzero( key, key_size);
		        mutils_bzero( saltpass, saltpass_size);
			mutils_free(key);
			mutils_free(saltpass);
			return(-MUTILS_INVALID_FUNCTION);
		}
	
		for (j = 0; j < i; j++)
			mhash(td, &null, 1);

		for (z = 0; z < count; z++)
		{
			mhash(td, saltpass, saltpass_size);
		}

		mhash(td, saltpass, rest);

		mhash_deinit(td, digest);
		
		mutils_memcpy( &key[i*block_size], digest, block_size);
	}
	mutils_memcpy(keyword, key, key_size);

	mutils_bzero(key, key_size);
	mutils_bzero(saltpass, saltpass_size);

	mutils_free(key);
	mutils_free(saltpass);

	return(MUTILS_OK);
}
