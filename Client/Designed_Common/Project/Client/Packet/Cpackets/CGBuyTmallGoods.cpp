#include "Client_PCH.h"
#include "CGBuyTmallGoods.h"

void CGBuyTmallGoods::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_GoodsID);
	iStream.read(m_Kind);

	__END_CATCH
}

void CGBuyTmallGoods::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_GoodsID);
	oStream.write(m_Kind);

	__END_CATCH
}

void CGBuyTmallGoods::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

	CGBuyTmallGoodsHandler::execute (this , pPlayer);
		
	__END_CATCH
}

std::string CGBuyTmallGoods::toString () 
	const 
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGBuyTmallGoods(" 
			<< ")";
	return msg.toString();

	__END_CATCH
}
