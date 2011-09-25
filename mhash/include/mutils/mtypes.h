/*
 *    Copyright (C) 2005 Jonathan Day, Nikos Mavroyanopoulos
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


#if !defined(__MTYPES_H)
#define __MTYPES_H

#include <mutils/mutils.h>

typedef struct __KEYGEN
{
	hashid		 hash_algorithm[2];
	mutils_word32	 count;
	void		*salt;
	mutils_word32	 salt_size;
} KEYGEN;

typedef void (*INIT_FUNC)( void*);
typedef void (*HASH_FUNC)(void*, const void*, int);
typedef void (*FINAL_FUNC)(void*);
typedef void (*DEINIT_FUNC)(void*, unsigned char*);

typedef struct __MHASH_INSTANCE
{
	mutils_word32 hmac_key_size;
	mutils_word32 hmac_block;
	mutils_word8 *hmac_key;

	mutils_word8 *state;
	mutils_word32 state_size;
	hashid algorithm_given;

	HASH_FUNC hash_func;
	FINAL_FUNC final_func;
	DEINIT_FUNC deinit_func;
} MHASH_INSTANCE;

typedef MHASH_INSTANCE *MHASH;

#define MHASH_FAILED ((MHASH) 0x0)

#endif
