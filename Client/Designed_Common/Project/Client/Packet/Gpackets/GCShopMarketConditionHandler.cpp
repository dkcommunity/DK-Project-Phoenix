//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopMarketConditionHandler.cpp
// Written By  : 김성민
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopMarketCondition.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShopShelf.h"
#include "MShop.h"
#include "MPriceManager.h"
#include "UIFunction.h"

void GCShopMarketConditionHandler::execute ( GCShopMarketCondition * pPacket , Player * pPlayer )
	 
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
		int objectID = pPacket->getObjectID();
		MCreature* pCreature = g_pZone->GetCreature(objectID);

		//20081023 그로벌 NPC를 위한 부분 npc를 못찾고 글로벌 변수로 왔을경우만 다음과 같이 크리처를 만들어서 쓴다.
		if(pCreature == NULL && objectID == 0)
		{
			(MNPC*) pCreature;
			pCreature = new MNPC;
		}

		//------------------------------------------------------
		// 그런 creature가 없는 경우
		//------------------------------------------------------
		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] There is no such Creature id=%d", pPacket->getObjectID());
		}
		//------------------------------------------------------
		// NPC인 경우
		//------------------------------------------------------
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{
			// release build mismatch korumasi
			MNPC* pNPC = dynamic_cast<MNPC*>(pCreature);
			if (pNPC == NULL)
			{
				DEBUG_ADD_FORMAT("[Error] dynamic_cast<MNPC*> failed id=%d", pPacket->getObjectID());
				return;
			}

			//------------------------------------------------------
			// 상점을 읽는다.
			//------------------------------------------------------
			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				// 상점이 없었으면 생성한다.
				pShop = new MShop;
				pShop->Init( MShopShelf::MAX_SHELF );

				// NPC에 상점 설정
				pNPC->SetShop( pShop );

				// normal item 선반을 생성한다.
				//pNPC->CreateFixedShelf();
			}

			//------------------------------------------------------
			// 선반의 가격 비율 
			//------------------------------------------------------
			g_pPriceManager->SetMarketCondBuy( pPacket->getMarketCondBuy() );
			g_pPriceManager->SetMarketCondSell( pPacket->getMarketCondSell() );
			
			//------------------------------------------------------
			// 팔기 위해서 inventory를 연다.
			//------------------------------------------------------
			UI_OpenInventoryToSell();

		}
		//------------------------------------------------------
		// NPC가 아닌 경우
		//------------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] The Creature is Not NPC. id=%d", pPacket->getObjectID());
		}
	}
#endif

	__END_CATCH
}
