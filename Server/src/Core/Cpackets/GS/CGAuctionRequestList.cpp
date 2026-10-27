//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionRequestList.cpp
// Description : Auction House listing request packet.
//////////////////////////////////////////////////////////////////////////////

#include "CGAuctionRequestList.h"

void CGAuctionRequestList::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	(void)iStream;
	__END_CATCH
}

void CGAuctionRequestList::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	(void)oStream;
	__END_CATCH
}

void CGAuctionRequestList::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	CGAuctionRequestListHandler::execute(this, pPlayer);
	__END_CATCH
}

string CGAuctionRequestList::toString() const
	throw()
{
	__BEGIN_TRY
	return "CGAuctionRequestList()";
	__END_CATCH
}
