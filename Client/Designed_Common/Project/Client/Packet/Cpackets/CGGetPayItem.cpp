#include "Client_PCH.h"
#include "CGGetPayItem.h"

void CGGetPayItem::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_MoneyID);

	__END_CATCH
}

void CGGetPayItem::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_MoneyID);

	__END_CATCH
}

void CGGetPayItem::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

	CGGetPayItemHandler::execute (this , pPlayer);
		
	__END_CATCH
}

std::string CGGetPayItem::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGGetPayItem(" 
			<< ")";
	return msg.toString();

	__END_CATCH
}
