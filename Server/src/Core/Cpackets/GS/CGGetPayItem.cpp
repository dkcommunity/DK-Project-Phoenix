#include "CGGetPayItem.h"

void CGGetPayItem::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_MoneyID);

	__END_CATCH
}

void CGGetPayItem::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_MoneyID);

	__END_CATCH
}

void CGGetPayItem::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGGetPayItemHandler::execute (this , pPlayer);
		
	__END_CATCH
}

string CGGetPayItem::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGGetPayItem(" 
			<< ")";
	return msg.toString();

	__END_CATCH
}
