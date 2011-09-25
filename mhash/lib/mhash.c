/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
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


/* $Id: mhash.c,v 1.42 2006/01/10 04:40:48 imipak Exp $ */

#include <libdefs.h>

#include <mhash_int.h>

#include <mhash_crc32.h>
#include <mhash_haval.h>
#include <mhash_md5.h>
#include <mhash_md4.h>
#include <mhash_md2.h>
#include <mhash_sha1.h>
#include <mhash_tiger.h>
#include <mhash_ripemd.h>
#include <mhash_sha256_sha224.h>
#include <mhash_sha512_sha384.h>
#include <mhash_adler32.h>
#include <mhash_gost.h>
#include <mhash_whirlpool.h>
#include <mhash_snefru.h>

/* 19/03/2000 Changes for better thread handling --nikos 
 * Actually it is thread safe.
 */

#define MAX_BLOCK_SIZE 128

#define MHASH_ENTRY(name, blksize, hash_pblock, state_size,		  \
		    init_func, hash_func, final_func, deinit_func)	  \
  { (mutils_word8 *) #name, name, blksize, hash_pblock, state_size,	  \
      (INIT_FUNC) init_func, (HASH_FUNC) hash_func,			  \
      (FINAL_FUNC) final_func, (DEINIT_FUNC) deinit_func }

struct mhash_hash_entry {
	mutils_word8 *name;
	hashid id;
	mutils_word32 blocksize;
	mutils_word32 hash_pblock;

	mutils_word32 state_size;
	INIT_FUNC init_func;
	HASH_FUNC hash_func;
	FINAL_FUNC final_func;
	DEINIT_FUNC deinit_func;
};

static __const mhash_hash_entry algorithms[] = {
#if defined(ENABLE_CRC32)
	MHASH_ENTRY(MHASH_CRC32, 4, 0, sizeof(mutils_word32), mhash_clear_crc32, 
		mhash_crc32, NULL, mhash_get_crc32),
	MHASH_ENTRY(MHASH_CRC32B, 4, 0, sizeof(mutils_word32), mhash_clear_crc32,
		mhash_crc32b, NULL, mhash_get_crc32),
#endif

#if defined(ENABLE_ADLER32)
	MHASH_ENTRY(MHASH_ADLER32, 4, 0, sizeof(mutils_word32), mhash_clear_adler32, 
		mhash_adler32, NULL, mhash_get_adler32),
#endif

#if defined(ENABLE_MD5)
	MHASH_ENTRY(MHASH_MD5, 16, 64, sizeof(MD5_CTX), MD5Init,
		MD5Update, NULL, MD5Final),
#endif

#if defined(ENABLE_MD4)
	MHASH_ENTRY(MHASH_MD4, 16, 64, sizeof(MD4_CTX), MD4Init, 
		MD4Update, NULL, MD4Final),
#endif

#if defined(ENABLE_MD2)
	MHASH_ENTRY(MHASH_MD2, 16, 16, sizeof(MD2_CTX), md2_init,
		md2_update, NULL, md2_digest),
#endif

#if defined(ENABLE_SHA1)
	MHASH_ENTRY(MHASH_SHA1, 20, 64, sizeof(SHA_CTX), sha_init, 
		sha_update, sha_final, sha_digest),
#endif

#if defined(ENABLE_SHA256_SHA224)
	MHASH_ENTRY(MHASH_SHA256, 32, 64, sizeof( SHA256_SHA224_CTX), sha256_init,
		sha256_sha224_update, sha256_sha224_final, sha256_digest),
	MHASH_ENTRY(MHASH_SHA224, 28, 64, sizeof( SHA256_SHA224_CTX), sha224_init,
		sha256_sha224_update, sha256_sha224_final, sha224_digest),
#endif

#if defined(ENABLE_SHA512_SHA384)
	MHASH_ENTRY(MHASH_SHA512, 64, 128, sizeof( SHA512_SHA384_CTX), sha512_init,
		sha512_sha384_update, sha512_sha384_final, sha512_digest),
	MHASH_ENTRY(MHASH_SHA384, 48, 128, sizeof( SHA512_SHA384_CTX), sha384_init,
		sha512_sha384_update, sha512_sha384_final, sha384_digest),
#endif

#if defined(ENABLE_HAVAL)
	MHASH_ENTRY(MHASH_HAVAL256, 32, 128, sizeof(havalContext), havalInit256,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL128, 16, 128, sizeof(havalContext), havalInit128,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL160, 20, 128, sizeof(havalContext), havalInit160,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL192, 24, 128, sizeof(havalContext), havalInit192,
		havalUpdate, NULL, havalFinal),
	MHASH_ENTRY(MHASH_HAVAL224, 28, 128, sizeof(havalContext), havalInit224,
		havalUpdate, NULL, havalFinal),
#endif

#if defined(ENABLE_RIPEMD)
	MHASH_ENTRY(MHASH_RIPEMD128, 16, 64, sizeof(RIPEMD_CTX), ripemd128_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD160, 20, 64, sizeof(RIPEMD_CTX), ripemd160_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD256, 32, 64, sizeof(RIPEMD_CTX), ripemd256_init, 
		ripemd_update, ripemd_final, ripemd_digest),
	MHASH_ENTRY(MHASH_RIPEMD320, 40, 64, sizeof(RIPEMD_CTX), ripemd320_init, 
		ripemd_update, ripemd_final, ripemd_digest),
#endif

#if defined(ENABLE_TIGER)
	MHASH_ENTRY(MHASH_TIGER, 24, 64, sizeof(TIGER_CTX), tiger_init, 
		tiger_update, tiger_final, tiger_digest),
	MHASH_ENTRY(MHASH_TIGER128, 16, 64, sizeof(TIGER_CTX), tiger_init,
		tiger_update, tiger_final, tiger128_digest),
	MHASH_ENTRY(MHASH_TIGER160, 20, 64, sizeof(TIGER_CTX), tiger_init, 
		tiger_update, tiger_final, tiger160_digest),
#endif

#if defined(ENABLE_GOST)
	MHASH_ENTRY(MHASH_GOST, 32, 0, sizeof(GostHashCtx), gosthash_reset, 
		gosthash_update, NULL, gosthash_final),
#endif

#if defined(ENABLE_WHIRLPOOL)
	MHASH_ENTRY(MHASH_WHIRLPOOL, 64, 64, sizeof(WHIRLPOOL_CTX), whirlpool_init, 
		    whirlpool_update, whirlpool_final, whirlpool_digest),
#endif

#if defined(ENABLE_SNEFRU)
	MHASH_ENTRY(MHASH_SNEFRU128, 16, 64, sizeof(SNEFRU_CTX), snefru_init, 
		    snefru128_update, snefru128_final, snefru128_digest),
	MHASH_ENTRY(MHASH_SNEFRU256, 32, 64, sizeof(SNEFRU_CTX), snefru_init, 
		    snefru256_update, snefru256_final, snefru256_digest),
#endif

	{0}
};

