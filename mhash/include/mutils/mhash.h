/*
 *    Copyright (C) 1998 Nikos Mavroyanopoulos
 *    Copyright (C) 1999,2000 Sascha Schumman, Nikos Mavroyanopoulos
 *    Copyright (C) 2001 Nikos Mavroyanopoulos
 *    Copyright (C) 2004,2005,2006,2007,2008 MHash Development Group 
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


#if !defined(__MUTILS_MHASH_H)
#define __MUTILS_MHASH_H

/* $Id: mhash.h,v 1.2 2006/01/09 07:27:04 imipak Exp $ */

#if defined(MUTILS_USE_MHASH_CONFIG)
#include <mutils/mhash_config.h>
#endif

#include <mutils/mglobal.h>
#include <mutils/mutils.h>
#include <mutils/mtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* these are for backwards compatibility and will 
   be removed at some time */
#ifdef MHASH_BACKWARDS_COMPATIBLE
# define hmac_mhash_init mhash_hmac_init
# define hmac_mhash_end mhash_hmac_end
#endif

/* typedefs */

typedef struct mhash_hash_entry mhash_hash_entry;

#if defined(PROTOTYPES)

/* information prototypes */

mutils_word32 mhash_count(void);
mutils_word32 mhash_get_block_size(hashid type);
mutils_word8 *mhash_get_hash_name(hashid type);
void mhash_free(void *ptr);

__const mutils_word8 *mhash_get_hash_name_static(hashid type);

/* initializing prototypes */

MHASH mhash_init(hashid type);

/* copy prototypes */

MHASH mhash_cp(MHASH from);

/* update prototype */

mutils_boolean mhash(MHASH thread, const void *plaintext, mutils_word32 size);

/* finalizing prototype */

void *mhash_end(MHASH thread);
void *mhash_end_m(MHASH thread, void *(*hash_malloc) (mutils_word32));
void mhash_deinit(MHASH thread, void *result);

/* informational */

mutils_word32 mhash_get_hash_pblock(hashid type);
hashid mhash_get_mhash_algo(MHASH tmp);

/* HMAC */

MHASH mhash_hmac_init(const hashid type, void *key, mutils_word32 keysize, mutils_word32 block);
void *mhash_hmac_end_m(MHASH thread, void *(*hash_malloc) (mutils_word32));
void *mhash_hmac_end(MHASH thread);
mutils_boolean mhash_hmac_deinit(MHASH thread, void *result);

/* Save state functions */

mutils_boolean mhash_save_state_mem(MHASH thread, void *mem, mutils_word32 *mem_size);
MHASH mhash_restore_state_mem(void *mem);

/* Key generation functions */

mutils_error mhash_keygen(keygenid algorithm, hashid opt_algorithm,
			  mutils_word64 count,
			  void *keyword, mutils_word32 keysize,
			  void *salt, mutils_word32 saltsize,
			  mutils_word8 *password, mutils_word32 passwordlen);

mutils_error mhash_keygen_ext(keygenid algorithm, KEYGEN data,
			      void *keyword, mutils_word32 keysize,
			      mutils_word8 *password, mutils_word32 passwordlen);

mutils_word8 *mhash_get_keygen_name(keygenid type);

mutils_word32 mhash_get_keygen_salt_size(keygenid type);
mutils_word32 mhash_get_keygen_max_key_size(keygenid type);
mutils_word32 mhash_keygen_count(void);

mutils_boolean mhash_keygen_uses_salt(keygenid type);
mutils_boolean mhash_keygen_uses_count(keygenid type);
mutils_boolean mhash_keygen_uses_hash_algorithm(keygenid type);

#else

/* information prototypes */

mutils_word32 mhash_count();
mutils_word32 mhash_get_block_size();
mutils_word8 *mhash_get_hash_name();
void mhash_free();

__const mutils_word8 *mhash_get_hash_name_static();

/* initializing prototypes */

MHASH mhash_init();

/* copy prototypes */

MHASH mhash_cp();

/* update prototype */

mutils_boolean mhash();

/* finalizing prototype */

void *mhash_end();
void *mhash_end_m();
void mhash_deinit();

/* informational */

mutils_word32 mhash_get_hash_pblock();
hashid mhash_get_mhash_algo();

/* HMAC */

MHASH mhash_hmac_init();
void *mhash_hmac_end_m();
void *mhash_hmac_end();
mutils_boolean mhash_hmac_deinit();

/* Save state functions */

mutils_boolean mhash_save_state_mem();
MHASH mhash_restore_state_mem();

/* Key generation functions */

mutils_error mhash_keygen();
mutils_error mhash_keygen_ext();

mutils_word8 *mhash_get_keygen_name();

mutils_word32 mhash_get_keygen_salt_size();
mutils_word32 mhash_get_keygen_max_key_size();
mutils_word32 mhash_keygen_count();

mutils_boolean mhash_keygen_uses_salt();
mutils_boolean mhash_keygen_uses_count();
mutils_boolean mhash_keygen_uses_hash_algorithm();

#endif

#ifdef __cplusplus
}
#endif
#endif				/* !MHASH_H */
