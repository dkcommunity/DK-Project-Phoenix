//////////////////////////////////////////////////////////////////////
//
// Filename    : StringStream.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __STRING_STREAM_H__
#define __STRING_STREAM_H__

#pragma warning(disable:4786)

// include files
#include "Types.h"
#include <list>

// end of stream
const char eos = '\n';

class StringStream {
	
public :
	
	// constructor
	StringStream ();
	
	// destructor
	~StringStream ();
	
	
public :
	
	// add std::string to stream
	StringStream & operator << ( bool T );
	StringStream & operator << ( char T );
	StringStream & operator << ( uchar T );
	StringStream & operator << ( short T );
	StringStream & operator << ( ushort T );
	StringStream & operator << ( int T );
	StringStream & operator << ( uint T );
	StringStream & operator << ( long T );
	StringStream & operator << ( ulong T );
	StringStream & operator << ( float T );
	StringStream & operator << ( double T );
	StringStream & operator << ( const char * str );
	StringStream & operator << ( const std::string & str );

	// make std::string
	std::string toString () const;
	
	// true if stream is empty
	bool isEmpty () const { return m_Size == 0; }

private :
	
	// list of std::string
	std::list<std::string> m_Strings;

	// size of std::string which will be generated
	ushort m_Size;
	
	// inserted flag 
	mutable bool m_bInserted;
	
	// buffer for std::string will be generated
	mutable std::string m_Buffer;

};

#endif