#define MHASH_LOOP(b) \
	__const mhash_hash_entry *p; \
	for (p = algorithms; p->name != NULL; p++) { b ; }

#define MHASH_ALG_LOOP(a) \
	MHASH_LOOP( if (p->id == type) { a; break; } )

#ifndef MMAX
# define MMAX(x,y) (x>y?x:y)
#endif

WIN32DLL_DEFINE mutils_word32 mhash_count(void)
{
	hashid count = 0;

	MHASH_LOOP( count = MMAX( p->id, count) );

	return count;
}

WIN32DLL_DEFINE mutils_word32 mhash_get_block_size(hashid type)
{
	mutils_word32 ret = 0;

	MHASH_ALG_LOOP(ret = p->blocksize);
	return ret;
}

WIN32DLL_DEFINE mutils_word32 _mhash_get_state_size(hashid type)
{
	/* Fixed: Valid state sizes that exceeded maximum
	   positive value for integer would be returned as
	   an error.
	*/

	mutils_word32 size = 0;

	MHASH_ALG_LOOP(size = p->state_size);
	return size;
}

WIN32DLL_DEFINE INIT_FUNC _mhash_get_init_func(hashid type)
{
	INIT_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->init_func);
	return func;
}
WIN32DLL_DEFINE HASH_FUNC _mhash_get_hash_func(hashid type)
{
	HASH_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->hash_func);
	return func;
}
WIN32DLL_DEFINE FINAL_FUNC _mhash_get_final_func(hashid type)
{
	FINAL_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->final_func);
	return func;
}
WIN32DLL_DEFINE DEINIT_FUNC _mhash_get_deinit_func(hashid type)
{
	DEINIT_FUNC func = NULL;

	MHASH_ALG_LOOP(func = p->deinit_func);
	return func;
}


