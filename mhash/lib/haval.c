/*
 * The HAVAL hashing function
 *
 * Public domain implementation by Paulo S.L.M. Barreto <pbarreto@nw.com.br>
 *
 * Version 1.1 (1998.04.19)
 * 
 * Differences from Version 1.0 (1997.04.03):
 *
 * -	Added more parentheses to enforce operator precedence in the definitions
 *		of the nonlinear F functions, as some compilers do not automatically
 *		apply the standard precedence rules of the C language (Borland C++ 5.02
 *      being the culprit in mind)
 *
 * -    Similar changes in HavalUpdate() and in the ROTR macro for 
 *      the same reason.
 *
 * -    Explicitly make casts when reducing precision of an integer value
 *      or from char to byte (= unsigned char) to silence compilation warnings
 *      from Borland C++
 *
 * -	Explicitly defined LITTLE_ENDIAN for compilation under MSVC++ and
 *		Borland C++.
 *
 * -	Fixed the sense of the ROTR macro
 *
 * -	Fixed behaviour on big-endian platforms by doing byte reversal when
 *		unpacking the final digest from the hash context structure to the
 *      digest argument of HavalFinal()
 *
 * -    Forcefully note that the code as is does not work on VMS
 *
 * We are grateful to Mr. Tines <tines@windsong.demon.co.uk> for pointing out
 * these problems, providing solutions for them, and testing the fixed code.
 *
 * -----------------------------------------------------------------------------
 *
 * Reference: Zheng, Y., Pieprzyk, J., Seberry, J.:
 * "HAVAL - a one-way hashing algorithm with variable length of output",
 * Advances in Cryptology (AusCrypt'92), LNCS 718 (1993), 83-104, Springer-Verlag.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libdefs.h>

#if defined(ENABLE_HAVAL)

#include <mhash_haval.h>

#define HAVAL_VERSION	1

/* Nonlinear F functions */

#define F1(X6, X5, X4, X3, X2, X1, X0) \
	(((X1) & ((X4) ^ (X0))) ^ ((X2) & (X5)) ^ ((X3) & (X6)) ^ (X0))

#define F2(X6, X5, X4, X3, X2, X1, X0) \
	(((X2) & (((X1) & (~(X3))) ^ ((X4) & (X5)) ^ (X6) ^ (X0))) ^ \
	(((X4) & ((X1) ^ (X5))) ^ ((X3) & (X5)) ^ (X0)))

#define F3(X6, X5, X4, X3, X2, X1, X0) \
	(((X3) & (((X1) & (X2)) ^ (X6) ^ (X0))) ^ ((X1) & (X4)) ^ \
	((X2) & (X5)) ^ (X0))

#define F4(X6, X5, X4, X3, X2, X1, X0) \
	(((X4) & (((~(X2)) & (X5)) ^ ((X3) | (X6)) ^ (X1) ^ (X0))) ^ \
	((X3) & (((X1) & (X2)) ^ (X5) ^ (X6))) ^ ((X2) & (X6)) ^ (X0))

#define F5(X6, X5, X4, X3, X2, X1, X0) \
	(((X1) & ((X4) ^ ((X0) & (X2) & (X3)))) ^ \
	(((X2) ^ (X0)) & (X5)) ^ ((X3) & (X6)) ^ (X0))


#define ROTR(v,n)   (((v) >> ((mutils_word32)(n))) | ((v) << (32 - (mutils_word32)(n))))

/*
The following macro would generate better code on an Intel platform,
however it is never used there, and this code when used in e.g
havalTransform3()

  		t = ((mutils_word32 *)in)[i];

blithely casts byte aligned data to a longword aligned type, causing
an immediate core dump on a RISC system

#define BYTE_REVERSE(out, in) \
{ \
	register mutils_word32 t; \
	register mutils_word8 i; \
	for (i = 0; i < 32; i++) { \
		t = ((mutils_word32 *)in)[i]; \
		t = ROTR (t, 16); \
		((mutils_word32 *)out)[i] = ((t & 0xFF00FF00UL) >> 8) | ((t & 0x00FF00FFUL) << 8); \
	} \
}

Therefore, allow for unaligned access and move the bytes by hand.
This assumes that out and in do not overlap, which is the case in
practice here.
*/

