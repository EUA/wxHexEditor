dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(MHASH, 0.9.9, mhash-devel@sourceforge.net)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([lib/mhash.c])
AM_INIT_AUTOMAKE

AC_DEFINE([MHASH_VERSION], PROGRAM_VERSION, "MHash Version")
AC_CONFIG_HEADER([include/mutils/mhash_config.h])


dnl Check system.

AC_GNU_SOURCE

AC_AIX

AC_ISC_POSIX

AC_MINIX

dnl Checks for programs.

AM_PROG_AS
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_GCC_TRADITIONAL
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Check Make characteristics
AC_PROG_MAKE_SET

dnl Check C characteristics

AC_LANG(C)

AC_C_CONST
AC_C_INLINE
AC_C_PROTOTYPES
AC_C_BIGENDIAN

dnl Miscellaneous checks

AC_CONST

AC_LIBTOOL_WIN32_DLL

AM_PROG_LIBTOOL

AC_TYPE_SIZE_T

dnl Checks for headers

AC_HEADER_STDBOOL
AC_HEADER_STDC

AC_CHECK_HEADERS(sys/endian.h sys/stat.h sys/types.h sys/mman.h)
AC_CHECK_HEADERS(assert.h byteswap.h dlfcn.h ctype.h errno.h)
AC_CHECK_HEADERS(endian.h fcntl.h inttypes.h limits.h)
AC_CHECK_HEADERS(malloc.h unistd.h utime.h values.h)
AC_CHECK_HEADERS(signal.h stdint.h stdio.h stdlib.h string.h strings.h)

dnl Checks for functions

AC_FUNC_MALLOC
AC_FUNC_MEMCMP

AC_CHECK_FUNCS(bcopy bzero)
AC_CHECK_FUNCS(fcntl fsync)
AC_CHECK_FUNCS(memset memmove mlock)
AC_CHECK_FUNCS(sigaction signal)
AC_CHECK_FUNCS(readdir_r readdir)
AC_CHECK_FUNCS(strtol)
AC_CHECK_FUNCS(lstat stat)
AC_CHECK_FUNCS(umask utime)

dnl Checks type sizes.

AC_CHECK_SIZEOF(unsigned long long int, 8)
AC_CHECK_SIZEOF(unsigned long int, 4)
AC_CHECK_SIZEOF(unsigned int, 4)
AC_CHECK_SIZEOF(unsigned short int, 2)
AC_CHECK_SIZEOF(unsigned char, 1)

dnl Checks for optional libraries and tools

AM_WITH_DMALLOC
AM_MAINTAINER_MODE

AC_ARG_WITH(CC,
[  --with-CC               choose compiler],
  if test "$withval" != "no" && test "$withval" != "yes" ; then
    CC="$withval"
  fi
)

AC_ARG_WITH(CFLAGS,
[  --with-CFLAGS           set C compiler flags],
  if test "$withval" != "no" && test "$withval" != "yes" ; then
    CFLAGS="$withval"
  fi
)

AC_ARG_WITH(CPPFLAGS,
[  --with-CPPFLAGS         set C preprocessor flags],
  if test "$withval" != "no" && test "$withval" != "yes" ; then
    CPPFLAGS="$withval"
  fi
)

AC_ARG_WITH(LDFLAGS,
[  --with-LDFLAGS          set linker flags],
  if test "$withval" != "no" && test "$withval" != "yes" ; then
    LDFLAGS="$withval"
  fi
)

if test x$ac_cv_prog_gcc = xyes; then
        if test x$opt_maintainer_mode = xyes; then
          CFLAGS=" -ggdb3 -O2 -Wall -Wpointer-arith"
        fi

        if test x$opt_dmalloc_mode = xyes; then
          AC_CHECK_LIB( dmalloc, main)
          AC_DEFINE(USE_DMALLOC, 1, [dmalloc])
        fi
fi

AC_ARG_ENABLE(md5,
[  --disable-md5           disable the MD5 algorithm],
, 
AC_DEFINE(ENABLE_MD5, 1, "MD-5 Algorithm"))

AC_ARG_ENABLE(sha1,
[  --disable-sha1          disable the SHA1 algorithm],
, 
AC_DEFINE(ENABLE_SHA1, 1, "SHA-1 Algorithm"))

AC_ARG_ENABLE(md4,
[  --disable-md4           disable the MD4 algorithm],
, 
AC_DEFINE(ENABLE_MD4, 1, "MD-4 Algorithm"))

AC_ARG_ENABLE(md2,
[  --disable-md2           disable the MD2 algorithm],
,
AC_DEFINE(ENABLE_MD2, 1, "MD-2 Algorithm"))

AC_ARG_ENABLE(tiger,
[  --disable-tiger         disable the TIGER algorithm],
, 
AC_DEFINE(ENABLE_TIGER, 1, "Tiger Algorithm"))

AC_ARG_ENABLE(haval,
[  --disable-haval         disable the HAVAL algorithm],
, 
AC_DEFINE(ENABLE_HAVAL, 1, "Haval Algorithm"))

AC_ARG_ENABLE(crc32,
[  --disable-crc32         disable the CRC32 algorithm],
, 
AC_DEFINE(ENABLE_CRC32, 1, "CRC32 Algorithm"))

AC_ARG_ENABLE(adler32,
[  --disable-adler32       disable the ADLER32 algorithm],
, 
AC_DEFINE(ENABLE_ADLER32, 1, "Adler32 Algorithm"))

AC_ARG_ENABLE(ripemd,
[  --disable-ripemd        disable the RIPEMD128/160/256/320 algorithms],
, 
AC_DEFINE(ENABLE_RIPEMD, 1, "RIPE-MD - RIPE Substitute for MD"))

AC_ARG_ENABLE(gost,
[  --disable-gost          disable the GOST algorithm],
, 
AC_DEFINE(ENABLE_GOST, 1, "GOST Algorithm"))

AC_ARG_ENABLE(sha256-sha224,
[  --disable-sha256-sha224 disable the SHA256 and SHA224 algorithms],
, 
AC_DEFINE(ENABLE_SHA256_SHA224, 1, "SHA-224 and SHA-256 - SHA-2 Algorithms"))

AC_ARG_ENABLE(sha512-sha384,
[  --disable-sha512-sha384 disable the SHA512 and SHA384 algorithms],
, 
AC_DEFINE(ENABLE_SHA512_SHA384, 1, "SHA-384 and SHA-512 - SHA-2 Algorithms"))

AC_ARG_ENABLE(snefru,
[  --disable-snefru        disable the SNEFRU algorithm],
,
AC_DEFINE(ENABLE_SNEFRU, 1, "Snefru Algorithm"))

AC_ARG_ENABLE(whirlpool,
[  --disable-whirlpool     disable the WHIRLPOOL algorithm],
, 
AC_DEFINE(ENABLE_WHIRLPOOL, 1, "Whirlpool Algorithm"))

AC_CONFIG_FILES([Makefile
		 mhash.spec
		 include/Makefile
		 doc/Makefile
		 src/Makefile
		 lib/Makefile])
AC_OUTPUT
