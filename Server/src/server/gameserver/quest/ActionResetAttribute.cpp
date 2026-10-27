////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionResetAttribute.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionResetAttribute.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Zone.h"
#include "GamePlayer.h"
#include "DB.h"
#include "VariableManager.h"
#include "StringPool.h"
#include <stdio.h>

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSystemMessage.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionResetAttribute::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionResetAttribute::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isVampire() || pCreature2->isOusters());

	Player* pPlayer = pCreature2->getPlayer();
	Assert(pPlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );

	// 먼저 클라이언트를 위해 GCNPCResponse를 보내준다.
	GCNPCResponse okpkt;
	pPlayer->sendPacket(&okpkt);

	int remainCount = 0;

	Statement* pStmt = NULL;
	Result* pResult = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery( "SELECT RemainCount FROM ResetAttributeCount WHERE Name = '%s'", pPC->getName().c_str() );

		if ( pResult->next() )
		{
			remainCount = pResult->getInt(1);
		}
		else
		{
			remainCount = 3;
			pStmt->executeQuery( "REPLACE INTO ResetAttributeCount VALUES ( '%s', '%s', %d, 3 )", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), (int)pPC->getRace()+1 );
		}
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	if ( remainCount <= 0 )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CANNOT_ANYMORE_RESET_ABILITY_POINT) );
		pPlayer->sendPacket( &gcSystemMessage );

		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &gcNPCResponse );

		return;
	}

	if ( pCreature2->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature2);
		Assert( pVampire != NULL );

		// 능력치를 변경하기에 앞서 기존의 능력치를 저장한다.
		VAMPIRE_RECORD prev;
		pVampire->getVampireRecord(prev);

		Attr_t diff = 0;
		diff += pVampire->getSTR( ATTR_BASIC ) - 20;
		diff += pVampire->getDEX( ATTR_BASIC ) - 20;
		diff += pVampire->getINT( ATTR_BASIC ) - 20;

		pVampire->setSTR( 20, ATTR_BASIC );
		pVampire->setDEX( 20, ATTR_BASIC );
		pVampire->setINT( 20, ATTR_BASIC );
		pVampire->setBonus( pVampire->getBonus() + diff );

		StringStream sav;
		sav << "STR=20,DEX=20,INTE=20,Bonus=" << (int)pVampire->getBonus();
		pVampire->tinysave( sav.toString() );

		pVampire->initAllStat();
		pVampire->sendModifyInfo( prev );
	}
	else if ( pCreature2->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature2);
		Assert( pOusters != NULL );

		// 능력치를 변경하기에 앞서 기존의 능력치를 저장한다.
		OUSTERS_RECORD prev;
		pOusters->getOustersRecord(prev);

		Attr_t diff = 0;
		diff += pOusters->getSTR( ATTR_BASIC ) - 15;
		diff += pOusters->getDEX( ATTR_BASIC ) - 15;
		diff += pOusters->getINT( ATTR_BASIC ) - 15;

		pOusters->setSTR( 15, ATTR_BASIC );
		pOusters->setDEX( 15, ATTR_BASIC );
		pOusters->setINT( 15, ATTR_BASIC );
		pOusters->setBonus( pOusters->getBonus() + diff );

		StringStream sav;
		sav << "STR=15,DEX=15,INTE=15,Bonus=" << (int)pOusters->getBonus();
		pOusters->tinysave( sav.toString() );

		pOusters->initAllStat();
		pOusters->sendModifyInfo( prev );
	}

	GCNPCResponse gcNPCResponse;
	gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer->sendPacket( &gcNPCResponse );

	remainCount--;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "UPDATE ResetAttributeCount SET RemainCount = %d WHERE Name = '%s'", remainCount, pPC->getName().c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	char str[80];
	sprintf(str, g_pStringPool->c_str( STRID_RESET_ABILITY_POINT_REMAIN_POINT ), remainCount);
	//StringStream msg;
	//msg << "능력치를 초기화 하였습니다. "
	//	<< remainCount
	//	<< " 회 더 초기화 할 수 있습니다.";

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage(str); // 20070814
	pPlayer->sendPacket( &gcSystemMessage );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionResetAttribute::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionResetAttribute("
	    << ")";
	return msg.toString();

	__END_CATCH
}






