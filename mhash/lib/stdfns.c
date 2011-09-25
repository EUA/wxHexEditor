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
   $Id: stdfns.c,v 1.2 2006/01/10 03:47:18 imipak Exp $ 
 */

#include "libdefs.h"

/**
 * Some of these are wrappers. The idea is to eventually produce an extremely
 * lightweight set of robust, portable functions that are guaranteed to produce
 * a "safe" result, even with bogus inputs. We can't trust native C libraries
 * to validate inputs.
 */

WIN32DLL_DEFINE
int mutils_mlock(__const void *addr, __const mutils_word32 len)
{
	int ret;

	if ((addr == NULL) || (len == 0))
	{
		ret = -1;
		errno = EINVAL;
	}

#if defined(HAVE_MLOCK)
	ret = mlock(addr, len);
#endif

	return(ret);
}

WIN32DLL_DEFINE
int mutils_munlock(__const void *addr, __const mutils_word32 len)
{
	int ret;

	if ((addr == NULL) || (len == 0))
	{
		ret = -1;
		errno = EINVAL;
	}

#if defined(HAVE_MLOCK)
	ret = munlock(addr, len);
#endif

	return(ret);
}

/*
 * NOTE: In order for memory locking to be useful, we need our own malloc
 * implementation, as we cannot guarantee that the system-supplied one will
 * let us know how much memory has been malloc()'ed. For much the same reason,
 * neither free() nor realloc() can zero safely. This allows the potential
 * leakage of information.
 */

WIN32DLL_DEFINE
void *
mutils_malloc(__const mutils_word32 n)
{
	void *ptr;

	if (n == 0)
	{
		return(NULL);
	}

	ptr = malloc(n);

	if (ptr != NULL)
	{
		mutils_bzero(ptr, n);
	}

	return(ptr);
}

WIN32DLL_DEFINE
void *
mutils_realloc(__const void *ptr, __const mutils_word32 n)
{
	void *ptrOut;

	if ((ptr == NULL) && (n > 0))
	{
		ptrOut = mutils_malloc(n);
	}
	else
	{
		if ((ptr != NULL) && (n == 0))
		{
			mutils_free(ptr);
			ptrOut = NULL;
		}
		else
		{
		  ptrOut = realloc((void *) ptr, n);
		}
	}
	return(ptrOut);
}

WIN32DLL_DEFINE
void
mutils_free(__const void *ptr)
{
	if (ptr == NULL)
	{
		return;
	}
	free((void *) ptr);
	return;
}

WIN32DLL_DEFINE
void
mutils_bzero(void *s, __const mutils_word32 n)
{
	mutils_word8 *stmp = (mutils_word8 *) s;
	mutils_word32 i;

	if ((s == NULL) || (n == 0))
	{
		return;
	}

	for (i = 0; i < n; i++, stmp++)
	{
		*stmp = '\0';
	}
}

WIN32DLL_DEFINE
void
mutils_memset(void *s, __const mutils_word8 c, __const mutils_word32 n)
{
#if defined(sparc)
	/* Sparc needs 8-bit alignment - just use standard memset */
	memset(s, (int) c, (size_t) n);
#else
	mutils_word8 *stmp;
	mutils_word32 *ltmp = (mutils_word32 *) s;
	mutils_word32 lump;
	mutils_word32 i;
	mutils_word32 words;
	mutils_word32 remainder;

	if ((s == NULL) || (n == 0))
	{
		return;
	}

	lump = (c << 24) + (c << 16) + (c << 8) + c;

	words = n >> 2;
	remainder = n - (words << 2);

	for (i = 0; i < words; i++, ltmp++)
	{
		*ltmp = lump;
	}

	stmp = (mutils_word8 *) ltmp;

	for (i = 0; i < remainder; i++, stmp++)
	{
		*stmp = c;
	}
#endif
}

static void
mutils_memcpy8(mutils_word8 *p_dest, __const mutils_word8 *p_src,
	__const mutils_word32 n)
{
	mutils_word32 i;

	for (i = 0; i < n; i++)
	{
		*p_dest++ = *p_src++;
	}
}

