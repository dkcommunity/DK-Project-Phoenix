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

class StringInfo {

public :
	
	// constructor
	StringInfo () throw ();
	
	// destructor
	~StringInfo () throw ();

public :
	
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	PacketSize_t getSize () const throw ();
	static uint getMaxSize() throw();

	// get packet's debug string
	string toString () const throw ();
	
	void SetString(const string& str);
	string GetString() const  { return m_String; }
	
	BYTE GetLength() const { return m_String.size(); }
	
	void SetMaxLength(BYTE maxLength) { m_maxLength = maxLength; }
	BYTE GetMaxLength() const  { return m_maxLength; }

private :
	string m_String;
	
	BYTE m_maxLength;
};

#endif