static void havalTransform3 (mutils_word32 E[8], __const mutils_word32 D[32], mutils_word32 T[8])
{
	mutils_word32 *W;

#if defined(WORDS_BIGENDIAN)
	W = mutils_word32nswap(D, 32, MUTILS_FALSE);
#else  /* LITTLE ENDIAN */
	W = (mutils_word32 *) D;
#endif /* Byte re-ordering */

	/* PASS 1: */

	T[7] = ROTR (F1 (E[1], E[0], E[3], E[5], E[6], E[2], E[4]), 7) + ROTR (E[7], 11) + W[ 0];
	T[6] = ROTR (F1 (E[0], T[7], E[2], E[4], E[5], E[1], E[3]), 7) + ROTR (E[6], 11) + W[ 1];
	T[5] = ROTR (F1 (T[7], T[6], E[1], E[3], E[4], E[0], E[2]), 7) + ROTR (E[5], 11) + W[ 2];
	T[4] = ROTR (F1 (T[6], T[5], E[0], E[2], E[3], T[7], E[1]), 7) + ROTR (E[4], 11) + W[ 3];
	T[3] = ROTR (F1 (T[5], T[4], T[7], E[1], E[2], T[6], E[0]), 7) + ROTR (E[3], 11) + W[ 4];
	T[2] = ROTR (F1 (T[4], T[3], T[6], E[0], E[1], T[5], T[7]), 7) + ROTR (E[2], 11) + W[ 5];
	T[1] = ROTR (F1 (T[3], T[2], T[5], T[7], E[0], T[4], T[6]), 7) + ROTR (E[1], 11) + W[ 6];
	T[0] = ROTR (F1 (T[2], T[1], T[4], T[6], T[7], T[3], T[5]), 7) + ROTR (E[0], 11) + W[ 7];


	T[7] = ROTR (F1 (T[1], T[0], T[3], T[5], T[6], T[2], T[4]), 7) + ROTR (T[7], 11) + W[ 8];
	T[6] = ROTR (F1 (T[0], T[7], T[2], T[4], T[5], T[1], T[3]), 7) + ROTR (T[6], 11) + W[ 9];
	T[5] = ROTR (F1 (T[7], T[6], T[1], T[3], T[4], T[0], T[2]), 7) + ROTR (T[5], 11) + W[10];
	T[4] = ROTR (F1 (T[6], T[5], T[0], T[2], T[3], T[7], T[1]), 7) + ROTR (T[4], 11) + W[11];
	T[3] = ROTR (F1 (T[5], T[4], T[7], T[1], T[2], T[6], T[0]), 7) + ROTR (T[3], 11) + W[12];
	T[2] = ROTR (F1 (T[4], T[3], T[6], T[0], T[1], T[5], T[7]), 7) + ROTR (T[2], 11) + W[13];
	T[1] = ROTR (F1 (T[3], T[2], T[5], T[7], T[0], T[4], T[6]), 7) + ROTR (T[1], 11) + W[14];
	T[0] = ROTR (F1 (T[2], T[1], T[4], T[6], T[7], T[3], T[5]), 7) + ROTR (T[0], 11) + W[15];

	T[7] = ROTR (F1 (T[1], T[0], T[3], T[5], T[6], T[2], T[4]), 7) + ROTR (T[7], 11) + W[16];
	T[6] = ROTR (F1 (T[0], T[7], T[2], T[4], T[5], T[1], T[3]), 7) + ROTR (T[6], 11) + W[17];
	T[5] = ROTR (F1 (T[7], T[6], T[1], T[3], T[4], T[0], T[2]), 7) + ROTR (T[5], 11) + W[18];
	T[4] = ROTR (F1 (T[6], T[5], T[0], T[2], T[3], T[7], T[1]), 7) + ROTR (T[4], 11) + W[19];
	T[3] = ROTR (F1 (T[5], T[4], T[7], T[1], T[2], T[6], T[0]), 7) + ROTR (T[3], 11) + W[20];
	T[2] = ROTR (F1 (T[4], T[3], T[6], T[0], T[1], T[5], T[7]), 7) + ROTR (T[2], 11) + W[21];
	T[1] = ROTR (F1 (T[3], T[2], T[5], T[7], T[0], T[4], T[6]), 7) + ROTR (T[1], 11) + W[22];
	T[0] = ROTR (F1 (T[2], T[1], T[4], T[6], T[7], T[3], T[5]), 7) + ROTR (T[0], 11) + W[23];

	T[7] = ROTR (F1 (T[1], T[0], T[3], T[5], T[6], T[2], T[4]), 7) + ROTR (T[7], 11) + W[24];
	T[6] = ROTR (F1 (T[0], T[7], T[2], T[4], T[5], T[1], T[3]), 7) + ROTR (T[6], 11) + W[25];
	T[5] = ROTR (F1 (T[7], T[6], T[1], T[3], T[4], T[0], T[2]), 7) + ROTR (T[5], 11) + W[26];
	T[4] = ROTR (F1 (T[6], T[5], T[0], T[2], T[3], T[7], T[1]), 7) + ROTR (T[4], 11) + W[27];
	T[3] = ROTR (F1 (T[5], T[4], T[7], T[1], T[2], T[6], T[0]), 7) + ROTR (T[3], 11) + W[28];
	T[2] = ROTR (F1 (T[4], T[3], T[6], T[0], T[1], T[5], T[7]), 7) + ROTR (T[2], 11) + W[29];
	T[1] = ROTR (F1 (T[3], T[2], T[5], T[7], T[0], T[4], T[6]), 7) + ROTR (T[1], 11) + W[30];
	T[0] = ROTR (F1 (T[2], T[1], T[4], T[6], T[7], T[3], T[5]), 7) + ROTR (T[0], 11) + W[31];

	/* PASS 2: */

	T[7] = ROTR (F2 (T[4], T[2], T[1], T[0], T[5], T[3], T[6]), 7) + ROTR (T[7], 11) + W[ 5] + 0x452821E6UL;
	T[6] = ROTR (F2 (T[3], T[1], T[0], T[7], T[4], T[2], T[5]), 7) + ROTR (T[6], 11) + W[14] + 0x38D01377UL;
	T[5] = ROTR (F2 (T[2], T[0], T[7], T[6], T[3], T[1], T[4]), 7) + ROTR (T[5], 11) + W[26] + 0xBE5466CFUL;
	T[4] = ROTR (F2 (T[1], T[7], T[6], T[5], T[2], T[0], T[3]), 7) + ROTR (T[4], 11) + W[18] + 0x34E90C6CUL;
	T[3] = ROTR (F2 (T[0], T[6], T[5], T[4], T[1], T[7], T[2]), 7) + ROTR (T[3], 11) + W[11] + 0xC0AC29B7UL;
	T[2] = ROTR (F2 (T[7], T[5], T[4], T[3], T[0], T[6], T[1]), 7) + ROTR (T[2], 11) + W[28] + 0xC97C50DDUL;
	T[1] = ROTR (F2 (T[6], T[4], T[3], T[2], T[7], T[5], T[0]), 7) + ROTR (T[1], 11) + W[ 7] + 0x3F84D5B5UL;
	T[0] = ROTR (F2 (T[5], T[3], T[2], T[1], T[6], T[4], T[7]), 7) + ROTR (T[0], 11) + W[16] + 0xB5470917UL;

	T[7] = ROTR (F2 (T[4], T[2], T[1], T[0], T[5], T[3], T[6]), 7) + ROTR (T[7], 11) + W[ 0] + 0x9216D5D9UL;
	T[6] = ROTR (F2 (T[3], T[1], T[0], T[7], T[4], T[2], T[5]), 7) + ROTR (T[6], 11) + W[23] + 0x8979FB1BUL;
	T[5] = ROTR (F2 (T[2], T[0], T[7], T[6], T[3], T[1], T[4]), 7) + ROTR (T[5], 11) + W[20] + 0xD1310BA6UL;
	T[4] = ROTR (F2 (T[1], T[7], T[6], T[5], T[2], T[0], T[3]), 7) + ROTR (T[4], 11) + W[22] + 0x98DFB5ACUL;
	T[3] = ROTR (F2 (T[0], T[6], T[5], T[4], T[1], T[7], T[2]), 7) + ROTR (T[3], 11) + W[ 1] + 0x2FFD72DBUL;
	T[2] = ROTR (F2 (T[7], T[5], T[4], T[3], T[0], T[6], T[1]), 7) + ROTR (T[2], 11) + W[10] + 0xD01ADFB7UL;
	T[1] = ROTR (F2 (T[6], T[4], T[3], T[2], T[7], T[5], T[0]), 7) + ROTR (T[1], 11) + W[ 4] + 0xB8E1AFEDUL;
	T[0] = ROTR (F2 (T[5], T[3], T[2], T[1], T[6], T[4], T[7]), 7) + ROTR (T[0], 11) + W[ 8] + 0x6A267E96UL;

	T[7] = ROTR (F2 (T[4], T[2], T[1], T[0], T[5], T[3], T[6]), 7) + ROTR (T[7], 11) + W[30] + 0xBA7C9045UL;
	T[6] = ROTR (F2 (T[3], T[1], T[0], T[7], T[4], T[2], T[5]), 7) + ROTR (T[6], 11) + W[ 3] + 0xF12C7F99UL;
	T[5] = ROTR (F2 (T[2], T[0], T[7], T[6], T[3], T[1], T[4]), 7) + ROTR (T[5], 11) + W[21] + 0x24A19947UL;
	T[4] = ROTR (F2 (T[1], T[7], T[6], T[5], T[2], T[0], T[3]), 7) + ROTR (T[4], 11) + W[ 9] + 0xB3916CF7UL;
	T[3] = ROTR (F2 (T[0], T[6], T[5], T[4], T[1], T[7], T[2]), 7) + ROTR (T[3], 11) + W[17] + 0x0801F2E2UL;
	T[2] = ROTR (F2 (T[7], T[5], T[4], T[3], T[0], T[6], T[1]), 7) + ROTR (T[2], 11) + W[24] + 0x858EFC16UL;
	T[1] = ROTR (F2 (T[6], T[4], T[3], T[2], T[7], T[5], T[0]), 7) + ROTR (T[1], 11) + W[29] + 0x636920D8UL;
	T[0] = ROTR (F2 (T[5], T[3], T[2], T[1], T[6], T[4], T[7]), 7) + ROTR (T[0], 11) + W[ 6] + 0x71574E69UL;

	T[7] = ROTR (F2 (T[4], T[2], T[1], T[0], T[5], T[3], T[6]), 7) + ROTR (T[7], 11) + W[19] + 0xA458FEA3UL;
	T[6] = ROTR (F2 (T[3], T[1], T[0], T[7], T[4], T[2], T[5]), 7) + ROTR (T[6], 11) + W[12] + 0xF4933D7EUL;
	T[5] = ROTR (F2 (T[2], T[0], T[7], T[6], T[3], T[1], T[4]), 7) + ROTR (T[5], 11) + W[15] + 0x0D95748FUL;
	T[4] = ROTR (F2 (T[1], T[7], T[6], T[5], T[2], T[0], T[3]), 7) + ROTR (T[4], 11) + W[13] + 0x728EB658UL;
	T[3] = ROTR (F2 (T[0], T[6], T[5], T[4], T[1], T[7], T[2]), 7) + ROTR (T[3], 11) + W[ 2] + 0x718BCD58UL;
	T[2] = ROTR (F2 (T[7], T[5], T[4], T[3], T[0], T[6], T[1]), 7) + ROTR (T[2], 11) + W[25] + 0x82154AEEUL;
	T[1] = ROTR (F2 (T[6], T[4], T[3], T[2], T[7], T[5], T[0]), 7) + ROTR (T[1], 11) + W[31] + 0x7B54A41DUL;
	T[0] = ROTR (F2 (T[5], T[3], T[2], T[1], T[6], T[4], T[7]), 7) + ROTR (T[0], 11) + W[27] + 0xC25A59B5UL;

	/* PASS 3: */

	T[7] = ROTR (F3 (T[6], T[1], T[2], T[3], T[4], T[5], T[0]), 7) + ROTR (T[7], 11) + W[19] + 0x9C30D539UL;
	T[6] = ROTR (F3 (T[5], T[0], T[1], T[2], T[3], T[4], T[7]), 7) + ROTR (T[6], 11) + W[ 9] + 0x2AF26013UL;
	T[5] = ROTR (F3 (T[4], T[7], T[0], T[1], T[2], T[3], T[6]), 7) + ROTR (T[5], 11) + W[ 4] + 0xC5D1B023UL;
	T[4] = ROTR (F3 (T[3], T[6], T[7], T[0], T[1], T[2], T[5]), 7) + ROTR (T[4], 11) + W[20] + 0x286085F0UL;
	T[3] = ROTR (F3 (T[2], T[5], T[6], T[7], T[0], T[1], T[4]), 7) + ROTR (T[3], 11) + W[28] + 0xCA417918UL;
	T[2] = ROTR (F3 (T[1], T[4], T[5], T[6], T[7], T[0], T[3]), 7) + ROTR (T[2], 11) + W[17] + 0xB8DB38EFUL;
	T[1] = ROTR (F3 (T[0], T[3], T[4], T[5], T[6], T[7], T[2]), 7) + ROTR (T[1], 11) + W[ 8] + 0x8E79DCB0UL;
	T[0] = ROTR (F3 (T[7], T[2], T[3], T[4], T[5], T[6], T[1]), 7) + ROTR (T[0], 11) + W[22] + 0x603A180EUL;

	T[7] = ROTR (F3 (T[6], T[1], T[2], T[3], T[4], T[5], T[0]), 7) + ROTR (T[7], 11) + W[29] + 0x6C9E0E8BUL;
	T[6] = ROTR (F3 (T[5], T[0], T[1], T[2], T[3], T[4], T[7]), 7) + ROTR (T[6], 11) + W[14] + 0xB01E8A3EUL;
	T[5] = ROTR (F3 (T[4], T[7], T[0], T[1], T[2], T[3], T[6]), 7) + ROTR (T[5], 11) + W[25] + 0xD71577C1UL;
	T[4] = ROTR (F3 (T[3], T[6], T[7], T[0], T[1], T[2], T[5]), 7) + ROTR (T[4], 11) + W[12] + 0xBD314B27UL;
	T[3] = ROTR (F3 (T[2], T[5], T[6], T[7], T[0], T[1], T[4]), 7) + ROTR (T[3], 11) + W[24] + 0x78AF2FDAUL;
	T[2] = ROTR (F3 (T[1], T[4], T[5], T[6], T[7], T[0], T[3]), 7) + ROTR (T[2], 11) + W[30] + 0x55605C60UL;
	T[1] = ROTR (F3 (T[0], T[3], T[4], T[5], T[6], T[7], T[2]), 7) + ROTR (T[1], 11) + W[16] + 0xE65525F3UL;
	T[0] = ROTR (F3 (T[7], T[2], T[3], T[4], T[5], T[6], T[1]), 7) + ROTR (T[0], 11) + W[26] + 0xAA55AB94UL;

	T[7] = ROTR (F3 (T[6], T[1], T[2], T[3], T[4], T[5], T[0]), 7) + ROTR (T[7], 11) + W[31] + 0x57489862UL;
	T[6] = ROTR (F3 (T[5], T[0], T[1], T[2], T[3], T[4], T[7]), 7) + ROTR (T[6], 11) + W[15] + 0x63E81440UL;
	T[5] = ROTR (F3 (T[4], T[7], T[0], T[1], T[2], T[3], T[6]), 7) + ROTR (T[5], 11) + W[ 7] + 0x55CA396AUL;
	T[4] = ROTR (F3 (T[3], T[6], T[7], T[0], T[1], T[2], T[5]), 7) + ROTR (T[4], 11) + W[ 3] + 0x2AAB10B6UL;
	T[3] = ROTR (F3 (T[2], T[5], T[6], T[7], T[0], T[1], T[4]), 7) + ROTR (T[3], 11) + W[ 1] + 0xB4CC5C34UL;
	T[2] = ROTR (F3 (T[1], T[4], T[5], T[6], T[7], T[0], T[3]), 7) + ROTR (T[2], 11) + W[ 0] + 0x1141E8CEUL;
	T[1] = ROTR (F3 (T[0], T[3], T[4], T[5], T[6], T[7], T[2]), 7) + ROTR (T[1], 11) + W[18] + 0xA15486AFUL;
	T[0] = ROTR (F3 (T[7], T[2], T[3], T[4], T[5], T[6], T[1]), 7) + ROTR (T[0], 11) + W[27] + 0x7C72E993UL;

	E[7] += T[7] = ROTR (F3 (T[6], T[1], T[2], T[3], T[4], T[5], T[0]), 7) + ROTR (T[7], 11) + W[13] + 0xB3EE1411UL;
	E[6] += T[6] = ROTR (F3 (T[5], T[0], T[1], T[2], T[3], T[4], T[7]), 7) + ROTR (T[6], 11) + W[ 6] + 0x636FBC2AUL;
	E[5] += T[5] = ROTR (F3 (T[4], T[7], T[0], T[1], T[2], T[3], T[6]), 7) + ROTR (T[5], 11) + W[21] + 0x2BA9C55DUL;
	E[4] += T[4] = ROTR (F3 (T[3], T[6], T[7], T[0], T[1], T[2], T[5]), 7) + ROTR (T[4], 11) + W[10] + 0x741831F6UL;
	E[3] += T[3] = ROTR (F3 (T[2], T[5], T[6], T[7], T[0], T[1], T[4]), 7) + ROTR (T[3], 11) + W[23] + 0xCE5C3E16UL;
	E[2] += T[2] = ROTR (F3 (T[1], T[4], T[5], T[6], T[7], T[0], T[3]), 7) + ROTR (T[2], 11) + W[11] + 0x9B87931EUL;
	E[1] += T[1] = ROTR (F3 (T[0], T[3], T[4], T[5], T[6], T[7], T[2]), 7) + ROTR (T[1], 11) + W[ 5] + 0xAFD6BA33UL;
	E[0] += T[0] = ROTR (F3 (T[7], T[2], T[3], T[4], T[5], T[6], T[1]), 7) + ROTR (T[0], 11) + W[ 2] + 0x6C24CF5CUL;

#if defined(WORDS_BIGENDIAN)
	mutils_free(W);
#endif
} /* havalTransform3 */


