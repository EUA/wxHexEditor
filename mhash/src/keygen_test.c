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


#define MUTILS_USE_MHASH_CONFIG
#include <mhash.h>

#define PASSWORD1 "Hello there"
#define KEY1 "95686ac64b9d18e71e3f41445ef0e5cbec88445e83aa03a8255cb5aa234d6f8c7bd6bfc2f2eb6051c29658cfb77cd0fc1a8254612193e3ec64b38a77803eee54788421b72508372a1363e4761a83c5775bdd2edd8dc12ba4321a4a113a73902eac824ac9"

#define PASSWORD2 "One two three four"
#define KEY2 "7208aa08e528ca3ff036ca369fc067bc0aea65b8868aae3dccd9b5f25c5a23c4f074e9a421ce5f78107d820335f2f7b38ca114080b5ec47c52d7c4a18c1f79b32b258172b7c13a0f50b929fed354dc9bab4a4eea13b7fe58675624ed66095bc2b7a925a3"

int main()
{
	mutils_word8 *tmp;
	mutils_word8 *password;
	mutils_word8 *salt;
	mutils_word32 passlen;
	mutils_word32 keysize;
	mutils_word32 salt_size;
	KEYGEN data;
	mutils_word8 *key;
	mutils_word32 j;
	int result;

	passlen=sizeof(PASSWORD1);
	password = mutils_malloc(passlen+1);
	mutils_strncpy(password, (mutils_word8 *) PASSWORD1, passlen);
	
	if (mhash_get_keygen_max_key_size(KEYGEN_MCRYPT)==0) {
		keysize=100;
	} else {
		keysize = mhash_get_keygen_max_key_size(KEYGEN_MCRYPT);
	}

	if (mhash_get_keygen_salt_size(KEYGEN_MCRYPT)==0) {
		salt_size=10;
	} else {
		salt_size = mhash_get_keygen_salt_size(KEYGEN_MCRYPT);
	}

	salt = (mutils_word8 *) mutils_malloc(salt_size);
	key = (mutils_word8 *) mutils_malloc(keysize);

	if ((salt == NULL) || (key == NULL))
	{
		fprintf(stderr, "KEYGEN-Test (KEYGEN_MCRYPT): Failed\n");
		fprintf(stderr, "Cannot allocate memory.\n");
		return(MUTILS_SYSTEM_RESOURCE_ERROR);
	}

	data.hash_algorithm[0] = MHASH_MD5;
	data.count = 0;
	data.salt = salt;
	data.salt_size = salt_size;
	
	mhash_keygen_ext(KEYGEN_MCRYPT, data, key, keysize, password, passlen);

	tmp = mutils_asciify(key, keysize);

	if (tmp == NULL)
	{
		fprintf(stderr, "KEYGEN-Test (KEYGEN_MCRYPT): Failed\n");
		fprintf(stderr, "Cannot allocate memory (2).\n");
		return(MUTILS_SYSTEM_RESOURCE_ERROR);
	}

	result = mutils_strcmp((mutils_word8 *) KEY1, tmp);

	mutils_free(password);
	mutils_free(key);

	if (result != 0) {
		fprintf(stderr, "KEYGEN-Test (KEYGEN_MCRYPT): Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", KEY1, tmp);
		return(MUTILS_INVALID_RESULT);
	}

	mutils_free(tmp);

	passlen = sizeof(PASSWORD2);
	password = (mutils_word8 *) mutils_malloc(passlen + 1);
	mutils_strncpy(password, (mutils_word8 *) PASSWORD2, passlen);
	
	if (mhash_get_keygen_max_key_size(KEYGEN_S2K_SALTED)==0) {
		keysize = 100;
	} else {
		keysize = mhash_get_keygen_max_key_size(KEYGEN_S2K_SALTED);
	}
	if (mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED)==0) {
		salt_size = 10;
	} else {
		salt_size = mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED);
	}
	
	salt = (mutils_word8 *) mutils_malloc(salt_size);
	key = (mutils_word8 *) mutils_malloc(keysize);
	
	data.hash_algorithm[0] = MHASH_SHA1;
	data.count = 0;
	data.salt = salt;
	data.salt_size = salt_size;
	
	mhash_keygen_ext(KEYGEN_S2K_SALTED, data, key, keysize, password, passlen);

	mutils_memset(tmp, 0, keysize * 2);

	tmp = mutils_asciify(key, keysize);

	result = mutils_strcmp((mutils_word8 *) KEY2, tmp);

	mutils_free(password);
	mutils_free(key);

	if (mutils_strcmp((mutils_word8 *) KEY2, tmp) != 0) {
		fprintf(stderr, "KEYGEN-Test (KEYGEN_S2K_SALTED): Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", KEY2, tmp);
		return(MUTILS_INVALID_RESULT);
	}

	mutils_free(tmp);

	fprintf(stderr, "KEYGEN-Test: Succeed\n");

	return(MUTILS_OK);
}
