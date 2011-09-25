#if defined(ENABLE_MD5)

#if !defined(__MHASH_MD5_H)
#define __MHASH_MD5_H

struct MD5Context {
	mutils_word32 buf[4];
	mutils_word32 bits[2];
	mutils_word8  in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, mutils_word8 __const *buf, mutils_word32 len);
void MD5Final(struct MD5Context *context, mutils_word8 *digest);
void MD5Transform(mutils_word32 buf[4], mutils_word32 __const in[16]);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct MD5Context MD5_CTX;

#endif /* !__MHASH_MD5_H */

#endif
