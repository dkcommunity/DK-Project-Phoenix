////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionMapleRegister.cpp
// Written By  : smallheart
// Description : 2008년 가을(10월) 이벤트
////////////////////////////////////////////////////////////////////////////////
#include "ActionMapleRegister.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "Player.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "Zone.h"
#include "VariableManager.h"
#include "FlagSet.h"
#include "DB.h"
#include "SkillUtil.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
// ActionBlitzRegister
////////////////////////////////////////////////////////////////////////////////
ActionMapleRegister::ActionMapleRegister()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionMapleRegister::~ActionMapleRegister()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionMapleRegister::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionMapleRegister::execute(Creature * pCreature1 , Creature * pCreature2)
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

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	ItemType_t		itemType = 80;	//메이플 문양 아이템타입

	int				 itemNum = 0;

	itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_EVENT_TREE, itemType );

	if(itemNum > 0)
		pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_EVENT_TREE, itemType, itemNum );

	int MapleCount = 0;
	int MapleRemainCount = 0;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT MapleCount, MapleRemainCount FROM MapleEvent WHERE OwnerID = '%s'",
				pPC->getName().c_str() );
		
		
		if(pResult->next())
		{
			MapleCount = pResult->getInt(1);
			MapleRemainCount = pResult->getInt(2);
			
			if(itemNum > 0)
			{
				MapleCount += itemNum;
				MapleRemainCount += itemNum;
				pStmt->executeQuery("UPDATE MapleEvent SET MapleCount = %d, MapleRemainCount = %d WHERE OwnerID = '%s'",
						MapleCount, MapleRemainCount, pPC->getName().c_str());
			}
		}
		else
		{
			if(itemNum > 0)
			{
				MapleCount += itemNum;
				MapleRemainCount += itemNum;
				pStmt->executeQuery("INSERT INTO MapleEvent (MapleCount, MapleRemainCount, OwnerID) VALUES (%d, %d, '%s')",
						MapleCount, MapleRemainCount, pPC->getName().c_str());
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_BLITZ_COUPON );	//이 경우의 NPC응답 코드는 NPC_RESPONSE_BLITZ_COUPON 를 공통으로 사용한다.
	response.setParameter( MAKEDWORD(MapleCount, MapleRemainCount) );
	pPlayer->sendPacket( &response );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionMapleRegister::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionMapleRegister("
	    << ")";
	return msg.toString();

	__END_CATCH
}
