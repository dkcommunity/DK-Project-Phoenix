////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckEventLevelUp200604.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionCheckEventLevelUp200604.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "NPC.h"
#include "Player.h"
#include "VariableManager.h"
#include "DB.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

////////////////////////////////////////////////////////////////////////////////
// ActionCheckEventLevelUp200604
////////////////////////////////////////////////////////////////////////////////
ActionCheckEventLevelUp200604::ActionCheckEventLevelUp200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionCheckEventLevelUp200604::~ActionCheckEventLevelUp200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionCheckEventLevelUp200604::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_Level = (Level_t)propertyBuffer.getPropertyInt("Level");
		m_SatisfyScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("SatisfyScriptID");
		m_UnsatisfyScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("UnsatisfyScriptID");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionCheckEventLevelUp200604::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert(pPC != NULL);

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	Level_t level = pPC->getLevel();

	if ( level < m_Level )
	{
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_UnsatisfyScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );
	}
	else
	{
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_SatisfyScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionCheckEventLevelUp200604::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionCheckEventLevelUp200604("
		<< "Level:" << (int)m_Level
		<< "SatisfyScriptID:" << (int)m_SatisfyScriptID
		<< "UnsatisfyScriptID:" << (int)m_UnsatisfyScriptID
	    << ")";
	return msg.toString();

	__END_CATCH
}
