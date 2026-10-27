//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestInfo.cpp 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestInfo.h"

void CGRequestInfo::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_Code);
	iStream.read(m_Value);

	__END_CATCH
}

void CGRequestInfo::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_Code);
	oStream.write(m_Value);

	__END_CATCH
}

void CGRequestInfo::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__
	CGRequestInfoHandler::execute (this , pPlayer);
#endif		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGRequestInfo::toString () const 
	
{
	StringStream msg;
	msg << "CGRequestInfo(" 
		<< "Code : " << (int)m_Code
		<< "Value : " << (int)m_Value
		<< ")";
	return msg.toString();
}
#endif
