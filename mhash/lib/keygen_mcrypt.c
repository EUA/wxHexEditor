/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
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


#define MUTILS_USE_MHASH_CONFIG

#include <mhash.h>
#include <libdefs.h>
#include <keygen.h>

/* Key generation using any hash algorithm */
mutils_error _mhash_gen_key_mcrypt(hashid algorithm,
				   void *keyword, mutils_word32 key_size,
				   void *salt, mutils_word32 salt_size,
				   mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8 *key = mutils_malloc(key_size);
	mutils_word8 *digest = NULL;
	mutils_word32 size = key_size;
	MHASH td;
	mutils_word8 *cp = key, *cp0 = key;
	mutils_boolean salt_z = MUTILS_TRUE;		/* flag to be used when freeing salt[] */
	mutils_word32 block_size = mhash_get_block_size(algorithm);

#if defined(MHASH_ROBUST)
	if (key == NULL)
		return(-MUTILS_SYSTEM_RESOURCE_ERROR);
#endif

	mutils_bzero(key, key_size);

	if (salt == NULL) {
		salt_z = MUTILS_FALSE;
	}
	
	while (1) {
		td = mhash_init(algorithm);
		if (td == MHASH_FAILED)
			return(-MUTILS_INVALID_FUNCTION);
		
		if (salt_z == MUTILS_TRUE)
			mhash(td, salt, salt_size);

		mhash(td, password, plen);

		if (cp - cp0 > 0)
			mhash(td, cp0, cp - cp0);

		digest = mhash_end(td);

#if defined(MHASH_ROBUST)
		if (digest == NULL)
			return(-MUTILS_INVALID_RESULT);
#endif

		if (size > block_size) {
			mutils_memcpy(cp, digest, block_size);
			mutils_free(digest);
			size -= block_size;
			cp += block_size;
		} else {
			mutils_memcpy(cp, digest, size);
			mutils_free(digest);
			break;
		}
	}
	mutils_memcpy(keyword, key, key_size);
	mutils_free(key);
	return(MUTILS_OK);
}

mutils_error _mhash_gen_key_pkdes(void *keyword, mutils_word32 key_size,
				  mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8 *ptr = keyword;
	mutils_word32 cnt;
	mutils_word32 i;
	mutils_word32 c;

#if defined(MHASH_ROBUST)
	if (keyword == NULL)
		return(-MUTILS_SYSTEM_RESOURCE_ERROR);
#endif

	if (plen > key_size)
		return(-MUTILS_INVALID_SIZE);

	mutils_bzero(keyword, key_size);

	mutils_memcpy(keyword, password, plen);

	for (cnt = 0; cnt < key_size; cnt++, ptr++) {
		c = 0;
		for (i = 0; i < 7; i++)
			if (*ptr & (1 << i))
				c++;
		if ((c & 1) == 0)
			*ptr |= 0x80;
		else
			*ptr &= ~0x80;
	}

	return(MUTILS_OK);
}
