////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterMenegroth.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__

#include "ActionEnterMenegroth.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "ZoneUtil.h"

#include "StringPool.h"
#include "MenegrothDoungeonManager.h"
#include "EffectMenegrothDoungeon.h"

#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCNPCResponse.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////

void ActionEnterMenegroth::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try 
	{
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
void ActionEnterMenegroth::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	//cout << "ActionEnterMenegroth::execute()" << endl;

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	ZoneID_t ZoneIDToMove;
	ZoneCoord_t XPosToMove;
	ZoneCoord_t YPosToMove;

	Player *pPlayer = pCreature2->getPlayer();

	if ( !g_MenegrothDoungeonManager.isValidLevel(pCreature2) )
	{
        GCSystemMessage gcSystemMessage;

        gcSystemMessage.setMessage( g_pStringPool->getString( STRID_NOT_ENOUGH_MENEGROTH_REQUIREDLEVEL ) );
        pPlayer->sendPacket(&gcSystemMessage);

        GCNPCResponse response;

        response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
        pPlayer->sendPacket( &response );

		return;
	}

	if ( !g_MenegrothDoungeonManager.hasRequiredEntranceItems(pCreature2) )
	{
        GCSystemMessage gcSystemMessage;

        gcSystemMessage.setMessage( g_pStringPool->getString( STRID_NOT_ENOUGH_MENEGROTH_REQUIREDITEM ) );
        pPlayer->sendPacket(&gcSystemMessage);

        GCNPCResponse response;

        response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
        pPlayer->sendPacket( &response );


        return;
    }

	if ( !g_MenegrothDoungeonManager.getDoungeonZoneToMove(pCreature2, ZoneIDToMove, XPosToMove, YPosToMove) )
	{
        GCNPCResponse response;

        response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
        pPlayer->sendPacket( &response );

		return;
	}

	g_MenegrothDoungeonManager.takeRequiredEntranceItems(pCreature2);

	EffectMenegrothDoungeon* pEffect = new EffectMenegrothDoungeon(pCreature2);
	pEffect->setNextTime(0);
	pEffect->setDeadline(600 * 60);	// 600 * 60 = 60 minutes
	pCreature2->addEffect(pEffect);

	pCreature2->setFlag(Effect::EFFECT_CLASS_MENEGROTH_DOUNGEON);

	/*
	Timeval currentTime;
	getCurrentTime(currentTime);
	Timeval remainTime = timediff( pEffect->getDeadline(), currentTime );
	Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

	char message[512];
	sprintf(message, g_pStringPool->c_str(STRID_MENEGROTH_DOUNGEON_LIMITED_TIMER), remainTurn);

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
	gcSystemMessage.setMessage(message);

	pCreature2->getPlayer()->sendPacket(&gcSystemMessage);
	*/
	g_MenegrothDoungeonManager.sendRemainMenegrothTime(pCreature2);

	transportCreature( pCreature2, ZoneIDToMove, XPosToMove, YPosToMove, false );
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionEnterMenegroth::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionEnterMenegroth("
	    << ")";

	return msg.toString();

	__END_CATCH
}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
