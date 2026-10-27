//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoSelf.cpp
// Description : AOS/MOBA self score packet.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCMobaInfoSelf.h"

void GCMobaInfoSelf::read(SocketInputStream& iStream)
{
	__BEGIN_TRY

	iStream.read(m_Level);
	iStream.read(m_KillPlayer);
	iStream.read(m_Dead);
	iStream.read(m_KillMonster);
	iStream.read(m_TimeLeft);

	__END_CATCH
}

void GCMobaInfoSelf::write(SocketOutputStream& oStream) const
{
	__BEGIN_TRY

	oStream.write(m_Level);
	oStream.write(m_KillPlayer);
	oStream.write(m_Dead);
	oStream.write(m_KillMonster);
	oStream.write(m_TimeLeft);

	__END_CATCH
}

void GCMobaInfoSelf::execute(Player* pPlayer)
{
	__BEGIN_TRY

	GCMobaInfoSelfHandler::execute(this, pPlayer);

	__END_CATCH
}

std::string GCMobaInfoSelf::toString() const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCMobaInfoSelf(Level:" << m_Level
		<< ",KillPlayer:" << m_KillPlayer
		<< ",Dead:" << m_Dead
		<< ",KillMonster:" << m_KillMonster
		<< ",TimeLeft:" << m_TimeLeft
		<< ")";
	return msg.toString();

	__END_CATCH
}
