//////////////////////////////////////////////////////////////////////
// 
// Filename    : ScriptParameter.h 
// Written By  : 
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SCRIPT_PARAMETER_H__
#define __SCRIPT_PARAMETER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class ScriptParameter;
//
// 
//
//////////////////////////////////////////////////////////////////////

class ScriptParameter {

public :
	
	// constructor
	ScriptParameter () ;
	
	// destructor
	~ScriptParameter() ;

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const ;

	// get packet's body size
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () ;

	static uint getMaxSize()  {
		return szBYTE + 255 + szBYTE + 255;
	}

	// get packet's debug std::string
	std::string toString () const ;

	// get/set Name
	const std::string& getName() const  { return m_Name; }
	void setName( const std::string& name )  { m_Name = name; }

	// get/set Value
	const std::string& getValue() const  { return m_Value; }
	void setValue( const std::string& value )  { m_Value = value; }

private :

	std::string m_Name;
	std::string m_Value;
};

#endif
