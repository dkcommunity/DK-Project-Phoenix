////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckComebackEventItem200604.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionCheckComebackEventItem200604.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Zone.h"
#include "VariableManager.h"
#include "FlagSet.h"
#include "DB.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
// ActionCheckComebackEventItem200604
////////////////////////////////////////////////////////////////////////////////
ActionCheckComebackEventItem200604::ActionCheckComebackEventItem200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionCheckComebackEventItem200604::~ActionCheckComebackEventItem200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionCheckComebackEventItem200604::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_EventCheckType = propertyBuffer.getProperty("EventCheckType");
		m_EventScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("EventScriptID");
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
void ActionCheckComebackEventItem200604::execute(Creature * pCreature1 , Creature * pCreature2)
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

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	BYTE	CheckType;

	if( m_EventCheckType == "ALL" )
		CheckType = 0;
	else if( m_EventCheckType == "COMEBACK2006" )
		CheckType = 1;
	else if( m_EventCheckType == "COMEBACKPREMIUM2006" )
		CheckType = 2;
	else
	{
		Assert( false );
	}

	Statement* pStmt = NULL;

	bool bComebackEvent = false;
	bool bPremiumCombackEvent = false;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection( "DARKEDEN" )->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT RecvItemDate, PayPremiumDate, RecvPremiumItemDate FROM Event200604 WHERE PlayerID = '%s'",
				pGamePlayer->getID().c_str() );

		if ( pResult->next() )
		{
			string RecvItemDate = pResult->getString(1);
			string PayPremiumDate = pResult->getString(2);
			string RecvPremiumItemDate = pResult->getString(3);

			if ( RecvItemDate == "0000-00-00" )
			{
				bComebackEvent = true;
			}

			if ( PayPremiumDate != "0000-00-00" && RecvPremiumItemDate == "0000-00-00" )
			{
				bPremiumCombackEvent = true;
			}
		}
		else
		{
			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			gcNPCResponse.setParameter( YOU_ARE_NOT_EVENT_TARGET );
			pPlayer->sendPacket( &gcNPCResponse );

			SAFE_DELETE( pStmt );

			return;
		}
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )


	bool bSatisfy = false;

	if ( CheckType == 0 )
	{
		if ( bComebackEvent || bPremiumCombackEvent )
			bSatisfy = true;
	}
	else if ( CheckType == 1 )
	{
		if ( bComebackEvent )
			bSatisfy = true;
	}
	else if ( CheckType == 2 )
	{
		if ( bPremiumCombackEvent )
			bSatisfy = true;
	}

	if( bSatisfy )
	{
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_EventScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG);
		response.setParameter( YOU_ARE_NOT_EVENT_TARGET );
		pPlayer->sendPacket( &response );

		return;
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionCheckComebackEventItem200604::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionCheckComebackEventItem200604("
		<< "EventScriptID" << m_EventScriptID
		<< "EventCheckType" << m_EventCheckType
	    << ")";
	return msg.toString();

	__END_CATCH
}
