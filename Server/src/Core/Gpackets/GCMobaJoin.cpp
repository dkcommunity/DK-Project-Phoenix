//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaJoin.cpp
// Description : AOS/MOBA queue state packet.
//////////////////////////////////////////////////////////////////////////////

#include "GCMobaJoin.h"

void GCMobaJoin::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	iStream.read(m_State);
	iStream.read(m_JoinedCount);
	iStream.read(m_MaxCount);
	iStream.read(m_RemainingSeconds);

	__END_CATCH
}

void GCMobaJoin::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	oStream.write(m_State);
	oStream.write(m_JoinedCount);
	oStream.write(m_MaxCount);
	oStream.write(m_RemainingSeconds);

	__END_CATCH
}

void GCMobaJoin::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	GCMobaJoinHandler::execute(this, pPlayer);

	__END_CATCH
}

string GCMobaJoin::toString() const
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCMobaJoin(State:" << (int)m_State
		<< ",Joined:" << (int)m_JoinedCount
		<< ",Max:" << (int)m_MaxCount
		<< ",Remain:" << (int)m_RemainingSeconds
		<< ")";
	return msg.toString();

	__END_CATCH
}
