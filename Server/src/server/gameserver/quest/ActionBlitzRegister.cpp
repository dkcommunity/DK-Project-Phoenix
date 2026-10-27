////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionBlitzRegister.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionBlitzRegister.h"
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
ActionBlitzRegister::ActionBlitzRegister()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionBlitzRegister::~ActionBlitzRegister()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionBlitzRegister::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionBlitzRegister::execute(Creature * pCreature1 , Creature * pCreature2)
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

	ItemType_t		itemType = 67;

	int				 itemNum = 0;

	itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_EVENT_TREE, itemType );

	if(itemNum > 0)
		pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_EVENT_TREE, itemType, itemNum ); // 최대 10개까지 인벤에 넣을 수 있다.

	int BlitzCount = 0;
	int BlitzRemainCount = 0;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT BlitzCount, BlitzRemainCount FROM BlitzEvent WHERE OwnerID = '%s'",
				pPC->getName().c_str() );
		
		
		if(pResult->next())
		{
			BlitzCount = pResult->getInt(1);
			BlitzRemainCount = pResult->getInt(2);
			
			if(itemNum > 0)
			{
				BlitzCount += itemNum;
				BlitzRemainCount += itemNum;
				pStmt->executeQuery("UPDATE BlitzEvent SET BlitzCount = %d, BlitzRemainCount = %d WHERE OwnerID = '%s'",
						BlitzCount, BlitzRemainCount, pPC->getName().c_str());
			}
		}
		else
		{
			if(itemNum > 0)
			{
				BlitzCount += itemNum;
				BlitzRemainCount += itemNum;
				pStmt->executeQuery("INSERT INTO BlitzEvent (BlitzCount, BlitzRemainCount, OwnerID) VALUES (%d, %d, '%s')",
						BlitzCount, BlitzRemainCount, pPC->getName().c_str());
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_BLITZ_COUPON );
	response.setParameter( MAKEDWORD(BlitzCount, BlitzRemainCount) );
	pPlayer->sendPacket( &response );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionBlitzRegister::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionBlitzRegister("
	    << ")";
	return msg.toString();

	__END_CATCH
}
