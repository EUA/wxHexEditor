
hashlib++ - a simple hash library for C++
Copyright (c) 2007-2010 Benjamin Grüdelbach

About this document:
This document explains the basics of the hashlib++ library. 
You should read this stuff if you are new to hashlib++.

Apr 2010

-------------------------------------------------------------------------------	

Table of contents: 
0) About hashlib++
1) Current version / What's new?
2) Supported hash functions  
3) Building hashlib++
4) The structure of hashlib++
5) The library's files
6) Using hashlib++
7) Supported operation systems
8) License
9) Feedback, Questions, Contact
10) Third party agreements

-------------------------------------------------------------------------------	

0) About hashlib++?

hashlib++ is a simple and very easy to use library to create a cryptographic
checksum called "hash". hashlib++ is written in plain C++ and should work with
every compiler and platform. hashlib++ is released under the BSD-license (see
section 8) and therefore free software.

-------------------------------------------------------------------------------	

1) Current version / What's new?

The current stable version of hashlib++ is:  0.3.2

Improvements for hashlib++ version 0.3.2:

	* BUGFIX on 64-Bit Systems: hashlib++ runs finally stable there.
	* Added test functions to test the hash-implementations on the target system
	  at run time.
	* Renamed hashlib++ internal typedefs in order to fix typedef conflicts
	  with OpenSSL and others. 
	* Re-Added the Makefile to build hashlib++ as static library on UNIX-
	  like systems.

-------------------------------------------------------------------------------	

2) Supported hash functions  

By now hashlib++ supports the following functions:

	* MD5		Message-Digest algorithm 5
	* SHA1  	Secure Hash Algorithm 1
	* SHA2-Family 	Secure Hash Algorithm 256, 384 and 512

-------------------------------------------------------------------------------	
3) Building hashlib++

To use hashlib++ you have one of the following options:

	3.1) Using hashlib++ as a source code library

 		hashlib++ comes as a source code library. You can add the 
		files (or only those you need) to your project as you like and 
		compile it. That's all!

     	3.2) Using hashlib++ as static library 

		Another option is to to use hashlib++ as a static library and integrate this 
		library into your build process. 
		
		If you are using a UNIX-like operating system (like Linux, FreeBSD ...)
		you can use the Makefile in the src-directory to build hashlib++
		as a static library. 
		Before you start, you should jump into the Makefile an edit some
		settings to meet your needs:
		While INCLUDE_PATH is the path where the header files will be
		installed, LIB_PATH is the path where the static library will 
		be installed.
		* Type 'make' to build the library.
		* Type 'make install' to build and install the library.
		* Type 'make clean' to clean the object files.
		  (On FreeBSD you need gmake)
		The name of the lib will be libhl++.a so you have to link
		against hl++. For example like 
		 	g++ h.cpp -I/usr/include/hashlib++/ -lhl++

		If you are using a Windows operating system, you can create 
		a new blank project (for example in Visual Studio), add the
		hashlib++ source files and compile everything as library.
		Of cause you can also use the Unix-Makefile within Cygwin.
	

-------------------------------------------------------------------------------	

4) The structure of hashlib++

hashlib++ provides so called "wrappers" for each supported hash function which
simplify the creation of the relevant hash. Instead of implementing the full
algorithm for the hash function you only need to instantiate a desired wrapper
and call a member function like getHashFromString() or getHashFromFile().  

The wrappers:

	* md5wrapper	for MD5		in hl_md5wrapper.h
	* sha1wrapper	for SHA1	in hl_sha1wrapper.h
	* sha256wrapper	for SHA256	in hl_sha256wrapper.h
	* sha384wrapper	for SHA384	in hl_sha384wrapper.h
	* sha512wrapper	for SHA512	in hl_sha512wrapper.h


-------------------------------------------------------------------------------	

5) The library's files

The library consist of the the following files:

	* Makefile 
	  The Unix Makefile to build hashlib++ as 
	  static library on UNIX-like systems.

	* hashlibpp.h
	  Basic include-file. Include this file to 
	  access hashlib++. 

	* hl_exception.h
	  Exception-Class for error-handling.

	* hl_hashwrapper.h
	  This is a base class of all wrappers.

	* hl_md5.h and hl_md5.cpp
	  These two files implement the MD5 algorithm. 
	
	* hl_md5wrapper.h and hl_md5wrapper.cpp
	  These files represents the MD5 wrapper. 
	
        * hl_sha1.h and hl_sha1.cpp
	  These two files implement the SHA1 algorithm. 
	
	* hl_sha1wrapper.h and hl_sha1wrapper.cpp
	  These files represents the SHA1 wrapper. 

        * hl_sha256.h and hl_sha256.cpp
	  These two files implement the SHA256 algorithm. 
	
	* hl_sha256wrapper.h and hl_sha256wrapper.cpp
	  These files represents the SHA256 wrapper. 

	* hl_sha2ext.h and hl_sha2ext.cpp
	  This is the implementation of SHA384 and SHA512
	  algorithm.

	* hl_2mac.h
	  Contains some macros used in the SHA384 and SHA512
	  implementation.

	* hl_sha384wrapper.h and hl_sha384wrapper.cpp
	  The wrapper class for SHA384.

	* hl_sha512wrapper.h and hl_sha512wrapper.cpp
	  The wrapper class for SHA512.

	* hl_types.h
	  Contains internal typedefs.
	
