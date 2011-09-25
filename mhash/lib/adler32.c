/*
 *    Copyright (C) 2001 Nikos Mavroyanopoulos
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

#if defined(ENABLE_ADLER32)

#include "mhash_adler32.h"

/* This is the implementation of Manuel Kasper <mk@neon1.net>. 
 */

void mhash_clear_adler32(mutils_word32 *adler)
{
	*adler = 0x1;
}

void mhash_get_adler32(__const mutils_word32 *adler, void *ret)
{
	mutils_word32 tmp = *adler;

#if defined(WORDS_BIGENDIAN)
	tmp = mutils_word32swap(tmp);
#endif
	if (ret!=NULL)
	{
		mutils_memcpy(ret, &tmp, sizeof(mutils_word32));
	}
}

/**
 *    Generic C implementation of Adler32
 *    Peak performance:
 *      VC++:       52 MB/s on AMD K7 600 MHz
 *      gcc:        44 MB/s
 *      Borland 5:  48 MB/s
 *      (all with speed optimizations on)
 */

void mhash_adler32(mutils_word32 * adler, __const void *given_buf, mutils_word32 len)
{
	mutils_word32 s1 = (*adler) & 0x0000FFFF;
	mutils_word32 s2 = ((*adler) >> 16) & 0x0000FFFF;
	mutils_word32 n;
	mutils_word8 *p = (mutils_word8 *) given_buf;

	for (n = 0; n < len; n++, p++)
	{
		s1 += *p;
		if (s1 >= 65521)	/* using modulo took about 7 times longer on my CPU! */
		{
			s1 -= 65521;	/* WARNING: it's meant to be >= 65521, not just > ! */
		}
		s2 += s1;
		if (s2 >= 65521)	/* same warning applies here, too */
		{
			s2 -= 65521;
		}
	}
	*adler = (s2 << 16) + s1;
}

#endif /* ENABLE_ADLER32 */
