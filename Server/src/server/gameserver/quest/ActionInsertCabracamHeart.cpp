////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionInsertCabracamHeart.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////


#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "ActionInsertCabracamHeart.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "ZoneUtil.h"

#include "StringPool.h"
#include "Inventory.h"
#include "MenegrothDoungeonZoneManager.h"
#include "MenegrothDoungeonManager.h"
#include "EffectCabracamAltar.h"

#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCAddEffect.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////

void ActionInsertCabracamHeart::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try 
	{
		m_iIncompleteScriptID = propertyBuffer.getPropertyInt("IncompleteScriptID");
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
void ActionInsertCabracamHeart::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	//cout << "ActionInsertCabracamHeart::execute()" << endl;

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	Zone *pZone = pCreature1->getZone();

	NPC *pNPC = dynamic_cast<NPC*>(pCreature1);
	PlayerCreature *pPC = dynamic_cast<PlayerCreature*>(pCreature2);

	Player *pPlayer2 = pCreature2->getPlayer();

	if ( pZone == NULL )
	{
		return;
	}

	Inventory *pInventory = pPC->getInventory();
	if ( pInventory == NULL )
	{
		return;
	}

	MenegrothDoungeonZoneManager *pMenegrothDoungeonZoneManager;
	pMenegrothDoungeonZoneManager = pZone->getMenegrothDoungeonZoneManager();

	if ( pCreature1->isFlag(Effect::EFFECT_CLASS_MENEGROTH_CABRACAM_ALTAR) )
	{
		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer2->sendPacket( &gcNPCResponse );

		return;
	}

	if ( pMenegrothDoungeonZoneManager->IsAlreadySummonCabracam() )
	{
		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer2->sendPacket( &gcNPCResponse );

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_SUMMON_CABRACAM_MONSTER_ALREADY) ); 
		pPlayer2->sendPacket( &gcSystemMessage );

		return;
	}

	if ( !g_MenegrothDoungeonManager.hasRequiredCabracamHeart(pCreature2) )
	{
		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer2->sendPacket( &gcNPCResponse );

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_ENOUGH_CABRACAM_HEART) ); 
		pPlayer2->sendPacket( &gcSystemMessage );
		
		return;
	}

	g_MenegrothDoungeonManager.takeRequiredCabracamHeart(pCreature2);

	pMenegrothDoungeonZoneManager->AddCabracamHeartToAltar(pNPC->getObjectID());

	EffectCabracamAltar* pEffect = new EffectCabracamAltar(pCreature1);
	pCreature1->addEffect(pEffect);

	pEffect->affect();

	cout << "ZoneID = " << pZone->getZoneID()
		<< "CabracamHeaert Count = " << pMenegrothDoungeonZoneManager->GetCabracamHeartCountToAltar() << endl;

	if ( pMenegrothDoungeonZoneManager->GetCabracamHeartCountToAltar() < 4 )
	{
		/*
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID(pNPC->getObjectID());
		gcNPCAsk.setScriptID(m_iIncompleteScriptID);
		gcNPCAsk.setNPCID(pNPC->getNPCID());

		pPlayer2->sendPacket(&gcNPCAsk);
		*/

		GCNPCResponse gcNPCResponse;

		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer2->sendPacket( &gcNPCResponse );

		return;
	}

	// Now, collect 4 cabracam heart. so, summon cabracam......

	// 카브라캄 소환 준비
	if ( !pMenegrothDoungeonZoneManager->BeReadyToSummonCabracam() )
	{
		// already summon cabracam

		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer2->sendPacket( &gcNPCResponse );

		return;
	}
	
	pMenegrothDoungeonZoneManager->ResetCabracamHeartToAltar();

	GCNPCResponse gcNPCResponse;
	gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer2->sendPacket( &gcNPCResponse );

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_SUMMON_CABRACAM_MONSTER) ); 
	pPlayer2->sendPacket( &gcSystemMessage );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionInsertCabracamHeart::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionInsertCabracamHeart("
	    << ")";

	return msg.toString();

	__END_CATCH
}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
