////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeMoney.cpp 
// Written By  : 김성민
// Description : 
////////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGTradeMoney.h"

////////////////////////////////////////////////////////////////////////////////
// It reads data from the input stream (buffer) to initialize the packet.
////////////////////////////////////////////////////////////////////////////////
void CGTradeMoney::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read( m_TargetObjectID );
	iStream.read( m_Gold );
	iStream.read( m_Code );

	__END_CATCH
}

		    
////////////////////////////////////////////////////////////////////////////////
// Sends the binary image of the packet to the output stream (buffer).
////////////////////////////////////////////////////////////////////////////////
void CGTradeMoney::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write( m_TargetObjectID );
	oStream.write( m_Gold );
	oStream.write( m_Code );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// execute packet's handler
////////////////////////////////////////////////////////////////////////////////
void CGTradeMoney::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CGTradeMoneyHandler::execute ( this , pPlayer );
	#endif	
		
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug std::string
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string CGTradeMoney::toString () 
		const 
	{
		__BEGIN_TRY

		StringStream msg;
		msg << "CGTradeMoney(" 
			<< "TargetObjectID : " << (int)m_TargetObjectID << ","
			<< "Gold : "           << (int)m_Gold           << ","
			<< "CODE : "           << (int)m_Code
			<< ")";
		return msg.toString();

		__END_CATCH
	}
#endif
