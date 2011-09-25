## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I../include

EXTRA_DIST = hash_test.sh 

TESTS = hash_test.sh hmac_test keygen_test rest_test frag_test

hmac_test_SOURCES = hmac_test.c
hmac_test_LDADD = ../lib/libmhash.la
keygen_test_SOURCES = keygen_test.c
keygen_test_LDADD = ../lib/libmhash.la
driver_SOURCES = driver.c
driver_LDADD = ../lib/libmhash.la
rest_test_SOURCES = mhash_save_restore_test.c 
rest_test_LDADD = ../lib/libmhash.la
frag_test_SOURCES = frag_test.c 
frag_test_LDADD = ../lib/libmhash.la

noinst_PROGRAMS = driver hmac_test keygen_test rest_test frag_test