WIN32DLL_DEFINE hashid mhash_get_mhash_algo( MHASH tmp) {
	return tmp->algorithm_given;
}

WIN32DLL_DEFINE mutils_word8 *mhash_get_hash_name(hashid type)
{
	mutils_word8 *ret = NULL;

	/* avoid prefix */
	MHASH_ALG_LOOP(ret = p->name);

	if (ret != NULL)
	{
		ret += sizeof("MHASH_") - 1;
	}

	return mutils_strdup(ret);
}

WIN32DLL_DEFINE __const mutils_word8 *mhash_get_hash_name_static(hashid type)
{
	mutils_word8 *ret = NULL;

	/* avoid prefix */
	MHASH_ALG_LOOP( ret = p->name);
	
	if ( ret != NULL)
	{
		ret += sizeof("MHASH_") - 1;
	}

	return ret;
}

MHASH mhash_cp(MHASH from) {
MHASH ret;

	ret = (MHASH) mutils_malloc(sizeof(MHASH_INSTANCE));
 
	if (ret == NULL)
	{
	  return(MHASH_FAILED);
	}

	mutils_memcpy(ret, from, sizeof(MHASH_INSTANCE));
	
	/* copy the internal state also */
	ret->state = (mutils_word8 *) mutils_malloc(ret->state_size);

	if (ret->state == NULL)
	{
		mutils_free(ret);
		return(MHASH_FAILED);
	}

	mutils_memcpy(ret->state, from->state, ret->state_size);
	
	/* copy the key in case of hmac*/
	if (ret->hmac_key_size != 0)
	{
		ret->hmac_key = (mutils_word8 *) mutils_malloc(ret->hmac_key_size);

		if (ret == NULL)
		{
			mutils_free(ret->state);
			mutils_free(ret);
			return(MHASH_FAILED);
		}

		mutils_memcpy(ret->hmac_key, from->hmac_key, ret->hmac_key_size);
	}
	return ret;
}

MHASH mhash_init_int(__const hashid type)
{
	MHASH ret;
	INIT_FUNC func;

	ret = (MHASH) mutils_malloc(sizeof(MHASH_INSTANCE));
	if (ret == NULL)
	{
		return MHASH_FAILED;
	}

	mutils_memset(ret, 0, sizeof(MHASH_INSTANCE));

	ret->algorithm_given = type;

	ret->state_size = _mhash_get_state_size(type);
	if (ret->state_size == 0)
	{
		mutils_free(ret);
		return(MHASH_FAILED);
	}
	
	if ( (ret->state = mutils_malloc(ret->state_size)) == NULL)
	{
		mutils_free(ret);
		return(MHASH_FAILED);
	}

	func = _mhash_get_init_func( type);

	if (func != NULL)
	{
		func(ret->state);
	}
	else
	{
		mutils_free(ret->state);
		mutils_free(ret);
		return(MHASH_FAILED);
	}

	ret->hash_func = _mhash_get_hash_func( type);
	ret->deinit_func = _mhash_get_deinit_func( type);
	ret->final_func = _mhash_get_final_func( type);

	return ret;
		
}

/* plaintext should be a multiple of the algorithm's block size */

mutils_boolean mhash(MHASH td, __const void *plaintext, mutils_word32 size)
{
	
	if (td->hash_func!=NULL)
	{
		td->hash_func( td->state, plaintext, size);
	}

	return(MUTILS_OK);
}

WIN32DLL_DEFINE
    void mhash_deinit(MHASH td, void *result)
{
	
	if (td->final_func != NULL)
	{
		td->final_func( td->state);
	}

	if (td->deinit_func != NULL)
	{
		td->deinit_func( td->state, result);
	}

	if (NULL != td->state)
	{
		mutils_free(td->state);
	}

	mutils_free(td);

	return;
}

WIN32DLL_DEFINE
    void *mhash_end_m(MHASH td, void *(*hash_malloc) (mutils_word32))
{
	void *digest;
	mutils_word32 size;

	size = mhash_get_block_size( td->algorithm_given);
	
	digest = mutils_malloc(size);

	if (digest==NULL)
	{
		return NULL;
	}

	mhash_deinit( td, digest);
	
	return(digest);
}

WIN32DLL_DEFINE void *mhash_end(MHASH td)
{
	return(mhash_end_m(td, mutils_malloc));
}


WIN32DLL_DEFINE MHASH mhash_init(__const hashid type)
{
	MHASH ret = MHASH_FAILED;

	ret = mhash_init_int(type);

	return(ret);
}

