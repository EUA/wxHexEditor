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



/*
 * This is a simple test driver for use in combination with test_hash.sh
 *
 * It's ugly, limited and you should hit :q! now
 *
 * $Id: driver.c,v 1.7 2006/01/08 09:08:29 imipak Exp $
 */

#define MUTILS_USE_MHASH_CONFIG
#include <mhash.h>

#define MAX_DIGEST_SIZE 256

int main(int argc, char **argv)
{
	mutils_word8 digest[MAX_DIGEST_SIZE]; /* enough space to hold digests */
	mutils_word8 data[1024];
	ssize_t r;
	mutils_word32 i;
	mutils_boolean found;
	hashid hashid;
	MHASH td;

	if (argc != 2)
	{
		fprintf(stderr, "Syntax: %s <name of hash function>\n", argv[0]);
		exit(1);
	}

	/* Look for the right mhash hash id */
	for (found = MUTILS_FALSE, hashid = 0; hashid <= mhash_count(); hashid++)
	{
		if (mhash_get_hash_name_static(hashid))
		{
			if (! mutils_strcmp((mutils_word8 *) argv[1], mhash_get_hash_name_static(hashid)))
			{
				found = MUTILS_TRUE;
				break;
			}
		}
	}

	if (found == MUTILS_FALSE)
	{
		fprintf(stderr, "FATAL: hash function %s not available!\n", argv[1]);
		exit(MUTILS_INVALID_FUNCTION);
	}

	assert(mhash_get_block_size(hashid) <= MAX_DIGEST_SIZE);

	td = mhash_init(hashid);  /* hash stdin until EOF ist reached */

	do
	{
		r = read(0, data, sizeof data);
		assert(r >= 0);
		mhash(td, data, r);
	} while (r);

	mhash_deinit(td, digest);

	for (i = 0; i < mhash_get_block_size(hashid); i++)
	{
		printf("%02X", digest[i]);
	}

	printf("\n");
	
	return(MUTILS_OK);
}
