/* 
 * hashlib++ - a simple hash library for C++
 * 
 * Copyright (c) 2007-2010 Benjamin Grüdelbach
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 	1)     Redistributions of source code must retain the above copyright
 * 	       notice, this list of conditions and the following disclaimer.
 * 
 * 	2)     Redistributions in binary form must reproduce the above copyright
 * 	       notice, this list of conditions and the following disclaimer in
 * 	       the documentation and/or other materials provided with the
 * 	       distribution.
 * 	     
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//---------------------------------------------------------------------- 

/*
 * This is a short program which tests the hash implementations
 * on your system.
 */


//---------------------------------------------------------------------- 
//hashlib++ includes
#include <hashlibpp.h>

//----------------------------------------------------------------------	
//misc. includes
#include <iostream>
#include <string.h>
//----------------------------------------------------------------------	

bool okay = true;

//----------------------------------------------------------------------	
void testWrapper(hashwrapper *w);

//----------------------------------------------------------------------	

int main ( int argc, char **argv)
{
	std::cout << std::endl;
	std::cout << "hashlib++ version: " << _HASHLIBPP_VERSION_ << std::endl;
	std::cout << "Now testing hashlib++ on your system:" << std::endl;

	std::cout << std::endl;
	std::cout << "--> md5..."; 
	testWrapper(new md5wrapper());
	std::cout << std::endl;
	std::cout << "--> sha1..."; 
	testWrapper(new sha1wrapper());
	std::cout << std::endl;
	std::cout << "--> sha256..."; 
	testWrapper(new sha256wrapper());
	std::cout << std::endl;
	std::cout << "--> sha384..."; 
	testWrapper(new sha384wrapper());
	std::cout << std::endl;
	std::cout << "--> sha512..."; 
	testWrapper(new sha512wrapper());
	std::cout << std::endl;
	std::cout << std::endl;
	if( okay )
	{
		std::cout << "hashlib++ was tested successful " 
			  <<  "on your system." 
			  << std::endl;
		return 0;
	}
	std::cout << "Error: hashlib++ was NOT tested successful " 
		  <<  "on your system." 
		  << std::endl;
	return -1;
}

void testWrapper(hashwrapper *w)
{
	try
	{
		w->test();
		std::cout << "OK.";
		delete w;
		w=0;
	}
	catch(hlException &e)
	{
		std::cout << "test failed.";
		okay = false;
		delete w;
		w=0;
	}
}


//----------------------------------------------------------------------	
//EOF
