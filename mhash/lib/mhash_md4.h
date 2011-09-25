#if defined(ENABLE_MD4)

#if !defined(__MHASH_MD4_H)
#define __MHASH_MD4_H

struct MD4Context {
	mutils_word32 buf[4];
	mutils_word32 bits[2];
	mutils_word8  in[64];
};

void MD4Init(struct MD4Context *context);
void MD4Update(struct MD4Context *context, mutils_word8 __const *buf, mutils_word32 len);
void MD4Final( struct MD4Context *context, mutils_word8 *digest);
void MD4Transform(mutils_word32 buf[4], mutils_word32 __const in[16]);

typedef struct MD4Context MD4_CTX;

#endif /* !__MHASH_MD4_H */

#endif
