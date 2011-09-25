%define prefix   /usr

Summary: Thread-safe hash library
Name: mhash
Version: 0.9.9
Release: 1
Copyright: LGPL
Group: System Environment/Libraries
Source: http://mhash.sf.net/dl/%{name}-%{version}.tar.gz
BuildRoot: /tmp/%{name}-%{version}-root
Packager: Germano Rizzo <mano@pluto.linux.it>
URL: http://mhash.sf.net

%description
mhash is a thread-safe hash library, implemented in C, and provides a
uniform interface to a large number of hash algorithms. These 
algorithms can be used to compute checksums, message digests, and 
other signatures. The HMAC support implements the basics for message 
authentication, following RFC 2104.

Algorithms currently supplied are:

CRC-32, CRC-32B, ALDER-32, MD-2, MD-4, MD-5, RIPEMD-128, RIPEMD-160,
RIPEMD-256, RIPEMD-320, SHA-1, SHA-224, SHA-256, SHA-384, SHA-512, HAVAL-128,
HAVAL-160, HAVAL-192, HAVAL-256, TIGER, TIGER-128, TIGER-160, GOST, WHIRLPOOL,
SNEFRU-128, SNEFRU-256

Algorithms will be added to this list over time.

%package devel
Summary: Header files and libraries for developing apps which will use mhash
Group: Development/Libraries
Requires: mhash

%description devel
The mhash-devel package contains the header files and libraries needed
to develop programs that use the mhash library.

Install the mhash-devel package if you want to develop applications that
will use the mhash library.

%changelog
* Tue Feb 20 2007 Jonathan Day <imipak@yahoo.com>
- Fixed symbol conflicts in config file
* Mon Feb 19 2007 Jonathan Day <imipak@yahoo.com>
- Included fixes for memory leaks and endian issues
* Sun Jan 8 2006 Jonathan Day <imipak@yahoo.com>
- Interface cleanup
* Wed Jan 12 2005 Jonathan Day <imipak@yahoo.com>
- Finally got round to integrating a lot of people's patches
* Thu Nov 18 2004 Casper Pedersen <cpedersen[at]c-note.dk>
- added '--enable-static=yes' to create static lib.
* Fri Feb 01 2002 Germano Rizzo <mano@pluto.linux.it>
- Built version 0.8.13 basing on Kyle Wheeler's SPEC file
* Sat Jun 10 2000 Kyle Wheeler <memoryhole@penguinpowered.com>
- Updated for version 0.8.1
* Wed Feb 9 2000 Clinton Work <clinton@scripty.com>
- Created a new spec file for version 0.6.1
- Created both a shared library and devel packages

%prep
%setup

%build
ENABLING="--enable-static=yes --enable-shared=yes"
CFLAGS="${RPM_OPT_FLAGS}"
CFLAGS="$RPM_OPT_FLAGS" ./configure "$ENABLING" --prefix=%{prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root, 0755)
%{prefix}/lib/libmhash.so.*

%files devel
%defattr(-, root, root, 0755)
%doc AUTHORS COPYING INSTALL ChangeLog NEWS README TODO
%doc doc/example.c doc/md5-rfc1321.txt doc/mhash.html doc/skid2-authentication
%{prefix}/man/man3/mhash.3*
%{prefix}/lib/*.a
%{prefix}/lib/*.la
%{prefix}/lib/*.so
%{prefix}/include/*.h
%{prefix}/include/mutils/*.h


