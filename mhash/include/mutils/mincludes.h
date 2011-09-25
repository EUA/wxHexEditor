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


/*
 * The function of this header is to make sure that all system-specific headers
 * files that are required are included. Eventually, system headers will only
 * be pulled in when local definitions don't exist or are suboptimal.
 *
 * For now, we're also defining MAXINT and MININT in here, as there are
 * reported problems with the way we're trying to find it.
 */

#if !defined(__MINCLUDES_H)
#define __MINCLUDES_H

#if defined(MUTILS_USE_MHASH_CONFIG)
#include <mutils/mhash_config.h>
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#endif

#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#endif

#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_MALLOC_H)
#include <malloc.h>
#endif

#if defined(HAVE_STDIO_H)
#include <stdio.h>
#endif

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(HAVE_VALUES_H)
#include <values.h>
#endif

#if defined(HAVE_SYS_MMAN_H)
#include <sys/mman.h>
#endif

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#if defined(HAVE_ERROR_H)
#include <error.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if !defined(INT_MAX)
#define INT_MAX ((int) (~0U>>1))
#endif

#if !defined(MAXINT)
#define MAXINT INT_MAX
#endif

#if !defined(INT_MIN)
#define INT_MIN (-INT_MAX - 1)
#endif

#if !defined(MININT)
#define MININT INT_MIN
#endif

#endif
