//----------------------------------------------------------------------
//
// Filename    : TextInfo.h
// Writen By   : Reiot
//
//----------------------------------------------------------------------

#ifndef __TEXT_INFO_H__
#define __TEXT_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

const uint maxTexts = 10;

//----------------------------------------------------------------------
//
// class TextInfo;
//
//----------------------------------------------------------------------

class TextInfo {

public :

	//----------------------------------------------------------------------
	// 패킷에 embedding 되려는 객체는 아래와 같은 데이타를 명시해야 한다.
	//----------------------------------------------------------------------

	// read data from socket input stream
	void read ( SocketInputStream & iStream ) ;

	// write data to socket output stream
	void write ( SocketOutputStream & oStream ) const ;

	// get size of object
	uint getSize () const  
	{ 
		return szDWORD 					// text id
			+ szBYTE + m_Writer.size() 	// writer
			+ szBYTE + m_Topic.size() 	// topic
			+ szDWORD; 					// hits
	}

	// get debug std::string
	std::string toString () const ;

	// get max size()
	static uint getMaxSize ()  
	{ 
		return szDWORD		// text id
			+ szBYTE + 20 	// writer
			+ szBYTE + 255	// topic
			+ szDWORD; 		// hits
	} 
	
public :

	uint getID () const  { return m_ID; }
	void setID ( uint id )  { m_ID = id; }
	
	const std::string& getWriter () const  { return m_Writer ; }
	void setWriter( std::string writer )  { m_Writer = writer ; }
	
	const std::string& getTopic () const  { return m_Topic ; }
	void setTopic ( std::string topic )  { m_Topic = topic ; }
	
	uint getHit () const  { return m_Hit ; }
	void setHit ( uint hit)  { m_Hit = hit ; }
	
private :

	DWORD 	m_ID;
	std::string 	m_Writer;
	std::string 	m_Topic;
	DWORD 	m_Hit;

};

#endif