static void havalTransform4 (mutils_word32 E[8], __const mutils_word32 D[32], mutils_word32 T[8])
{
	mutils_word32 *W;

#if defined(WORDS_BIGENDIAN)
	W = mutils_word32nswap(D, 32, MUTILS_FALSE);
#else  /* LITTLE ENDIAN */
	W = (mutils_word32 *) D;
#endif /* Byte re-ordering */

	/* PASS 1: */

	T[7] = ROTR (F1 (E[2], E[6], E[1], E[4], E[5], E[3], E[0]), 7) + ROTR (E[7], 11) + W[ 0];
	T[6] = ROTR (F1 (E[1], E[5], E[0], E[3], E[4], E[2], T[7]), 7) + ROTR (E[6], 11) + W[ 1];
	T[5] = ROTR (F1 (E[0], E[4], T[7], E[2], E[3], E[1], T[6]), 7) + ROTR (E[5], 11) + W[ 2];
	T[4] = ROTR (F1 (T[7], E[3], T[6], E[1], E[2], E[0], T[5]), 7) + ROTR (E[4], 11) + W[ 3];
	T[3] = ROTR (F1 (T[6], E[2], T[5], E[0], E[1], T[7], T[4]), 7) + ROTR (E[3], 11) + W[ 4];
	T[2] = ROTR (F1 (T[5], E[1], T[4], T[7], E[0], T[6], T[3]), 7) + ROTR (E[2], 11) + W[ 5];
	T[1] = ROTR (F1 (T[4], E[0], T[3], T[6], T[7], T[5], T[2]), 7) + ROTR (E[1], 11) + W[ 6];
	T[0] = ROTR (F1 (T[3], T[7], T[2], T[5], T[6], T[4], T[1]), 7) + ROTR (E[0], 11) + W[ 7];

	T[7] = ROTR (F1 (T[2], T[6], T[1], T[4], T[5], T[3], T[0]), 7) + ROTR (T[7], 11) + W[ 8];
	T[6] = ROTR (F1 (T[1], T[5], T[0], T[3], T[4], T[2], T[7]), 7) + ROTR (T[6], 11) + W[ 9];
	T[5] = ROTR (F1 (T[0], T[4], T[7], T[2], T[3], T[1], T[6]), 7) + ROTR (T[5], 11) + W[10];
	T[4] = ROTR (F1 (T[7], T[3], T[6], T[1], T[2], T[0], T[5]), 7) + ROTR (T[4], 11) + W[11];
	T[3] = ROTR (F1 (T[6], T[2], T[5], T[0], T[1], T[7], T[4]), 7) + ROTR (T[3], 11) + W[12];
	T[2] = ROTR (F1 (T[5], T[1], T[4], T[7], T[0], T[6], T[3]), 7) + ROTR (T[2], 11) + W[13];
	T[1] = ROTR (F1 (T[4], T[0], T[3], T[6], T[7], T[5], T[2]), 7) + ROTR (T[1], 11) + W[14];
	T[0] = ROTR (F1 (T[3], T[7], T[2], T[5], T[6], T[4], T[1]), 7) + ROTR (T[0], 11) + W[15];

	T[7] = ROTR (F1 (T[2], T[6], T[1], T[4], T[5], T[3], T[0]), 7) + ROTR (T[7], 11) + W[16];
	T[6] = ROTR (F1 (T[1], T[5], T[0], T[3], T[4], T[2], T[7]), 7) + ROTR (T[6], 11) + W[17];
	T[5] = ROTR (F1 (T[0], T[4], T[7], T[2], T[3], T[1], T[6]), 7) + ROTR (T[5], 11) + W[18];
	T[4] = ROTR (F1 (T[7], T[3], T[6], T[1], T[2], T[0], T[5]), 7) + ROTR (T[4], 11) + W[19];
	T[3] = ROTR (F1 (T[6], T[2], T[5], T[0], T[1], T[7], T[4]), 7) + ROTR (T[3], 11) + W[20];
	T[2] = ROTR (F1 (T[5], T[1], T[4], T[7], T[0], T[6], T[3]), 7) + ROTR (T[2], 11) + W[21];
	T[1] = ROTR (F1 (T[4], T[0], T[3], T[6], T[7], T[5], T[2]), 7) + ROTR (T[1], 11) + W[22];
	T[0] = ROTR (F1 (T[3], T[7], T[2], T[5], T[6], T[4], T[1]), 7) + ROTR (T[0], 11) + W[23];

	T[7] = ROTR (F1 (T[2], T[6], T[1], T[4], T[5], T[3], T[0]), 7) + ROTR (T[7], 11) + W[24];
	T[6] = ROTR (F1 (T[1], T[5], T[0], T[3], T[4], T[2], T[7]), 7) + ROTR (T[6], 11) + W[25];
	T[5] = ROTR (F1 (T[0], T[4], T[7], T[2], T[3], T[1], T[6]), 7) + ROTR (T[5], 11) + W[26];
	T[4] = ROTR (F1 (T[7], T[3], T[6], T[1], T[2], T[0], T[5]), 7) + ROTR (T[4], 11) + W[27];
	T[3] = ROTR (F1 (T[6], T[2], T[5], T[0], T[1], T[7], T[4]), 7) + ROTR (T[3], 11) + W[28];
	T[2] = ROTR (F1 (T[5], T[1], T[4], T[7], T[0], T[6], T[3]), 7) + ROTR (T[2], 11) + W[29];
	T[1] = ROTR (F1 (T[4], T[0], T[3], T[6], T[7], T[5], T[2]), 7) + ROTR (T[1], 11) + W[30];
	T[0] = ROTR (F1 (T[3], T[7], T[2], T[5], T[6], T[4], T[1]), 7) + ROTR (T[0], 11) + W[31];

	/* PASS 2: */

	T[7] = ROTR (F2 (T[3], T[5], T[2], T[0], T[1], T[6], T[4]), 7) + ROTR (T[7], 11) + W[ 5] + 0x452821E6UL;
	T[6] = ROTR (F2 (T[2], T[4], T[1], T[7], T[0], T[5], T[3]), 7) + ROTR (T[6], 11) + W[14] + 0x38D01377UL;
	T[5] = ROTR (F2 (T[1], T[3], T[0], T[6], T[7], T[4], T[2]), 7) + ROTR (T[5], 11) + W[26] + 0xBE5466CFUL;
	T[4] = ROTR (F2 (T[0], T[2], T[7], T[5], T[6], T[3], T[1]), 7) + ROTR (T[4], 11) + W[18] + 0x34E90C6CUL;
	T[3] = ROTR (F2 (T[7], T[1], T[6], T[4], T[5], T[2], T[0]), 7) + ROTR (T[3], 11) + W[11] + 0xC0AC29B7UL;
	T[2] = ROTR (F2 (T[6], T[0], T[5], T[3], T[4], T[1], T[7]), 7) + ROTR (T[2], 11) + W[28] + 0xC97C50DDUL;
	T[1] = ROTR (F2 (T[5], T[7], T[4], T[2], T[3], T[0], T[6]), 7) + ROTR (T[1], 11) + W[ 7] + 0x3F84D5B5UL;
	T[0] = ROTR (F2 (T[4], T[6], T[3], T[1], T[2], T[7], T[5]), 7) + ROTR (T[0], 11) + W[16] + 0xB5470917UL;

	T[7] = ROTR (F2 (T[3], T[5], T[2], T[0], T[1], T[6], T[4]), 7) + ROTR (T[7], 11) + W[ 0] + 0x9216D5D9UL;
	T[6] = ROTR (F2 (T[2], T[4], T[1], T[7], T[0], T[5], T[3]), 7) + ROTR (T[6], 11) + W[23] + 0x8979FB1BUL;
	T[5] = ROTR (F2 (T[1], T[3], T[0], T[6], T[7], T[4], T[2]), 7) + ROTR (T[5], 11) + W[20] + 0xD1310BA6UL;
	T[4] = ROTR (F2 (T[0], T[2], T[7], T[5], T[6], T[3], T[1]), 7) + ROTR (T[4], 11) + W[22] + 0x98DFB5ACUL;
	T[3] = ROTR (F2 (T[7], T[1], T[6], T[4], T[5], T[2], T[0]), 7) + ROTR (T[3], 11) + W[ 1] + 0x2FFD72DBUL;
	T[2] = ROTR (F2 (T[6], T[0], T[5], T[3], T[4], T[1], T[7]), 7) + ROTR (T[2], 11) + W[10] + 0xD01ADFB7UL;
	T[1] = ROTR (F2 (T[5], T[7], T[4], T[2], T[3], T[0], T[6]), 7) + ROTR (T[1], 11) + W[ 4] + 0xB8E1AFEDUL;
	T[0] = ROTR (F2 (T[4], T[6], T[3], T[1], T[2], T[7], T[5]), 7) + ROTR (T[0], 11) + W[ 8] + 0x6A267E96UL;

	T[7] = ROTR (F2 (T[3], T[5], T[2], T[0], T[1], T[6], T[4]), 7) + ROTR (T[7], 11) + W[30] + 0xBA7C9045UL;
	T[6] = ROTR (F2 (T[2], T[4], T[1], T[7], T[0], T[5], T[3]), 7) + ROTR (T[6], 11) + W[ 3] + 0xF12C7F99UL;
	T[5] = ROTR (F2 (T[1], T[3], T[0], T[6], T[7], T[4], T[2]), 7) + ROTR (T[5], 11) + W[21] + 0x24A19947UL;
	T[4] = ROTR (F2 (T[0], T[2], T[7], T[5], T[6], T[3], T[1]), 7) + ROTR (T[4], 11) + W[ 9] + 0xB3916CF7UL;
	T[3] = ROTR (F2 (T[7], T[1], T[6], T[4], T[5], T[2], T[0]), 7) + ROTR (T[3], 11) + W[17] + 0x0801F2E2UL;
	T[2] = ROTR (F2 (T[6], T[0], T[5], T[3], T[4], T[1], T[7]), 7) + ROTR (T[2], 11) + W[24] + 0x858EFC16UL;
	T[1] = ROTR (F2 (T[5], T[7], T[4], T[2], T[3], T[0], T[6]), 7) + ROTR (T[1], 11) + W[29] + 0x636920D8UL;
	T[0] = ROTR (F2 (T[4], T[6], T[3], T[1], T[2], T[7], T[5]), 7) + ROTR (T[0], 11) + W[ 6] + 0x71574E69UL;

	T[7] = ROTR (F2 (T[3], T[5], T[2], T[0], T[1], T[6], T[4]), 7) + ROTR (T[7], 11) + W[19] + 0xA458FEA3UL;
	T[6] = ROTR (F2 (T[2], T[4], T[1], T[7], T[0], T[5], T[3]), 7) + ROTR (T[6], 11) + W[12] + 0xF4933D7EUL;
	T[5] = ROTR (F2 (T[1], T[3], T[0], T[6], T[7], T[4], T[2]), 7) + ROTR (T[5], 11) + W[15] + 0x0D95748FUL;
	T[4] = ROTR (F2 (T[0], T[2], T[7], T[5], T[6], T[3], T[1]), 7) + ROTR (T[4], 11) + W[13] + 0x728EB658UL;
	T[3] = ROTR (F2 (T[7], T[1], T[6], T[4], T[5], T[2], T[0]), 7) + ROTR (T[3], 11) + W[ 2] + 0x718BCD58UL;
	T[2] = ROTR (F2 (T[6], T[0], T[5], T[3], T[4], T[1], T[7]), 7) + ROTR (T[2], 11) + W[25] + 0x82154AEEUL;
	T[1] = ROTR (F2 (T[5], T[7], T[4], T[2], T[3], T[0], T[6]), 7) + ROTR (T[1], 11) + W[31] + 0x7B54A41DUL;
	T[0] = ROTR (F2 (T[4], T[6], T[3], T[1], T[2], T[7], T[5]), 7) + ROTR (T[0], 11) + W[27] + 0xC25A59B5UL;

	/* PASS 3: */

	T[7] = ROTR (F3 (T[1], T[4], T[3], T[6], T[0], T[2], T[5]), 7) + ROTR (T[7], 11) + W[19] + 0x9C30D539UL;
	T[6] = ROTR (F3 (T[0], T[3], T[2], T[5], T[7], T[1], T[4]), 7) + ROTR (T[6], 11) + W[ 9] + 0x2AF26013UL;
	T[5] = ROTR (F3 (T[7], T[2], T[1], T[4], T[6], T[0], T[3]), 7) + ROTR (T[5], 11) + W[ 4] + 0xC5D1B023UL;
	T[4] = ROTR (F3 (T[6], T[1], T[0], T[3], T[5], T[7], T[2]), 7) + ROTR (T[4], 11) + W[20] + 0x286085F0UL;
	T[3] = ROTR (F3 (T[5], T[0], T[7], T[2], T[4], T[6], T[1]), 7) + ROTR (T[3], 11) + W[28] + 0xCA417918UL;
	T[2] = ROTR (F3 (T[4], T[7], T[6], T[1], T[3], T[5], T[0]), 7) + ROTR (T[2], 11) + W[17] + 0xB8DB38EFUL;
	T[1] = ROTR (F3 (T[3], T[6], T[5], T[0], T[2], T[4], T[7]), 7) + ROTR (T[1], 11) + W[ 8] + 0x8E79DCB0UL;
	T[0] = ROTR (F3 (T[2], T[5], T[4], T[7], T[1], T[3], T[6]), 7) + ROTR (T[0], 11) + W[22] + 0x603A180EUL;

	T[7] = ROTR (F3 (T[1], T[4], T[3], T[6], T[0], T[2], T[5]), 7) + ROTR (T[7], 11) + W[29] + 0x6C9E0E8BUL;
	T[6] = ROTR (F3 (T[0], T[3], T[2], T[5], T[7], T[1], T[4]), 7) + ROTR (T[6], 11) + W[14] + 0xB01E8A3EUL;
	T[5] = ROTR (F3 (T[7], T[2], T[1], T[4], T[6], T[0], T[3]), 7) + ROTR (T[5], 11) + W[25] + 0xD71577C1UL;
	T[4] = ROTR (F3 (T[6], T[1], T[0], T[3], T[5], T[7], T[2]), 7) + ROTR (T[4], 11) + W[12] + 0xBD314B27UL;
	T[3] = ROTR (F3 (T[5], T[0], T[7], T[2], T[4], T[6], T[1]), 7) + ROTR (T[3], 11) + W[24] + 0x78AF2FDAUL;
	T[2] = ROTR (F3 (T[4], T[7], T[6], T[1], T[3], T[5], T[0]), 7) + ROTR (T[2], 11) + W[30] + 0x55605C60UL;
	T[1] = ROTR (F3 (T[3], T[6], T[5], T[0], T[2], T[4], T[7]), 7) + ROTR (T[1], 11) + W[16] + 0xE65525F3UL;
	T[0] = ROTR (F3 (T[2], T[5], T[4], T[7], T[1], T[3], T[6]), 7) + ROTR (T[0], 11) + W[26] + 0xAA55AB94UL;

	T[7] = ROTR (F3 (T[1], T[4], T[3], T[6], T[0], T[2], T[5]), 7) + ROTR (T[7], 11) + W[31] + 0x57489862UL;
	T[6] = ROTR (F3 (T[0], T[3], T[2], T[5], T[7], T[1], T[4]), 7) + ROTR (T[6], 11) + W[15] + 0x63E81440UL;
	T[5] = ROTR (F3 (T[7], T[2], T[1], T[4], T[6], T[0], T[3]), 7) + ROTR (T[5], 11) + W[ 7] + 0x55CA396AUL;
	T[4] = ROTR (F3 (T[6], T[1], T[0], T[3], T[5], T[7], T[2]), 7) + ROTR (T[4], 11) + W[ 3] + 0x2AAB10B6UL;
	T[3] = ROTR (F3 (T[5], T[0], T[7], T[2], T[4], T[6], T[1]), 7) + ROTR (T[3], 11) + W[ 1] + 0xB4CC5C34UL;
	T[2] = ROTR (F3 (T[4], T[7], T[6], T[1], T[3], T[5], T[0]), 7) + ROTR (T[2], 11) + W[ 0] + 0x1141E8CEUL;
	T[1] = ROTR (F3 (T[3], T[6], T[5], T[0], T[2], T[4], T[7]), 7) + ROTR (T[1], 11) + W[18] + 0xA15486AFUL;
	T[0] = ROTR (F3 (T[2], T[5], T[4], T[7], T[1], T[3], T[6]), 7) + ROTR (T[0], 11) + W[27] + 0x7C72E993UL;

	T[7] = ROTR (F3 (T[1], T[4], T[3], T[6], T[0], T[2], T[5]), 7) + ROTR (T[7], 11) + W[13] + 0xB3EE1411UL;
	T[6] = ROTR (F3 (T[0], T[3], T[2], T[5], T[7], T[1], T[4]), 7) + ROTR (T[6], 11) + W[ 6] + 0x636FBC2AUL;
	T[5] = ROTR (F3 (T[7], T[2], T[1], T[4], T[6], T[0], T[3]), 7) + ROTR (T[5], 11) + W[21] + 0x2BA9C55DUL;
	T[4] = ROTR (F3 (T[6], T[1], T[0], T[3], T[5], T[7], T[2]), 7) + ROTR (T[4], 11) + W[10] + 0x741831F6UL;
	T[3] = ROTR (F3 (T[5], T[0], T[7], T[2], T[4], T[6], T[1]), 7) + ROTR (T[3], 11) + W[23] + 0xCE5C3E16UL;
	T[2] = ROTR (F3 (T[4], T[7], T[6], T[1], T[3], T[5], T[0]), 7) + ROTR (T[2], 11) + W[11] + 0x9B87931EUL;
	T[1] = ROTR (F3 (T[3], T[6], T[5], T[0], T[2], T[4], T[7]), 7) + ROTR (T[1], 11) + W[ 5] + 0xAFD6BA33UL;
	T[0] = ROTR (F3 (T[2], T[5], T[4], T[7], T[1], T[3], T[6]), 7) + ROTR (T[0], 11) + W[ 2] + 0x6C24CF5CUL;

	/* PASS 4: */

	T[7] = ROTR (F4 (T[6], T[4], T[0], T[5], T[2], T[1], T[3]), 7) + ROTR (T[7], 11) + W[24] + 0x7A325381UL;
	T[6] = ROTR (F4 (T[5], T[3], T[7], T[4], T[1], T[0], T[2]), 7) + ROTR (T[6], 11) + W[ 4] + 0x28958677UL;
	T[5] = ROTR (F4 (T[4], T[2], T[6], T[3], T[0], T[7], T[1]), 7) + ROTR (T[5], 11) + W[ 0] + 0x3B8F4898UL;
	T[4] = ROTR (F4 (T[3], T[1], T[5], T[2], T[7], T[6], T[0]), 7) + ROTR (T[4], 11) + W[14] + 0x6B4BB9AFUL;
	T[3] = ROTR (F4 (T[2], T[0], T[4], T[1], T[6], T[5], T[7]), 7) + ROTR (T[3], 11) + W[ 2] + 0xC4BFE81BUL;
	T[2] = ROTR (F4 (T[1], T[7], T[3], T[0], T[5], T[4], T[6]), 7) + ROTR (T[2], 11) + W[ 7] + 0x66282193UL;
	T[1] = ROTR (F4 (T[0], T[6], T[2], T[7], T[4], T[3], T[5]), 7) + ROTR (T[1], 11) + W[28] + 0x61D809CCUL;
	T[0] = ROTR (F4 (T[7], T[5], T[1], T[6], T[3], T[2], T[4]), 7) + ROTR (T[0], 11) + W[23] + 0xFB21A991UL;

	T[7] = ROTR (F4 (T[6], T[4], T[0], T[5], T[2], T[1], T[3]), 7) + ROTR (T[7], 11) + W[26] + 0x487CAC60UL;
	T[6] = ROTR (F4 (T[5], T[3], T[7], T[4], T[1], T[0], T[2]), 7) + ROTR (T[6], 11) + W[ 6] + 0x5DEC8032UL;
	T[5] = ROTR (F4 (T[4], T[2], T[6], T[3], T[0], T[7], T[1]), 7) + ROTR (T[5], 11) + W[30] + 0xEF845D5DUL;
	T[4] = ROTR (F4 (T[3], T[1], T[5], T[2], T[7], T[6], T[0]), 7) + ROTR (T[4], 11) + W[20] + 0xE98575B1UL;
	T[3] = ROTR (F4 (T[2], T[0], T[4], T[1], T[6], T[5], T[7]), 7) + ROTR (T[3], 11) + W[18] + 0xDC262302UL;
	T[2] = ROTR (F4 (T[1], T[7], T[3], T[0], T[5], T[4], T[6]), 7) + ROTR (T[2], 11) + W[25] + 0xEB651B88UL;
	T[1] = ROTR (F4 (T[0], T[6], T[2], T[7], T[4], T[3], T[5]), 7) + ROTR (T[1], 11) + W[19] + 0x23893E81UL;
	T[0] = ROTR (F4 (T[7], T[5], T[1], T[6], T[3], T[2], T[4]), 7) + ROTR (T[0], 11) + W[ 3] + 0xD396ACC5UL;

	T[7] = ROTR (F4 (T[6], T[4], T[0], T[5], T[2], T[1], T[3]), 7) + ROTR (T[7], 11) + W[22] + 0x0F6D6FF3UL;
	T[6] = ROTR (F4 (T[5], T[3], T[7], T[4], T[1], T[0], T[2]), 7) + ROTR (T[6], 11) + W[11] + 0x83F44239UL;
	T[5] = ROTR (F4 (T[4], T[2], T[6], T[3], T[0], T[7], T[1]), 7) + ROTR (T[5], 11) + W[31] + 0x2E0B4482UL;
	T[4] = ROTR (F4 (T[3], T[1], T[5], T[2], T[7], T[6], T[0]), 7) + ROTR (T[4], 11) + W[21] + 0xA4842004UL;
	T[3] = ROTR (F4 (T[2], T[0], T[4], T[1], T[6], T[5], T[7]), 7) + ROTR (T[3], 11) + W[ 8] + 0x69C8F04AUL;
	T[2] = ROTR (F4 (T[1], T[7], T[3], T[0], T[5], T[4], T[6]), 7) + ROTR (T[2], 11) + W[27] + 0x9E1F9B5EUL;
	T[1] = ROTR (F4 (T[0], T[6], T[2], T[7], T[4], T[3], T[5]), 7) + ROTR (T[1], 11) + W[12] + 0x21C66842UL;
	T[0] = ROTR (F4 (T[7], T[5], T[1], T[6], T[3], T[2], T[4]), 7) + ROTR (T[0], 11) + W[ 9] + 0xF6E96C9AUL;

	E[7] += T[7] = ROTR (F4 (T[6], T[4], T[0], T[5], T[2], T[1], T[3]), 7) + ROTR (T[7], 11) + W[ 1] + 0x670C9C61UL;
	E[6] += T[6] = ROTR (F4 (T[5], T[3], T[7], T[4], T[1], T[0], T[2]), 7) + ROTR (T[6], 11) + W[29] + 0xABD388F0UL;
	E[5] += T[5] = ROTR (F4 (T[4], T[2], T[6], T[3], T[0], T[7], T[1]), 7) + ROTR (T[5], 11) + W[ 5] + 0x6A51A0D2UL;
	E[4] += T[4] = ROTR (F4 (T[3], T[1], T[5], T[2], T[7], T[6], T[0]), 7) + ROTR (T[4], 11) + W[15] + 0xD8542F68UL;
	E[3] += T[3] = ROTR (F4 (T[2], T[0], T[4], T[1], T[6], T[5], T[7]), 7) + ROTR (T[3], 11) + W[17] + 0x960FA728UL;
	E[2] += T[2] = ROTR (F4 (T[1], T[7], T[3], T[0], T[5], T[4], T[6]), 7) + ROTR (T[2], 11) + W[10] + 0xAB5133A3UL;
	E[1] += T[1] = ROTR (F4 (T[0], T[6], T[2], T[7], T[4], T[3], T[5]), 7) + ROTR (T[1], 11) + W[16] + 0x6EEF0B6CUL;
	E[0] += T[0] = ROTR (F4 (T[7], T[5], T[1], T[6], T[3], T[2], T[4]), 7) + ROTR (T[0], 11) + W[13] + 0x137A3BE4UL;

#if defined(WORDS_BIGENDIAN)
	mutils_free(W);
#endif
} /* havalTransform4 */


