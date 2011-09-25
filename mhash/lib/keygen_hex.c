/*
 *    Copyright (C) 1998,2002 Nikos Mavroyanopoulos
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



#include "libdefs.h"

#ifdef WIN32
/* Provided by Stefan Hetzl <shetzl@chello.at>
 * in order for isxdigit to work on cygwin.
 */
static char hexdigits[] = { '0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9',
			'a', 'b', 'c', 'd', 'e', 'f',
			'A', 'B', 'C', 'D', 'E', 'F' } ;

static mutils_boolean ishexdigit (mutils_word8 d)
{
	mutils_word8 i;

	for (i = 0; i < 22 ; i++)
		if (d == hexdigits[i]) 
			return(MUTILS_TRUE);

	return(MUTILS_FALSE);
}
#else
# include <ctype.h>
# define ishexdigit isxdigit
#endif

static mutils_boolean check_hex(mutils_word8 *given_chain, mutils_word32 len)
{
	mutils_word8 i;

	for (i = 0; i < len; i++)
		if (ishexdigit(given_chain[i]) == MUTILS_FALSE)
			return(MUTILS_FALSE);

	return(MUTILS_TRUE);
}


mutils_error _mhash_gen_key_hex(void *keyword, mutils_word32 key_size, 
				mutils_word8 *password, mutils_word32 plen)
{
	mutils_word8 *chain = password;
	mutils_word8 *pkeyword = keyword;
	mutils_word8 tmp[3];
	mutils_word32 i;

	mutils_bzero(keyword, key_size);

	/* The chain should have 2*n characters 
	 */

	if (plen % 2 != 0 || plen > key_size*2)
		return(-MUTILS_INVALID_SIZE);

	if (check_hex(chain, plen) == MUTILS_FALSE)
		return(-MUTILS_INVALID_FORMAT);

	mutils_bzero( keyword, key_size);

	for (i = 0; i < plen; i += 2) {
		mutils_memcpy(tmp, &chain[i], 2);
		tmp[2] = '\0';
		pkeyword[i / 2] = mutils_strtol(tmp, (mutils_word8 **) NULL, 16);
	}

	return(MUTILS_OK);
}


