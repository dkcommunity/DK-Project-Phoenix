//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopSoldHandler.cpp
// Written By  : 김성민
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopSold.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShop.h"
#include "MShopShelf.h"
#include "UIFunction.h"


void GCShopSoldHandler::execute ( GCShopSold * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] There is no such Creature. id=%d", pPacket->getObjectID());
		}
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{
			// release build mismatch korumasi
			MNPC* pNPC = dynamic_cast<MNPC*>(pCreature);
			if (pNPC == NULL)
			{
				DEBUG_ADD_FORMAT("[Error] dynamic_cast<MNPC*> failed id=%d", pPacket->getObjectID());
				return;
			}

			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				DEBUG_ADD_FORMAT("[Error] the Creature has NO Shop. creatureID=%d", pPacket->getObjectID());
			}
			else
			{
				int rawShelfType = (int)pPacket->getShopType();
				if (rawShelfType < 0 || rawShelfType >= MShopShelf::MAX_SHELF)
				{
					DEBUG_ADD_FORMAT("[Error] invalid SHELF_TYPE=%d, max=%d", rawShelfType, (int)MShopShelf::MAX_SHELF);
					return;
				}
				MShopShelf* pShopShelf = pShop->GetShelf( (MShopShelf::SHELF_TYPE)rawShelfType );

				if (pShopShelf==NULL)
				{
					DEBUG_ADD_FORMAT("[Error] There is no such Shelf. type=%d", rawShelfType);
				}
				else
				{
					//------------------------------------------------------
					// Normal shelf가 아닌 경우에만 delete한다.
					//------------------------------------------------------
					if (pPacket->getShopType()==SHOP_RACK_SPECIAL)
					{
						MItem* pRemovedItem = pShopShelf->RemoveItem( pPacket->getShopIndex() );

						if (pRemovedItem!=NULL)
						{
							// item정보 제거
							UI_RemoveDescriptor( (void*)pRemovedItem );

							// 팔린 아이템 제거
							delete pRemovedItem;
						}
					}

					// 새로운 버전 설정
					pShopShelf->SetVersion( pPacket->getShopVersion() );									
				}
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] the Creature is not NPC. id=%d", pPacket->getObjectID());
		}
	}

#endif

	__END_CATCH
}
