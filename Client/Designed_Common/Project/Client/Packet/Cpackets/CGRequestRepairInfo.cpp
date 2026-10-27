//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRepairInfo.cpp 
// Written By  : 김성민
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestRepairInfo.h"

void CGRequestRepairInfo::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);

	__END_CATCH
}

void CGRequestRepairInfo::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);

	__END_CATCH
}

void CGRequestRepairInfo::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

	CGRequestRepairInfoHandler::execute (this , pPlayer);
		
	__END_CATCH
}

std::string CGRequestRepairInfo::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGRequestRepairInfo("
		<< "ObjectID : " << (int)m_ObjectID 
		<< ")";
	return msg.toString();

	__END_CATCH
}
