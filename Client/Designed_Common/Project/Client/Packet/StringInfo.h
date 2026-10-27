//////////////////////////////////////////////////////////////////////
// 
// Filename    :  
// Written By  : rappi76
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __FRIEND_STRING_INFO_H__
#define __FRIEND_STRING_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class StringInfo;
//
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__FRIEND_ADDITION)
class StringInfo {

public :
	
	// constructor
	StringInfo () ;
	
	// destructor
	~StringInfo() ;

public :
	
    void read (SocketInputStream & iStream) ;
    void write (SocketOutputStream & oStream) const ;

	PacketSize_t getSize () const ;
	static uint getMaxSize() ;

	// get packet's debug std::string
	std::string toString () const ;
	
	void SetString(const std::string& str);
	std::string GetString() const  { return m_String; }
	
	BYTE GetLength() const { return m_String.size(); }
	
	void SetMaxLength(BYTE maxLength) { m_maxLength = maxLength; }
	BYTE GetMaxLength() const  { return m_maxLength; }

private :
	std::string m_String;
	
	BYTE m_maxLength;
};
#endif //__FRIEND_ADDITION

#endif
