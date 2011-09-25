#define MUTILS_USE_MHASH_CONFIG
#include <mhash.h>

int main(void)
{
	mutils_word32 i;
	mutils_word32 buf_len;
	MHASH td1, td2, td3;
	const mutils_word8 *buf = (mutils_word8 *) "This is a test buffer to test saving and restoring, see?";
	mutils_word8 *hash1;
	mutils_word8 *hash2;
	hashid alg;
	mutils_word8 mem[1024];
	mutils_word32 mem_size = sizeof(mem);

	buf_len = mutils_strlen(buf);

	/*
	 * NOTE: For laziness sake, I just loop through the enum,
	 * skipping invalid integers.
	 *
	 * If the enum should change, this loop will have to change!
	 */

	for (alg = 0; alg <= mhash_count(); ++alg)
	{
		/* if algorithm does not exist */
	  if (mhash_get_hash_name_static(alg) == (mutils_word8 *) NULL)
			continue;

		printf("Testing save/restore for algorithm %s: ", mhash_get_hash_name(alg));

		td1 = mhash_init(alg);

		if (td1 == MHASH_FAILED)
		{
			fprintf(stderr, "Failed to init td1.\n");
			exit(MUTILS_INVALID_FUNCTION);
		}

		for (i = 0; i < buf_len; ++i)
		{
			mhash(td1, buf+i, 1);
		}

		hash1 = mhash_end(td1);

		td2 = mhash_init(alg);

		if (td2 == MHASH_FAILED)
		{
			fprintf(stderr, "Failed to init td2.\n");
			exit(MUTILS_INVALID_FUNCTION);
		}

		for (i = 0; i < buf_len/2; ++i)
		{
			mhash(td2, buf+i, 1);
		}

		if (mhash_save_state_mem(td2, mem, &mem_size)!=0)
		{
			fprintf(stderr, "Error saving state. Size: %d\n", mem_size);
			exit(MUTILS_INVALID_RESULT);
		}

		td3 = mhash_restore_state_mem( mem);

		if (td3 == MHASH_FAILED)
		{
			fprintf(stderr, "Error restoring state.\n");
			exit(MUTILS_INVALID_RESULT);
		}

		for (i = buf_len/2; i < buf_len; ++i)
		{
			mhash(td3, buf+i, 1);
		}

		hash2 = mhash_end(td3);

		if (mutils_memcmp(hash1, hash2, mhash_get_block_size(alg)) == 0)
		{
			printf("Ok\n");
		}
		else
		{
			printf("Failed\n");
			exit(MUTILS_INVALID_RESULT);
		}
	}
	exit(MUTILS_OK);
}