static void havalTransform5 (mutils_word32 E[8], __const mutils_word32 D[32], mutils_word32 T[8])
{
	mutils_word32 *W;

#if defined(WORDS_BIGENDIAN)
	W = mutils_word32nswap(D, 32, MUTILS_FALSE);
#else  /* LITTLE ENDIAN */
	W = (mutils_word32 *) D;
#endif /* Byte re-ordering */

	/* PASS 1: */

	T[7] = ROTR (F1 (E[3], E[4], E[1], E[0], E[5], E[2], E[6]), 7) + ROTR (E[7], 11) + W[ 0];
	T[6] = ROTR (F1 (E[2], E[3], E[0], T[7], E[4], E[1], E[5]), 7) + ROTR (E[6], 11) + W[ 1];
	T[5] = ROTR (F1 (E[1], E[2], T[7], T[6], E[3], E[0], E[4]), 7) + ROTR (E[5], 11) + W[ 2];
	T[4] = ROTR (F1 (E[0], E[1], T[6], T[5], E[2], T[7], E[3]), 7) + ROTR (E[4], 11) + W[ 3];
	T[3] = ROTR (F1 (T[7], E[0], T[5], T[4], E[1], T[6], E[2]), 7) + ROTR (E[3], 11) + W[ 4];
	T[2] = ROTR (F1 (T[6], T[7], T[4], T[3], E[0], T[5], E[1]), 7) + ROTR (E[2], 11) + W[ 5];
	T[1] = ROTR (F1 (T[5], T[6], T[3], T[2], T[7], T[4], E[0]), 7) + ROTR (E[1], 11) + W[ 6];
	T[0] = ROTR (F1 (T[4], T[5], T[2], T[1], T[6], T[3], T[7]), 7) + ROTR (E[0], 11) + W[ 7];

	T[7] = ROTR (F1 (T[3], T[4], T[1], T[0], T[5], T[2], T[6]), 7) + ROTR (T[7], 11) + W[ 8];
	T[6] = ROTR (F1 (T[2], T[3], T[0], T[7], T[4], T[1], T[5]), 7) + ROTR (T[6], 11) + W[ 9];
	T[5] = ROTR (F1 (T[1], T[2], T[7], T[6], T[3], T[0], T[4]), 7) + ROTR (T[5], 11) + W[10];
	T[4] = ROTR (F1 (T[0], T[1], T[6], T[5], T[2], T[7], T[3]), 7) + ROTR (T[4], 11) + W[11];
	T[3] = ROTR (F1 (T[7], T[0], T[5], T[4], T[1], T[6], T[2]), 7) + ROTR (T[3], 11) + W[12];
	T[2] = ROTR (F1 (T[6], T[7], T[4], T[3], T[0], T[5], T[1]), 7) + ROTR (T[2], 11) + W[13];
	T[1] = ROTR (F1 (T[5], T[6], T[3], T[2], T[7], T[4], T[0]), 7) + ROTR (T[1], 11) + W[14];
	T[0] = ROTR (F1 (T[4], T[5], T[2], T[1], T[6], T[3], T[7]), 7) + ROTR (T[0], 11) + W[15];

	T[7] = ROTR (F1 (T[3], T[4], T[1], T[0], T[5], T[2], T[6]), 7) + ROTR (T[7], 11) + W[16];
	T[6] = ROTR (F1 (T[2], T[3], T[0], T[7], T[4], T[1], T[5]), 7) + ROTR (T[6], 11) + W[17];
	T[5] = ROTR (F1 (T[1], T[2], T[7], T[6], T[3], T[0], T[4]), 7) + ROTR (T[5], 11) + W[18];
	T[4] = ROTR (F1 (T[0], T[1], T[6], T[5], T[2], T[7], T[3]), 7) + ROTR (T[4], 11) + W[19];
	T[3] = ROTR (F1 (T[7], T[0], T[5], T[4], T[1], T[6], T[2]), 7) + ROTR (T[3], 11) + W[20];
	T[2] = ROTR (F1 (T[6], T[7], T[4], T[3], T[0], T[5], T[1]), 7) + ROTR (T[2], 11) + W[21];
	T[1] = ROTR (F1 (T[5], T[6], T[3], T[2], T[7], T[4], T[0]), 7) + ROTR (T[1], 11) + W[22];
	T[0] = ROTR (F1 (T[4], T[5], T[2], T[1], T[6], T[3], T[7]), 7) + ROTR (T[0], 11) + W[23];

	T[7] = ROTR (F1 (T[3], T[4], T[1], T[0], T[5], T[2], T[6]), 7) + ROTR (T[7], 11) + W[24];
	T[6] = ROTR (F1 (T[2], T[3], T[0], T[7], T[4], T[1], T[5]), 7) + ROTR (T[6], 11) + W[25];
	T[5] = ROTR (F1 (T[1], T[2], T[7], T[6], T[3], T[0], T[4]), 7) + ROTR (T[5], 11) + W[26];
	T[4] = ROTR (F1 (T[0], T[1], T[6], T[5], T[2], T[7], T[3]), 7) + ROTR (T[4], 11) + W[27];
	T[3] = ROTR (F1 (T[7], T[0], T[5], T[4], T[1], T[6], T[2]), 7) + ROTR (T[3], 11) + W[28];
	T[2] = ROTR (F1 (T[6], T[7], T[4], T[3], T[0], T[5], T[1]), 7) + ROTR (T[2], 11) + W[29];
	T[1] = ROTR (F1 (T[5], T[6], T[3], T[2], T[7], T[4], T[0]), 7) + ROTR (T[1], 11) + W[30];
	T[0] = ROTR (F1 (T[4], T[5], T[2], T[1], T[6], T[3], T[7]), 7) + ROTR (T[0], 11) + W[31];

	/* PASS 2: */

	T[7] = ROTR (F2 (T[6], T[2], T[1], T[0], T[3], T[4], T[5]), 7) + ROTR (T[7], 11) + W[ 5] + 0x452821E6UL;
	T[6] = ROTR (F2 (T[5], T[1], T[0], T[7], T[2], T[3], T[4]), 7) + ROTR (T[6], 11) + W[14] + 0x38D01377UL;
	T[5] = ROTR (F2 (T[4], T[0], T[7], T[6], T[1], T[2], T[3]), 7) + ROTR (T[5], 11) + W[26] + 0xBE5466CFUL;
	T[4] = ROTR (F2 (T[3], T[7], T[6], T[5], T[0], T[1], T[2]), 7) + ROTR (T[4], 11) + W[18] + 0x34E90C6CUL;
	T[3] = ROTR (F2 (T[2], T[6], T[5], T[4], T[7], T[0], T[1]), 7) + ROTR (T[3], 11) + W[11] + 0xC0AC29B7UL;
	T[2] = ROTR (F2 (T[1], T[5], T[4], T[3], T[6], T[7], T[0]), 7) + ROTR (T[2], 11) + W[28] + 0xC97C50DDUL;
	T[1] = ROTR (F2 (T[0], T[4], T[3], T[2], T[5], T[6], T[7]), 7) + ROTR (T[1], 11) + W[ 7] + 0x3F84D5B5UL;
	T[0] = ROTR (F2 (T[7], T[3], T[2], T[1], T[4], T[5], T[6]), 7) + ROTR (T[0], 11) + W[16] + 0xB5470917UL;

	T[7] = ROTR (F2 (T[6], T[2], T[1], T[0], T[3], T[4], T[5]), 7) + ROTR (T[7], 11) + W[ 0] + 0x9216D5D9UL;
	T[6] = ROTR (F2 (T[5], T[1], T[0], T[7], T[2], T[3], T[4]), 7) + ROTR (T[6], 11) + W[23] + 0x8979FB1BUL;
	T[5] = ROTR (F2 (T[4], T[0], T[7], T[6], T[1], T[2], T[3]), 7) + ROTR (T[5], 11) + W[20] + 0xD1310BA6UL;
	T[4] = ROTR (F2 (T[3], T[7], T[6], T[5], T[0], T[1], T[2]), 7) + ROTR (T[4], 11) + W[22] + 0x98DFB5ACUL;
	T[3] = ROTR (F2 (T[2], T[6], T[5], T[4], T[7], T[0], T[1]), 7) + ROTR (T[3], 11) + W[ 1] + 0x2FFD72DBUL;
	T[2] = ROTR (F2 (T[1], T[5], T[4], T[3], T[6], T[7], T[0]), 7) + ROTR (T[2], 11) + W[10] + 0xD01ADFB7UL;
	T[1] = ROTR (F2 (T[0], T[4], T[3], T[2], T[5], T[6], T[7]), 7) + ROTR (T[1], 11) + W[ 4] + 0xB8E1AFEDUL;
	T[0] = ROTR (F2 (T[7], T[3], T[2], T[1], T[4], T[5], T[6]), 7) + ROTR (T[0], 11) + W[ 8] + 0x6A267E96UL;

	T[7] = ROTR (F2 (T[6], T[2], T[1], T[0], T[3], T[4], T[5]), 7) + ROTR (T[7], 11) + W[30] + 0xBA7C9045UL;
	T[6] = ROTR (F2 (T[5], T[1], T[0], T[7], T[2], T[3], T[4]), 7) + ROTR (T[6], 11) + W[ 3] + 0xF12C7F99UL;
	T[5] = ROTR (F2 (T[4], T[0], T[7], T[6], T[1], T[2], T[3]), 7) + ROTR (T[5], 11) + W[21] + 0x24A19947UL;
	T[4] = ROTR (F2 (T[3], T[7], T[6], T[5], T[0], T[1], T[2]), 7) + ROTR (T[4], 11) + W[ 9] + 0xB3916CF7UL;
	T[3] = ROTR (F2 (T[2], T[6], T[5], T[4], T[7], T[0], T[1]), 7) + ROTR (T[3], 11) + W[17] + 0x0801F2E2UL;
	T[2] = ROTR (F2 (T[1], T[5], T[4], T[3], T[6], T[7], T[0]), 7) + ROTR (T[2], 11) + W[24] + 0x858EFC16UL;
	T[1] = ROTR (F2 (T[0], T[4], T[3], T[2], T[5], T[6], T[7]), 7) + ROTR (T[1], 11) + W[29] + 0x636920D8UL;
	T[0] = ROTR (F2 (T[7], T[3], T[2], T[1], T[4], T[5], T[6]), 7) + ROTR (T[0], 11) + W[ 6] + 0x71574E69UL;

	T[7] = ROTR (F2 (T[6], T[2], T[1], T[0], T[3], T[4], T[5]), 7) + ROTR (T[7], 11) + W[19] + 0xA458FEA3UL;
	T[6] = ROTR (F2 (T[5], T[1], T[0], T[7], T[2], T[3], T[4]), 7) + ROTR (T[6], 11) + W[12] + 0xF4933D7EUL;
	T[5] = ROTR (F2 (T[4], T[0], T[7], T[6], T[1], T[2], T[3]), 7) + ROTR (T[5], 11) + W[15] + 0x0D95748FUL;
	T[4] = ROTR (F2 (T[3], T[7], T[6], T[5], T[0], T[1], T[2]), 7) + ROTR (T[4], 11) + W[13] + 0x728EB658UL;
	T[3] = ROTR (F2 (T[2], T[6], T[5], T[4], T[7], T[0], T[1]), 7) + ROTR (T[3], 11) + W[ 2] + 0x718BCD58UL;
	T[2] = ROTR (F2 (T[1], T[5], T[4], T[3], T[6], T[7], T[0]), 7) + ROTR (T[2], 11) + W[25] + 0x82154AEEUL;
	T[1] = ROTR (F2 (T[0], T[4], T[3], T[2], T[5], T[6], T[7]), 7) + ROTR (T[1], 11) + W[31] + 0x7B54A41DUL;
	T[0] = ROTR (F2 (T[7], T[3], T[2], T[1], T[4], T[5], T[6]), 7) + ROTR (T[0], 11) + W[27] + 0xC25A59B5UL;

	/* PASS 3: */

	T[7] = ROTR (F3 (T[2], T[6], T[0], T[4], T[3], T[1], T[5]), 7) + ROTR (T[7], 11) + W[19] + 0x9C30D539UL;
	T[6] = ROTR (F3 (T[1], T[5], T[7], T[3], T[2], T[0], T[4]), 7) + ROTR (T[6], 11) + W[ 9] + 0x2AF26013UL;
	T[5] = ROTR (F3 (T[0], T[4], T[6], T[2], T[1], T[7], T[3]), 7) + ROTR (T[5], 11) + W[ 4] + 0xC5D1B023UL;
	T[4] = ROTR (F3 (T[7], T[3], T[5], T[1], T[0], T[6], T[2]), 7) + ROTR (T[4], 11) + W[20] + 0x286085F0UL;
	T[3] = ROTR (F3 (T[6], T[2], T[4], T[0], T[7], T[5], T[1]), 7) + ROTR (T[3], 11) + W[28] + 0xCA417918UL;
	T[2] = ROTR (F3 (T[5], T[1], T[3], T[7], T[6], T[4], T[0]), 7) + ROTR (T[2], 11) + W[17] + 0xB8DB38EFUL;
	T[1] = ROTR (F3 (T[4], T[0], T[2], T[6], T[5], T[3], T[7]), 7) + ROTR (T[1], 11) + W[ 8] + 0x8E79DCB0UL;
	T[0] = ROTR (F3 (T[3], T[7], T[1], T[5], T[4], T[2], T[6]), 7) + ROTR (T[0], 11) + W[22] + 0x603A180EUL;

	T[7] = ROTR (F3 (T[2], T[6], T[0], T[4], T[3], T[1], T[5]), 7) + ROTR (T[7], 11) + W[29] + 0x6C9E0E8BUL;
	T[6] = ROTR (F3 (T[1], T[5], T[7], T[3], T[2], T[0], T[4]), 7) + ROTR (T[6], 11) + W[14] + 0xB01E8A3EUL;
	T[5] = ROTR (F3 (T[0], T[4], T[6], T[2], T[1], T[7], T[3]), 7) + ROTR (T[5], 11) + W[25] + 0xD71577C1UL;
	T[4] = ROTR (F3 (T[7], T[3], T[5], T[1], T[0], T[6], T[2]), 7) + ROTR (T[4], 11) + W[12] + 0xBD314B27UL;
	T[3] = ROTR (F3 (T[6], T[2], T[4], T[0], T[7], T[5], T[1]), 7) + ROTR (T[3], 11) + W[24] + 0x78AF2FDAUL;
	T[2] = ROTR (F3 (T[5], T[1], T[3], T[7], T[6], T[4], T[0]), 7) + ROTR (T[2], 11) + W[30] + 0x55605C60UL;
	T[1] = ROTR (F3 (T[4], T[0], T[2], T[6], T[5], T[3], T[7]), 7) + ROTR (T[1], 11) + W[16] + 0xE65525F3UL;
	T[0] = ROTR (F3 (T[3], T[7], T[1], T[5], T[4], T[2], T[6]), 7) + ROTR (T[0], 11) + W[26] + 0xAA55AB94UL;

	T[7] = ROTR (F3 (T[2], T[6], T[0], T[4], T[3], T[1], T[5]), 7) + ROTR (T[7], 11) + W[31] + 0x57489862UL;
	T[6] = ROTR (F3 (T[1], T[5], T[7], T[3], T[2], T[0], T[4]), 7) + ROTR (T[6], 11) + W[15] + 0x63E81440UL;
	T[5] = ROTR (F3 (T[0], T[4], T[6], T[2], T[1], T[7], T[3]), 7) + ROTR (T[5], 11) + W[ 7] + 0x55CA396AUL;
	T[4] = ROTR (F3 (T[7], T[3], T[5], T[1], T[0], T[6], T[2]), 7) + ROTR (T[4], 11) + W[ 3] + 0x2AAB10B6UL;
	T[3] = ROTR (F3 (T[6], T[2], T[4], T[0], T[7], T[5], T[1]), 7) + ROTR (T[3], 11) + W[ 1] + 0xB4CC5C34UL;
	T[2] = ROTR (F3 (T[5], T[1], T[3], T[7], T[6], T[4], T[0]), 7) + ROTR (T[2], 11) + W[ 0] + 0x1141E8CEUL;
	T[1] = ROTR (F3 (T[4], T[0], T[2], T[6], T[5], T[3], T[7]), 7) + ROTR (T[1], 11) + W[18] + 0xA15486AFUL;
	T[0] = ROTR (F3 (T[3], T[7], T[1], T[5], T[4], T[2], T[6]), 7) + ROTR (T[0], 11) + W[27] + 0x7C72E993UL;

	T[7] = ROTR (F3 (T[2], T[6], T[0], T[4], T[3], T[1], T[5]), 7) + ROTR (T[7], 11) + W[13] + 0xB3EE1411UL;
	T[6] = ROTR (F3 (T[1], T[5], T[7], T[3], T[2], T[0], T[4]), 7) + ROTR (T[6], 11) + W[ 6] + 0x636FBC2AUL;
	T[5] = ROTR (F3 (T[0], T[4], T[6], T[2], T[1], T[7], T[3]), 7) + ROTR (T[5], 11) + W[21] + 0x2BA9C55DUL;
	T[4] = ROTR (F3 (T[7], T[3], T[5], T[1], T[0], T[6], T[2]), 7) + ROTR (T[4], 11) + W[10] + 0x741831F6UL;
	T[3] = ROTR (F3 (T[6], T[2], T[4], T[0], T[7], T[5], T[1]), 7) + ROTR (T[3], 11) + W[23] + 0xCE5C3E16UL;
	T[2] = ROTR (F3 (T[5], T[1], T[3], T[7], T[6], T[4], T[0]), 7) + ROTR (T[2], 11) + W[11] + 0x9B87931EUL;
	T[1] = ROTR (F3 (T[4], T[0], T[2], T[6], T[5], T[3], T[7]), 7) + ROTR (T[1], 11) + W[ 5] + 0xAFD6BA33UL;
	T[0] = ROTR (F3 (T[3], T[7], T[1], T[5], T[4], T[2], T[6]), 7) + ROTR (T[0], 11) + W[ 2] + 0x6C24CF5CUL;

	/* PASS 4: */

	T[7] = ROTR (F4 (T[1], T[5], T[3], T[2], T[0], T[4], T[6]), 7) + ROTR (T[7], 11) + W[24] + 0x7A325381UL;
	T[6] = ROTR (F4 (T[0], T[4], T[2], T[1], T[7], T[3], T[5]), 7) + ROTR (T[6], 11) + W[ 4] + 0x28958677UL;
	T[5] = ROTR (F4 (T[7], T[3], T[1], T[0], T[6], T[2], T[4]), 7) + ROTR (T[5], 11) + W[ 0] + 0x3B8F4898UL;
	T[4] = ROTR (F4 (T[6], T[2], T[0], T[7], T[5], T[1], T[3]), 7) + ROTR (T[4], 11) + W[14] + 0x6B4BB9AFUL;
	T[3] = ROTR (F4 (T[5], T[1], T[7], T[6], T[4], T[0], T[2]), 7) + ROTR (T[3], 11) + W[ 2] + 0xC4BFE81BUL;
	T[2] = ROTR (F4 (T[4], T[0], T[6], T[5], T[3], T[7], T[1]), 7) + ROTR (T[2], 11) + W[ 7] + 0x66282193UL;
	T[1] = ROTR (F4 (T[3], T[7], T[5], T[4], T[2], T[6], T[0]), 7) + ROTR (T[1], 11) + W[28] + 0x61D809CCUL;
	T[0] = ROTR (F4 (T[2], T[6], T[4], T[3], T[1], T[5], T[7]), 7) + ROTR (T[0], 11) + W[23] + 0xFB21A991UL;

	T[7] = ROTR (F4 (T[1], T[5], T[3], T[2], T[0], T[4], T[6]), 7) + ROTR (T[7], 11) + W[26] + 0x487CAC60UL;
	T[6] = ROTR (F4 (T[0], T[4], T[2], T[1], T[7], T[3], T[5]), 7) + ROTR (T[6], 11) + W[ 6] + 0x5DEC8032UL;
	T[5] = ROTR (F4 (T[7], T[3], T[1], T[0], T[6], T[2], T[4]), 7) + ROTR (T[5], 11) + W[30] + 0xEF845D5DUL;
	T[4] = ROTR (F4 (T[6], T[2], T[0], T[7], T[5], T[1], T[3]), 7) + ROTR (T[4], 11) + W[20] + 0xE98575B1UL;
	T[3] = ROTR (F4 (T[5], T[1], T[7], T[6], T[4], T[0], T[2]), 7) + ROTR (T[3], 11) + W[18] + 0xDC262302UL;
	T[2] = ROTR (F4 (T[4], T[0], T[6], T[5], T[3], T[7], T[1]), 7) + ROTR (T[2], 11) + W[25] + 0xEB651B88UL;
	T[1] = ROTR (F4 (T[3], T[7], T[5], T[4], T[2], T[6], T[0]), 7) + ROTR (T[1], 11) + W[19] + 0x23893E81UL;
	T[0] = ROTR (F4 (T[2], T[6], T[4], T[3], T[1], T[5], T[7]), 7) + ROTR (T[0], 11) + W[ 3] + 0xD396ACC5UL;

	T[7] = ROTR (F4 (T[1], T[5], T[3], T[2], T[0], T[4], T[6]), 7) + ROTR (T[7], 11) + W[22] + 0x0F6D6FF3UL;
	T[6] = ROTR (F4 (T[0], T[4], T[2], T[1], T[7], T[3], T[5]), 7) + ROTR (T[6], 11) + W[11] + 0x83F44239UL;
	T[5] = ROTR (F4 (T[7], T[3], T[1], T[0], T[6], T[2], T[4]), 7) + ROTR (T[5], 11) + W[31] + 0x2E0B4482UL;
	T[4] = ROTR (F4 (T[6], T[2], T[0], T[7], T[5], T[1], T[3]), 7) + ROTR (T[4], 11) + W[21] + 0xA4842004UL;
	T[3] = ROTR (F4 (T[5], T[1], T[7], T[6], T[4], T[0], T[2]), 7) + ROTR (T[3], 11) + W[ 8] + 0x69C8F04AUL;
	T[2] = ROTR (F4 (T[4], T[0], T[6], T[5], T[3], T[7], T[1]), 7) + ROTR (T[2], 11) + W[27] + 0x9E1F9B5EUL;
	T[1] = ROTR (F4 (T[3], T[7], T[5], T[4], T[2], T[6], T[0]), 7) + ROTR (T[1], 11) + W[12] + 0x21C66842UL;
	T[0] = ROTR (F4 (T[2], T[6], T[4], T[3], T[1], T[5], T[7]), 7) + ROTR (T[0], 11) + W[ 9] + 0xF6E96C9AUL;

	T[7] = ROTR (F4 (T[1], T[5], T[3], T[2], T[0], T[4], T[6]), 7) + ROTR (T[7], 11) + W[ 1] + 0x670C9C61UL;
	T[6] = ROTR (F4 (T[0], T[4], T[2], T[1], T[7], T[3], T[5]), 7) + ROTR (T[6], 11) + W[29] + 0xABD388F0UL;
	T[5] = ROTR (F4 (T[7], T[3], T[1], T[0], T[6], T[2], T[4]), 7) + ROTR (T[5], 11) + W[ 5] + 0x6A51A0D2UL;
	T[4] = ROTR (F4 (T[6], T[2], T[0], T[7], T[5], T[1], T[3]), 7) + ROTR (T[4], 11) + W[15] + 0xD8542F68UL;
	T[3] = ROTR (F4 (T[5], T[1], T[7], T[6], T[4], T[0], T[2]), 7) + ROTR (T[3], 11) + W[17] + 0x960FA728UL;
	T[2] = ROTR (F4 (T[4], T[0], T[6], T[5], T[3], T[7], T[1]), 7) + ROTR (T[2], 11) + W[10] + 0xAB5133A3UL;
	T[1] = ROTR (F4 (T[3], T[7], T[5], T[4], T[2], T[6], T[0]), 7) + ROTR (T[1], 11) + W[16] + 0x6EEF0B6CUL;
	T[0] = ROTR (F4 (T[2], T[6], T[4], T[3], T[1], T[5], T[7]), 7) + ROTR (T[0], 11) + W[13] + 0x137A3BE4UL;

	/* PASS 5: */

	T[7] = ROTR (F5 (T[2], T[5], T[0], T[6], T[4], T[3], T[1]), 7) + ROTR (T[7], 11) + W[27] + 0xBA3BF050UL;
	T[6] = ROTR (F5 (T[1], T[4], T[7], T[5], T[3], T[2], T[0]), 7) + ROTR (T[6], 11) + W[ 3] + 0x7EFB2A98UL;
	T[5] = ROTR (F5 (T[0], T[3], T[6], T[4], T[2], T[1], T[7]), 7) + ROTR (T[5], 11) + W[21] + 0xA1F1651DUL;
	T[4] = ROTR (F5 (T[7], T[2], T[5], T[3], T[1], T[0], T[6]), 7) + ROTR (T[4], 11) + W[26] + 0x39AF0176UL;
	T[3] = ROTR (F5 (T[6], T[1], T[4], T[2], T[0], T[7], T[5]), 7) + ROTR (T[3], 11) + W[17] + 0x66CA593EUL;
	T[2] = ROTR (F5 (T[5], T[0], T[3], T[1], T[7], T[6], T[4]), 7) + ROTR (T[2], 11) + W[11] + 0x82430E88UL;
	T[1] = ROTR (F5 (T[4], T[7], T[2], T[0], T[6], T[5], T[3]), 7) + ROTR (T[1], 11) + W[20] + 0x8CEE8619UL;
	T[0] = ROTR (F5 (T[3], T[6], T[1], T[7], T[5], T[4], T[2]), 7) + ROTR (T[0], 11) + W[29] + 0x456F9FB4UL;

	T[7] = ROTR (F5 (T[2], T[5], T[0], T[6], T[4], T[3], T[1]), 7) + ROTR (T[7], 11) + W[19] + 0x7D84A5C3UL;
	T[6] = ROTR (F5 (T[1], T[4], T[7], T[5], T[3], T[2], T[0]), 7) + ROTR (T[6], 11) + W[ 0] + 0x3B8B5EBEUL;
	T[5] = ROTR (F5 (T[0], T[3], T[6], T[4], T[2], T[1], T[7]), 7) + ROTR (T[5], 11) + W[12] + 0xE06F75D8UL;
	T[4] = ROTR (F5 (T[7], T[2], T[5], T[3], T[1], T[0], T[6]), 7) + ROTR (T[4], 11) + W[ 7] + 0x85C12073UL;
	T[3] = ROTR (F5 (T[6], T[1], T[4], T[2], T[0], T[7], T[5]), 7) + ROTR (T[3], 11) + W[13] + 0x401A449FUL;
	T[2] = ROTR (F5 (T[5], T[0], T[3], T[1], T[7], T[6], T[4]), 7) + ROTR (T[2], 11) + W[ 8] + 0x56C16AA6UL;
	T[1] = ROTR (F5 (T[4], T[7], T[2], T[0], T[6], T[5], T[3]), 7) + ROTR (T[1], 11) + W[31] + 0x4ED3AA62UL;
	T[0] = ROTR (F5 (T[3], T[6], T[1], T[7], T[5], T[4], T[2]), 7) + ROTR (T[0], 11) + W[10] + 0x363F7706UL;

	T[7] = ROTR (F5 (T[2], T[5], T[0], T[6], T[4], T[3], T[1]), 7) + ROTR (T[7], 11) + W[ 5] + 0x1BFEDF72UL;
	T[6] = ROTR (F5 (T[1], T[4], T[7], T[5], T[3], T[2], T[0]), 7) + ROTR (T[6], 11) + W[ 9] + 0x429B023DUL;
	T[5] = ROTR (F5 (T[0], T[3], T[6], T[4], T[2], T[1], T[7]), 7) + ROTR (T[5], 11) + W[14] + 0x37D0D724UL;
	T[4] = ROTR (F5 (T[7], T[2], T[5], T[3], T[1], T[0], T[6]), 7) + ROTR (T[4], 11) + W[30] + 0xD00A1248UL;
	T[3] = ROTR (F5 (T[6], T[1], T[4], T[2], T[0], T[7], T[5]), 7) + ROTR (T[3], 11) + W[18] + 0xDB0FEAD3UL;
	T[2] = ROTR (F5 (T[5], T[0], T[3], T[1], T[7], T[6], T[4]), 7) + ROTR (T[2], 11) + W[ 6] + 0x49F1C09BUL;
	T[1] = ROTR (F5 (T[4], T[7], T[2], T[0], T[6], T[5], T[3]), 7) + ROTR (T[1], 11) + W[28] + 0x075372C9UL;
	T[0] = ROTR (F5 (T[3], T[6], T[1], T[7], T[5], T[4], T[2]), 7) + ROTR (T[0], 11) + W[24] + 0x80991B7BUL;

	E[7] += T[7] = ROTR (F5 (T[2], T[5], T[0], T[6], T[4], T[3], T[1]), 7) + ROTR (T[7], 11) + W[ 2] + 0x25D479D8UL;
	E[6] += T[6] = ROTR (F5 (T[1], T[4], T[7], T[5], T[3], T[2], T[0]), 7) + ROTR (T[6], 11) + W[23] + 0xF6E8DEF7UL;
	E[5] += T[5] = ROTR (F5 (T[0], T[3], T[6], T[4], T[2], T[1], T[7]), 7) + ROTR (T[5], 11) + W[16] + 0xE3FE501AUL;
	E[4] += T[4] = ROTR (F5 (T[7], T[2], T[5], T[3], T[1], T[0], T[6]), 7) + ROTR (T[4], 11) + W[22] + 0xB6794C3BUL;
	E[3] += T[3] = ROTR (F5 (T[6], T[1], T[4], T[2], T[0], T[7], T[5]), 7) + ROTR (T[3], 11) + W[ 4] + 0x976CE0BDUL;
	E[2] += T[2] = ROTR (F5 (T[5], T[0], T[3], T[1], T[7], T[6], T[4]), 7) + ROTR (T[2], 11) + W[ 1] + 0x04C006BAUL;
	E[1] += T[1] = ROTR (F5 (T[4], T[7], T[2], T[0], T[6], T[5], T[3]), 7) + ROTR (T[1], 11) + W[25] + 0xC1A94FB6UL;
	E[0] += T[0] = ROTR (F5 (T[3], T[6], T[1], T[7], T[5], T[4], T[2]), 7) + ROTR (T[0], 11) + W[15] + 0x409F60C4UL;

#if defined(WORDS_BIGENDIAN)
	mutils_free(W);
#endif
} /* havalTransform5 */

