////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionEnterCastleB2F.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionEnterCastleB2F.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "StringPool.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "MonsterManager.h"

#include <stdio.h>

#include "Gpackets/GCSystemMessage.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionEnterCastleB2F::read (PropertyBuffer & pb)
{
    __BEGIN_TRY

	try 
	{
		m_ZoneID = pb.getPropertyInt("ZoneID");
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
void ActionEnterCastleB2F::execute (Creature * pNPC , Creature * pCreature)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pCreature != NULL);
	Assert(pCreature->isPC());

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
	Assert( pGamePlayer != NULL );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	Zone* pTargetZone = getZoneByZoneID( m_ZoneID );
	Assert( pZone != NULL );

	if ( !pZone->getMonsterManager()->isAllEventMonsterNotExist() )
	{
		GCSystemMessage message;
		message.setMessage( g_pStringPool->getString( STRID_CANNOT_ENTER ) );
		pGamePlayer->sendPacket( &message );

		return;
	}

	ZoneCoord_t randX = Random( 0, pTargetZone->getWidth() );
	ZoneCoord_t randY = Random( 0, pTargetZone->getHeight() );

	transportCreature( pCreature, m_ZoneID, randX, randY, true );

	__END_DEBUG
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionEnterCastleB2F::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionEnterCastleB2F("
	    << ")";
	return msg.toString();

	__END_CATCH
}