/* HMAC functions */

WIN32DLL_DEFINE mutils_word32 mhash_get_hash_pblock(hashid type)
{
	mutils_word32 ret = 0;

	MHASH_ALG_LOOP(ret = p->hash_pblock);
	return(ret);
}

WIN32DLL_DEFINE
    mutils_error mhash_hmac_deinit(MHASH td, void *result)
{
	mutils_word8 *opad;
	mutils_word8 _opad[MAX_BLOCK_SIZE];
	MHASH tmptd;
	mutils_word32 i;
	mutils_word32 opad_alloc = 0;

	if (td->hmac_block > MAX_BLOCK_SIZE)
	{
		opad = mutils_malloc(td->hmac_block);
		if (opad == NULL)
		{
			return(-MUTILS_SYSTEM_RESOURCE_ERROR);
		}
		opad_alloc = 1;
	}
	else
	{
		opad = _opad;
	}


	for (i = 0; i < td->hmac_key_size; i++)
	{
		opad[i] = (0x5C) ^ td->hmac_key[i];
	}

	for (; i < td->hmac_block; i++)
	{
		opad[i] = (0x5C);
	}

	tmptd = mhash_init(td->algorithm_given);
	mhash(tmptd, opad, td->hmac_block);

	if (td->final_func != NULL)
	{
		td->final_func(td->state);
	}

	if (td->deinit_func != NULL)
	{
		td->deinit_func(td->state, result);
	}

	if (result != NULL)
	{
		mhash(tmptd, result,
		      mhash_get_block_size(td->algorithm_given));
	}

	mutils_free(td->state);

	if (opad_alloc!=0)
	{
		mutils_free(opad);
	}

	mutils_bzero(td->hmac_key, td->hmac_key_size);
	mutils_free(td->hmac_key);
	mutils_free(td);

	mhash_deinit(tmptd, result);

	return(MUTILS_OK);
}


WIN32DLL_DEFINE
    void *mhash_hmac_end_m(MHASH td, void *(*hash_malloc) (mutils_word32))
{
	void *digest;

	digest =
	    hash_malloc(mhash_get_block_size
			(td->algorithm_given));

	if (digest == NULL)
	{
		return(NULL);
	}

	mhash_hmac_deinit(td, digest);
	
	return(digest);
}

WIN32DLL_DEFINE void *mhash_hmac_end(MHASH td)
{
	return mhash_hmac_end_m(td, mutils_malloc);
}

WIN32DLL_DEFINE
    MHASH mhash_hmac_init(__const hashid type,
			  void *key, mutils_word32 keysize,
			  mutils_word32 block)
{
	MHASH ret = MHASH_FAILED;
	MHASH tmptd;
	mutils_word8 *ipad;
	mutils_word8 _ipad[MAX_BLOCK_SIZE];
	mutils_word32 i;
	mutils_boolean ipad_alloc = MUTILS_FALSE;
	mutils_boolean res;

	if (block == 0)
	{
		block = 64;	/* the default for ripemd,md5,sha-1 */
	}

	ret = mhash_init_int(type);

	if (ret != MHASH_FAILED)
	{
		/* Initial hmac calculations */
		ret->hmac_block = block;

		if ( ret->hmac_block > MAX_BLOCK_SIZE)
		{
			ipad = mutils_malloc(ret->hmac_block);
			if (ipad == NULL)
			{
				return MHASH_FAILED;
			}
			ipad_alloc = MUTILS_TRUE;
		}
		else
		{
			ipad = _ipad;
		}
		
		if (keysize > ret->hmac_block)
		{
			tmptd = mhash_init(type);
			mhash(tmptd, key, keysize);
			ret->hmac_key_size = mhash_get_block_size(type);
			ret->hmac_key = mhash_end(tmptd);
		}
		else
		{
			ret->hmac_key = mutils_malloc(ret->hmac_block);
			mutils_bzero(ret->hmac_key, ret->hmac_block);
			mutils_memcpy(ret->hmac_key, key, keysize);
			ret->hmac_key_size = ret->hmac_block;
		}

		/* IPAD */

		for (i = 0; i < ret->hmac_key_size; i++)
		{
			ipad[i] = (0x36) ^ ret->hmac_key[i];
		}

		for (; i < ret->hmac_block; i++)
		{
			ipad[i] = (0x36);
		}

		res = mhash(ret, ipad, ret->hmac_block);

		if (ipad_alloc == MUTILS_TRUE)
		{
			mutils_free(ipad);
		}
	}

	return(ret);
}

