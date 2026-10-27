//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddMouseToGear.cpp 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGAddMouseToGear.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
CGAddMouseToGear::CGAddMouseToGear () 
     
{
	__BEGIN_TRY
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//CGAddMouseToGear::CGAddMouseToGear			m_GearSlotID	= 0;
		m_GearSlotID	= 0;
#endif //__GEAR_SWAP_CHANGE
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
CGAddMouseToGear::~CGAddMouseToGear() {
	__BEGIN_TRY
#if __CONTENTS(__GEAR_SWAP_CHANGE)
		m_GearSlotID	= 0;
#endif //__GEAR_SWAP_CHANGE
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void CGAddMouseToGear::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//CGAddMouseToGear::read		iStream.read(m_GearSlotID);
	iStream.read(m_GearSlotID);
#endif //__GEAR_SWAP_CHANGE
	iStream.read( m_ObjectID );
	iStream.read( m_SlotID );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void CGAddMouseToGear::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// CGAddMouseToGear::write 서버에 장착창의 ID를 보낸다.
	oStream.write(m_GearSlotID);
#endif //__GEAR_SWAP_CHANGE
	oStream.write( m_ObjectID ); 
	oStream.write( m_SlotID );
 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void CGAddMouseToGear::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CGAddMouseToGearHandler::execute ( this , pPlayer );
	#endif
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string CGAddMouseToGear::toString () 
		const 
	{
		__BEGIN_TRY
			
	StringStream msg;
	msg << "CGAddMouseToGear("
#if __CONTENTS(__GEAR_SWAP_CHANGE)
		<< "GearSlotID : " << (int)m_GearSlotID 
#endif //__GEAR_SWAP_CHANGE
		<< "ObjectID : " << (int)m_ObjectID 
		<< ",SlotID" << (int)m_SlotID
		<< ")";
	return msg.toString();

		__END_CATCH
	}

	// global variable definition
	//CGAddMouseToGearFactory g_CGAddMouseToGearFactory;
#endif
