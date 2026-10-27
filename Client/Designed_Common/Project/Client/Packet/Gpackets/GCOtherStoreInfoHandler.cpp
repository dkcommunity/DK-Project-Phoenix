//////////////////////////////////////////////////////////////////////
//
// Filename    : GCOtherStoreInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCOtherStoreInfo.h"
#include "MStorage.h"
#include "MPlayer.h"
#include "PCItemInfo.h"
#include "DebugInfo.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "VS_UI.h"
#include "UserOption.h"

//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCOtherStoreInfoHandler::execute ( GCOtherStoreInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	//__BEGIN_DEBUG_EX
	
#ifdef __GAME_CLIENT__



		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());
		if(pCreature == NULL)
			return; 
		if(pCreature->IsNPC() == false)
			pCreature->SetPersnalShop(pPacket->getStoreInfo()->isOpen());
		
		if(pCreature->CurPernalShop() == 1 && pCreature->IsNPC() == false)
		{
			WORD colorIndex = pPacket->getStoreInfo()->getPaintColor();
			pCreature->SetPersnalShopColor( MCreature::s_PersnalShopColor[colorIndex] );

			/* 남의 상점까지도 채팅 색상으로 열고 있다. -_-;;
			if(!pPacket->getStoreInfo()->getSign().empty())
				pCreature->SetPersnalString((char*)pPacket->getStoreInfo()->getSign().c_str(),g_pUserOption->ChattingColor);
			else
				pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
			*/

			// 요청에 따라 디폴트 색으로 열리도록 수정
			if(!pPacket->getStoreInfo()->getSign().empty())
				pCreature->SetPersnalString( (char*)pPacket->getStoreInfo()->getSign().c_str() );
			else
				pCreature->SetPersnalString( (*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString() );
			// by chyaya 2006.04.19
		}

		if(g_pStorage2 != NULL && g_pStorage2->GetCuropenid() !=NULL)
		{
			if(false == pPacket->getStoreInfo()->isOpen() && g_pStorage2->GetCuropenid() == pPacket->getObjectID())
			{
				if(gC_vs_ui.IsRunningPersnalShop())
					gC_vs_ui.ClosePersnalShop();
				return;
			}
		}

		if(strlen(pPacket->getStoreInfo()->getSign().c_str())>0 && pCreature->IsNPC() == false)
			pCreature->SetPersnalShopMessage(pPacket->getStoreInfo()->getSign().c_str());

		if(false == pPacket->isRequested()) // 내가 선택해서 상점이 열리는 경우가 아니고 그냥 열렸다고 알림 일때
			return;


		// 내가 선택해서 개인상점이 열리는 경우 아래쪽 처리
		if (g_pStorage2!=NULL) 
			delete g_pStorage2;

		g_pStorage2 = new MStorage;
		g_pStorage2->Init( 1 ); //STASH_RACK_MAX );	// 쩝.. 3개일까?? 
		g_pStorage2->SetCurrent( 0 );
		pPacket->getStoreInfo()->getSign();
		g_pStorage2->SetCuropenid(pPacket->getObjectID());

		for (int rack=0; rack<1; rack++)
		{
			//------------------------------------------------------------
			// 접근하는 Storage를 지정한다.
			//------------------------------------------------------------
			//int numitem = pPacket->getStoreInfo().getItems().size();
			int numitem = pPacket->getStoreInfo()->getItems().size();
			for (int index=0; index<numitem; index++)
			{
//
				StoreItemInfo&	item = pPacket->getStoreInfo()->getStoreItemInfo(index);
				//if(item.isItemExist() == 45)
				if(item.isItemExist() != 0)
				{
					
					//------------------------------------------------------------
					// item을 생성한다.
					//------------------------------------------------------------
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)item.getItemClass() );
					if (pItem == NULL)
					{
						DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL for class=%d (other-store-main)", (int)item.getItemClass());
						continue;
					}

					pItem->SetID( item.getObjectID() );

					pItem->SetItemType( item.getItemType() );
					pItem->SetItemOptionList( item.getOptionType() );
#if __CONTENTS(__PET_MIXINGFORGE)
					pItem->SetPetMixingOptionList(item.getMixOptionType());
#endif //__PET_MIXINGFORGE
					pItem->SetCurrentDurability( item.getDurability() );
					pItem->SetPersnalPrice(item.getPrice());
					pItem->SetNumber(pPacket->getStoreInfo()->getStoreItemInfo(index).getItemNum());

					pItem->SetItemThirdOptionList(item.getThirdOptionType() );
					pItem->SetItemThirdOptionType(item.getThirdEnchantType());
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
					pItem->SetCashItem(item.getCashItem());
#endif
					//------------------------------------------
					// 개수
					//------------------------------------------
					// 총인 경우
					//------------------------------------------
					if (pItem->IsGunItem())
					{
						MItem* pRawMagazine = MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );
						if (pRawMagazine == NULL)
						{
							DEBUG_ADD("[Error] MItem::NewItem() NULL for ITEM_CLASS_MAGAZINE (other-store)");
							continue;
						}
						// release build mismatch korumasi: NewItem(ITEM_CLASS_MAGAZINE) MMagazine donmeli
						MMagazine* pMagazine = dynamic_cast<MMagazine*>(pRawMagazine);
						if (pMagazine == NULL)
						{
							DEBUG_ADD("[Error] dynamic_cast<MMagazine*> failed (other-store)");
							delete pRawMagazine;
							continue;
						}

						// 의미 없음 - -;
						pMagazine->SetID( 0 );

						// 이거는 총에 맞춰서 해줘야된다.
						for (int j=0; j<(*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); j++)
						{
							pMagazine->SetItemType(	j );

							if (pMagazine->IsInsertToItem( pItem ))
							{
								break;
							}
						}

						// 의미 없음
						pMagazine->ClearItemOption();


						//------------------------------------
						// 탄창 설정
						//------------------------------------
						// release build mismatch korumasi: IsGunItem() true ise MGunItem olmali
						MGunItem* pGunItem = dynamic_cast<MGunItem*>(pItem);
						if (pGunItem == NULL)
						{
							DEBUG_ADD_FORMAT("[Error] dynamic_cast<MGunItem*> failed id=%d (other-store)", (int)pItem->GetID());
							delete pMagazine;
							continue;
						}
						pGunItem->SetMagazine( pMagazine );
					}
					//------------------------------------------
					// 총이 아닌 경우
					//------------------------------------------
					else
					{
						pItem->SetSilver( item.getSilver() );
					}

					pItem->SetGrade( item.getGrade() );
					pItem->SetEnchantLevel( item.getEnchantLevel() );

					//------------------------------------------------------------
					// Sub Item이 있으면 생성한다.
					//------------------------------------------------------------

					int subNum =item.getListNum();
	
					if (subNum!=0)
					{
						//------------------------------------------------------------
						// Belt인 경우
						//------------------------------------------------------------
						if (pItem->GetItemClass()==ITEM_CLASS_BELT)
						{
							// release build mismatch korumasi: ITEM_CLASS_BELT ise MBelt olmali
							MBelt* pBelt = dynamic_cast<MBelt*>(pItem);
							if (pBelt == NULL)
							{
								DEBUG_ADD_FORMAT("[Error] dynamic_cast<MBelt*> failed id=%d (other-store)", (int)pItem->GetID());
							}
							else
							{
							std::list<SubItemInfo*>& listSubItem = item.getSubItems();
							std::list<SubItemInfo*>::const_iterator iItem = listSubItem.begin();

							//------------------------------------------------------------
							// 각각의 sub item을 설정한다.
							//------------------------------------------------------------
							while (iItem != listSubItem.end())
							{
								SubItemInfo* pItemInfo = *iItem;

								if (pItemInfo!=NULL)
								{
									//------------------------------------------------------------
									// sub item을 생성한다.
									//------------------------------------------------------------
									MItem* pSubItem = MItem::NewItem( (ITEM_CLASS)pItemInfo->getItemClass() );
									if (pSubItem == NULL)
									{
										DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL for class=%d (other-store-belt-sub)", (int)pItemInfo->getItemClass());
										iItem++;
										continue;
									}

									pSubItem->SetID( pItemInfo->getObjectID() );

									pSubItem->SetItemType( pItemInfo->getItemType() );
									pSubItem->SetNumber( pItemInfo->getItemNum() );

									if (!pBelt->AddItem( pSubItem, pItemInfo->getSlotID() ))
									{
										delete pSubItem;

										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
									}
								}

								iItem++;
							}
							}
						}
						else if (pItem->GetItemClass()==ITEM_CLASS_OUSTERS_ARMSBAND)
						{
							// release build mismatch korumasi: OUSTERS_ARMSBAND ise MOustersArmsBand olmali
							MOustersArmsBand* pBelt = dynamic_cast<MOustersArmsBand*>(pItem);
							if (pBelt == NULL)
							{
								DEBUG_ADD_FORMAT("[Error] dynamic_cast<MOustersArmsBand*> failed id=%d (other-store)", (int)pItem->GetID());
							}
							else
							{
							std::list<SubItemInfo*>& listSubItem = item.getSubItems();

							std::list<SubItemInfo*>::const_iterator iItem = listSubItem.begin();

							//------------------------------------------------------------
							// 각각의 sub item을 설정한다.
							//------------------------------------------------------------
							while (iItem != listSubItem.end())
							{
								SubItemInfo* pItemInfo = *iItem;

								if (pItemInfo!=NULL)
								{
									//------------------------------------------------------------
									// sub item을 생성한다.
									//------------------------------------------------------------
									MItem* pSubItem = MItem::NewItem( (ITEM_CLASS)pItemInfo->getItemClass() );
									if (pSubItem == NULL)
									{
										DEBUG_ADD_FORMAT("[Error] MItem::NewItem() NULL for class=%d (other-store-armsband-sub)", (int)pItemInfo->getItemClass());
										iItem++;
										continue;
									}

									pSubItem->SetID( pItemInfo->getObjectID() );

									pSubItem->SetItemType( pItemInfo->getItemType() );
									pSubItem->SetNumber( pItemInfo->getItemNum() );

									if (!pBelt->AddItem( pSubItem, pItemInfo->getSlotID() ))
									{
										delete pSubItem;

										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
									}
								}

								iItem++;
							}
							}
						}
						else
						{
							DEBUG_ADD_FORMAT("[Error] This item can't have subitems. itemClass=%d", (int)pItem->GetItemClass());
						}
					}
					
					//------------------------------------------------------------
					// Storage에 item 설정
					//------------------------------------------------------------
					if (!g_pStorage2->SetItem( index, pItem ))
					{
						// 뭐지..
						delete pItem;
						
						DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. rack=%d, slot=%d", rack, index);
					}
			//		}
				}
			}
		}
		
		gC_vs_ui.setinventory_mode(2);
		gC_vs_ui.RunPersnalShop();
		gC_vs_ui.setinventory_mode(2);
		gC_vs_ui.SetPersnalShop( g_pStorage2 ); 		
			 

#endif

	//__END_DEBUG_EX 
	__END_CATCH
}