WIN32DLL_DEFINE
void
mutils_memcpy(void *dest, __const void *src, __const mutils_word32 n)
{
	mutils_word8 *ptr1;
	mutils_word8 *ptr2;
	mutils_word32 *bigptr1;
	mutils_word32 *bigptr2;
	mutils_word32 i;
	mutils_word32 words;
	mutils_word32 remainder;

	if ((dest == NULL) || (src == NULL) || (n == 0))
	{
		return;
	}

	ptr1 = (mutils_word8 *)src;
	ptr2 = dest;

	/* copy byte-by-byte for small and/or unaligned copies */
	if ((n < 16) || ((mutils_word32)ptr1 & 0x3) || ((mutils_word32)ptr2 
& 0x3))
	{
		mutils_memcpy8(ptr2, ptr1, n);
		return;
	}

	words = n >> 2;
	remainder = n - (words << 2);

	bigptr1 = (mutils_word32 *) dest;
	bigptr2 = (mutils_word32 *) src;

	for (i = 0; i < words; i ++, bigptr1++, bigptr2++)
	{
		*bigptr1 = *bigptr2;
	}

	ptr1 = (mutils_word8 *) bigptr1;
	ptr2 = (mutils_word8 *) bigptr2;

	for (i = 0; i < remainder; i++, ptr1++, ptr2++)
	{
		*ptr1 = *ptr2;
	}
}

/*
 * If the system ordering is big-endian, convert to little-endian format.
 * In both cases, the data is held in a temporary workspace that will be
 * overwritten the next time this function is called. This function is
 * deprecated and should NOT be called in new code. The ordering macros
 * should be used instead, for all single-word conversions.
 */
WIN32DLL_DEFINE
mutils_word32
mutils_word32swap(mutils_word32 x)
{
	static mutils_word32 value;

	value = mutils_lend32(x);
	return(value);
}

/*
 * Byte swap a series of 32-bit integers. If destructive is set to false, the
 * output will be placed in a freshly malloc()'ed buffer and the original
 * data will remain intact.
 */
WIN32DLL_DEFINE
mutils_word32 *
mutils_word32nswap(mutils_word32 *x, mutils_word32 n, mutils_boolean destructive)
{
	mutils_word32 loop;
	mutils_word32 *buffer;
	mutils_word32 *ptrIn;
	mutils_word32 *ptrOut;
	mutils_word32 count = n * 4;

	if (destructive == MUTILS_FALSE)
	{
		buffer = mutils_malloc(count);
		if (buffer == NULL)
		{
			return(NULL);
		}
	}
	else
	{
		buffer = x;
	}

/*
 * This loop is totally useless for destructive processing of little-endian
 * data on a little-endian machine.
 */

	for (loop = 0, ptrIn = x, ptrOut = buffer; loop < n; loop++, ptrOut++, ptrIn++)
	{
		*ptrOut = mutils_lend32(*ptrIn);
	}

	return(buffer);
}

WIN32DLL_DEFINE
void
mutils_memmove(void *dest, __const void *src, const mutils_word32 n)
{
	mutils_word8 *ptr1;
	mutils_word8 *ptr2;
	mutils_word32 i;
	mutils_word32 *bigptr1;
	mutils_word32 *bigptr2;
	mutils_word32 words;
	mutils_word32 remainder;

	if ((dest == NULL) || (src == NULL) || (n == 0))
	{
		return;
	}

	bigptr1 = (mutils_word32 *) dest;
	bigptr2 = (mutils_word32 *) src;

	words = n >> 2;
	remainder = n - (words << 2);

	for (i = 0; i < words; i++, bigptr1++, bigptr2++)
	{
		*bigptr1 = *bigptr2;
	}

	ptr1 = (mutils_word8 *) bigptr1;
	ptr2 = (mutils_word8 *) bigptr2;

	for (i = 0; i < remainder; i++, ptr1++, ptr2++)
	{
		*ptr1 = *ptr2;
	}
}

WIN32DLL_DEFINE
int
mutils_memcmp(__const void *s1, const void *s2, const mutils_word32 n)
{
	if (n == 0)
	{
		return(0);
	}
	if (s1 == NULL)
	{
		if (s2 == NULL)
		{
			return(0);
		}
		return(-MAXINT);
	}
	if (s2 == NULL)
	{
		return(MAXINT);
	}

	return(memcmp(s1, s2, n));
}

/* Ugh. This could do nasty things, if a string isn't correctly terminated,
 * particularly as memory can be larger than can be stored in a 32-bit word.
 * If it is possible to produce an incorrect result, or segfault, then it
 * is not a robust solution.
 */

WIN32DLL_DEFINE
mutils_word32
mutils_strlen(__const mutils_word8 *str)
{
	mutils_word32 ret;
	mutils_word8 *endStr = (mutils_word8 *) str;

	if (str == NULL)
	{
		return(0);
	}

	while (*endStr != 0)
	{
		endStr++;
	}

	ret = endStr - str;

	return(ret);
}

