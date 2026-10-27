////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRedistributeAttr50.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionRedistributeAttr50.h"
#include "Slayer.h"
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
void ActionRedistributeAttr50::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script type
		string AttrType = propertyBuffer.getProperty("AttrType");

		if ( AttrType == "STR" ) m_AttrType = 0;
		else if ( AttrType == "DEX" ) m_AttrType = 1;
		else if ( AttrType == "INT" ) m_AttrType = 2;
		else
		{
			cout << "ActionRedistributeAttr50::read() : Unknown ATTR type" << endl;
		}
	}
	catch ( NoSuchElementException& nsee )
	{
		cout << nsee.toString() << endl;
		throw Error( nsee.toString() );
	}

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionRedistributeAttr50::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

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
		pResult = pStmt->executeQuery( "SELECT RemainCount FROM RedistributeAttr50Count WHERE Name = '%s'", pPC->getName().c_str() );

		if ( pResult->next() )
		{
			remainCount = pResult->getInt(1);
		}
		else
		{
			remainCount = 50;
			pStmt->executeQuery( "REPLACE INTO RedistributeAttr50Count VALUES ( '%s', '%s', %d, 50 )", pPC->getPlayer()->getID().c_str(), pPC->getName().c_str(), (int)pPC->getRace()+1 );
		}
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	if ( remainCount <= 0 )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
		pPlayer->sendPacket( &gcSystemMessage );

		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &gcNPCResponse );

		return;
	}

	if ( pCreature2->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature2);
		Assert( pSlayer != NULL );

		// 능력치를 변경하기에 앞서 기존의 능력치를 저장한다.
		SLAYER_RECORD prev;
		pSlayer->getSlayerRecord( prev );

		// STR 재분배
		if ( m_AttrType == 0 )
		{
			if ( pSlayer->popAdvancedBonusFromSTR() )
			{
				pSlayer->saveExps();
				pSlayer->initAllStat();
				pSlayer->sendModifyInfo( prev );
				pSlayer->sendRealWearingInfo();
			}
			else
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}
		}
		// DEX 재분배
		else if ( m_AttrType == 1 )
		{
			if ( pSlayer->popAdvancedBonusFromDEX() )
			{
				pSlayer->saveExps();
				pSlayer->initAllStat();
				pSlayer->sendModifyInfo( prev );
				pSlayer->sendRealWearingInfo();
			}
			else
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}
		}
		// INT 재분배
		else if ( m_AttrType == 2 )
		{
			if ( pSlayer->popAdvancedBonusFromINT() )
			{
				pSlayer->saveExps();
				pSlayer->initAllStat();
				pSlayer->sendModifyInfo( prev );
				pSlayer->sendRealWearingInfo();
			}
			else
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}
		}
		else
		{
			Assert(false);
		}
	}
	else if ( pCreature2->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pCreature2);
		Assert( pVampire != NULL );

		// 능력치를 변경하기에 앞서 기존의 능력치를 저장한다.
		VAMPIRE_RECORD prev;
		pVampire->getVampireRecord(prev);

		StringStream sql;

		// STR 재분배
		if ( m_AttrType == 0 )
		{
			// 순수힘이 20이하라면 더 이상 재분배할 수 없다.
			if ( pVampire->getSTR(ATTR_BASIC) <= 20 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pVampire->setSTR(pVampire->getSTR(ATTR_BASIC) - 1, ATTR_BASIC );
			pVampire->setBonus( pVampire->getBonus() + 1 );

			sql << "STR=" << (int)pVampire->getSTR(ATTR_BASIC) << ",Bonus=" << pVampire->getBonus();
		}
		// DEX 재분배
		else if ( m_AttrType == 1 )
		{
			if ( pVampire->getDEX(ATTR_BASIC) <= 20 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pVampire->setDEX(pVampire->getDEX(ATTR_BASIC) - 1, ATTR_BASIC );
			pVampire->setBonus( pVampire->getBonus() + 1 );

			sql << "DEX=" << (int)pVampire->getDEX(ATTR_BASIC) << ",Bonus=" << pVampire->getBonus();
		}
		// INT 재분배
		else if ( m_AttrType == 2 )
		{
			if ( pVampire->getINT(ATTR_BASIC) <= 20 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pVampire->setINT(pVampire->getINT(ATTR_BASIC) - 1, ATTR_BASIC );
			pVampire->setBonus( pVampire->getBonus() + 1 );

			sql << "INTE=" << (int)pVampire->getINT(ATTR_BASIC) << ",Bonus=" << pVampire->getBonus();
		}
		else
		{
			Assert(false);
		}

		pVampire->tinysave( sql.toString() );
		pVampire->initAllStat();
		pVampire->sendModifyInfo( prev );
		pVampire->sendRealWearingInfo();
	}
	else if ( pCreature2->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pCreature2);
		Assert( pOusters != NULL );

		// 능력치를 변경하기에 앞서 기존의 능력치를 저장한다.
		OUSTERS_RECORD prev;
		pOusters->getOustersRecord(prev);

		StringStream sql;

		// STR 재분배
		if ( m_AttrType == 0 )
		{
			// 순수힘이 10이하라면 더 이상 재분배할 수 없다.
			if ( pOusters->getSTR(ATTR_BASIC) <= 10 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pOusters->setSTR(pOusters->getSTR(ATTR_BASIC) - 1, ATTR_BASIC );
			pOusters->setBonus( pOusters->getBonus() + 1 );

			sql << "STR=" << (int)pOusters->getSTR(ATTR_BASIC) << ",Bonus=" << pOusters->getBonus();
		}
		// DEX 재분배
		else if ( m_AttrType == 1 )
		{
			if ( pOusters->getDEX(ATTR_BASIC) <= 10 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pOusters->setDEX(pOusters->getDEX(ATTR_BASIC) - 1, ATTR_BASIC );
			pOusters->setBonus( pOusters->getBonus() + 1 );

			sql << "DEX=" << (int)pOusters->getDEX(ATTR_BASIC) << ",Bonus=" << pOusters->getBonus();
		}
		// INT 재분배
		else if ( m_AttrType == 2 )
		{
			if ( pOusters->getINT(ATTR_BASIC) <= 10 )
			{
				GCSystemMessage gcSystemMessage;
				gcSystemMessage.setMessage(g_pStringPool->c_str(STRID_CANNOT_PROCESS_ANYMORE) ); // 20070814
				pPlayer->sendPacket( &gcSystemMessage );

				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &gcNPCResponse );

				return;
			}

			pOusters->setINT(pOusters->getINT(ATTR_BASIC) - 1, ATTR_BASIC );
			pOusters->setBonus( pOusters->getBonus() + 1 );

			sql << "INTE=" << (int)pOusters->getINT(ATTR_BASIC) << ",Bonus=" << pOusters->getBonus();
		}
		else
		{
			Assert(false);
		}

		pOusters->tinysave( sql.toString() );
		pOusters->initAllStat();
		pOusters->sendModifyInfo( prev );
		pOusters->sendRealWearingInfo();
	}

	GCNPCResponse gcNPCResponse;
	gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer->sendPacket( &gcNPCResponse );

	remainCount--;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "UPDATE RedistributeAttr50Count SET RemainCount = %d WHERE Name = '%s'", remainCount, pPC->getName().c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	char str[80];
	sprintf(str, g_pStringPool->c_str( STRID_CHANGE_OK_REMAIN_POINT ), remainCount);
	//StringStream msg;
	//msg << "정상적으로 전환되었습니다. 전환 가능 포인트가 "
	//	<< remainCount
	//	<< "포인트 남았습니다.";

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setMessage(str);
	pPlayer->sendPacket( &gcSystemMessage );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionRedistributeAttr50::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionRedistributeAttr50("
	    << ")";
	return msg.toString();

	__END_CATCH
}






