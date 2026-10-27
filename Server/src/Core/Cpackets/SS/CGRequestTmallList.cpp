//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestTmallList.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGRequestTmallList.h"


void CGRequestTmallList::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
	
	iStream.read( m_Kind );
		
	__END_CATCH
}

void CGRequestTmallList::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
	
	oStream.write( m_Kind );
		
	__END_CATCH
}

void CGRequestTmallList::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CGRequestTmallListHandler::execute(this , pPlayer);

	__END_CATCH
}

string CGRequestTmallList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGRequestTmallList("
		<< "Kind : " << m_Kind
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
