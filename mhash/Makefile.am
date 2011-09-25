#AUTOMAKE_OPTIONS = foreign
SUBDIRS = include lib doc src
EXTRA_DIST = mhash.spec.in mhash.spec
USE_MHASH_CONFIG = yes

ChangeLog:
	cvs2cl --utc --fsf -t -S --prune
