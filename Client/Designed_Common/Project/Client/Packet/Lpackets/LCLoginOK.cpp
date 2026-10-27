//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCLoginOK.cpp
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCLoginOK.h"
#include "UserInformation.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void LCLoginOK::read ( SocketInputStream & iStream )
	
{
	__BEGIN_TRY

	// 서버 그룹 이름을 읽는다.
	iStream.read( m_isAdult );
	iStream.read( m_bUnderFifthteen );
	iStream.read( m_bFamily );
	iStream.read( m_Stat );
//	iStream.read( m_LastDays );
#if __CONTENTS(__PAYZONE_PASS_TICKET)
	iStream.read( m_PayType );	
#endif //__PAYZONE_PASS_TICKET

#if __CONTENTS(__LOGIN_PACKET)
	iStream.read( m_ReservedValue );
#endif //__LOGIN_PACKET
	iStream.read(m_ServerID);
	iStream.read(m_ServerIDA);
	iStream.read(m_ServerIDB);
	iStream.read(m_ServerIDC);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void LCLoginOK::write ( SocketOutputStream & oStream ) const
	
{
	__BEGIN_TRY

	// 그룹 이름을 쓴다.
	oStream.write( m_isAdult );
	oStream.write( m_bUnderFifthteen );
	oStream.write( m_bFamily );
	oStream.write( m_Stat );
	
//	oStream.write( m_LastDays );
#if __CONTENTS(__PAYZONE_PASS_TICKET)
	oStream.write( m_PayType );
#endif //__PAYZONE_PASS_TICKET

#if __CONTENTS(__LOGIN_PACKET)
	oStream.write( m_ReservedValue );
#endif //__LOGIN_PACKET
	__END_CATCH
	oStream.write(m_ServerID);
	oStream.write(m_ServerIDA);
	oStream.write(m_ServerIDB);
	oStream.write(m_ServerIDC);
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void LCLoginOK::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	LCLoginOKHandler::execute( this , pPlayer );
		
	__END_CATCH
}


PacketSize_t LCLoginOK::getPacketSize() const  
{ 
	return szBYTE + szBYTE + szBYTE + szBYTE
#if __CONTENTS(__PAYZONE_PASS_TICKET)
		+ szBYTE
#endif //__PAYZONE_PASS_TICKET

#if __CONTENTS(__LOGIN_PACKET)
		+ szBYTE
#endif //__LOGIN_PACKET
		+ szBYTE
		+ szBYTE
		+ szBYTE
		+ szBYTE
		; 
}