-------------------------------------------------------------------------------	

6) Using hashlib++

Using hashlib++ is very easy. First you have to include the hashlib++ header file :

	#include <hashlibpp.h>

Now you are ready to create a wrapper object, for example a MD5-Wrapper:

	hashwrapper *myWrapper = new md5wrapper();

If you like you can perform a test to check if the wrapper works fine
on the target system, by calling the test function:
	
	try
	{
		myWrapper->test();
	}
	catch(hlException &e)
	{
		//your error handling here 
	}

If everything worked fine you can create a hash based on a string or a file:

	try
	{
		std::cout << myWrapper->getHashFromString("Hello world") << std::endl;
		std::cout << myWrapper->getHashFromFile("/etc/motd") << std::endl;
	}
	catch(hlException &e)
	{
		//your error handling here
	}


Don't forget to clean up (or create the wrapper on the stack)

	delete myWrapper;
	myWrapper = NULL;

More examples can be found in the "tools"-directory: 

	* tools/hello.cpp
	  Prints "Hello World" as MD5 to stdout.

	* tools/h.cpp 
	  A commandline tool to create checksums.

	* tools/libtest.cpp
	  Performs a Test for every supported hash function.

-------------------------------------------------------------------------------	
  
7) Supported operation systems

hashlib++ was written in standard C++, so it is platform independent and
should work with every C++ compiler. It was tested on the following systems
which are official supported:

	FreeBSD 6.2 with g++ 3.4.6 and 4.1.3
	Microsoft Windows XP with Microsoft Visual C++ 2005 Express Edition
	Gnu/Linux 2.6 with g++ 3.4.6 and 4.3.4

-------------------------------------------------------------------------------	

8) License

hashlib++ - a simple hash library for C++

Copyright (c) 2007-2010 Benjamin Grüdelbach

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	1)     Redistributions of source code must retain the above copyright
	       notice, this list of conditions and the following disclaimer.

	2)     Redistributions in binary form must reproduce the above copyright
	       notice, this list of conditions and the following disclaimer in
	       the documentation and/or other materials provided with the
	       distribution.
	     
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-------------------------------------------------------------------------------	

9) Feedback, Questions, Contact

I would be pleased to hear some feedback from you.
For special question feel free to send me an e-mail.

Benjamin Grüdelbach
bennygr@users.sourceforge.net
http://hashlib2plus.sourceforge.net/ 

-------------------------------------------------------------------------------	

10) Third party agreements

	* MD5 implementation

	  The hashlib++ MD5 implementation is derivative from the sourcecode
	  published in RFC 1321 which contains the following copyright: 

	  Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights
	  reserved.

	  License to copy and use this software is granted provided that it is
	  identified as the "RSA Data Security, Inc. MD5 Message-Digest
	  Algorithm" in all material mentioning or referencing this software or
	  this function.

	  License is also granted to make and use derivative works provided that
	  such works are identified as "derived from the RSA Data Security, Inc.
	  MD5 Message-Digest Algorithm" in all material mentioning or
	  referencing the derived work.

	  RSA Data Security, Inc. makes no representations concerning either the
	  merchantability of this software or the suitability of this software
	  for any particular purpose. It is provided "as is" without express or
	  implied warranty of any kind.

	  These notices must be retained in any copies of any part of this
	  documentation and/or software.

	* SHA1 implementation
	  
	  The hashlib++ SHA1 implementation is derivative from the sourcecode
	  published in RFC 3174 which contains the following copyright: 
	  
	  Copyright (C) The Internet Society (2001).  All Rights Reserved.
	  
	  This document and translations of it may be copied and furnished to
	  others, and derivative works that comment on or otherwise explain it
	  or assist in its implementation may be prepared, copied, published and
	  distributed, in whole or in part, without restriction of any kind,
	  provided that the above copyright notice and this paragraph are
	  included on all such copies and derivative works.  However, this
	  document itself may not be modified in any way, such as by removing
	  the copyright notice or references to the Internet Society or other
	  Internet organizations, except as needed for the purpose of developing
	  Internet standards in which case the procedures for copyrights defined
	  in the Internet Standards process must be followed, or as required to
	  translate it into languages other than English.
	  
	  The limited permissions granted above are perpetual and will not be
	  revoked by the Internet Society or its successors or assigns.
	  
	  This document and the information contained herein is provided on an
	  "AS IS" basis and THE INTERNET SOCIETY AND THE INTERNET ENGINEERING
	  TASK FORCE DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
	  NOT LIMITED TO ANY WARRANTY THAT THE USE OF THE INFORMATION HEREIN
	  WILL NOT INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES OF
	  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.

	* SHA256 implementation

	  The hashlib++ SHA256,SHA384 and SHA512 implementation is derivative 
	  from the sourcecode published by Aaron D. Gifford
	  
	  Copyright (c) 2000-2001, Aaron D. Gifford All rights reserved.
	  
	  Redistribution and use in source and binary forms, with or without
	  modification, are permitted provided that the following conditions are
	  met:
	  1. Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.  
	  2. Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	  3. Neither the name of the copyright holder nor the names of
	  contributors may be used to endorse or promote products derived from
	  this software without specific prior written permission.
	  
	  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS''
	  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S)
       	  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
       	  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	  THE POSSIBILITY OF SUCH DAMAGE.
	    
-------------------------------------------------------------------------------	
EOF
