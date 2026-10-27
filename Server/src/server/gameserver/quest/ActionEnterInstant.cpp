//////////////////////////////////////////////////////////////////////////////
// ActionEnterInstant.cpp
//////////////////////////////////////////////////////////////////////////////

#include "ActionEnterInstant.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "ZoneUtil.h"
#include "DynamicZoneMoba.h"
#include "StringStream.h"
#include "Gpackets/GCNPCResponse.h"

namespace
{
	const ZoneID_t AOS_MOBA_TEMPLATE_ZONE_ID = 8111;
}

void ActionEnterInstant::read(PropertyBuffer& propertyBuffer)
{
	__BEGIN_TRY

	try
	{
		m_ZoneID = propertyBuffer.getPropertyInt("ZoneID");
		m_X = propertyBuffer.getPropertyInt("X");
		m_Y = propertyBuffer.getPropertyInt("Y");
	}
	catch (NoSuchElementException& nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

void ActionEnterInstant::execute(Creature* pNPC, Creature* pCreature)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert(pPC != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert(pGamePlayer != NULL);

	if (m_ZoneID == AOS_MOBA_TEMPLATE_ZONE_ID)
	{
		DynamicZoneMoba::JoinQueue(pPC);
	}
	else
	{
		transportCreature(pCreature, m_ZoneID, m_X, m_Y, true);
	}

	GCNPCResponse response;
	response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
	pGamePlayer->sendPacket(&response);

	__END_CATCH
}

string ActionEnterInstant::toString() const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionEnterInstant("
		<< "ZoneID:" << (int)m_ZoneID
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ")";
	return msg.toString();

	__END_CATCH
}
