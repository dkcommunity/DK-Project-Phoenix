//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestTmallList.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGRequestTmallList.h"


void CGRequestTmallList::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
	
	iStream.read( m_Kind );
		
	__END_CATCH
}

void CGRequestTmallList::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
	
	oStream.write( m_Kind );
		
	__END_CATCH
}

void CGRequestTmallList::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestTmallListHandler::execute(this , pPlayer);

	__END_CATCH
}

std::string CGRequestTmallList::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestTmallList("
		<< "Kind : " << m_Kind
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
