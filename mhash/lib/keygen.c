/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000,2001 Sascha Schumman, Nikos Mavroyanopoulos
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


#define KEYGEN_INTERNALS

#include <libdefs.h>
#include <mhash_int.h>
#include <keygen.h>

WIN32DLL_DEFINE
mutils_boolean mhash_keygen_uses_hash_algorithm(keygenid type)
{
	mutils_boolean ret = MUTILS_FALSE;
	KEYGEN_ALG_LOOP(ret = p->uses_hash_algorithm);
	return(ret);
}

WIN32DLL_DEFINE
mutils_boolean mhash_keygen_uses_count(keygenid type)
{
	mutils_boolean ret = MUTILS_FALSE;
	KEYGEN_ALG_LOOP(ret = p->uses_count);
	return(ret);
}

WIN32DLL_DEFINE
mutils_boolean mhash_keygen_uses_salt(keygenid type)
{
	mutils_boolean ret = MUTILS_FALSE;
	KEYGEN_ALG_LOOP(ret = p->uses_salt);
	return(ret);
}

#ifndef MMAX
# define MMAX(x,y) (x>y?x:y)
#endif

WIN32DLL_DEFINE
mutils_word32 mhash_keygen_count(void)
{
	keygenid count = 0;

	KEYGEN_LOOP(count = MMAX(p->id, count) );

	return(count);
}

WIN32DLL_DEFINE
mutils_word32 mhash_get_keygen_salt_size(keygenid type)
{
	mutils_word32 ret = 0;

	KEYGEN_ALG_LOOP(ret = p->salt_size);

	return(ret);
}

WIN32DLL_DEFINE
mutils_word32 mhash_get_keygen_max_key_size(keygenid type)
{
	mutils_word32 ret = 0;

	KEYGEN_ALG_LOOP(ret = p->max_key_size);

	return(ret);
}

WIN32DLL_DEFINE
mutils_word8 *mhash_get_keygen_name(hashid type)
{
	mutils_word8 *ret = NULL;

	/* avoid prefix */

	KEYGEN_ALG_LOOP(ret = p->name);

	if (ret != NULL)
		ret += sizeof("KEYGEN_") - 1;

	return(mutils_strdup(ret));
}

WIN32DLL_DEFINE
__const mutils_word8 *mhash_get_keygen_name_static(hashid type)
{
	mutils_word8 *ret = NULL;

	/* avoid prefix */

	KEYGEN_ALG_LOOP(ret = p->name);

	if (ret != NULL)
		ret += sizeof("KEYGEN_") - 1;

	return(ret);
}


WIN32DLL_DEFINE
mutils_error mhash_keygen(keygenid algorithm, hashid opt_algorithm,
			  mutils_word64 count,
			  void *keyword, mutils_word32 keysize,
			  void *salt, mutils_word32 saltsize,
			  mutils_word8 *password, mutils_word32 passwordlen)
{
	mutils_error x;

	switch (algorithm) {

	case KEYGEN_MCRYPT:
		x=_mhash_gen_key_mcrypt(opt_algorithm,
					keyword, keysize,
					salt, saltsize,
					password, passwordlen);
		break;
	case KEYGEN_ASIS:
		x=_mhash_gen_key_asis(keyword, keysize,
				      password, passwordlen);
		break;
	case KEYGEN_PKDES:
		x=_mhash_gen_key_pkdes(keyword, keysize,
				       password, passwordlen);
		break;
	case KEYGEN_HEX:
		x=_mhash_gen_key_hex(keyword, keysize,
				     password, passwordlen);
		break;
	case KEYGEN_S2K_SIMPLE:
		x=_mhash_gen_key_s2k_simple(opt_algorithm,
					    keyword, keysize,
					    password, passwordlen);
		break;
	case KEYGEN_S2K_SALTED:
		x=_mhash_gen_key_s2k_salted(opt_algorithm,
					    keyword, keysize,
					    salt, saltsize,
					    password, passwordlen);
		break;
	case KEYGEN_S2K_ISALTED:
		x=_mhash_gen_key_s2k_isalted(opt_algorithm, count,
					     keyword, keysize,
					     salt, saltsize,
					     password, passwordlen);
		break;
	default:
		return(-MUTILS_INVALID_FUNCTION);
	}

	return(x);
}

WIN32DLL_DEFINE
mutils_error mhash_keygen_ext(keygenid algorithm, KEYGEN data,
			      void *keyword, mutils_word32 keysize,
			      mutils_word8 *password, mutils_word32 passwordlen)
{
	mutils_error x;
	hashid opt_algorithm = data.hash_algorithm[0];
	mutils_word64 count = data.count;
	void *salt = data.salt;
	mutils_word32 saltsize = data.salt_size;

	switch (algorithm) {

	case KEYGEN_MCRYPT:
		x=_mhash_gen_key_mcrypt(opt_algorithm,
					keyword, keysize,
					salt, saltsize,
					password, passwordlen);
		break;
	case KEYGEN_ASIS:
		x=_mhash_gen_key_asis(keyword, keysize,
				      password, passwordlen);
		break;
	case KEYGEN_PKDES:
		x=_mhash_gen_key_pkdes(keyword, keysize,
				       password, passwordlen);
		break;
	case KEYGEN_HEX:
		x=_mhash_gen_key_hex(keyword, keysize,
				     password, passwordlen);
		break;
	case KEYGEN_S2K_SIMPLE:
		x=_mhash_gen_key_s2k_simple(opt_algorithm,
					    keyword, keysize,
					    password, passwordlen);
		break;
	case KEYGEN_S2K_SALTED:
		x=_mhash_gen_key_s2k_salted(opt_algorithm,
					    keyword, keysize,
					    salt, saltsize,
					    password, passwordlen);
		break;
	case KEYGEN_S2K_ISALTED:
		x=_mhash_gen_key_s2k_isalted(opt_algorithm, count,
					     keyword, keysize,
					     salt, saltsize,
					     password, passwordlen);
		break;
	default:
		return(-MUTILS_INVALID_FUNCTION);
	}

	return(x);
}
