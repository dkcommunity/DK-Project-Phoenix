////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRedeemMotorcycle.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionRedeemMotorcycle.h"
#include "Creature.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "ParkingCenter.h"
#include "Item.h"
#include "Zone.h"
#include "Belt.h"
#include "DB.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"

#include "item/Key.h"
#include "Gpackets/GCNPCResponse.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRedeemMotorcycle::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRedeemMotorcycle::execute (Creature * pCreature1 , Creature* pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	Player* pPlayer = pCreature2->getPlayer();
	Assert(pPlayer != NULL);

	// 일단 클라이언트를 위해 ok패킷을 하나 날려주고...
	GCNPCResponse answerOKpkt;
	pPlayer->sendPacket(&answerOKpkt);

	// 플레이어가 슬레이어인지 검사한다.
	if (pCreature2->isSlayer())
	{
		Slayer*    pSlayer     = dynamic_cast<Slayer*>(pCreature2);
		Zone*      pZone       = pSlayer->getZone();
		Inventory* pInventory  = pSlayer->getInventory();
		uint       InvenWidth  = pInventory->getWidth();
		uint       InvenHeight = pInventory->getHeight();
		Item*      pItem       = NULL;

		Inventory* pBeltInventory = NULL;
		uint		BeltInvenWidth = 0;
		uint		BeltInvenHeight = 0;
		Item*      pBelt          = NULL;
		
		pBelt	= pSlayer->getWearItem(Slayer::WEAR_BELT);
		if(pBelt != NULL)
		{
			pBeltInventory = ((Belt*)pBelt)->getInventory();

			BeltInvenWidth = pBeltInventory->getWidth();
			BeltInvenHeight = pBeltInventory->getHeight();
		}
							  
		// 인벤토리를 검색한다.
		for (uint y=0; y<InvenHeight; y++)
		{
			for (uint x=0; x<InvenWidth; x++)
			{
				// x, y에 아이템이 있다면...
				if (pInventory->hasItem(x, y))
				{
					pItem = pInventory->getItem(x, y);
					if (load(pItem, pSlayer, pZone, pSlayer->getX(), pSlayer->getY()))
					{
						return;
					}
				}
			}
		}

		if(pBelt != NULL)
		{
			// 벨트를 검색한다
			for (uint y = 0; y < BeltInvenHeight; y++)
			{
				for(uint x = 0; x < BeltInvenWidth; x++)
				{

					if(pBeltInventory->hasItem(x, y))
					{
						pItem= pBeltInventory->getItem(x, y);
						if (load(pItem, pSlayer, pZone, pSlayer->getX(), pSlayer->getY()))
						{
							return;
						}
					}

				}
			}
		}
	}
	else // 뱀파이어라면...오토바이를 찾아줄 이유가 있을까?
	{
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool ActionRedeemMotorcycle::load(Item* pItem, Slayer* pSlayer, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) const
{
	bool bFound = false;

	__BEGIN_TRY

	// 단서가 되는 아이템이 키가 아니라면 false를 리턴.
	if (pItem->getItemClass() != Item::ITEM_CLASS_KEY) return false;

	Key* pKey = dynamic_cast<Key*>(pItem);
	
	bFound = pSlayer->recallMotorcycle(pKey, false, true);

	__END_CATCH

	return bFound;
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionRedeemMotorcycle::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionRedeemMotorcycle(" << ")";
	return msg.toString();

	__END_CATCH
}
