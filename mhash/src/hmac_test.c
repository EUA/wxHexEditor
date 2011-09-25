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

#define KEY1 "Jefe"
#define DATA1 "what do ya want for nothing?"
#define DIGEST1 "750c783e6ab0b503eaa86e310a5db738"

#define KEY2 ""
#define DATA2 "Hi There"
#define DIGEST2 "9294727a3638bb1c13f48ef8158bfc9d"

int main()
{

	mutils_word8 *tmp;
	mutils_word8 *password;
	mutils_word32 passlen;
	mutils_word8 *data;
	mutils_word32 datalen;
	MHASH td;
	mutils_word8 *mac;
	mutils_word32 j;
	int result;

	passlen=sizeof(KEY1) - 1;
	password = mutils_malloc(passlen + 1);
	mutils_memcpy(password, (mutils_word8 *) KEY1, passlen);

	datalen = mutils_strlen((mutils_word8 *) DATA1);
	data = mutils_malloc(datalen+1);
	mutils_strcpy(data, (mutils_word8 *) DATA1);

	td = mhash_hmac_init(MHASH_MD5, password, passlen,
			    mhash_get_hash_pblock(MHASH_MD5));

	mhash(td, data, datalen);
	mac = mhash_hmac_end(td);

	tmp = mutils_asciify(mac, mhash_get_block_size(MHASH_MD5));
	
	result = mutils_strcmp((mutils_word8 *) DIGEST1, tmp);

	mutils_free(password);
	mutils_free(data);

	if (result != 0) {
		fprintf(stderr, "HMAC-Test: Failed\n");
		fprintf(stderr, "Digest size: %d\n", mhash_get_block_size(MHASH_MD5));
		fprintf(stderr, "Expecting: 0x%s\n", DIGEST1);
		fprintf(stderr, "Got: 0x%s\n", tmp);
		return(MUTILS_INVALID_RESULT);
	}

	mutils_free(tmp);

	/* Test No 2 */	
	
	mutils_memset(tmp, 0, sizeof(tmp));
	
	passlen=sizeof(KEY2) - 1;
	password = (mutils_word8 *) mutils_malloc(passlen+1);
	mutils_memcpy(password, KEY2, passlen);
	
	datalen = mutils_strlen((mutils_word8 *) DATA2);
	data = (mutils_word8 *) mutils_malloc(datalen+1);
	mutils_strcpy(data, (mutils_word8 *) DATA2);

	td = mhash_hmac_init(MHASH_MD5, password, passlen,
			    mhash_get_hash_pblock(MHASH_MD5));

	mhash(td, data, datalen);
	mac = mhash_hmac_end(td);

	tmp = mutils_asciify(mac, mhash_get_block_size(MHASH_MD5));
	
	result = mutils_strcmp((mutils_word8 *) DIGEST2, tmp);

	mutils_free(password);
	mutils_free(data);

	if (result != 0)
	{
		fprintf(stderr, "HMAC-Test: Failed\n");
		fprintf(stderr, "Expecting: 0x%s\nGot: 0x%s\n", DIGEST2, tmp);
		return(MUTILS_INVALID_RESULT);
	}

	fprintf(stderr, "MD5 HMAC-Test: Ok\n");

	mutils_free(tmp);

	return(MUTILS_OK);
}
