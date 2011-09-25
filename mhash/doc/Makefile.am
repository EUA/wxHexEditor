
EXTRA_DIST = mhash.pod md5-rfc1321.txt \
		mhash.html mhash.0 mhash.3 skid2-authentication example.c

man_MANS = $(srcdir)/mhash.3

PODPARAMS=--section=3 --center="mhash library" --date="2000/03/23" --release="mhash @MHASH_VERSION@"

dist_targets = mhash.0 $(srcdir)/mhash.3 mhash.html

MAINTAINERCLEANFILES=$(dist_targets)

dist-hook: mhash-doc

mhash-doc: $(dist_targets)
	
$(srcdir)/mhash.html: $(srcdir)/mhash.pod
	pod2html --noindex --netscape --title="mhash library" $< | sed 's/MHASH_VERSION/@MHASH_VERSION@/' > $@

$(srcdir)/mhash.0: $(srcdir)/mhash.3
	nroff -man $< > $@

$(srcdir)/mhash.3: $(srcdir)/mhash.pod
	pod2man $(PODPARAMS) $< | sed 's/MHASH_VERSION/@MHASH_VERSION@/' > $@

