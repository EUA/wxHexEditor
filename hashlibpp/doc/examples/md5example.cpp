#include <hashlibpp.h>
#include <string>
#include <iostream>  //for "cerr"

/*
 * creating a wrapper object
 */
hashwrapper *myWrapper = new md5wrapper();

try
{
	/*
	 * create a hash from a string
	 */
	std::string hash1 = myWrapper->getHashFromString("Hello World");

	/*
	 * create a hash based on a file
	 */
	std::string hash2 = myWrapper->getHashFromFile("README.TXT");
}
catch(hlException &e)
{

	std::cerr << "Error(" 
		  << e.error_number()
		  << "): "
		  << e.erro_message()
		  << std::endl;
}

delete myWrapper;
