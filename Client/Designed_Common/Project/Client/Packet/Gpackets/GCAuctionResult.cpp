//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionResult.cpp
// Description : Auction House operation result packet.
//////////////////////////////////////////////////////////////////////////////

#include "GCAuctionResult.h"

GCAuctionResult::GCAuctionResult()
	throw()
	: m_ResultCode(AUCTION_RESULT_OK)
{
}

void GCAuctionResult::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	iStream.read(m_ResultCode);
	BYTE size;
	iStream.read(size);
	if (size > 128)
		throw InvalidProtocolException("GCAuctionResult message too large");
	m_Message = "";
	if (size > 0)
		iStream.read(m_Message, size);
	__END_CATCH
}

void GCAuctionResult::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	if (m_Message.size() > 128)
		throw InvalidProtocolException("GCAuctionResult message too large");
	oStream.write(m_ResultCode);
	BYTE size = (BYTE)m_Message.size();
	oStream.write(size);
	if (size > 0)
		oStream.write(m_Message);
	__END_CATCH
}

void GCAuctionResult::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	GCAuctionResultHandler::execute(this, pPlayer);
	__END_CATCH
}

string GCAuctionResult::toString() const
	throw()
{
	__BEGIN_TRY
	StringStream msg;
	msg << "GCAuctionResult(Code:" << (int)m_ResultCode << ", Message:" << m_Message << ")";
	return msg.toString();
	__END_CATCH
}
