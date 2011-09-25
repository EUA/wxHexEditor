#AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../include

lib_LTLIBRARIES = libmhash.la

FRAMEWORK = mhash.c stdfns.c mhash.h mhash_int.h libdefs.h

KEYGEN = keygen_hex.c keygen_mcrypt.c keygen_asis.c keygen.c keygen_s2k.c \
		keygen.h

CRC = crc32.c mhash_crc32.h adler32.c mhash_adler32.h

MD_SERIES = md2.c mhash_md2.h md4.c mhash_md4.h md5.c mhash_md5.h \
		ripemd.c mhash_ripemd.h

SHA_SERIES = sha1.c sha256_sha224.c sha512_sha384.c \
		mhash_sha1.h mhash_sha256_sha224.h mhash_sha512_sha384.h

TIGER = tiger.c tiger_sboxes.c mhash_tiger.h

MISCELLANEOUS = haval.c mhash_haval.h gosthash.c mhash_gost.h \
		whirlpool.c mhash_whirlpool.h snefru.c mhash_snefru.h

libmhash_la_SOURCES = $(FRAMEWORK) $(KEYGEN) $(CRC) $(MD_SERIES) \
			$(SHA_SERIES) $(TIGER) $(MISCELLANEOUS)

libmhash_la_LDFLAGS = -version-info 2:1

EXTRA_DIST = libmhash.sym

dist-hook:
	find $(distdir) -name CVS -o -name .cvsignore | xargs rm -rf
