//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRepairInfo.cpp 
// Written By  : ±è¼º¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestRepairInfo.h"

void CGRequestRepairInfo::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);

	__END_CATCH
}

void CGRequestRepairInfo::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);

	__END_CATCH
}

void CGRequestRepairInfo::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGRequestRepairInfoHandler::execute (this , pPlayer);
		
	__END_CATCH
}

string CGRequestRepairInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGRequestRepairInfo("
		<< "ObjectID : " << (int)m_ObjectID 
		<< ")";
	return msg.toString();

	__END_CATCH
}