WIN32DLL_DEFINE
mutils_word8 *
mutils_strdup(__const mutils_word8 *str)
{
	mutils_word8 *ret = NULL;
	mutils_word8 *ptr1;
	mutils_word8 *ptr2;
	mutils_word32 len;

	if (str == NULL)
	{
		return(NULL);
	}

	len = mutils_strlen(str) + 1;

	ret = (mutils_word8 *) mutils_malloc(len);

	if (ret != NULL)
	{
		ptr1 = (mutils_word8 *) str;
		ptr2 = ret;

		for (len = mutils_strlen(str); len > 0; len--, ptr1++, ptr2++)
		{
			*ptr2 = *ptr1;
		}
	}

	return(ret);
}

WIN32DLL_DEFINE
mutils_word8 *
mutils_strcat(mutils_word8 *dest, __const mutils_word8 *src)
{
	mutils_word8 *ptrIn = (mutils_word8 *) src;
	mutils_word8 *ptrOut = dest + mutils_strlen(dest);

	if (dest == NULL)
	{
		return(NULL);
	}
	if (src == NULL)
	{
		return(dest);
	}	
	while (*src != 0)
	{
		*ptrOut = *ptrIn;
		ptrOut++;
		ptrIn++;
	}
	*ptrOut = '\0';
	return((mutils_word8 *) dest);
}

WIN32DLL_DEFINE
mutils_word8 *
mutils_strcpy(mutils_word8 *dest, __const mutils_word8 *src)
{
	if (dest == NULL)
	{
		return(NULL);
	}
	return((mutils_word8 *) strcpy((char *) dest, (char *) src));
}

WIN32DLL_DEFINE
mutils_word8 *
mutils_strncpy(mutils_word8 *dest, __const mutils_word8 *src, const mutils_word32 n)
{
	if (dest == NULL)
	{
		return(NULL);
	}
	if (n == 0)
	{
		return(NULL);
	}
	return((mutils_word8 *) strncpy((char *) dest, (char *) src, n));
}

WIN32DLL_DEFINE
int
mutils_strcmp(__const mutils_word8 *src1, const mutils_word8 *src2)
{
	if (src1 == NULL)
	{
		if (src2 == NULL)
		{
			return(0);
		}
		return(-MAXINT);
	}
	if (src2 == NULL)
	{
		return(MAXINT);
	}
	return(strcmp((char *) src1, (char *) src2));
}

WIN32DLL_DEFINE
int
mutils_strncmp(__const mutils_word8 *src1, const mutils_word8 *src2, const mutils_word32 n)
{
	if (n == 0)
	{
		return(0);
	}
	if (src1 == NULL)
	{
		if (src2 == NULL)
		{
			return(0);
		}
		return(-MAXINT);
	}
	if (src2 == NULL)
	{
		return(MAXINT);
	}
	return(strncmp((char *) src1, (char *) src2, n));
}

WIN32DLL_DEFINE
long
mutils_strtol(__const mutils_word8 *nptr, mutils_word8 **endptr, const mutils_word8 base)
{
	return(strtol((char *) nptr, (char **) endptr, (int) base));
}

WIN32DLL_DEFINE
mutils_word8
mutils_val2char(mutils_word8 x)
{
	mutils_word8 out;

	static mutils_word8 *table = (mutils_word8 *) "0123456789abcdef";

	out = *(table + x);

	return(out);
}

WIN32DLL_DEFINE
mutils_word8 *
mutils_asciify(mutils_word8 *in, __const mutils_word32 len)
{
	mutils_word8 *ptrIn = in;
	mutils_word8 *buffer = mutils_malloc((2 * len) + 1);
	mutils_word8 *ptrOut = buffer;
	mutils_word32 loop;

	for (loop = 0; loop < len; loop++, ptrIn++)
	{
		*ptrOut++ = mutils_val2char((*ptrIn & 0xf0) >> 4);
		*ptrOut++ = mutils_val2char((*ptrIn & 0x0f));
	}
	return(buffer);
}

WIN32DLL_DEFINE
mutils_boolean
mutils_thequals(mutils_word8 *text, mutils_word8 *hash, __const mutils_word32 len)
{
	mutils_word8  *ptrText = text;
	mutils_word8  *ptrHash = hash;
	mutils_word32  loop;

	for (loop = 0; loop < len; loop++, ptrHash++)
	{
		if (mutils_val2char((*ptrHash & 0xf0) >> 4) != *ptrText++)
		{
			return(MUTILS_FALSE);
		}
		if (mutils_val2char((*ptrHash & 0x0f)) != *ptrText++)
		{
			return(MUTILS_FALSE);
		}
	}
	return(MUTILS_TRUE);
}

