//////////////////////////////////////////////////////////////////////////////
// Filename    : CLVersionCheck.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CLVersionCheck.h"

void CLVersionCheck::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	iStream.read(m_Version);
//	iStream.read(m_VersionT);

	__END_CATCH
}

void CLVersionCheck::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_Version);
	//oStream.write(m_VersionT);

	__END_CATCH
}

void CLVersionCheck::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CLVersionCheckHandler::execute (this , pPlayer);
		
	__END_CATCH
}

string CLVersionCheck::toString () const
	throw ()
{
	StringStream msg;
	msg << "CLVersionCheck("
		<< "Version:" << (int)m_Version
		<< ")";
	return msg.toString();
}

