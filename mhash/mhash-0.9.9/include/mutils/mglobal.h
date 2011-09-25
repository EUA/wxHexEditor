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


#if !defined(__MGLOBAL_H)
#define __MGLOBAL_H

#define MHASH_API_NONE          0
#define MHASH_API_CLASSIC       1
#define MHASH_API_FAMILY        2

#define MHASH_API_VERSION 20060101
#define MHASH_API_TYPE MHASH_API_CLASSIC
/* #define MHASH_API_TYPE MHASH_API_FAMILY */
#define MHASH_API_FULL

typedef enum __hashid {
#if (MHASH_API_TYPE == MHASH_API_CLASSIC)
	MHASH_CRC32		=  0,
	MHASH_MD5		=  1,
	MHASH_SHA1		=  2,
	MHASH_HAVAL256		=  3,
	MHASH_RIPEMD160		=  5,
	MHASH_TIGER192		=  7,
	MHASH_GOST		=  8,
	MHASH_CRC32B		=  9,
	MHASH_HAVAL224		= 10,
	MHASH_HAVAL192		= 11,
	MHASH_HAVAL160		= 12,
	MHASH_HAVAL128		= 13,
	MHASH_TIGER128		= 14,
	MHASH_TIGER160		= 15,
	MHASH_MD4		= 16,
	MHASH_SHA256		= 17,
	MHASH_ADLER32		= 18,
	MHASH_SHA224		= 19,
	MHASH_SHA512		= 20,
	MHASH_SHA384		= 21,
	MHASH_WHIRLPOOL		= 22,
	MHASH_RIPEMD128		= 23,
	MHASH_RIPEMD256		= 24,
	MHASH_RIPEMD320		= 25,
	MHASH_SNEFRU128		= 26,
	MHASH_SNEFRU256		= 27,
	MHASH_MD2		= 28,
	MHASH_AR		= 64, /* NOT YET IMPLEMENTED */
	MHASH_BOOGNISH		= 65, /* NOT YET IMPLEMENTED */
	MHASH_CELLHASH		= 66, /* NOT YET IMPLEMENTED */
	MHASH_FFT_HASH_I	= 67, /* NOT YET IMPLEMENTED */
	MHASH_FFT_HASH_II	= 68, /* NOT YET IMPLEMENTED */
	MHASH_NHASH		= 69, /* NOT YET IMPLEMENTED */
	MHASH_PANAMA		= 70, /* NOT YET IMPLEMENTED */
	MHASH_SMASH		= 71, /* NOT YET IMPLEMENTED */
	MHASH_SUBHASH		= 72  /* NOT YET IMPLEMENTED */
#endif
#if (MHASH_API_TYPE == MHASH_API_FAMILY)
	MHASH_CRC32		= 0x0001,
	MHASH_CRC32B		= 0x0002,
	MHASH_ADLER32		= 0x0011,
	MHASH_MD2		= 0x0101,
	MHASH_MD4		= 0x0111,
	MHASH_RIPEMD128		= 0x0112,
	MHASH_RIPEMD160		= 0x0113,
	MHASH_RIPEMD256		= 0x0114,
	MHASH_RIPEMD320		= 0x0115,
	MHASH_MD5		= 0x0121,
	MHASH_SHA1		= 0x0201,
	MHASH_SHA224		= 0x0201,
	MHASH_SHA256		= 0x0202,
	MHASH_SHA384		= 0x0203,
	MHASH_SHA512		= 0x0204,
	MHASH_HAVAL128		= 0x0301,
	MHASH_HAVAL160		= 0x0302,
	MHASH_HAVAL192		= 0x0303,
	MHASH_HAVAL224		= 0x0304,
	MHASH_HAVAL256		= 0x0305, /* 3 passes */
	MHASH_TIGER128		= 0x0401,
	MHASH_TIGER160		= 0x0402,
	MHASH_TIGER192		= 0x0403,
	MHASH_GOST		= 0x0501,
	MHASH_WHIRLPOOL		= 0x0601,
	MHASH_SNEFRU128		= 0x0701, /* 8 passes */
	MHASH_SNEFRU256		= 0x0702, /* 8 passes */
	MHASH_AR		= 0x0801, /* NOT YET IMPLEMENTED */
	MHASH_BOOGNISH		= 0x0901, /* NOT YET IMPLEMENTED */
	MHASH_CELLHASH		= 0x0a01, /* NOT YET IMPLEMENTED */
	MHASH_FFT_HASH_I	= 0x0b01, /* NOT YET IMPLEMENTED */
	MHASH_FFT_HASH_II	= 0x0b02, /* NOT YET IMPLEMENTED */
	MHASH_NHASH		= 0x0c01, /* NOT YET IMPLEMENTED */
	MHASH_PANAMA		= 0x0d01, /* NOT YET IMPLEMENTED */
	MHASH_SMASH		= 0x0e01, /* NOT YET IMPLEMENTED */
	MHASH_SUBHASH		= 0x0f01  /* NOT YET IMPLEMENTED */
#endif
} hashid;

/* These aliases are actually quite useful and so will be kept */

#define MHASH_HAVAL MHASH_HAVAL256
#define MHASH_TIGER MHASH_TIGER192

typedef enum __keygenid {
	KEYGEN_MCRYPT,		/* The key generator used in mcrypt */
	KEYGEN_ASIS,		/* Just returns the password as binary key */
	KEYGEN_HEX,		/* Just converts a hex key into a binary one */
	KEYGEN_PKDES,		/* The transformation used in Phil Karn's DES
				 * encryption program */
	KEYGEN_S2K_SIMPLE,	/* The OpenPGP (rfc2440) Simple S2K */
	KEYGEN_S2K_SALTED,	/* The OpenPGP Salted S2K */
	KEYGEN_S2K_ISALTED	/* The OpenPGP Iterated Salted S2K */
} keygenid;

#endif
