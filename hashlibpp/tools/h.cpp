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
 * hashlib++ example program.
 * This is a short example program which shows how to use hashlib++.
 * It produces hash-values for a given string or file.
 *
 * usage: 
 * [-t] [-s STRING | -f FILENAME] [-h HASHTYPE]
 *
 *  -t       Tests the implementation on your system before.
 *  -s       Creates a hash from a given string.
 *  -f       Creates a hash from a given file.
 *  -h       Hashfunction to use: md5,sha1,sha256,sha384,sha512.
 *
 * hashlib++ - a simple hash library for C++
 * Copyright (c) 2007,2008,2009 Benjamin Grüdelbach
 *
 */


//---------------------------------------------------------------------- 
//hashlib++ includes
#include <hashlibpp.h>

//----------------------------------------------------------------------	
//misc. includes
#include <iostream>
#include <string.h>

//----------------------------------------------------------------------	
//prototypes
void printUsage (void);
hashwrapper* factoryMethod(std::string htype);

//----------------------------------------------------------------------	

int main ( int argc, char **argv)
{
	bool bCreateFromText = false;
	bool bCreateFromFile = false;
	bool bTestFirst = false;
	std::string strInput = "";
	std::string strHashType = "";

	//just parse the commandline
	for( int i = 1; i < argc ; i++ )
	{
		if( strcmp(argv[i],"-s") == 0 )
		{
			bCreateFromText = true;	
			if( i != argc-1 )
			{
				strInput = argv[i+1];
			}
		}
		else if( strcmp(argv[i],"-f") == 0 )
		{
			bCreateFromFile = true;	
			if( i != argc-1 )
			{
				strInput = argv[i+1];
			}
		}
		else if( strcmp(argv[i],"-h") == 0)
		{
			if( i != argc-1 )
			{
				strHashType = argv[i+1];
			}
		}
		else if( strcmp(argv[i],"-t") == 0)
		{
			bTestFirst = true;
		}
	}

	// just print out some help if no options have been submited
	if( ((bCreateFromText == false) && (bCreateFromFile == false))  ||
	    (strHashType == "") )
	{
		printUsage();
		return -1;
	}

        //--------------------------------------------------------------
	
	//Create the wrapper using a simple factory-method
	hashwrapper *hw = factoryMethod(strHashType);
	if( hw != NULL)
	{
		try
		{
			if(bTestFirst)
			{
				//Tests the implemtation. If the test fails on your 
				//system this will throw a hlException
				hw->test();
			}
			if( bCreateFromText )
			{
				//Creates a hash based on the given string
				std::cout << hw->getHashFromString(strInput) << std::endl;
			}
			else if( bCreateFromFile )
			{
				//Create a hash based on the given file
				//This could throw a hlException if the given 
				//file does not exist
				std::cout << hw->getHashFromFile(strInput) << std::endl;
			}
		}
		catch(hlException &e)
		{
			std::cerr << std::endl;
			std::cerr << "Error (" 
				  << e.error_number()
				  << "): " 
				  << e.error_message()
				  << std::endl;
			delete hw;
			return -2;
		}
	}
	else
	{
		std::cerr << std::endl;
		std::cerr << "Error: Unknown hashtype \""
			  << strHashType  
			  << "\"."
			  << std::endl;

		printUsage();
		return -1;
	}

	delete hw;
	return 0;
}

//----------------------------------------------------------------------	

void printUsage (void)
{
	std::cerr << std::endl;
	std::cerr << "hashlib++ example program." << std::endl;
	std::cerr << "This is a short example program which shows how to use hashlib++." << std::endl;
	std::cerr << "It produces hash-values for a given string or file." <<std::endl;
	std::cerr << std::endl;
	std::cerr << "usage: " << std::endl;
	std::cerr << "[-t] [-s STRING | -f FILENAME] [-h HASHTYPE]" << std::endl;
	std::cerr << std::endl;
	std::cerr << "-t\t Tests the implementation on your system before." << std::endl;
	std::cerr << "-s\t Creates a hash from a given string." << std::endl;
	std::cerr << "-f\t Creates a hash from a given file." << std::endl;
	std::cerr << "-h\t Hashfunction to use: md5,sha1,sha256,sha384,sha512." << std::endl;
	std::cerr << std::endl;
 	std::cerr << "hashlib++ - a simple hash library for C++" <<std::endl;
	std::cerr << "Copyright (c) 2007,2008,2009,2010 Benjamin Grüdelbach" <<std::endl;
	std::cerr << std::endl;
}

hashwrapper* factoryMethod(std::string htype)
{
	if( htype == "md5" )
		return new md5wrapper();
	else if( htype == "sha1")
		return new sha1wrapper();
	else if( htype == "sha256")
		return new sha256wrapper();
	else if( htype == "sha384")
		return new sha384wrapper();
	else if( htype == "sha512")
		return new sha512wrapper();
	return NULL;
}

//----------------------------------------------------------------------	
//EOF