WIN32DLL_DEFINE void mhash_free(void *ptr)
{
        mutils_free(ptr);
}


/*
  Saves the state of a hashing algorithm such that it can be
  restored at some later point in time using
  mhash_restore_state().
  
  mem_size should contain the size of the given _mem pointer.
  Otherwise the required size will be copied there.

  Original version and idea by Blake Stephen <Stephen.Blake@veritect.com>
*/
WIN32DLL_DEFINE mutils_error mhash_save_state_mem(MHASH td, void *_mem, mutils_word32 *mem_size )
{
	mutils_word32 tot_size;
	mutils_word32 pos;
	mutils_word8 *mem = _mem;
	
	tot_size = sizeof(td->algorithm_given) + sizeof(td->hmac_key_size)
		+ sizeof(td->hmac_block) + td->hmac_key_size +
		+ sizeof(td->state_size) + td->state_size;
	
	if ( *mem_size < tot_size)
	{
		*mem_size = tot_size;
		return(MUTILS_INVALID_INPUT_BUFFER);
	}
	
	if ( mem != NULL)
	{
		pos = 0;
		mutils_memcpy( mem, &td->algorithm_given, sizeof(td->algorithm_given));
		pos = sizeof( td->algorithm_given);
		
		mutils_memcpy( &mem[pos], &td->hmac_key_size, sizeof(td->hmac_key_size));
		pos += sizeof(td->hmac_key_size);

		mutils_memcpy( &mem[pos], &td->hmac_block, sizeof(td->hmac_block));
		pos += sizeof(td->hmac_block);

		mutils_memcpy( &mem[pos], td->hmac_key, td->hmac_key_size);
		pos += td->hmac_key_size;

		mutils_memcpy( &mem[pos], &td->state_size, sizeof(td->state_size));
		pos += sizeof(td->state_size);

		mutils_memcpy( &mem[pos], td->state, td->state_size);
		pos += td->state_size;

	}
	return(MUTILS_OK);
}


/*
  Restores the state of a hashing algorithm that was saved
  using mhash_save_state(). Use like mhash_init.
*/
WIN32DLL_DEFINE MHASH mhash_restore_state_mem(void* _mem)
{
	mutils_word8 *mem = _mem;
	hashid algorithm_given;
	MHASH ret = MHASH_FAILED;
	mutils_word32 pos;

	if (mem==NULL)
	{
		return(ret);
	}

	mutils_memcpy( &algorithm_given, mem, sizeof(algorithm_given));
	
	if ((ret = mhash_init(algorithm_given)) == MHASH_FAILED)
	{
		return(ret);
	}

	ret->algorithm_given = algorithm_given;
	
	pos = sizeof(algorithm_given);

 	mutils_memcpy( &ret->hmac_key_size, &mem[pos], sizeof(ret->hmac_key_size));
	pos += sizeof( ret->hmac_key_size);
		
	mutils_memcpy( &ret->hmac_block, &mem[pos], sizeof(ret->hmac_block));
	pos += sizeof(ret->hmac_block);

	if (ret->hmac_key_size != 0)
	{
		ret->hmac_key = mutils_malloc(ret->hmac_key_size);
		if (ret->hmac_key == NULL)
		{
			goto freeall;
		}
	
		mutils_memcpy( ret->hmac_key, &mem[pos], ret->hmac_key_size);
		pos += sizeof(ret->hmac_key_size);
	}

	mutils_memcpy( &ret->state_size, &mem[pos], sizeof(ret->state_size));
	pos += sizeof( ret->state_size);

	ret->state = mutils_malloc(ret->state_size);
	if (ret->state==NULL)
		goto freeall;

	mutils_memcpy( ret->state, &mem[pos], ret->state_size);
	pos += ret->state_size;

	ret->hash_func = _mhash_get_hash_func( algorithm_given);
	ret->deinit_func = _mhash_get_deinit_func( algorithm_given);
	ret->final_func = _mhash_get_final_func( algorithm_given);

	return(ret);

	freeall:
		/* This uses too much internals
		 */
		mutils_free(ret->state);
		mutils_free(ret->hmac_key);
		mutils_free(ret);
		return(MHASH_FAILED);
}

#if defined(WIN32) || defined (__CYGWIN__)
WIN32DLL_DEFINE int main (void)
{
       /* empty main function to avoid linker error (see cygwin FAQ) */
}
#endif
