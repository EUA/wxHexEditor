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



#include <libdefs.h>

mutils_error _mhash_gen_key_asis(void *keyword, mutils_word32 key_size, mutils_word8 *password, mutils_word32 plen)
{
#if defined(MHASH_ROBUST)
	if ((keyword == NULL) || (key_size == 0) ||
	    (password == NULL) || (plen == 0))
		return(-MUTILS_INVALID_SIZE);
#endif

	if (plen > key_size)
		plen=key_size;
	mutils_bzero(keyword, key_size);
	mutils_memcpy(keyword, password, plen);
	return(MUTILS_OK);
}
