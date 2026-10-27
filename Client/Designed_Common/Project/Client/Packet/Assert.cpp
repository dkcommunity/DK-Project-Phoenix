//--------------------------------------------------------------------------------
// 
// Filename   : Assert.cpp 
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "Types.h"
#include "DarkAssert.h"
#include "Exception.h"
#include <time.h>

//--------------------------------------------------------------------------------
//
// __assert__
//
// ? ????? __BEGIN_TRY , __END_CATCH? wrapping? ??? ??.
//
//--------------------------------------------------------------------------------
void __assert__ ( const char * file , uint line , const char * func , const char * expr )
	
{
	StringStream msg;
	
	msg << eos
		<< "Assertion Failed : " << file << " : " << line;

	if ( func )
		msg << " : " << func;

	time_t currentTime = time(0);
	
	msg << expr << " at " << ctime(&currentTime);
	
	std::ofstream ofile("assertion_failed.log", std::ios::app);
	ofile << msg.toString().c_str() << std::endl;
	ofile.close();

	throw AssertionError( msg.toString() );
}