mutils_error havalInit256( havalContext *hcp) {
	return havalInit(hcp, 3, 256);
}
mutils_error havalInit224( havalContext *hcp) {
	return havalInit(hcp, 3, 224);
}
mutils_error havalInit192( havalContext *hcp) {
	return havalInit(hcp, 3, 192);
}
mutils_error havalInit160( havalContext *hcp) {
	return havalInit(hcp, 3, 160);
}
mutils_error havalInit128( havalContext *hcp) {
	return havalInit(hcp, 3, 128);
}

mutils_error havalInit (havalContext *hcp, mutils_word32 passes, mutils_word32 hashLength)
{
	if (hcp == NULL)
	{
		return(-MUTILS_INVALID_INPUT_BUFFER); /* bad context */
	}
	/* check number of passes: */
	if (passes != 3 && passes != 4 && passes != 5)
	{
		return(-MUTILS_INVALID_PASSES); /* invalid number of passes */
	}
	/* check hash length: */
	if (hashLength != 128 &&
		hashLength != 160 &&
		hashLength != 192 &&
		hashLength != 224 &&
		hashLength != 256)
	{
		return(-MUTILS_INVALID_SIZE); /* invalid hash length */
	}
	/* properly initialize HAVAL context: */
	mutils_bzero(hcp, sizeof (havalContext));
	hcp->passes = (mutils_word16) passes;
	hcp->hashLength = (mutils_word16) hashLength;
	hcp->digest[0] = 0x243F6A88UL;
	hcp->digest[1] = 0x85A308D3UL;
	hcp->digest[2] = 0x13198A2EUL;
	hcp->digest[3] = 0x03707344UL;
	hcp->digest[4] = 0xA4093822UL;
	hcp->digest[5] = 0x299F31D0UL;
	hcp->digest[6] = 0x082EFA98UL;
	hcp->digest[7] = 0xEC4E6C89UL;
	return(MUTILS_OK); /* OK */
} /* havalInit */


