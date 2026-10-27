////////////////////////////////////////////////////////////////////////////////
// Filename    : GCPetStashListHandler.cpp
// Written By  : 김성민
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPetStashList.h"
#include "ClientDef.h"
#include "MStorage.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "UIFunction.h"
#include "ExperienceTable.h"
void GCPetStashListHandler::execute ( GCPetStashList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
//	__BEGIN_DEBUG_EX
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__
		//------------------------------------------------------------
		// 기존에걸 지우고 다시 생성한다.
		//------------------------------------------------------------
		if (g_pStorage!=NULL) 
		{
			delete g_pStorage;
		}

		g_pStorage = new MStorage;

		
		g_pStorage->Init( 1 ); //STASH_RACK_MAX );	// 쩝.. 3개일까??

		//------------------------------------------------------------
		// 접근하는 Storage를 지정한다.
		//------------------------------------------------------------
		g_pStorage->SetCurrent( 0 );

		std::vector<PetStashItemInfo*> vPetInfo = pPacket->getPetStashItemInfos();

		std::vector<PetStashItemInfo*>::const_iterator		itr;

		itr = vPetInfo.begin();
		PetInfo* pPetInfo;
		PetStashItemInfo* petStashItemInfo;
		int index = 0;
		while(itr != vPetInfo.end() && index < 20)
		{
			petStashItemInfo = (PetStashItemInfo*)*itr;
			if(NULL == petStashItemInfo)
			{
				itr ++;
				index ++;
				continue;
			}
			pPetInfo = petStashItemInfo->pPetInfo;
			if(NULL == pPetInfo)
			{
				itr ++;
				index ++;
				continue;
			}
			MPetItem *pItem = NULL;
			pItem = dynamic_cast<MPetItem *>(MItem::NewItem( ITEM_CLASS_PET_ITEM ));
			if (pItem == NULL)
			{
				DEBUG_ADD_FORMAT("[Error] MItem::NewItem(ITEM_CLASS_PET_ITEM) NULL or cast failed. index=%d", index);
				itr ++;
				index ++;
				continue;
			}
			pItem->SetItemType( pPetInfo->getPetType() );
			pItem->SetCurrentDurability( pPetInfo->getPetHP() );
			pItem->SetEnchantLevel( pPetInfo->getPetAttr() );
			pItem->SetSilver( pPetInfo->getPetAttrLevel() );
			pItem->SetNumber( pPetInfo->getPetLevel() );
			pItem->SetPetFoodType( pPetInfo->getFoodType() );
			pItem->SetPetCreatureType( pPetInfo->getPetCreatureType() );
#if __CONTENTS(__FUNCTION_PETFOOD)
			pItem->setFunctionPetfoodType(pPetInfo->getFunctionPetfoodType());
#endif //__FUNCTION_PETFOOD
			int accum = 0;
			accum = g_pExperienceTable->GetPetExp(pPetInfo->getPetLevel()).AccumExp;
			pItem->SetPetExpRemain(accum-pPetInfo->getPetExp());
			pItem->SetPetCutHead( pPetInfo->canCutHead()?true:false );
			pItem->SetPetGamble( pPetInfo->canGamble()?true:false );
			pItem->SetPetAttack( pPetInfo->canAttack()?true:false );
			pItem->SetEnchantSkillType( pPetInfo->getEnchantSkillType() );
#if __CONTENTS(__PET_MIXINGFORGE)
			pItem->SetPetMixingOptionList(pPetInfo->getMixOptionType());
#endif //__PET_MIXINGFORGE
			
			pItem->ClearItemOption();
			if(pPetInfo->getPetOption() != NULL)
			{
#if __CONTENTS(__PET_VISION_AMPLE)
				pItem->SetPet_RedBirdOptionNum(pPetInfo->getPetOption());
#endif //SetPet_RedBirdOptionNum
				pItem->AddItemOption(pPetInfo->getPetOption());
			}

#if __CONTENTS(__PET_VISION_AMPLE)
			if(pPetInfo->getPetOption2() != NULL)
			{
				pItem->SetPet_AmpulOptionNum(pPetInfo->getPetOption2());
				pItem->AddItemOption(pPetInfo->getPetOption2());
			}
			if(pPetInfo->getPetOption3() != NULL)
			{
				pItem->SetPet_op3(pPetInfo->getPetOption3());
				pItem->AddItemOption(pPetInfo->getPetOption3());
			}
#endif //__PET_VISION_AMPLE

			pItem->SetID( pPetInfo->getPetItemObjectID());
			pItem->SetItemType( pPetInfo->getPetType() );
			
			pItem->SetPetKeepedDay(petStashItemInfo->KeepDays);
			//------------------------------------------------------------
			// Storage에 item 설정
			//------------------------------------------------------------
			if (!g_pStorage->SetItem( index, pItem ))
			{
				// 뭐지..
				delete pItem;

				DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. slot=%d", index);
			}

			itr ++;
			index ++;
		}

		//------------------------------------------------------------
		// 보관함을 띄운다.
		//------------------------------------------------------------
		UI_RunPetStorage();
		UI_SetPetStorage(g_pStorage);
#endif

	__END_DEBUG
//	__END_DEBUG_EX 
	__END_CATCH
}
