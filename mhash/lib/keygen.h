#if !defined(__KEYGEN_H)
#define __KEYGEN_H

#include <mutils/mhash_config.h>
#include <mutils/mglobal.h>

#if defined(PROTOTYPES)

mutils_error _mhash_gen_key_asis(void *keyword, mutils_word32 key_size,
				 mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_mcrypt(hashid algorithm,
				   void *keyword, mutils_word32 key_size,
				   void *salt, mutils_word32 salt_size,
				   mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_hex(void *keyword, mutils_word32 key_size,
				mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_s2k_simple(hashid algorithm,
				       void *keyword, mutils_word32 key_size,
				       mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_s2k_salted(hashid algorithm,
				       void *keyword, mutils_word32 key_size,
				       mutils_word8 *salt, mutils_word32 salt_size,
				       mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_s2k_isalted(hashid algorithm, mutils_word64 count,
					void *keyword, mutils_word32 key_size,
					mutils_word8 *salt, mutils_word32 salt_size,
					mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_pkdes(void *keyword, mutils_word32 key_size,
				  mutils_word8 *password, mutils_word32 plen);
mutils_error _mhash_gen_key_crypt(void *keyword, mutils_word32 key_size,
				  mutils_word8 *password, mutils_word32 plen,
				  void *salt, mutils_word32 salt_size);
mutils_error _mhash_gen_key_scrypt(void *keyword, mutils_word32 key_size,
				   mutils_word8 *password, mutils_word32 plen);

#else

mutils_error _mhash_gen_key_asis();
mutils_error _mhash_gen_key_mcrypt();
mutils_error _mhash_gen_key_hex();
mutils_error _mhash_gen_key_s2k_simple();
mutils_error _mhash_gen_key_s2k_salted();
mutils_error _mhash_gen_key_s2k_isalted();
mutils_error _mhash_gen_key_pkdes();
mutils_error _mhash_gen_key_crypt();
mutils_error _mhash_gen_key_scrypt();

#endif

#if defined(KEYGEN_INTERNALS)

#define KEYGEN_ENTRY(name, uses_hash_algorithm, uses_count, uses_salt, salt_size, max_key_size) \
        { (mutils_word8 *) #name, name, uses_hash_algorithm, uses_count, uses_salt, salt_size, max_key_size }

typedef struct __mhash_keygen_entry {
	mutils_word8 *name;
	keygenid id;
	mutils_boolean uses_hash_algorithm;
	mutils_boolean uses_count;
	mutils_boolean uses_salt;
	mutils_word32 salt_size;
	mutils_word32 max_key_size;
} mhash_keygen_entry;

static __const mhash_keygen_entry keygen_algorithms[] = {
	KEYGEN_ENTRY(KEYGEN_ASIS, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_PKDES, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_HEX, 0, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_MCRYPT, 1, 0, 1, 0, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_SIMPLE, 1, 0, 0, 0, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_SALTED, 1, 0, 1, 8, 0),
	KEYGEN_ENTRY(KEYGEN_S2K_ISALTED, 1, 1, 1, 8, 0),
	{0}
};

#define KEYGEN_LOOP(b) \
        __const mhash_keygen_entry *p; \
                for (p = keygen_algorithms; p->name != NULL; p++) { b ; }

#define KEYGEN_ALG_LOOP(a) \
                        KEYGEN_LOOP(if (p->id == type) { a; break; } )

#endif

#endif