mutils_error havalUpdate (havalContext *hcp, __const mutils_word8 *dataBuffer, mutils_word32 dataLength)
{
	if (hcp == NULL)
	{
		return(-MUTILS_INVALID_INPUT_BUFFER); /* bad context */
	}
	if (dataBuffer == NULL || dataLength == 0)
	{
		return(MUTILS_OK); /* nothing to do */
	}

	/* update bit count: Extra parentheses for Borland C++ --Tines*/
	if ( ((mutils_word32)dataLength << 3) > (0xFFFFFFFFUL - hcp->bitCount[0]) )
	{
		hcp->bitCount[1]++;
	}
	hcp->bitCount[0] += (mutils_word32)dataLength << 3;
	
	/* if the data buffer is not enough to complete */
	/* the context data block, just append it: */
	if (hcp->occupied + (mutils_word32)dataLength < 128)
	{ /* caveat: typecast avoids 16-bit overflow */
		mutils_memcpy (&hcp->block[hcp->occupied], dataBuffer, dataLength);
		hcp->occupied += dataLength;
		return(MUTILS_OK); /* delay processing */
	}

	/* complete the context data block: */
	mutils_memcpy (&hcp->block[hcp->occupied], dataBuffer, 128 - hcp->occupied);
	dataBuffer += 128 - hcp->occupied;
	dataLength -= 128 - hcp->occupied;

	switch (hcp->passes) {
	case 3:
		/* process the completed context data block: */
	  havalTransform3 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		/* process data in chunks of 128 bytes: */
		while (dataLength >= 128)
		{
		  havalTransform3 (hcp->digest, (mutils_word32 *) dataBuffer, hcp->temp);
			dataBuffer += 128;
			dataLength -= 128;
		}
		break;
	case 4:
		/* process the completed context data block: */
	  havalTransform4 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		/* process data in chunks of 128 bytes: */
		while (dataLength >= 128)
		{
		  havalTransform4 (hcp->digest, (mutils_word32 *) dataBuffer, hcp->temp);
			dataBuffer += 128;
			dataLength -= 128;
		}
		break;
	case 5:
		/* process the completed context data block: */
	  havalTransform5 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		/* process data in chunks of 128 bytes: */
		while (dataLength >= 128)
		{
		  havalTransform5 (hcp->digest, (mutils_word32 *) dataBuffer, hcp->temp);
			dataBuffer += 128;
			dataLength -= 128;
		}
		break;
	}

	/* delay processing of remaining data: */
	mutils_memcpy (hcp->block, dataBuffer, dataLength);
	hcp->occupied = dataLength; /* < 128 */
	
	return(MUTILS_OK); /* OK */
} /* havalUpdate */

mutils_error havalFinal (havalContext *hcp, mutils_word8 *digest)
{
	mutils_word32 w;

	if (hcp == NULL)
	{
	  return(-MUTILS_INVALID_INPUT_BUFFER); /* bad context */
	}
	if (digest == NULL)
	{
	  return(-MUTILS_INVALID_OUTPUT_BUFFER); /* bad digest buffer */
	}

/*	assert (hcp->occupied < 128);  invariant */

	/* append toggle to the context data block: */
	hcp->block[hcp->occupied] = 0x01; /* corrected from 0x80 */

	/* pad the message with null bytes to make it 944 (mod 1024) bits long: */
	if (hcp->occupied++ >= 118)
	{
		/* no room for tail data on the current context block */
		mutils_bzero (&hcp->block[hcp->occupied], 128 - hcp->occupied);
		/* process the completed context data block: */
		switch (hcp->passes) {
		case 3:
		  havalTransform3 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
			break;
		case 4:
		  havalTransform4 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
			break;
		case 5:
		  havalTransform5 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
			break;
		}
		mutils_bzero (hcp->block, 118);
	}
	else
	{
		mutils_bzero (&hcp->block[hcp->occupied], 118 - hcp->occupied);
	}
	/* append tail data and process last (padded) message block: */
	hcp->block[118] = (mutils_word8)(
		((hcp->hashLength & 0x03U) << 6) |
		((hcp->passes     & 0x07U) << 3) |
		(HAVAL_VERSION           & 0x07U));
	hcp->block[119] = (mutils_word8) (hcp->hashLength >> 2);
	w = hcp->bitCount[0];
	hcp->block[120] = (mutils_word8)(w);
	hcp->block[121] = (mutils_word8)(w >>  8);
	hcp->block[122] = (mutils_word8)(w >> 16);
	hcp->block[123] = (mutils_word8)(w >> 24);
	w = hcp->bitCount[1];
	hcp->block[124] = (mutils_word8)(w);
	hcp->block[125] = (mutils_word8)(w >>  8);
	hcp->block[126] = (mutils_word8)(w >> 16);
	hcp->block[127] = (mutils_word8)(w >> 24);
	switch (hcp->passes)
	{
	case 3:
	  havalTransform3 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		break;
	case 4:
	  havalTransform4 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		break;
	case 5:
	  havalTransform5 (hcp->digest, (mutils_word32 *) (hcp->block), hcp->temp);
		break;
	}

	/* fold 256-bit digest to fit the desired hash length (blaargh!): */
	/* Byte reverse each 32-bit section while outputting if big-endian -- Tines*/
	switch (hcp->hashLength)
	{
	case 128:
		hcp->digest[3] +=
			( (hcp->digest[7] & 0xFF000000UL)
			| (hcp->digest[6] & 0x00FF0000UL)
			| (hcp->digest[5] & 0x0000FF00UL)
			| (hcp->digest[4] & 0x000000FFUL)
			);
		hcp->digest[2] +=
			(((hcp->digest[7] & 0x00FF0000UL)
			| (hcp->digest[6] & 0x0000FF00UL)
			| (hcp->digest[5] & 0x000000FFUL)
			) << 8) |
			( (hcp->digest[4] & 0xFF000000UL) >> 24);
		hcp->digest[1] +=
			(((hcp->digest[7] & 0x0000FF00UL)
			| (hcp->digest[6] & 0x000000FFUL)) << 16) |
			(((hcp->digest[5] & 0xFF000000UL)
			| (hcp->digest[4] & 0x00FF0000UL)) >> 16);
		hcp->digest[0] +=
			(((hcp->digest[6] & 0xFF000000UL)
			| (hcp->digest[5] & 0x00FF0000UL)
			| (hcp->digest[4] & 0x0000FF00UL)
			) >> 8) |
			( (hcp->digest[7] & 0x000000FFUL) << 24);
#if defined(WORDS_BIGENDIAN)
		mutils_word32nswap((mutils_word32 *) (hcp->digest), 128/32, MUTILS_TRUE);
#endif
		mutils_memcpy(digest, hcp->digest, 128/8);
		break;
	case 160:
		hcp->digest[4] +=
			((hcp->digest[7] & 0xFE000000UL) | (hcp->digest[6] & 0x01F80000UL) | (hcp->digest[5] & 0x0007F000UL)) >> 12;
		hcp->digest[3] +=
			((hcp->digest[7] & 0x01F80000UL) | (hcp->digest[6] & 0x0007F000UL) | (hcp->digest[5] & 0x00000FC0UL)) >> 6;
		hcp->digest[2] +=
			((hcp->digest[7] & 0x0007F000UL) | (hcp->digest[6] & 0x00000FC0UL) | (hcp->digest[5] & 0x0000003FUL));
		hcp->digest[1] +=
			ROTR
			((hcp->digest[7] & 0x00000FC0UL) | (hcp->digest[6] & 0x0000003FUL) | (hcp->digest[5] & 0xFE000000UL), 25);
		hcp->digest[0] +=
			ROTR
			((hcp->digest[7] & 0x0000003FUL) | (hcp->digest[6] & 0xFE000000UL) | (hcp->digest[5] & 0x01F80000UL), 19);

#if defined(WORDS_BIGENDIAN)
		mutils_word32nswap((mutils_word32 *) (hcp->digest), 160/32, MUTILS_TRUE);
#endif
		mutils_memcpy (digest, hcp->digest, 160/8);

		break;
	case 192:
		hcp->digest[5] +=
			((hcp->digest[7] & 0xFC000000UL) | (hcp->digest[6] & 0x03E00000UL)) >> 21;
		hcp->digest[4] +=
			((hcp->digest[7] & 0x03E00000UL) | (hcp->digest[6] & 0x001F0000UL)) >> 16;
		hcp->digest[3] +=
			((hcp->digest[7] & 0x001F0000UL) | (hcp->digest[6] & 0x0000FC00UL)) >> 10;
		hcp->digest[2] +=
			((hcp->digest[7] & 0x0000FC00UL) | (hcp->digest[6] & 0x000003E0UL)) >>  5;
		hcp->digest[1] +=
			((hcp->digest[7] & 0x000003E0UL) | (hcp->digest[6] & 0x0000001FUL));
		hcp->digest[0] +=
			ROTR
			((hcp->digest[7] & 0x0000001FUL) | (hcp->digest[6] & 0xFC000000UL), 26);

#if defined(WORDS_BIGENDIAN)
		mutils_word32nswap((mutils_word32 *) (hcp->digest), 192/32, MUTILS_TRUE);
#endif
		mutils_memcpy (digest, hcp->digest, 192/8);

		break;
	case 224:
		hcp->digest[6] += (hcp->digest[7]      ) & 0x0000000FUL;
		hcp->digest[5] += (hcp->digest[7] >>  4) & 0x0000001FUL;
		hcp->digest[4] += (hcp->digest[7] >>  9) & 0x0000000FUL;
		hcp->digest[3] += (hcp->digest[7] >> 13) & 0x0000001FUL;
		hcp->digest[2] += (hcp->digest[7] >> 18) & 0x0000000FUL;
		hcp->digest[1] += (hcp->digest[7] >> 22) & 0x0000001FUL;
		hcp->digest[0] += (hcp->digest[7] >> 27) & 0x0000001FUL;

#if defined(WORDS_BIGENDIAN)
		mutils_word32nswap((mutils_word32 *) (hcp->digest), 224/32, MUTILS_TRUE);
#endif
		mutils_memcpy (digest, hcp->digest, 224/8);

		break;
	case 256:

#if defined(WORDS_BIGENDIAN)
		mutils_word32nswap((mutils_word32 *) (hcp->digest), 256/32, MUTILS_TRUE);
#endif
		mutils_memcpy (digest, hcp->digest, 256/8);

		break;
	}

	/* destroy sensitive information: */
	mutils_bzero (hcp, sizeof (havalContext));
	return(MUTILS_OK); /* OK */
} /* havalFinal */


#endif /* ENABLE_HAVAL */
