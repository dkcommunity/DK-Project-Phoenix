//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddItemToItemHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

// ?? ???????? ??? ??? ??????? ??ùù?.
/* [Client]                                               [Server]
           -- CGAddItemToItem
              (mouseItem_ObjectID, invenX, invenY) 
			  =(???ùJ?? ?????? ??, upgrade?? item XY) -->

            <-- GCAddItemToItemVerify(??????, ???ùù?) ---
*/   
//-------------+---------------------------------------------
// ??? packet | client ???
//-------------+---------------------------------------------
// ??? packet | ??? message ???
// ???? packet | ???? message ???, ?? ????, [?????? ????]
// ???? packet | ?? ????, option????
//-------------+---------------------------------------------



#include "CGAddItemToItem.h"

#ifdef __GAME_SERVER__
	#include <stdio.h>
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Inventory.h"
	#include "Item.h"
	#include "ItemInfoManager.h"
	#include "ItemFactoryManager.h"
	#include "OptionInfo.h"
	#include "ItemUtil.h"
	#include "EventStar.h"
	#include "PetItem.h"
	#include "PetEnchantItem.h"
	#include "PetInfo.h"
	#include "PetAttrInfo.h"
	#include "PetTypeInfo.h"
	#include "CreatureUtil.h"
	#include "MixingItem.h"
	#include "SlayerTunningItem.h" //20080604
	#include "VampireTunningItem.h" //20080604
	#include "OustersTunningItem.h" //20080604
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"

	#include "PacketUtil.h"
	#include "Utility.h"
	#include "DB.h"
	#include "Zone.h"
	#include "EventGiftBoxRewardManager.h"
	#include "EventGiftBox.h"

	#include "Gpackets/GCAddItemToItemVerify.h"
	#include "Gpackets/GCDeleteInventoryItem.h"
	#include "Gpackets/GCCreateItem.h"
	#include "Gpackets/GCPetInfo.h"
	#include "Gpackets/GCSystemMessage.h"
		#include "ZoneGroup.h"
	#include "ZoneGroupManager.h"
	#include "ZoneInfo.h"
	#include "ZoneInfoManager.h"
	#include "ZonePlayerManager.h"
	#include "SystemAvailabilitiesManager.h"
#ifdef VERSION_THIRD_ENCHANT_1
	#include "ItemClassInfo.h"
	#include "ThirdEnchantInfo.h"
	void executeThirdEnchantOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType) throw(Error);
	void sendThirdEnchantOK(GamePlayer* pGamePlayer, list<OptionType_t>& changeOption, BYTE enchantClass)  throw (Error);
	void sendThirdEnchantFail(GamePlayer* pGamePlayer, BYTE grade) throw (Error);
	void saveThirdOptionType(Item* pItem) throw (Error);
#endif

	void executeUpGrade(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void yellowDropLog(PlayerCreature* pPC, Item* pItem, int ratio, int success) throw(Error);
	void executeEnchantOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY) throw(Error);
	static bool isCommonPetInjectionBasePet(const PetItem* pPetItem, const PetInfo* pPetInfo)
		throw()
	{
		if (pPetItem == NULL || pPetInfo == NULL)
		{
			return false;
		}

		const ItemType_t baseItemType = pPetItem->getItemType();
		const PetType_t runtimePetType = pPetInfo->getPetType();

		if (runtimePetType == PET_COMMON)
		{
			return true;
		}

		// Phoenix Gara Bezz ItemType 0 runtime tarafinda Mashimaro olarak tutulur.
		return baseItemType == GaraBezzPetItemType
			&& runtimePetType == getRuntimePetTypeForItem(GaraBezzPetItemType);
	}

	static bool getEmptySlotAfterRemovingBasePet(Inventory* pInventory, Item* pNewItem, Item* pBaseItem, _TPOINT& pt)
		throw()
	{
		if (pInventory == NULL || pNewItem == NULL || pBaseItem == NULL)
		{
			return false;
		}

		const int itemWidth = pNewItem->getVolumeWidth();
		const int itemHeight = pNewItem->getVolumeHeight();
		const int xLimit = pInventory->getWidth() - itemWidth;
		const int yLimit = pInventory->getHeight() - itemHeight;

		for (int x = 0; x <= xLimit; ++x)
		{
			for (int y = 0; y <= yLimit; ++y)
			{
				bool canPlace = true;
				for (int ix = x; canPlace && ix < x + itemWidth; ++ix)
				{
					for (int iy = y; canPlace && iy < y + itemHeight; ++iy)
					{
						Item* pSlotItem = pInventory->getInventorySlot(ix, iy).getItem();
						if (pSlotItem != NULL && pSlotItem != pBaseItem)
						{
							canPlace = false;
						}
					}
				}

				if (canPlace)
				{
					pt.x = x;
					pt.y = y;
					return true;
				}
			}
		}

		return false;
	}

	void executeEnchantRareOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY) throw(Error);
	void executeEnchantRareOptionTwo(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY) throw(Error);
	void executeAddOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType) throw(Error);
	void executeTransKit(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	// 20070725 ??????
	void executeAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType) throw(Error);
	// 20080218 by Kim Sung Yong
	void executeCrystalAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeOneOpCrystalAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	// Blue Crystal (EventStar 81)
	void executeBlueCrystalAddSecondOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeBlueCrystalRerollTwoOptions(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	//20080604
	void executeTunningItem(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);

	extern bool isPossibleOptionItemClass(Item::ItemClass IClass);
	
	void executeAddHiddenOption2(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem,  OptionType_t addOptionType2) throw(Error);
	void executeHiddenEnchantOption2(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY) throw(Error);

	
	OptionInfo* getOptionInfo(OptionType_t optionType);

	void    	processUpgradeOptionType(Item* pItem, OptionType_t currentOptionType, OptionInfo* pOptionInfo) throw (Error);
	void    	processUpgradeOptionType2(Item* pItem, OptionType_t currentOptionType, OptionInfo* pOptionInfo) throw (Error);
	void    	processUpgradeOptionType3(Item* pItem, OptionType_t currentOptionType, OptionInfo* pOptionInfo) throw (Error);
	void		crashItem(Item* pItem, Inventory* pInventory, CoordInven_t invenX, CoordInven_t invenY) throw (Error);
	void    	downgradeOptionType(Item* pItem, OptionType_t currentOptionType, OptionInfo* pOptionInfo) throw (Error);

	void		sendEnchantImpossible(GamePlayer* pGamePlayer) throw (Error);
	void		sendEnchantOK(GamePlayer* pGamePlayer, DWORD optionChange) throw (Error);


	// Ordinary Crystal (EventStar 68)
	void executeOrdinaryCrystalAddThirdOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeOrdinaryCrystalRerollThirdOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeNewTunning(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeUpGradeCoreZap(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeHappyNewYear(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);

	void executeGaiZaoAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);
	void executeOneGaiZaoAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error);

	// Protocol ERROR: ta? t?ketilmez. Extra slot'taki leftover mouse item
	// envantere yazùùlmazsa extra kilitlenir ve logout'ta ?antadan kaybolur.
	static void sendAddItemToItemProtocolError(GamePlayer* pGamePlayer) throw()
	{
		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
		if (pPC != NULL)
		{
			pPC->restoreExtraInventorySlotToInventory(true);
		}
	}
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAddItemToItemHandler::execute (CGAddItemToItem* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

//	cout << pPacket->toString().c_str() << endl;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	Assert(pGamePlayer != NULL);	// by sigi

	if (pGamePlayer->getPlayerStatus() == GPS_NORMAL) 
	{
		SYSTEM_ASSERT( SYSTEM_ENCHANT );

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
		ObjectID_t      objectID    = pPacket->getObjectID();
		Inventory* 		pInventory  = pPC->getInventory();
		CoordInven_t 	invenX      = pPacket->getX();
		CoordInven_t 	invenY      = pPacket->getY();

		// ?ùù????? ???????? ??????(??????? ??? ???.)
		if (!pInventory->hasItem(invenX, invenY))
		{
			sendAddItemToItemProtocolError(pGamePlayer);
			return;
		}

		// Extra slot = server-side mouse. Packet OID client mouse ID'sidir;
		// split sonrasi OID sapmasi enchant'i ERROR yapmamali.
		Item*           pMouseItem  = pPC->getExtraInventorySlotItem();

		Item*      		pItem       = pInventory->getItem(invenX, invenY);

		if (pMouseItem==NULL || pItem==NULL)
		{
			sendAddItemToItemProtocolError(pGamePlayer);
			return;
		}

		if (pMouseItem->getNum() != 1)
		{
			// Stack ile enchant yapilamaz; extra tuketilmedigi icin cantaya yaz.
			sendAddItemToItemProtocolError(pGamePlayer);
			return;
		}

		if (pMouseItem->getObjectID() != objectID)
		{
			filelog("EventStarExtraSlot.log",
				"CGAddItemToItem OID mismatch ignored: packetOID=%u extraOID=%u extraNum=%d extraClass=%d name=%s",
				objectID, pMouseItem->getObjectID(), (int)pMouseItem->getNum(),
				(int)pMouseItem->getItemClass(), pPC->getName().c_str());
		}


		switch (pMouseItem->getItemClass())
		{
			//---------------------------------------------------------
			//
			// 				ITEM_CLASS_EVENT_STAR
			//
			//---------------------------------------------------------
			// ???????? ????? upgrade ?????.
			//---------------------------------------------------------
			case Item::ITEM_CLASS_EVENT_STAR : 
			{
				ItemInfo* pItemInfo = g_pEventStarInfoManager->getItemInfo( pMouseItem->getItemType() );
				Assert(pItemInfo!=NULL);

				EventStarInfo* pEventStarInfo = dynamic_cast<EventStarInfo*>(pItemInfo);
				Assert(pEventStarInfo!=NULL);

				// 20071003 International Version ?????? ?????? ??????? ??ùù??? ???????.
/*				if( pPC->isSlayer() )
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
					Assert( pSlayer != NULL );

					if( !pSlayer->isRealWearing( pItem ) )
					{
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
				}
				else if( pPC->isVampire() )
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
					Assert( pVampire != NULL );

					if( !pVampire->isRealWearing( pItem ) )
					{
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
				}
				else if( pPC->isOusters() )
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
					Assert( pOusters != NULL );

					if( !pOusters->isRealWearing( pItem ) )
					{
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
				}*/

				int optionSize = pItem->getOptionTypeSize();
//				cout << "Third ??? ??? pEventStarInfo->getFunctionFlag() : " << (int)pEventStarInfo->getFunctionFlag() << endl;
//				cout << "Third ??? ??? pEventStarInfo->isFunctionThirdEnchantOption() : " << (int)pEventStarInfo->isFunctionThirdEnchantOption() << endl;

				if ( (pEventStarInfo->getFunctionFlag() & EventStarInfo::FUNCTION_UP_GRADE) != 0 )
				{
					// 2007 03 05
					if( pMouseItem->getItemType() == 91 && pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP && pMouseItem->getNum() == 1
						&& pItem->getGrade() < 50)
					{
						executeUpGradeCoreZap(pGamePlayer, pMouseItem, pItem);
						return;
					}
					if( pMouseItem->getItemType() == 48 && pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP && pMouseItem->getNum() == 1
						&& pItem->getOptionTypeSize() < 2 && pItemInfo->getReqAdvancedLevel() == 0 && pItem->getGrade() < 8 && pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP
						&& pItem->getItemClass() != Item::ITEM_CLASS_ETHEREAL_CHAIN )
					{
						executeUpGrade(pGamePlayer, pMouseItem, pItem);
						return;
					}
					//2007.11.20 kim sung yong :: yellow drop 3
					if( pMouseItem->getItemType() == 57 && pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP && pMouseItem->getNum() == 1
						&& pItem->getOptionTypeSize() < 3 && pItemInfo->getReqAdvancedLevel() == 0 && pItem->getGrade() < 10 && pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP
						&& pItem->getItemClass() != Item::ITEM_CLASS_ETHEREAL_CHAIN )
					{
						executeUpGrade(pGamePlayer, pMouseItem, pItem);
						return;
					}

					if ( pItem->getOptionTypeSize() > 1 || pItem->getGrade() < 1 || pItem->getGrade() > 5 || 
						pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP || pMouseItem->getNum() != 1 || 
						pItemInfo->getReqAdvancedLevel() > 0 )
					{
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
					executeUpGrade(pGamePlayer, pMouseItem, pItem);
				}
				else if (pEventStarInfo->isFunctionTransKit())
				{
					if ( pEventStarInfo->getItemType() == 87 ||  pEventStarInfo->getItemType() == 88 ||  pEventStarInfo->getItemType() == 89||  pEventStarInfo->getItemType() == 90||  pEventStarInfo->getItemType() == 92)
					{
						executeNewTunning(pGamePlayer, pMouseItem, pItem);
					}
					else if( pEventStarInfo->getItemType() == 93 ||  pEventStarInfo->getItemType() == 94)
					{
						executeHappyNewYear(pGamePlayer, pMouseItem, pItem);
					}
					else
						executeTransKit(pGamePlayer, pMouseItem, pItem);
				}
								else if (pMouseItem->getItemType()==117)
				{
					if(pItem->getOptionTypeSize() == 0)
					{
						int addOptionType = pEventStarInfo->getFunctionValue();
						//executeAddHiddenOption2(pGamePlayer, pMouseItem, pItem, addOptionType);
					}
					else
					{
						//executeHiddenEnchantOption2(pGamePlayer, pMouseItem, pItem, invenX, invenY);
					}
				}
				
				// Enchant Option
				else if (optionSize>1 && pEventStarInfo->getItemType() != 95)	// by sigi. 2002.10.21
				{
					//if (pEventStarInfo->isFunctionEnchantRareOption())
                                         if ( pEventStarInfo->getItemType() == 12)
					{
						executeEnchantRareOption(pGamePlayer, pMouseItem, pItem, invenX, invenY);
					}
                                     //   else if (pEventStarInfo->isFunctionEnchantRareOption2())
                                         else if (pEventStarInfo->getItemType() == 82)
					{
						executeEnchantRareOptionTwo(pGamePlayer, pMouseItem, pItem, invenX, invenY);
					}
					else if(pItem->getOptionTypeSize() == 2 && pEventStarInfo->getItemType() == 81)
					{
						executeBlueCrystalRerollTwoOptions(pGamePlayer, pMouseItem, pItem);
					}
					else if(pEventStarInfo->getItemType() == 68 && pItem->getOptionTypeSize() == 3)
					{
						executeOrdinaryCrystalRerollThirdOption(pGamePlayer, pMouseItem, pItem);
					}
					else if(pEventStarInfo->getItemType() == 68 && pItem->getOptionTypeSize() == 2)
					{
						executeOrdinaryCrystalAddThirdOption(pGamePlayer, pMouseItem, pItem);
					}

#ifdef VERSION_THIRD_ENCHANT_1
					// 20071010 ThirdEnchat ???
					else if (pEventStarInfo->isFunctionThirdEnchantOption() && pEventStarInfo->getItemType() != 81)
					{
						int addOptionType = pEventStarInfo->getFunctionValue();
//						cout << "Third ??? ??? addOptionType : " << addOptionType << endl;

						executeThirdEnchantOption(pGamePlayer, pMouseItem, pItem, addOptionType);
					}
#endif			
					else
					{
						//cout<<"//////////////////////here can't/////////////////////////////"<<endl;
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
				}
				else if (pEventStarInfo->getItemType() == 81 && pItem->getOptionTypeSize() == 1)
				{
					executeBlueCrystalAddSecondOption(pGamePlayer, pMouseItem, pItem);
				}
				else if (pEventStarInfo->isFunctionEnchantOption())
				{
					executeEnchantOption(pGamePlayer, pMouseItem, pItem, invenX, invenY);
				}
				// Add Option
				else if (pEventStarInfo->isFunctionAddOption())
				{
					int addOptionType = pEventStarInfo->getFunctionValue();
					executeAddOption(pGamePlayer, pMouseItem, pItem, addOptionType);
				}
				// 20070725 ??? ???
				else if (pEventStarInfo->isFunctionAddRandomOption())
				{
					int addOptionType = pEventStarInfo->getFunctionValue();
			 	    // 20080218 by Kim Sung Yong
					if(pEventStarInfo->getItemType() == 58)
					{
						if(pItem->getOptionTypeSize() == 0)
						{
							executeCrystalAddRandomOption(pGamePlayer, pMouseItem, pItem);
						}
						else if(pItem->getOptionTypeSize() == 1)
						{
							executeOneOpCrystalAddRandomOption(pGamePlayer, pMouseItem, pItem);
						}
                                        
					} 
					else if(pEventStarInfo->getItemType() == 95 )
					{
						if(pItem->getGTwoOptionTypeSize() == 0)
						{
							executeGaiZaoAddRandomOption(pGamePlayer, pMouseItem, pItem);
						}
						else if(pItem->getGTwoOptionTypeSize() == 1)
						{
							executeOneGaiZaoAddRandomOption(pGamePlayer, pMouseItem, pItem);
						}
                                        
					} 
					else
					{
						executeAddRandomOption(pGamePlayer, pMouseItem, pItem, addOptionType);
					}
				}
			}
			break;

			case Item::ITEM_CLASS_PET_ENCHANT_ITEM :
			{
				//cout << "Trying Pet Enchant... " << endl;
				if ( pItem->getItemClass() != Item::ITEM_CLASS_PET_ITEM )
				{
					//cout << "?? ?????????? ??? ????." << endl;
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				PetEnchantItemInfo* pItemInfo = dynamic_cast<PetEnchantItemInfo*>(g_pPetEnchantItemInfoManager->getItemInfo( pMouseItem->getItemType() ));
				Assert( pItemInfo != NULL );

				PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
				PetInfo* pPetInfo = NULL;
				if ( pPetItem == NULL || ( pPetInfo = pPetItem->getPetInfo() ) == NULL )
				{
					//cout << "?? ???????? ????? ?? ??????? ?? ???ùù? ???????." << endl;
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				/*
					ENCHANT_FUNCTION = 0,
					SPEC_OPTION_FUNCTION = 1,
					REVIVAL_FUNCTION = 13,
					RARE_FUNCTION = 14
				*/
				switch ( pItemInfo->getFunction() )
				{
					case PetEnchantItemInfo::ENCHANT_FUNCTION :
					{
						if ( pPetInfo->getPetAttr() != 0xff )
						{
							//cout << "??? ??ùù??? ??????????." << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						//cout << "??? ??ùù? ?????????." << endl;
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						if ( PetAttrInfoManager::Instance().enchantRandomAttr( pPetInfo, pItemInfo->getFunctionGrade() ) )
						{
							//cout << "??ùù? ????." << endl;
							pPetItem->savePetInfo();

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
							gcAddItemToItemVerify.setParameter( ((uint)pPetInfo->getPetAttr())<<16 | (uint)pPetInfo->getPetAttrLevel() );

							pGamePlayer->sendPacket( &gcAddItemToItemVerify );

							if ( pPC->getPetInfo() == pPetInfo )
							{
								//cout << "??? ???????? ??? ?????? ??????." << endl;
								sendPetInfo( pGamePlayer, true );
								pPC->initAllStatAndSend();
							}
							//1210 wlzzi - ?ùù???? ???
							//	- ??ùù? ???? ??, ?? ????? ???? ?? ????????? ???? ??????ùù? ??? ?? ?????? ??? ???.
							//	- ???????~ ?????? ?? ????? ???? UI??ùù? ?????????? ??????.
							//	- ?? ??????~ ?? AddItemToItemVerify ????? ???? ?????  ??.. ????? ?? ?????? ?? ????????? ??????? ????
							//	---> ??? ????... AddItemToItemVerify??????.. ???? ??ùù ????~~ ????..?? ??????? ?????? ???? ???????..
							//		????.. ????????? ???? ????.. GCPetInfo?? ?????? ???.
							else
							{
								GCPetInfo gcPetInfo;
								gcPetInfo.setPetInfo( pPetInfo );
								gcPetInfo.setSummonInfo( 0 );
								gcPetInfo.setObjectID( pPC->getObjectID() );
								pGamePlayer->sendPacket( &gcPetInfo );
							}
							return;
						}
						else
						{
							//cout << "??ùù? ????" << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
					}
					break;
//1204 wlzzi - ??? ??? ??ùù?
					case PetEnchantItemInfo::ADDED_OPTION_FUNCTION:
						// ??ùù? ??? ??????.. ?????? ???? ????? .. ??ùù? ???. -- ?????????? ????.
						// ??ùù? ??? ???????? ??? ??? ?????????, ??ùù???? ????????? ?????? ??????? ??????? ???.
						//   --> ?ùù? ???? ?? ??? ???? ?????? : ??????? ?????????? .. ???????,
						//		????, ???? ??ùù? ???????? ????? ??~ ????ùù??? ??????~ ????????? ???.
					{
						int AddedOptionClass = pItemInfo->getFunctionGrade();
						int value = rand()%100;
						OptionType_t rOptionType;
						switch (AddedOptionClass)
						{
						case 18:	//Vision
							if (value < 60 )
								rOptionType = 77;
							else if (value < 90)
								rOptionType = 161;
							else
								rOptionType = 162;
							break;
						default:
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );
						
						pPetInfo->setPetOption2( rOptionType );
						
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( rOptionType );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						char query[100];
						sprintf(query, "PetOption2=%u", (uint)rOptionType);
						pItem->tinysave(query);

						if ( pPC->getPetInfo() == pPetInfo )
						{
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						else
						{
//							sendPetInfo( pGamePlayer);
							GCPetInfo gcPetInfo;
							gcPetInfo.setPetInfo( pPetInfo );
							gcPetInfo.setSummonInfo( 0 );
							gcPetInfo.setObjectID( pPC->getObjectID() );
							pGamePlayer->sendPacket( &gcPetInfo );
						}
					}
					break;
					case PetEnchantItemInfo::ADDED_OPTION_FUNCTION_PET3:
						// ??ùù? ??? ??????.. ?????? ???? ????? .. ??ùù? ???. -- ?????????? ????.
						// ??ùù? ??? ???????? ??? ??? ?????????, ??ùù???? ????????? ?????? ??????? ??????? ???.
						//   --> ?ùù? ???? ?? ??? ???? ?????? : ??????? ?????????? .. ???????,
						//		????, ???? ??ùù? ???????? ????? ??~ ????ùù??? ??????~ ????????? ???.
					{
						int AddedOptionClass = pItemInfo->getFunctionGrade();
						int value = rand()%100;
						OptionType_t petnewop;
						switch (AddedOptionClass)
						{
						case 100:	//Vision
							if (value < 90 )
								petnewop = 185;//arr 3
							if (value < 80)
								petnewop = 205;//4
							if (value < 70)
								petnewop = 206;//5
							if (value < 30)
								petnewop = 207;//6
							if (value < 10)
								petnewop = 217;//7
							if (value < 8)
								petnewop = 218;//8
							if (value < 5)
								petnewop = 219;//9
                                                        if (value < 2)
								petnewop = 220;//10
							break;
						default:
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );
						
						pPetInfo->setPetOption3( petnewop );
						
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( petnewop );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						char query[100];
						sprintf(query, "PetOption3=%u", (uint)petnewop);
						pItem->tinysave(query);

						if ( pPC->getPetInfo() == pPetInfo )
						{
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						else
						{
//							sendPetInfo( pGamePlayer);
							GCPetInfo gcPetInfo;
							gcPetInfo.setPetInfo( pPetInfo );
							gcPetInfo.setSummonInfo( 0 );
							gcPetInfo.setObjectID( pPC->getObjectID() );
							pGamePlayer->sendPacket( &gcPetInfo );
						}
					}
					break;
//-
					case PetEnchantItemInfo::SPEC_OPTION_FUNCTION :
					{
						if ( pPetInfo->getPetAttr() != 0xff )
						{
							//cout << "??? ??ùù??? ??????????." << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						if ( PetAttrInfoManager::Instance().enchantSpecAttr( pPetInfo, (PetAttr_t)pItemInfo->getFunctionGrade() ) )
						{
							pPetItem->savePetInfo();
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
							gcAddItemToItemVerify.setParameter( ((uint)pPetInfo->getPetAttr())<<16 | (uint)pPetInfo->getPetAttrLevel() );
//							gcAddItemToItemVerify.setParameter( pPetInfo->getPetAttr() );

							pGamePlayer->sendPacket( &gcAddItemToItemVerify );

							if ( pPC->getPetInfo() == pPetInfo )
							{
								sendPetInfo( pGamePlayer, true );
								pPC->initAllStatAndSend();
							}
							//1210 wlzzi - ?ùù???? ???
							//	- ??ùù? ???? ??, ?? ????? ???? ?? ????????? ???? ??????ùù? ??? ?? ?????? ??? ???.
							//	- ???????~ ?????? ?? ????? ???? UI??ùù? ?????????? ??????.
							//	- ?? ??????~ ?? AddItemToItemVerify ????? ???? ?????  ??.. ????? ?? ?????? ?? ????????? ??????? ????
							//	---> ??? ????... AddItemToItemVerify??????.. ???? ??ùù ????~~ ????..?? ??????? ?????? ???? ???????..
							//		????.. ????????? ???? ????.. GCPetInfo?? ?????? ???.
							else
							{
								GCPetInfo gcPetInfo;
								gcPetInfo.setPetInfo( pPetInfo );
								gcPetInfo.setSummonInfo( 0 );
								gcPetInfo.setObjectID( pPC->getObjectID() );
								pGamePlayer->sendPacket( &gcPetInfo );
							}
							return;
						}
						else
						{
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
					}
					break;

					case PetEnchantItemInfo::RARE_FUNCTION:
					{
						//const list<PetEnchantOption*>& petEnchantOptions = g_pOptionInfoManager->getPetEnchantOptionList();
						//cout << "?? ????? ??? " << petEnchantOptions.size() << endl;
						//cout << "?? ???? ??ùù? ??????." << endl;
						if ( (pMouseItem->getItemType() == 15 && (pPetInfo->getPetLevel() != 49 || pPetInfo->getPetOption() != 0) )
							|| (pMouseItem->getItemType() == 26 && pPetInfo->getPetLevel() < 49)
							// 20070719 ??? ???????2 ????? ???????.
	//						&& ( pMouseItem->getItemType() != 26 && pPetInfo->getPetLevel() >= 49 )
							// 2007 04 10 ????ùù?... ??? ?????????... ???
							//&& petEnchantOptions.size() > 1 
							)
						{
							//cout << "?????? ??? ?? ??ùù????? ????" << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							//							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_LEVEL_FAIL ); // 20070807 ???? ?ùù???? ????? ???? ??????? ???? ??? ???
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						/*
						OptionType_t targetOption = rand()%87;
						targetOption++;
						*/

						OptionType_t targetOption = 0;
						OptionType_t targetOption2 = 0;
						OptionType_t targetOption3 = 0;
						int ratio = rand() % 100000;
						int ratio2 = rand() % 100000;
						int ratio3 = rand() % 100000;
						if( pMouseItem->getItemType() == 15 )
						{
							const list<PetEnchantOption*>& petEnchantOptions = g_pOptionInfoManager->getPetEnchantOptionList();

							list<PetEnchantOption*>::const_iterator itr = petEnchantOptions.begin();
							list<PetEnchantOption*>::const_iterator endItr = petEnchantOptions.end();

							for ( ; itr != endItr ; itr++ )
							{
								PetEnchantOption* pPetEnchantOption = *itr;
								if ( pPetEnchantOption->getRatio() > ratio )
								{
									targetOption = pPetEnchantOption->getOptionType();
									break;
								}
							}
						}
						else if( pMouseItem->getItemType() == 26 )
						{


							/*if(pPetInfo->getOptionTypeSize() > 0)
							{
								
						char query[100];
						sprintf(query, "OptionType=''");
						pItem->tinysave(query);
						
						
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID(pItem->getObjectID());
						pGamePlayer->sendPacket(&gcDeleteInventoryItem);

						CoordInven_t X = -1, Y;
						pPC->getInventory()->findItemOID( pItem->getObjectID(), X, Y );
						Assert( X != -1 );

						GCCreateItem gcCreateItem;
						makeGCCreateItem( &gcCreateItem, pItem, X, Y );
						pGamePlayer->sendPacket(&gcCreateItem);
							}*/
							
							const list<PetEnchantOption*>& petEnchantOptions = g_pOptionInfoManager->getPetEnchantOptionList2();
							const list<PetEnchantOption*>& petEnchantOptions2 = g_pOptionInfoManager->getPetEnchantOptionList2();
							const list<PetEnchantOption*>& petEnchantOptions3 = g_pOptionInfoManager->getPetEnchantOptionList3();
							
							list<PetEnchantOption*>::const_iterator itr = petEnchantOptions.begin();
							list<PetEnchantOption*>::const_iterator endItr = petEnchantOptions.end();
							
							list<PetEnchantOption*>::const_iterator itr2 = petEnchantOptions2.begin();
							list<PetEnchantOption*>::const_iterator endItr2 = petEnchantOptions2.end();
							
							list<PetEnchantOption*>::const_iterator itr3 = petEnchantOptions3.begin();
							list<PetEnchantOption*>::const_iterator endItr3 = petEnchantOptions3.end();
							

							for ( ; itr != endItr ; itr++ )
							{
								PetEnchantOption* pPetEnchantOption = *itr;
								if ( pPetEnchantOption->getRatio() > ratio )
								{
									targetOption = pPetEnchantOption->getOptionType();
									break;
								}
							}
						
													for ( ; itr2 != endItr2 ; itr2++ )
							{
								PetEnchantOption* pPetEnchantOption = *itr2;
								if ( pPetEnchantOption->getRatio() > ratio2 )
								{
									targetOption2 = pPetEnchantOption->getOptionType();
									break;
								}
							}
						
													for ( ; itr3 != endItr3 ; itr3++ )
							{
								PetEnchantOption* pPetEnchantOption = *itr3;
								if ( pPetEnchantOption->getRatio() > ratio3 )
								{
									targetOption3 = pPetEnchantOption->getOptionType();
									break;
								}
							}
						}

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );
						
						//OptionType_t currentOptionType = pPetInfo->getOptionTypeSize();	
						//pPetInfo->removeOptionType( currentOptionType );
						
						
						list<OptionType_t> oList = pPetInfo->getOptionType();
					oList.clear();
				pPetInfo->setOptionType( oList );
		
		
						//cout << "test = " << (int)currentOptionType << endl;
						cout << "Before = " << getOptionTypeToString( pPetInfo->getOptionType() ) << endl;
						
						pPetInfo->addOptionType( targetOption );
						pPetInfo->addOptionType( targetOption2 );
						pPetInfo->addOptionType( targetOption3 );
						
		//	list<OptionType_t> options;
		//options.push_back( getOptionTypeToString(pPetInfo->getOptionType()) );
		
		//pPetItem->getPetInfo()->setOptionType( options );
		

					//	string optionField="";
					//	setOptionTypeToField( pPetInfo->getOptionType(), optionField );
					//	cout << "test = " << (int)currentOptionType << endl;
						cout << "after = " << getOptionTypeToString( pPetInfo->getOptionType() ) << endl;
						/*
							list<OptionType_t> oList;
					int MinID,MaxID,LoopT;
		int i = 0;
			while(i)
			{
				int lastSize = oList.size();
				oList.push_back(targetOption);
				if(lastSize<oList.size())
				{
					i++;
				}
			}*/
			
						//pPetInfo->setOptionType(  pItem->getOptionTypeList() );
						
						//string optionField;
						//setOptionTypeToField( pItem->getOptionTypeList(), optionField );

						
						char query[100];
						sprintf(query, "OptionType='%u/%u/%u'", targetOption , targetOption2, targetOption3);
						pItem->tinysave(query);
						
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( targetOption );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						
						
						GCDeleteInventoryItem gcDeleteInventoryItem;
						gcDeleteInventoryItem.setObjectID(pItem->getObjectID());
						pGamePlayer->sendPacket(&gcDeleteInventoryItem);

						CoordInven_t X = -1, Y;
						pPC->getInventory()->findItemOID( pItem->getObjectID(), X, Y );
						Assert( X != -1 );

						GCCreateItem gcCreateItem;
						makeGCCreateItem( &gcCreateItem, pItem, X, Y );
						pGamePlayer->sendPacket(&gcCreateItem);
						
						//pPetInfo->setOptionType(  pItem->getOptionTypeList() );
						
						
//1210 wlzzi - ??ùù? ??? ?????? ????						
						if ( pPC->getPetInfo() == pPetInfo )
						{
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						else
						{
//							sendPetInfo( pGamePlayer);
							GCPetInfo gcPetInfo;
							gcPetInfo.setPetInfo( pPetInfo );
							gcPetInfo.setSummonInfo( 0 );
							gcPetInfo.setObjectID( pPC->getObjectID() );
							pGamePlayer->sendPacket( &gcPetInfo );
						}
//						sendPetInfo( pGamePlayer );
					}
					break;

					case PetEnchantItemInfo::REVIVAL_FUNCTION:
					{
						//cout << "??? ????? ????."  << endl;
						if ( pPetInfo->getPetHP() != 0 )
						{
							//cout << "?? ???? ?? ???????? ????" << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						VSDateTime current = VSDateTime::currentDateTime();
						int Days = pPetInfo->getLastFeedTime().daysTo(current);
						if ( Days < 0 ) Days = 0;

						if ( pItemInfo->getItemType() != 14 )
						{
							int value = rand()%100;
							if ( Days >= 8 || value >= (10-Days/2)*10 )
							{
								GCAddItemToItemVerify gcAddItemToItemVerify;
								gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL );
								pGamePlayer->sendPacket( &gcAddItemToItemVerify );
								return;
							}
						}

						pPetInfo->setPetHP( pItemInfo->getFunctionGrade() );
						pPetInfo->setFeedTime( VSDateTime::currentDateTime() );
						pPetInfo->getPetItem()->savePetInfo();
						
						
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK );
						gcAddItemToItemVerify.setParameter( pPetInfo->getPetHP() );

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					}
					break;

					case PetEnchantItemInfo::MUTANT_FUNCTION:
					{
						PetType_t currentPetType = pPetInfo->getPetType();
						PetType_t afterPetType = 0;
						if(currentPetType>1)
							afterPetType = 1;
						if ( afterPetType == 0 )
						{
							cout << "??? ?? ?? ???? ?? ???????.." << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						bool summonPet = pPC->getPetInfo() == pPetInfo;

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo(afterPetType);
						Assert( pPetTypeInfo != NULL );

						pPetInfo->setPetType(afterPetType);
						pPetInfo->setPetCreatureType( pPetTypeInfo->getPetCreatureType( pPetInfo->getPetLevel() ) );
						pPetItem->setItemType(afterPetType);

						char tinySQL[16];
						sprintf( tinySQL, "ItemType=%u", afterPetType );
						pPetItem->tinysave( tinySQL );

//						if ( pPC->getPetInfo() == pPetInfo ) sendPetInfo( pGamePlayer );
						if ( afterPetType >= 3 && afterPetType <= 5 )
						{
							pPetInfo->setAttack(1);
							pPetItem->savePetInfo();
						}
						if ( summonPet )
						{
							pPC->setPetInfo( NULL );
							sendPetInfo( pGamePlayer );
						}

						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( 0 );

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						GCDeleteInventoryItem gcDI;
						gcDI.setObjectID( pPetItem->getObjectID() );
						pGamePlayer->sendPacket( &gcDI );

						CoordInven_t X = -1, Y;
						pPC->getInventory()->findItemOID( pPetItem->getObjectID(), X, Y );
						Assert( X != -1 );

//						pPC->getInventory()->deleteItem( pPetItem->getObjectID() );
//						pPC->getInventory()->addItem( X, Y, pPetItem );

						GCCreateItem gcCI;
						makeGCCreateItem( &gcCI, pPetItem, X, Y );
						pGamePlayer->sendPacket( &gcCI );

						if ( summonPet )
						{
							pPC->setPetInfo( pPetInfo );
							sendPetInfo( pGamePlayer );
						}
					}
					break;

					case PetEnchantItemInfo::POLYMORPH_FUNCTION:
					{
						PetType_t currentPetType = pPetInfo->getPetType();
						PetType_t afterPetType = 0;

						if ( pPC->isSlayer() )
						{
							if ( currentPetType == 1 )
								afterPetType = 3;
							else if ( currentPetType == 3 )
								afterPetType = 1;
						}
						else if ( pPC->isVampire() )
						{
							if ( currentPetType == 1 )
								afterPetType = 4;
							else if ( currentPetType == 4 )
								afterPetType = 1;
						}
						else if ( pPC->isOusters() )
						{
							if ( currentPetType == 1 )
								afterPetType = 5;
							else if ( currentPetType == 5 )
								afterPetType = 1;
						}

						if ( afterPetType == 0 )
						{
							cout << "??? ?? ?? ???? ?? ???????.." << endl;
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						bool summonPet = pPC->getPetInfo() == pPetInfo;

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo(afterPetType);
						Assert( pPetTypeInfo != NULL );

						pPetInfo->setPetType(afterPetType);
						pPetInfo->setPetCreatureType( pPetTypeInfo->getPetCreatureType( pPetInfo->getPetLevel() ) );
						pPetItem->setItemType(afterPetType);

						char tinySQL[16];
						sprintf( tinySQL, "ItemType=%u", afterPetType );
						pPetItem->tinysave( tinySQL );

						// 3,4,5 ?? ??????? ????? 30???? ?????? Attack ??? ?ùù?
						if ( afterPetType >= 3 && afterPetType <= 5 )
						{
							pPetInfo->setAttack(1);
							pPetItem->savePetInfo();
						}

						if ( summonPet )
						{
							pPC->setPetInfo( NULL );
							sendPetInfo( pGamePlayer );
						}

						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( 0 );

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						GCDeleteInventoryItem gcDI;
						gcDI.setObjectID( pPetItem->getObjectID() );
						pGamePlayer->sendPacket( &gcDI );

						CoordInven_t X = -1, Y;
						pPC->getInventory()->findItemOID( pPetItem->getObjectID(), X, Y );
						Assert( X != -1 );

						GCCreateItem gcCI;
						makeGCCreateItem( &gcCI, pPetItem, X, Y );
						pGamePlayer->sendPacket( &gcCI );

						if ( summonPet )
						{
							pPC->setPetInfo( pPetInfo );
							sendPetInfo( pGamePlayer );
						}
					}
					break;
				
				//smallheart - 20080930 - ????? ???
				// ?????ùZ ???? ???????? ????? Function???? ????? ????.
				// ?? ????????~ ??????????ùù? ??? ???? ????? Function????...
				// ??????, ?ùù????????, ????????????? ????? Function????.. ???? ?? ?????? FunctionGrade?????? ???????.
					
					case PetEnchantItemInfo::COMMON_PET_INJECTION:
					{
						//???????????? ?????????? ?ùù???? ???????? ?????? ???????.
						//?ùù ?????????? ????? ???????? ???? ??? ????.
						
						int PetItemType = pItemInfo->getFunctionGrade ();

						if ( !isCommonPetInjectionBasePet( pPetItem, pPetInfo ) )
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION invalid base pet: player=%s baseItemType=%u runtimePetType=%u injectionType=%u targetType=%u",
								pPC->getName().c_str(),
								(uint)pPetItem->getItemType(),
								(uint)pPetInfo->getPetType(),
								(uint)pMouseItem->getItemType(),
								(uint)PetItemType );

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						PetTypeInfo* pPetTypeInfo = PetTypeInfoManager::getInstance()->getPetTypeInfo(PetItemType);
						ItemInfo* pPetItemInfo = g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_PET_ITEM, PetItemType );
						if ( pPetTypeInfo == NULL || pPetItemInfo == NULL )
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION missing target contract: player=%s injectionType=%u targetType=%u hasPetTypeInfo=%d hasPetItemInfo=%d",
								pPC->getName().c_str(),
								(uint)pMouseItem->getItemType(),
								(uint)PetItemType,
								pPetTypeInfo != NULL,
								pPetItemInfo != NULL );

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PET_ITEM, PetItemType, pPetItem->getOptionTypeList() );
						PetItem* pNewPetItem = dynamic_cast<PetItem*>(pItem);
						if ( pItem == NULL || pNewPetItem == NULL )
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION failed to create target pet item: player=%s targetType=%u item=%p petItem=%p",
								pPC->getName().c_str(),
								(uint)PetItemType,
								pItem,
								pNewPetItem );

							SAFE_DELETE( pItem );

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						_TPOINT pt;
						if ( !getEmptySlotAfterRemovingBasePet( pPC->getInventory(), pItem, pPetItem, pt ) )
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION no inventory room for target pet after removing base pet: player=%s targetType=%u width=%u height=%u",
								pPC->getName().c_str(),
								(uint)PetItemType,
								(uint)pItem->getVolumeWidth(),
								(uint)pItem->getVolumeHeight() );

							SAFE_DELETE( pItem );

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						PetInfo* pNewPetInfo = pPetInfo->createClone();
						if ( pNewPetInfo == NULL )
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION failed to clone PetInfo: player=%s baseItemType=%u targetType=%u",
								pPC->getName().c_str(),
								(uint)pPetItem->getItemType(),
								(uint)PetItemType );

							SAFE_DELETE( pItem );

							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}

						// PetItem::setPetInfo runtime PetType normalizasyonunu ve Gara Bezz alias akisini yonetir.
						pNewPetInfo->setOptionType(pPetItemInfo->getDefaultOptions());
						if (PetItemType==11)
						{
							pNewPetInfo->setAttack(1);
						}
						pNewPetItem->setPetInfo( pNewPetInfo );
						pNewPetInfo->setPetItem( pNewPetItem );

						bool summonPet = pPC->getPetInfo() == pPetInfo;
						ObjectID_t oldPetObjectID = pPetItem->getObjectID();

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						pPC->getInventory()->deleteItem( oldPetObjectID );
						if ( summonPet )
						{
							pPC->setPetInfo( NULL );
							sendPetInfo( pGamePlayer, true );
						}

						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_OK_MESSAGE );
						gcAddItemToItemVerify.setParameter( 0 );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						GCDeleteInventoryItem gcDI;
						gcDI.setObjectID( oldPetObjectID );
						pGamePlayer->sendPacket( &gcDI );

						Zone* pZone = pPC->getZone();
						pZone->registerObject( pItem );

						if( pPC->getInventory()->addItem(pt.x, pt.y, pItem) )
						{
							pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
							string strTraceLog;
							strTraceLog = pPetItemInfo->getEName ();
							strTraceLog += "Pet Changer";
							remainTraceLog( pItem, strTraceLog, pPC->getName(), ITEM_LOG_CREATE, DETAIL_ENCHANT);
						}
						else
						{
							filelog("PetItemError.txt",
								"COMMON_PET_INJECTION addItem failed after precheck: player=%s targetType=%u x=%u y=%u",
								pPC->getName().c_str(),
								(uint)PetItemType,
								(uint)pt.x,
								(uint)pt.y );
							pItem->destroy();
							return;
						}

						pPetItem->destroy();

						GCCreateItem gcCI;
						makeGCCreateItem( &gcCI, pItem, pt.x, pt.y );
						pGamePlayer->sendPacket( &gcCI );

						if ( summonPet )
						{
							pPC->setPetInfo( pNewPetItem->getPetInfo() );
							sendPetInfo( pGamePlayer, true, 1 );
						}
						else
						{
							GCPetInfo gcPetInfo;
							gcPetInfo.setPetInfo( pNewPetItem->getPetInfo() );
							gcPetInfo.setSummonInfo( 0 );
							gcPetInfo.setObjectID( pPC->getObjectID() );
							pGamePlayer->sendPacket( &gcPetInfo );
						}
					}
					break;					

					case PetEnchantItemInfo::CLEAR_FIRST_ATTRIBUTE_FUNCTION:
					{
                                                //by svi

						//if ( pPetInfo->getPetAttr() == 0xff || pPetInfo->getPetOption() != 0 )
                                                if ( pPetInfo->getPetAttr() == 0xff )
						{
							// ???(1?????) ?? ?????, ???(2?????)?? ???? ?????
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						//cout << "??? ??ùù? ?????????." << endl;
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						// ?? ???(1?????) ????
						pPetInfo->setPetAttr( 0xff );
						pPetInfo->setPetAttrLevel( 0 );

						// ????
						pPetItem->savePetInfo();

						// ??ùù? ???? ??? ??????
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( ((uint)pPetInfo->getPetAttr())<<16 | (uint)pPetInfo->getPetAttrLevel() );

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						if ( pPC->getPetInfo() == pPetInfo )
						{
							// ??? ??????????? ??? ??????.
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						return;
					}
					break;
					case PetEnchantItemInfo::MAGIC_ENCHANT_FUNCTION:
					{
						if ( pPetInfo->getMagicRemainSec() > 0 )
						{
							// ??? ??ùù? ??? ?????? ????
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						//cout << "??? ??ùù? ?????????." << endl;
						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						// ?? ???(1?????) ????
						VSDateTime vsDateTime = VSDateTime::currentDateTime().addDays(3);
						//vsDateTime.addDays( 3 );

						pPetInfo->setMagicExpireTime( vsDateTime );

						// MagicExpireTime????
						char sql[32];
						sprintf( sql, "MagicExpireTime='%s'", pPetInfo->getMagicExpireTime().toDateTime().c_str() );
						pPetItem->tinysave( sql );

						// ??ùù? ???? ??? ??????
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_MAGIC_ENCHANT_OK );
						gcAddItemToItemVerify.setParameter( pPetInfo->getMagicRemainSec() );

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						if ( pPC->getPetInfo() == pPetInfo )
						{
							// ??? ??????????? ??? ??????.
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						return;
					}
					break;
					//20080701 kim sung yong --> 1202 wlzzi ?? ???? ????	
					case PetEnchantItemInfo::EXPERT_PET_CHANGER_ENCHANT_FUNCTION:
                    {
                        if ( pPetInfo->getExpertPetChangerRemainSec() > 0 )
                        {
                            GCAddItemToItemVerify gcAddItemToItemVerify;
                            gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
                            pGamePlayer->sendPacket( &gcAddItemToItemVerify );
                            return;
                        }
                        pPC->deleteItemFromExtraInventorySlot();
                        pMouseItem->destroy();
                        SAFE_DELETE( pMouseItem );

                        VSDateTime vsDateTime = VSDateTime::currentDateTime().addDays(15);

                        pPetInfo->setExpertPetChangerExpireTime( vsDateTime );

                        char sql[32];
                        sprintf( sql, "ExpertExpireTime='%s'", pPetInfo->getExpertPetChangerExpireTime().toDateTime().c_str() );
                        pPetItem->tinysave( sql );

                        GCAddItemToItemVerify gcAddItemToItemVerify;
                        gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_EXPERT_ENCHANT_OK );
                        gcAddItemToItemVerify.setParameter( pPetInfo->getExpertPetChangerRemainSec() );

                        pGamePlayer->sendPacket( &gcAddItemToItemVerify );

                        if ( pPC->getPetInfo() == pPetInfo )
                        {
                            sendPetInfo( pGamePlayer, true );
                            pPC->initAllStatAndSend();
                        }
                        return;
                    }
                    break;
                    
					// 20070720
					case PetEnchantItemInfo::ENCHANT_SKILL_FUNCTION:
					{
						PetEnchantItemInfo* pItemInfo = dynamic_cast<PetEnchantItemInfo*>(g_pPetEnchantItemInfoManager->getItemInfo( pMouseItem->getItemType() ));
						// 20070724 Add ???? 50????? ??ùù? ??? ????. ????? ?????? ?? ????? ?????? ??ùù? ??? ????.
						if( pPetInfo->getPetLevel() < 50 )
						{
							GCAddItemToItemVerify gcAddItemToItemVerify;
							//	gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_LEVEL_FAIL ); // 20070807 ???? ?ùù???? ????? ???? ??????? ???? ??? ???
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}
						if(pItemInfo->getFunctionGrade() == pPetInfo->getEnchantSkill())
						{
							GCAddItemToItemVerify gcAddItemToItemVerify;
							//	gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_DUPLICATE_FAIL ); // 20070807 ???? ???? ??ùù?? ????? ?? ??? ???
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;

						}

						pPetInfo->setEnchantSkill( pItemInfo->getFunctionGrade() );
/*						// ??? ???
						if( pMouseItem->getItemType() == 27 )
						{
							pPetInfo->setEnchantSkill(1);
						}
						// ????? ???
						else if( pMouseItem->getItemType() == 28 )
						{
							pPetInfo->setEnchantSkill(2);
						}
						// ?????? ???
						else if( pMouseItem->getItemType() == 29 )
						{
							pPetInfo->setEnchantSkill(3);
						}
						else
						{
							GCAddItemToItemVerify gcAddItemToItemVerify;
							gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
							pGamePlayer->sendPacket( &gcAddItemToItemVerify );
							return;
						}*/

						char sql[32];
						sprintf( sql, "EnchantSkill='%d'", (int)pPetInfo->getEnchantSkill() );
						pPetItem->tinysave( sql );

						pPC->deleteItemFromExtraInventorySlot();
						pMouseItem->destroy();
						SAFE_DELETE( pMouseItem );

						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
//						gcAddItemToItemVerify.setParameter(0);

						pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						if ( pPC->getPetInfo() == pPetInfo )
						{
							//cout << "??? ???????? ??? ?????? ??????." << endl;
							sendPetInfo( pGamePlayer, true );
							pPC->initAllStatAndSend();
						}
						return;
					}
					break;
					default:
					{
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
				}
			}
			break;

			case Item::ITEM_CLASS_MIXING_ITEM :
			{
				if ( pItem->getItemClass() != Item::ITEM_CLASS_PET_ITEM )
				{
					//cout << "?? ?????????? ??? ????." << endl;
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				MixingItemInfo* pItemInfo = dynamic_cast<MixingItemInfo*>(g_pMixingItemInfoManager->getItemInfo( pMouseItem->getItemType() ));
				Assert( pItemInfo != NULL );

				if ( pItemInfo->getType() != MixingItemInfo::TYPE_DETACH )
				{
					//cout << "?? ?????????? ??? ????." << endl;
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				//20090601 ksym555
				PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
				PetInfo* pPetInfo = NULL;
				if ( pMouseItem->getItemType() != 30 )
				{
					if ( pPetItem == NULL || ( pPetInfo = pPetItem->getPetInfo() ) == NULL || pPetInfo->getPetOption() == 0 )
					{
						//cout << "?? ???????? ????? ?? ??????? ?? ???ùù? ???????." << endl;
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}

					pPC->deleteItemFromExtraInventorySlot();
					pMouseItem->destroy();
					SAFE_DELETE( pMouseItem );

					pPetInfo->setPetOption(0);
					pPetItem->tinysave("PetOption=0");
				}
				else if ( pMouseItem->getItemType() == 30 )
				{
					if ( pPetItem == NULL || ( pPetInfo = pPetItem->getPetInfo() ) == NULL || pPetInfo->getMixOptionTypeSize() == NULL )
					{
						//cout << "?? ???????? ????? ?? ??????? ?? ???ùù? ???????." << endl;
						GCAddItemToItemVerify gcAddItemToItemVerify;
						gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
						pGamePlayer->sendPacket( &gcAddItemToItemVerify );
						return;
					}
					
					pPC->deleteItemFromExtraInventorySlot();
					pMouseItem->destroy();
					SAFE_DELETE( pMouseItem );

					list<OptionType_t> oList = pPetInfo->getMixOptionType();
					oList.clear();
					pPetInfo->setMixOptionType( oList );
					pPetItem->tinysave("MixOptionType=''");
					
					GCPetInfo gcPetInfo;
					gcPetInfo.setPetInfo( pPetItem->getPetInfo() );
					gcPetInfo.setSummonInfo( 0 );
					gcPetInfo.setObjectID( pPC->getObjectID() );
					pGamePlayer->sendPacket( &gcPetInfo );
					
					pPC->initAllStatAndSend();
				}
				
				GCAddItemToItemVerify gcAddItemToItemVerify;
				gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
				gcAddItemToItemVerify.setParameter(0);

				pGamePlayer->sendPacket( &gcAddItemToItemVerify );

				if ( pPC->getPetInfo() == pPetInfo )
				{
					//cout << "??? ???????? ??? ?????? ??????." << endl;
					sendPetInfo( pGamePlayer, true );
					pPC->initAllStatAndSend();
				}
				return;
			}
			break;
			//20080604
			case Item::ITEM_CLASS_SLAYER_TUNNING_ITEM :
			{
				SlayerTunningItemInfo* pMouseItemInfo = dynamic_cast<SlayerTunningItemInfo*>(g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_SLAYER_TUNNING_ITEM, pMouseItem->getItemType() ));
				Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
				
				Assert(pSlayer != NULL);
									
				if( pSlayer->getTotalAttr() < pMouseItemInfo->getReqSum() || pSlayer->getLevel() < pMouseItemInfo->getReqAdvancedLevel() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				if ( pItem->getItemClass() != pMouseItemInfo->getTunningItemClass() || pItem->getItemType() != pMouseItemInfo->getTunningItemType() 
				|| pItem->isTimeLimitItem())
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()); 
				list<OptionType_t> options;
				if( !g_pItemInfoManager->isPossibleItem(pItem->getItemClass(), pItemInfo->getNextItemType(), options) )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}
				
				if( pItemInfo->getNextItemType() == pItem->getItemType() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				

				int ratio =0;
				    Statement* pStmt   = NULL;
				    Result*    pResult = NULL;

				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pResult = pStmt->executeQuery("SELECT Ratio FROM SlayerTunningItemInfo WHERE ItemType = '%d'", (int)pMouseItemInfo->getItemType());

					if(pResult->next())
					{
						ratio = pResult->getInt(1);
					}
		
					SAFE_DELETE( pStmt );
				}
				END_DB(pStmt);
				int ratio2 = Random(1, 100);

				pPC->deleteItemFromExtraInventorySlot();
				pMouseItem->destroy();
				SAFE_DELETE( pMouseItem );

				if(ratio >  ratio2)
				{
					pItem->setItemType( pItemInfo->getNextItemType() );
				}
				char buffer[80];
				snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
				pItem->tinysave(buffer);

				GCAddItemToItemVerify gcAddItemToItemVerify;
				gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
				gcAddItemToItemVerify.setParameter( pItem->getItemType() );
				pGamePlayer->sendPacket( &gcAddItemToItemVerify );

				return;	
			}
			break;
			case Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM :
			{
				VampireTunningItemInfo* pMouseItemInfo = dynamic_cast<VampireTunningItemInfo*>(g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_VAMPIRE_TUNNING_ITEM, pMouseItem->getItemType() ));
				
				Vampire* pVampire = dynamic_cast<Vampire*>(pPC);

				Assert(pVampire != NULL);

				if( pVampire->getLevel() < pMouseItemInfo->getReqLevel() || pVampire->getLevel() < pMouseItemInfo->getReqAdvancedLevel() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				if ( pItem->getItemClass() != pMouseItemInfo->getTunningItemClass() || pItem->getItemType() != pMouseItemInfo->getTunningItemType() 
				|| pItem->isTimeLimitItem())
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
				list<OptionType_t> options;
				if( !g_pItemInfoManager->isPossibleItem(pItem->getItemClass(), pItemInfo->getNextItemType(), options) )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				if( pItemInfo->getNextItemType() == pItem->getItemType() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				int ratio =0;
				    Statement* pStmt   = NULL;
				    Result*    pResult = NULL;

				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pResult = pStmt->executeQuery("SELECT Ratio FROM VampireTunningItemInfo WHERE ItemType = '%d'", (int)pMouseItemInfo->getItemType());

					if(pResult->next())
					{
						ratio = pResult->getInt(1);
					}
		
					SAFE_DELETE( pStmt );
				}
				END_DB(pStmt);
				int ratio2 = Random(1, 100);

				pPC->deleteItemFromExtraInventorySlot();
				pMouseItem->destroy();
				SAFE_DELETE( pMouseItem );

				if(ratio >  ratio2)
				{
					pItem->setItemType( pItemInfo->getNextItemType() );
				}
				char buffer[80];
				snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
				pItem->tinysave(buffer);

				GCAddItemToItemVerify gcAddItemToItemVerify;
				gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
				gcAddItemToItemVerify.setParameter( pItem->getItemType() );
				pGamePlayer->sendPacket( &gcAddItemToItemVerify );
				return;	

			}
			break;
			case Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM :
			{
				OustersTunningItemInfo* pMouseItemInfo = dynamic_cast<OustersTunningItemInfo*>( g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_OUSTERS_TUNNING_ITEM, pMouseItem->getItemType() ));
				
				Ousters* pOusters = dynamic_cast<Ousters*>(pPC);

				Assert(pOusters != NULL);

				if( pOusters->getLevel() < pMouseItemInfo->getReqLevel() || pOusters->getLevel() < pMouseItemInfo->getReqAdvancedLevel() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				if ( pItem->getItemClass() != pMouseItemInfo->getTunningItemClass() || pItem->getItemType() != pMouseItemInfo->getTunningItemType() 
				|| pItem->isTimeLimitItem())
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
				list<OptionType_t> options;
				if( !g_pItemInfoManager->isPossibleItem(pItem->getItemClass(), pItemInfo->getNextItemType(), options) )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}

				if( pItemInfo->getNextItemType() == pItem->getItemType() )
				{
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					return;
				}


				int ratio =0;
				    Statement* pStmt   = NULL;
				    Result*    pResult = NULL;

				BEGIN_DB
				{
					pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
					pResult = pStmt->executeQuery("SELECT Ratio FROM OustersTunningItemInfo WHERE ItemType = '%d'", (int)pMouseItemInfo->getItemType());

					if(pResult->next())
					{
						ratio = pResult->getInt(1);
					}
		
					SAFE_DELETE( pStmt );
				}
				END_DB(pStmt);
				int ratio2 = Random(1, 100);

				pPC->deleteItemFromExtraInventorySlot();
				pMouseItem->destroy();
				SAFE_DELETE( pMouseItem );

				if(ratio >  ratio2)
				{
					pItem->setItemType( pItemInfo->getNextItemType() );
				}
				char buffer[80];
				snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
				pItem->tinysave(buffer);

				GCAddItemToItemVerify gcAddItemToItemVerify;
				gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
				gcAddItemToItemVerify.setParameter( pItem->getItemType() );
				pGamePlayer->sendPacket( &gcAddItemToItemVerify );

						return;	
			}
			break;
			
			case Item::ITEM_CLASS_COMMON_QUEST_ITEM :
			{
				Inventory* pInventory = pPC->getInventory();
				Zone *pZone = pPC->getZone();

				Item *pResultItem;
				int LimitTime = 0;
				
				if ( !(pItem->getItemType() >= 36 && pItem->getItemType() <= 51 && pMouseItem->getItemType() == 35) /* ??? ????????? ???? ??? */ )
				{
					// ????????? ??? ??? ???? ??? ???
					
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_INCORRECT_KEY );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					return;
				}
				
				if ( pItem->getNum() > 1 || pMouseItem->getNum() > 1 )
				{
					// ?????? ????? ?????? ??????? ???? ???.
					
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_INCORRECT_KEY );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					return;
				}
				
				pResultItem = g_pEventGiftBoxRewardManager->getEventGiftBoxReward(
					pCreature, 
					( Item::ITEM_CLASS_COMMON_QUEST_ITEM * 10000 ) + pItem->getItemType(), 
					LimitTime
				);
				
				if ( pResultItem == NULL )
				{
					cout << "?????? ???? ???? ????" << endl;
					
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NO_ITEM );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					return;
				}
				
				bool bEnoughSpace = false;

				_TPOINT pt;
				
				if ( (pItem->getVolumeWidth() >= pResultItem->getVolumeWidth()) && 
					(pItem->getVolumeHeight() >= pResultItem->getVolumeHeight()) )
				{
					// ??? ???????? ??????? ??????
					bEnoughSpace = true;
				}
				else if ( pInventory->getEmptySlot(pResultItem, pt) )
				{
					bEnoughSpace = true;

					// ????????? ??? ????
					invenX = pt.x;
					invenY = pt.y;
				}
				else
				{
					cout << "?????? ???? ????" << endl;
					
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NOT_ENOUGH_SPACE );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					SAFE_DELETE(pResultItem);
					
					return;
				}
				
				pZone->registerObject(pResultItem);

				pPC->deleteItemFromExtraInventorySlot();
				pMouseItem->destroy();
				SAFE_DELETE( pMouseItem );
				
				ObjectID_t CaseItemObjectID;
				
				CaseItemObjectID = pItem->getObjectID();
				
				pInventory->deleteItem(CaseItemObjectID);
				pItem->destroy();
				SAFE_DELETE(pItem);
				
				GCDeleteInventoryItem gcDI;
				gcDI.setObjectID( CaseItemObjectID );
				pGamePlayer->sendPacket( &gcDI );
				
				if(pInventory->addItem(invenX, invenY, pResultItem))
				{
					pResultItem->create(pPC->getName(), STORAGE_INVENTORY, 0, invenX, invenY);

					if( LimitTime > 0)
					{
						pPC->addTimeLimitItem( pResultItem, LimitTime );        
						pPC->sendTimeLimitItemInfo();
					}

					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_OK );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					GCCreateItem gcCreateItem;
					makeGCCreateItem( &gcCreateItem, pResultItem, invenX, invenY );
					pGamePlayer->sendPacket(&gcCreateItem);

					// ItemTraceLog ?? ?????
					if ( pResultItem != NULL && pResultItem->isTraceItem() )
					{
						remainTraceLog( pResultItem, "Case", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
					}
				}
				else
				{
//					filelog("GiftBoxErrorLog.txt", "[Name] : %s : ?ùù????? ???????? ???? ?? ????. Item : %s\n", pCreature->getName().c_str(), pResultItem->toString().c_str());
					return;
				}				
			}
			break;

			default:
			{
				//cout << "upgrade error" << endl;

				GCAddItemToItemVerify gcAddItemToItemVerify;
				gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
				pGamePlayer->sendPacket( &gcAddItemToItemVerify );
				return;
			}
		}
	}

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}


#ifdef __GAME_SERVER__
void executeUpGradeCoreZap(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem)
	throw(Error)
{
	__BEGIN_TRY

	Creature*  		pCreature   = pGamePlayer->getCreature();
	PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

	bool success = false;
	int prevGrade = pItem->getGrade();
	string optionString="";
	int ratio =0;
	
	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT Ratio FROM CoreZapUpGrade WHERE Grade = '%d'", (int)pItem->getGrade());

		if(pResult->next())
		{
			ratio = pResult->getInt(1);
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);
	
	if ( rand()%100 < ratio)
	{
		success = true;
		pItem->setGrade( pItem->getGrade() + 1 );
		char buffer[80];
		snprintf(buffer, 80, "Grade=%u", pItem->getGrade() );
		pItem->tinysave(buffer);

		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK );
		gcAddItemToItemVerify.setParameter( pItem->getGrade() );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );
	}
	else
	{
		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );
	}

	pPC->deleteItemFromExtraInventorySlot();
	pMouseItem->destroy();
	SAFE_DELETE(pMouseItem);

	Statement* pStmt2 = NULL;

	BEGIN_DB
	{
		pStmt2 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt2->executeQuery("INSERT INTO YellowDropLog (PlayerID, Name, ReportTime, IClass, IType, IID, IGrade, IOption, Ratio, Success) "
				"VALUES ('%s', '%s', now(), %u, %u, %u, %u, '%s', %u, %u)",
				pGamePlayer->getID().c_str(), pPC->getName().c_str(), pItem->getItemClass(), pItem->getItemType(), pItem->getItemID(),
				prevGrade, optionString.c_str(), (uint)ratio, ((success)?1:0) );
		SAFE_DELETE( pStmt2 );
	}
	END_DB(pStmt2);

	__END_CATCH
}


	void executeUpGrade(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem)
		throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		InfoClassManager* pInfoClassManager = g_pItemInfoManager->getInfoManager( pItem->getItemClass() );
		ItemType_t maxItemType = pInfoClassManager->getInfoCount();

		float typeFailRatio = 1.0 / (maxItemType - pItem->getItemType() + 2 ) / 1.25;
		float optionFailRatio = 0;
		string optionString="";

		//2007.11.20 kim sung yong
		if ( pItem->getOptionTypeSize() >= 1 &&  pMouseItem->getItemType() != 57 )
		{
			OptionType_t option = pItem->getFirstOptionType();

			OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(option);
			const OptionClassInfo* pClassInfo = g_pOptionInfoManager->getOptionClassInfo( pOptionInfo->getClass() );

			int maxGrade = pClassInfo->getTotalGrade();
			int curGrade = pOptionInfo->getGrade();

			optionFailRatio = 1.0 / (maxGrade - curGrade + 2 ) / 2.0;

			static float optionLevelWeight[] = { 1.0, 1.0, 1.1, 1.15, 1.2 };

			optionFailRatio *= optionLevelWeight[min(4,(int)pClassInfo->getLevel())];
			optionString = pOptionInfo->getNickname();
		}

		float gradeFailRatio = 1.0 - ( 4.2 / (pItem->getGrade()+1.0) );
		bool success = false;
		int prevGrade = pItem->getGrade();
		float payItemBonus = 1.0f;
		if( pMouseItem->getItemType() == 22 )
		{
			payItemBonus = 2.0f;
		}
		else if( pMouseItem->getItemType() == 48 )
		{
			payItemBonus = 2.5f;
		}

		int ratio = 1000000 - ((typeFailRatio + optionFailRatio + gradeFailRatio)/payItemBonus * 1000000);

		//2007.11.20 kim sung yong :: yellow drop USE UpGrade  
		if(pItem->getOptionTypeSize() < 3 &&  pMouseItem->getItemType() == 57)
		{
			success = true;
			pItem->setGrade( pItem->getGrade() + 1 );
			char buffer[80];
			snprintf(buffer, 80, "Grade=%u", pItem->getGrade() );
			pItem->tinysave(buffer);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK );
			gcAddItemToItemVerify.setParameter( pItem->getGrade() );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		
		//2007.11.20 kim sung yong
		if ( rand()%1000000 < ratio &&  pMouseItem->getItemType() != 57 )
		{
			success = true;
			pItem->setGrade( pItem->getGrade() + 1 );
			char buffer[80];
			snprintf(buffer, 80, "Grade=%u", pItem->getGrade() );
			pItem->tinysave(buffer);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK );
			gcAddItemToItemVerify.setParameter( pItem->getGrade() );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}

		pPC->deleteItemFromExtraInventorySlot();
		pMouseItem->destroy();
		SAFE_DELETE(pMouseItem);

		Statement* pStmt = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery("INSERT INTO YellowDropLog (PlayerID, Name, ReportTime, IClass, IType, IID, IGrade, IOption, Ratio, Success) "
					"VALUES ('%s', '%s', now(), %u, %u, %u, %u, '%s', %u, %u)",
					pGamePlayer->getID().c_str(), pPC->getName().c_str(), pItem->getItemClass(), pItem->getItemType(), pItem->getItemID(),
					prevGrade, optionString.c_str(), (uint)ratio, ((success)?1:0) );
			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt);

		__END_CATCH
	}
	//////////////////////////////////
	// Blue Bird 2 Enchant Logic
	/////////////////////////////////
	void executeEnchantRareOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY)
    throw (Error)
{
    __BEGIN_TRY

    Creature*       pCreature   = pGamePlayer->getCreature();
    PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

    const ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( 
                                                pItem->getItemClass(), 
                                                pItem->getItemType() );

    if (pItem->getOptionTypeSize()!=2
        || pItemInfo->isUnique()
        || pMouseItem->getNum() != 1
    )
    {
        sendEnchantImpossible( pGamePlayer );
        return;
    }

    bool bFirstSucceed = false;
    uint optionChange = 0;
    int  shiftValue = 16;

    list<OptionType_t> optionList = pItem->getOptionTypeList();
    list<OptionType_t>::const_iterator itr = optionList.begin();

    OptionType_t firstOption = *itr++;
    OptionType_t secondOption = *itr;

    OptionInfo* pFirstOptionInfo = getOptionInfo( firstOption );
    OptionInfo* pSecondOptionInfo = getOptionInfo( secondOption );

    if ( pFirstOptionInfo == NULL || pSecondOptionInfo == NULL )
    {
        sendEnchantImpossible( pGamePlayer );
        return;
    }

    bool bFirstUpgradePossible = pFirstOptionInfo->isUpgradePossible();
    bool bSecondUpgradePossible = pSecondOptionInfo->isUpgradePossible();

    if ( !bFirstUpgradePossible && !bSecondUpgradePossible )
    {
        sendEnchantImpossible( pGamePlayer );
        return;
    }

    // ?? Basari oranini d¸s¸rmek iÁin Áarpan (ˆrnek: %50)
    const float fSuccessRateMultiplier = 0.5f;

    // ==================== BIRINCI SE«ENEK ====================
    if ( bFirstUpgradePossible )
    {
        int succeedRatio = (int)(pFirstOptionInfo->getUpgradeSecondRatio() * pItemInfo->getUpgradeRatio() * fSuccessRateMultiplier);
        int dice = rand() % 10000;

        if ( dice < succeedRatio )
        {
            // Basarili y¸kseltme
            processUpgradeOptionType( pItem, firstOption, pFirstOptionInfo );

            OptionType_t upgradeOptionType = pFirstOptionInfo->getUpgradeType();
            optionChange = (firstOption << (shiftValue+8)) | (upgradeOptionType << shiftValue);

            bFirstSucceed = true;
        }
        else
        {
            // Basarisiz oldu, 100'l¸k sistem
            int noChangeRatio = 66;                     // %66 hiÁbir sey olmaz
            int downgradeRatio = pItemInfo->getDowngradeRatio();
            int dice2 = rand() % 100;

            if ( dice2 < noChangeRatio )
            {
                // HiÁbir degisiklik yok
            }
            else
            {
                // Downgrade yapmadan ˆnce en alt seviye kontrol¸
                if ( pFirstOptionInfo->getPreviousType() != 0 )
                {
                    downgradeOptionType(pItem, firstOption, pFirstOptionInfo );

                    OptionType_t previousOptionType = pFirstOptionInfo->getPreviousType();
                    optionChange = (firstOption << (shiftValue+8)) | (previousOptionType << shiftValue);
                }
                // En alt seviyedeyse hiÁbir sey yapma
            }
        }
    }
    else
    {
        // Y¸kseltilemiyor, sadece downgrade m¸mk¸n (ˆnceden downgrade/silme vardi)
        OptionType_t prev = pFirstOptionInfo->getPreviousType();

        if ( prev != 0 )
        {
            int noChangeRatio = 66;
            int downgradeRatio = pItemInfo->getDowngradeRatio();
            int dice = rand() % 100;

            if ( dice < noChangeRatio )
            {
                // HiÁbir degisiklik yok
            }
            else
            {
                // En alt seviye kontrol¸ (prev zaten 0 degil, ama yine de kontrol edelim)
                if ( pFirstOptionInfo->getPreviousType() != 0 )
                {
                    downgradeOptionType(pItem, firstOption, pFirstOptionInfo );

                    OptionType_t previousOptionType = pFirstOptionInfo->getPreviousType();
                    optionChange = (firstOption << (shiftValue+8)) | (previousOptionType << shiftValue);
                }
            }
        }
    }

    shiftValue -= 16;

    // ==================== IKINCI SE«ENEK ====================
    if ( bSecondUpgradePossible )
    {
        int succeedRatio = (int)(g_pOptionInfoManager->getRareUpgradeRatio( secondOption, bFirstSucceed ) * pItemInfo->getUpgradeRatio() * fSuccessRateMultiplier);
        int dice = rand() % 10000;

        if ( dice < succeedRatio )
        {
            // Basarili y¸kseltme
            processUpgradeOptionType(pItem, secondOption, pSecondOptionInfo );

            OptionType_t upgradeOptionType = pSecondOptionInfo->getUpgradeType();
            optionChange |= (secondOption << (shiftValue+8)) | (upgradeOptionType << shiftValue);

            bFirstSucceed = true;
        }
        else
        {
            int noChangeRatio = 66;
            int downgradeRatio = pItemInfo->getDowngradeRatio();
            int dice2 = rand() % 100;

            if ( dice2 < noChangeRatio )
            {
                // HiÁbir degisiklik yok
            }
            else
            {
                // Downgrade ˆncesi en alt seviye kontrol¸
                if ( pSecondOptionInfo->getPreviousType() != 0 )
                {
                    downgradeOptionType(pItem, secondOption, pSecondOptionInfo );

                    OptionType_t previousOptionType = pSecondOptionInfo->getPreviousType();
                    optionChange |= (secondOption << (shiftValue+8)) | (previousOptionType << shiftValue);
                }
            }
        }
    }
    else
    {
        OptionType_t prev = pSecondOptionInfo->getPreviousType();

        if ( prev != 0 )
        {
            int noChangeRatio = 66;
            int downgradeRatio = pItemInfo->getDowngradeRatio();
            int dice = rand() % 100;

            if ( dice < noChangeRatio )
            {
                // HiÁbir degisiklik yok
            }
            else
            {
                if ( pSecondOptionInfo->getPreviousType() != 0 )
                {
                    downgradeOptionType(pItem, secondOption, pSecondOptionInfo );

                    OptionType_t previousOptionType = pSecondOptionInfo->getPreviousType();
                    optionChange |= (secondOption << (shiftValue+8)) | (previousOptionType << shiftValue);
                }
            }
        }
    }

    // Malzeme t¸ketimi ve sonuÁ gˆnderimi
    pPC->deleteItemFromExtraInventorySlot();
    pMouseItem->destroy();
    SAFE_DELETE(pMouseItem);
    sendEnchantOK(pGamePlayer, optionChange);

    __END_CATCH
}
	void executeEnchantRareOptionTwo(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY)
		throw (Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		const ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( 
													pItem->getItemClass(), 
													pItem->getItemType() );


		if (pItem->getOptionTypeSize()!=2
			|| pItemInfo->isUnique()
			|| pMouseItem->getNum() != 1
		)
		{
			//cout << "no Option " << endl;
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		bool bFirstSucceed = false;
		uint optionChange = 0;
		int  shiftValue = 16;

		list<OptionType_t> optionList = pItem->getOptionTypeList();
		list<OptionType_t>::const_iterator itr = optionList.begin();

		OptionType_t firstOption = *itr++;
		OptionType_t secondOption = *itr;

		OptionInfo* pFirstOptionInfo = getOptionInfo( firstOption );
		OptionInfo* pSecondOptionInfo = getOptionInfo( secondOption );

		if ( pFirstOptionInfo == NULL || pSecondOptionInfo == NULL )
		{
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		bool bFirstUpgradePossible = pFirstOptionInfo->isUpgradePossible();
		bool bSecondUpgradePossible = pSecondOptionInfo->isUpgradePossible();

		if ( !bFirstUpgradePossible && !bSecondUpgradePossible )
		{
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		if ( bFirstUpgradePossible )
		{
			// ???ùù ????? ??ùù? ????? ?????? 2?? ????? ??????.
			int succeedRatio = pFirstOptionInfo->getUpgradeBlueBirdRatio();
			int dice = rand()%10000;

			//cout << "???ùù ??? : " << pFirstOptionInfo->getHName() << " ??ùù? ??? " << succeedRatio << endl;

			if ( dice < succeedRatio )
			{
				//cout << "??ùù? ????" << endl;
				processUpgradeOptionType( pItem, firstOption, pFirstOptionInfo );

				OptionType_t upgradeOptionType = pFirstOptionInfo->getUpgradeType();
				optionChange = (firstOption << (shiftValue+8)) | (upgradeOptionType << shiftValue);

				bFirstSucceed = true;
			}
			
		}

		shiftValue -= 16;

		if ( bSecondUpgradePossible )
		{
			// ?ùù?ùù ????? ??ùù? ????? ???ùù ????? ???????ùù? ??????.
			int succeedRatio = pSecondOptionInfo->getUpgradeBlueBirdRatio();
			int dice = rand() %10000;

			//cout << "?ùù?ùù ??? : " << pSecondOptionInfo->getHName() << " ??ùù? ??? " << succeedRatio << endl;

			if ( dice < succeedRatio )
			{
				//cout << "??ùù? ????" << endl;
				processUpgradeOptionType(pItem, secondOption, pSecondOptionInfo );

				OptionType_t upgradeOptionType = pSecondOptionInfo->getUpgradeType();
				optionChange |= (secondOption << (shiftValue+8)) | (upgradeOptionType << shiftValue);

				bFirstSucceed = true;
			}
			
		}

		{
			// ??? ??? ?? ????
			pPC->deleteItemFromExtraInventorySlot();

			// DB???? ????
			pMouseItem->destroy();
			// ?????? ????
			SAFE_DELETE(pMouseItem);

			// ??? ??? ??????
			sendEnchantOK(pGamePlayer, optionChange);
		}

		__END_CATCH
	}

	void executeOrdinaryCrystalAddThirdOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		int ratio = rand() % 10000;

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize() == 2
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==68)
		{
			OptionType_t targetOption = 0;
			const list<CrystalEnchantOption*>& ordinaryCrystalOptions = g_pOptionInfoManager->getOrdinaryCrystalEnchantOptionList();
			list<CrystalEnchantOption*>::const_iterator itr = ordinaryCrystalOptions.begin();
			list<CrystalEnchantOption*>::const_iterator endItr = ordinaryCrystalOptions.end();

			for ( ; itr != endItr ; itr++ )
			{
				CrystalEnchantOption* pOrdinaryCrystalEnchantOption = *itr;
				if ( pOrdinaryCrystalEnchantOption->getRatio() > ratio )
				{
					targetOption = pOrdinaryCrystalEnchantOption->getOptionType();
					break;
				}
			}

			if (targetOption == 0)
			{
				sendEnchantImpossible( pGamePlayer );
				return;
			}

			pItem->addOptionType(targetOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION );
			gcAddItemToItemVerify.setParameter( targetOption );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			sendEnchantImpossible( pGamePlayer );
		}

		__END_CATCH
	}

	void executeOrdinaryCrystalRerollThirdOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		int ratio = rand() % 10000;

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize() == 3
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==68)
		{
			list<OptionType_t> optionList = pItem->getOptionTypeList();
			list<OptionType_t>::const_iterator Opitr = optionList.begin();

			Opitr++;
			Opitr++;
			OptionType_t thirdOption = *Opitr;

			OptionType_t newThirdOption = 0;
			const list<CrystalEnchantOption*>& ordinaryCrystalOptions = g_pOptionInfoManager->getOrdinaryCrystalEnchantOptionList();
			list<CrystalEnchantOption*>::const_iterator itr = ordinaryCrystalOptions.begin();
			list<CrystalEnchantOption*>::const_iterator endItr = ordinaryCrystalOptions.end();

			for ( ; itr != endItr ; itr++ )
			{
				CrystalEnchantOption* pOrdinaryCrystalEnchantOption = *itr;
				if ( pOrdinaryCrystalEnchantOption->getRatio() > ratio )
				{
					newThirdOption = pOrdinaryCrystalEnchantOption->getOptionType();
					break;
				}
			}

			if (newThirdOption == 0)
			{
				sendEnchantImpossible( pGamePlayer );
				return;
			}

			pItem->changeOptionType(thirdOption, newThirdOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			uint optionChange = (thirdOption << 8) | newThirdOption;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			sendEnchantImpossible( pGamePlayer );
		}

		__END_CATCH
	}

	void executeEnchantOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, CoordInven_t invenX, CoordInven_t invenY)
		throw (Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* 		pInventory  = pPC->getInventory();

		// Item?? OptionInfo?? ???ùù?.
		OptionType_t currentOptionType = //pItem->getFirstOptionType();
										pItem->getRandomOptionType();

		int optionSize = pItem->getOptionTypeSize();

		// ?????? ????? itemType?? ????? ???????.
		const ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( 
													pItem->getItemClass(), 
													pItem->getItemType() );

		// ??ùù? ????? ???????? ????????
		// ???? ??????????
		// ????? ???????? ???
		if (optionSize!=1
			|| pItemInfo->isUnique())
		{
			//cout << "no Option " << endl;
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		OptionInfo* pOptionInfo = getOptionInfo( currentOptionType );

		if (pOptionInfo==NULL)
		{
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		// ????? ???????? ???? ??????????? ???? ????..
		// ??? ??? ???????? ?ùù??? ??????
		// upgrade?????? option?? ??ùù...
		bool bUpgradePossible = pOptionInfo->isUpgradePossible();
		if (optionSize>1 || bUpgradePossible)
		{
			//---------------------------------------------------------
			// item?????? 1???? ??ùI ?? ?? ????? ??ùù?.
			// ???????? ??ùP ??????? ?????
			// ?????? ?????? ????????.. client?? ??????? ???.
			// upgrade ????????
			//---------------------------------------------------------
			// ?????? * ?????? / 10000
			int succeedRatio = 0;

			if ( pMouseItem->getItemType() == 15 )
			{
				// ???? ??? 2~ 2003.5.11
				succeedRatio = pOptionInfo->getUpgradeSecondRatio() * pItemInfo->getUpgradeRatio();
			}
			else
			{
				// ???? ??ùù? ?????
				succeedRatio = pOptionInfo->getUpgradeRatio() * pItemInfo->getUpgradeRatio();
			}

//			cout << "??? " << pOptionInfo->getHName() << " ??ùù? ??? : " << succeedRatio << endl;
			
			int dice = rand()%10000;
			bool bSucceed = (dice < succeedRatio);

			/*
			cout << "EnchantCheck = " 
				<< pOptionInfo->getUpgradeRatio() << " * "
				<< pItemInfo->getUpgradeRatio() << " = "
				<< succeedRatio << ", dice = " << dice << " , succeed=" << (int)bSucceed << endl;
			*/

			//cout << "Before = " << getOptionTypeToString( pItem->getOptionTypeList() ) << endl;
			//cout << "CurrentOptionType = " << (int)currentOptionType << endl;

			// ????? ?????? ?????? ??ùù ???? ?????? ????? upgrade?? ????????? ???ùù? ????.
			// ???? if???? optionSize>1 ?? ????? ??????.. bUpgradePossible?? ??ùù ????,
			// optionSize>1 ???? ????? ?? ???.
			if (bUpgradePossible
				&& pMouseItem->getNum()==1
				&& bSucceed)
			{
//				cout << "??ùù? ????" << endl;
				processUpgradeOptionType( pItem, currentOptionType, pOptionInfo );

				OptionType_t upgradeOptionType = pOptionInfo->getUpgradeType();
				uint optionChange = (currentOptionType << 24) | (upgradeOptionType << 16);

				sendEnchantOK( pGamePlayer, optionChange );

				//cout << gcAddItemToItemVerify.toString().c_str() << endl;
			}
			//---------------------------------------------------------
			// upgrade ????
			//---------------------------------------------------------
			else
			{
				//cout << "Failed!" << endl;
				bool bCrashItem = pItemInfo->isUpgradeCrash();

				// ???? packet : ???????? ?ùù??????? ????? ????
				GCAddItemToItemVerify gcAddItemToItemVerify;

				if ( bCrashItem )
				{
					// ?????? ?ùù??? ???? ItemTrace Log ?????? ??.??; 
					if ( pItem != NULL && pItem->isTraceItem() )
					{
						remainTraceLog( pItem, pCreature->getName(), "GOD", ITEM_LOG_DELETE, DETAIL_ENCHANT);
					}

					crashItem( pItem, pInventory, invenX, invenY );

					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH );

					//cout << "Broken" << endl;
				}
				else
				{
					downgradeOptionType( pItem, currentOptionType, pOptionInfo );

					OptionType_t previousOptionType = pOptionInfo->getPreviousType();
					uint optionChange = (currentOptionType << 24) | (previousOptionType << 16);

					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE );
					gcAddItemToItemVerify.setParameter( optionChange );
				}

				pGamePlayer->sendPacket( &gcAddItemToItemVerify );

				//cout << "Upgrade Failed " << (bCrashItem? ": Crash!":" ") << endl;
			}

			// ??? ??? ?? ????
			pPC->deleteItemFromExtraInventorySlot();

			// DB???? ????
			pMouseItem->destroy();

			// ?????? ????
			SAFE_DELETE(pMouseItem);
		}
		else
		{
			sendEnchantImpossible( pGamePlayer );
			//cout << "upgrade impossible" << endl;
		}

		__END_CATCH
	}

	// ???? ??????? ????? ????????.
	void executeAddOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType)
		throw (Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		// ??????? ????? ??? ???? ?????..
/*		if (!pGamePlayer->isPayPlaying()
			&& !pGamePlayer->isPremiumPlay())
		{
			//cout << "no Option " << endl;
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}
*/

		// ?? ??????? ???? ?? ??? ???????????..
		// ????? ????
		// ??????? ????
		// ???ùJ?? ????? ??? ??? ??ùù ??? ????? ???????.
		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize()==0 
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()!=47)
		{
			// ??? ????
			pItem->addOptionType( addOptionType );
			
			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );

			// DB????
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			uint optionChange = addOptionType;

			// ??? ??? ?? ????
			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 					// DB???? ????
			SAFE_DELETE(pMouseItem); 				// ?????? ????


			// ????packet : upgrade?? option?? ??????.
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			//cout << gcAddItemToItemVerify.toString().c_str() << endl;
		}
		else
		{
			//cout << "Enchant Impossible" << endl;
			//cout << "impossibleOption : " << (int)currentOptionType << endl;
			// upgrade ?? ?? ???? option???.
			// ??? packet
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			//cout << "upgrade impossible" << endl;
		}

		__END_CATCH
	}

// 20080218 by Kim Sung Yong
	void executeCrystalAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		OptionType_t  targetOption = 0;
		int ratio = rand() % 10000;

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize() < 1 
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()!=47)
		{
			const list<CrystalEnchantOption*>& CrystalEnchantOptions = g_pOptionInfoManager->getClystalEnchantOptionList();
			list<CrystalEnchantOption*>::const_iterator itr = CrystalEnchantOptions.begin();
			list<CrystalEnchantOption*>::const_iterator endItr = CrystalEnchantOptions.end();

			for ( ; itr != endItr ; itr++ )
			{
				CrystalEnchantOption* pCrystalEnchantOption = *itr;
				if ( pCrystalEnchantOption->getRatio() > ratio )
				{
					targetOption = pCrystalEnchantOption->getOptionType();
					break;
				}
			}

			pItem->addOptionType(targetOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			// DB..
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);
			uint optionChange = targetOption;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		
			cout << "ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK ////////// " << optionChange << "//////////////// T_T;;;" << endl;
		}
		
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}

		__END_CATCH

	}
	
	// 20080218 by Kim Sung Yong
	void executeOneOpCrystalAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);


		OptionType_t targetOption = 0;
		uint optionChange = 0;
		int ratio = rand() % 10000;

		if (isPossibleOptionItemClass(pItem->getItemClass())	
			&& pItem->getOptionTypeSize() == 1 
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()!=47)
		{
			list<OptionType_t> optionList = pItem->getOptionTypeList();
			list<OptionType_t>::const_iterator itr_op = optionList.begin();

			OptionType_t firstOption = *itr_op;

			const list<CrystalEnchantOption*>& CrystalEnchantOptions = g_pOptionInfoManager->getClystalEnchantOptionList();

			list<CrystalEnchantOption*>::const_iterator itr = CrystalEnchantOptions.begin();
			list<CrystalEnchantOption*>::const_iterator endItr = CrystalEnchantOptions.end();

			for ( ; itr != endItr ; itr++ )
			{
				CrystalEnchantOption* pCrystalEnchantOption = *itr;
				if ( pCrystalEnchantOption->getRatio() > ratio )
				{
					targetOption = pCrystalEnchantOption->getOptionType();
					break;
				}
			}
		
			pItem->changeOptionType(firstOption, targetOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			// DB..
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			optionChange = targetOption;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			cout << "ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK ///////// " << targetOption << "//////////////////" << endl;
		}
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		__END_CATCH

	}

	// Blue Crystal (EventStar 81) ù BlueCrystalEnchantInfo pool
	static OptionType_t pickBlueCrystalEnchantOption()
	{
		int ratio = rand() % 10000;
		const list<CrystalEnchantOption*>& blueCrystalOptions = g_pOptionInfoManager->getBlueCrystalEnchantOptionList();
		list<CrystalEnchantOption*>::const_iterator itr = blueCrystalOptions.begin();
		list<CrystalEnchantOption*>::const_iterator endItr = blueCrystalOptions.end();

		for ( ; itr != endItr ; itr++ )
		{
			CrystalEnchantOption* pBlueCrystalEnchantOption = *itr;
			if ( pBlueCrystalEnchantOption->getRatio() > ratio )
			{
				return pBlueCrystalEnchantOption->getOptionType();
			}
		}

		return 0;
	}

	void executeBlueCrystalAddSecondOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize() == 1
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==81)
		{
			OptionType_t targetOption = pickBlueCrystalEnchantOption();
			if (targetOption == 0)
			{
				sendEnchantImpossible( pGamePlayer );
				return;
			}

			pItem->addOptionType(targetOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE(pMouseItem);

			sendEnchantOK( pGamePlayer, targetOption );
		}
		else
		{
			sendEnchantImpossible( pGamePlayer );
		}

		__END_CATCH
	}

	void executeBlueCrystalRerollTwoOptions(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize() == 2
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==81)
		{
			list<OptionType_t> optionList = pItem->getOptionTypeList();
			list<OptionType_t>::const_iterator Opitr = optionList.begin();

			OptionType_t firstOption = *Opitr++;
			OptionType_t secondOption = *Opitr;

			OptionType_t newFirstOption = pickBlueCrystalEnchantOption();
			OptionType_t newSecondOption = pickBlueCrystalEnchantOption();
			if (newFirstOption == 0 || newSecondOption == 0)
			{
				sendEnchantImpossible( pGamePlayer );
				return;
			}

			pItem->changeOptionType(firstOption, newFirstOption);
			pItem->changeOptionType(secondOption, newSecondOption);

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			uint optionChange = (firstOption << 8) | newFirstOption;
			optionChange |= ((secondOption << 8) | newSecondOption) << 16;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			sendEnchantImpossible( pGamePlayer );
		}

		__END_CATCH
	}

 //20070725 ??????
	void executeAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType) throw(Error)
	{
		__BEGIN_TRY

			Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		// ??????? ????? ??? ???? ?????..
		/*		if (!pGamePlayer->isPayPlaying()
		&& !pGamePlayer->isPremiumPlay())
		{
		//cout << "no Option " << endl;
		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		return;
		}
		*/

		// ?? ??????? ???? ?? ??? ???????????..
		// ????? ????
		// ??????? ????
		// ???ùJ?? ????? ??? ??? ??ùù ??? ????? ???????.
		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getOptionTypeSize()==0 
			&& !pItem->isUnique()
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()!=47)
		{
			int randValue = rand()%100;
//			cout << "??? ??? ??? randValue : " << randValue << endl;
			OptionType_t resultAddOptionType = addOptionType;
			if(addOptionType == 183)
			{
				if(randValue >= 90) // ?????? ??? ATTR+1?? ???? ??? 90% ATTR+2?? ???? ??? 10%
				{
					resultAddOptionType = addOptionType + 1;
				}
			}
			else
			{
				if(randValue < 40 && randValue >= 10) // ?????? ????? +1?? ???? ??? 60%, +2?? ???? ??? 30%, +3?? ???? ??? 10%
				{
					resultAddOptionType = addOptionType + 1;
				}
				else if(randValue < 10)
				{
					resultAddOptionType = addOptionType + 2;
				}
			}
			// ??? ????
			pItem->addOptionType( resultAddOptionType );

			string optionField;
			setOptionTypeToField( pItem->getOptionTypeList(), optionField );

			// DB????
			char pField[80];
			sprintf(pField, "OptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			uint optionChange = resultAddOptionType;

			// ??? ??? ?? ????
			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 					// DB???? ????
			SAFE_DELETE(pMouseItem); 				// ?????? ????


			// ????packet : upgrade?? option?? ??????.
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			//cout << gcAddItemToItemVerify.toString().c_str() << endl;
		}
		else
		{
			//cout << "Enchant Impossible" << endl;
			//cout << "impossibleOption : " << (int)currentOptionType << endl;
			// upgrade ?? ?? ???? option???.
			// ??? packet
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			//cout << "upgrade impossible" << endl;
		}

		__END_CATCH
	}
	void executeNewTunning(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
				

		bool succse = false;
		if (pItem->isTimeLimitItem())
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}

		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
		list<OptionType_t> options;
		if( !g_pItemInfoManager->isPossibleItem(pItem->getItemClass(), pItemInfo->getNextItemType(), options) )
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}

		if( pItemInfo->getNextItemType() == pItem->getItemType() )
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}

		int level = pItemInfo->getReqAdvancedLevel();
		if(level == 51 && pMouseItem->getItemType() == 87)//201 to 211
		{
			succse = true;
		}
		if(level == 61 && pMouseItem->getItemType() == 88)//211 to 221
		{
			succse = true;
		}
		if(level == 71 && pMouseItem->getItemType() == 89)//221 to 231
		{
			succse = true;
		}
		if(level == 81 && pMouseItem->getItemType() == 90 )//231 to 241
		{

			succse = true;
		}
		if(level == 91 && pMouseItem->getItemType() == 92 )//241 to 251
		{

			succse = true;
		}
		if(succse == true)
		{
			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE( pMouseItem );

			pItem->setItemType( pItemInfo->getNextItemType() );
			char buffer[80];
			snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
			pItem->tinysave(buffer);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
			gcAddItemToItemVerify.setParameter( pItem->getItemType() );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}

		__END_CATCH
	}
	void executeHappyNewYear(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
				

		bool succse = false;
		if (pItem->isTimeLimitItem())
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}
		if(pItem->getItemClass()!=Item::ITEM_CLASS_ETHEREAL_CHAIN && pItem->getItemType()<3)
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}

		int levelType = pItem->getItemType();
		
		if(levelType == 12)
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ERROR );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
			return;
		}
		if(levelType>=3 && levelType<=11)//201 to 211
		{
			int ratio = 0;
			switch(levelType)
			{
				case 3://1
				ratio = 90;
				break;

				case 4://2
				ratio = 70;
				break;

				case 5://3
				ratio = 60;
				break;

				case 6://4
				ratio = 30;
				break;

				case 7://5
				ratio = 10;
				break;

				case 8://6
				ratio = 5;
				break;

				case 9://7
				ratio = 3;
				break;

				case 10://8
				ratio = 2;
				break;

				case 11://9
				ratio = 1;
				break;

			}

			if(pMouseItem->getItemType() == 93)
			{
				ratio+=5;
			}
			
			if(pMouseItem->getItemType() == 4)
			{
				ratio+=10;
			}

			int ratio2 = Random(1, 100);

			if(ratio>ratio2)
			{
				succse = true;
			}
			
		}

		if(succse == true)
		{
			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE( pMouseItem );

			pItem->setItemType( levelType+1 );
			char buffer[80];
			snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
			pItem->tinysave(buffer);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
			gcAddItemToItemVerify.setParameter( pItem->getItemType() );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		else
		{
			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy();
			SAFE_DELETE( pMouseItem );

			pItem->setItemType( levelType );
			char buffer[80];
			snprintf(buffer, 80, "ItemType=%u", pItem->getItemType() );
			pItem->tinysave(buffer);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_TUNNING_OK );
			gcAddItemToItemVerify.setParameter( pItem->getItemType() );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}

		__END_CATCH
	}
	void executeTransKit(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
				
		GCAddItemToItemVerify gcResult;
		uint optionChange;

		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() );
		if ( pItemInfo->getReqGender() == GENDER_BOTH || pItemInfo->getReqGender() == GENDER_MAX
			|| pItemInfo->isUnique() 
			|| pMouseItem->getNum() != 1 )
		{
			gcResult.setCode( ADD_ITEM_TO_ITEM_VERIFY_TRANS_IMPOSSIBLE );
			pGamePlayer->sendPacket(&gcResult);
			return;
		}

		ItemType_t newItemType = pItem->getItemType() ^ 1;
		optionChange = ( pItem->getItemType() << 16 ) | newItemType;
		pItem->setItemType( newItemType );

		char query[50];
		sprintf(query, "ItemType=%u", newItemType );
		pItem->tinysave(query);

		gcResult.setCode( ADD_ITEM_TO_ITEM_VERIFY_TRANS_OK );
		gcResult.setParameter( optionChange );
		pGamePlayer->sendPacket(&gcResult);

		pPC->deleteItemFromExtraInventorySlot();
		pMouseItem->destroy();
		SAFE_DELETE(pMouseItem);

		__END_CATCH
	}

	OptionInfo* getOptionInfo(OptionType_t optionType)
	{
		OptionInfo* pOptionInfo = NULL;

		try {
			pOptionInfo = g_pOptionInfoManager->getOptionInfo( optionType );

			if (pOptionInfo==NULL)
			{
				filelog("optionError.txt", "CGAddItemToItemHandler: no option [%d]", optionType);

				return NULL;
			}
		} catch (NoSuchElementException& t) {
			filelog("optionError.txt", "CGAddItemToItemHandler: no option [%d]", optionType);

			return NULL;
		}

		return pOptionInfo;
	}


	void	processUpgradeOptionType2(
					Item* pItem,
					OptionType_t currentOptionType,
					OptionInfo* pOptionInfo)
		throw (Error)
	{
		__BEGIN_TRY

		//cout << "Succeed!" << endl;
		// upgrage?? option
		OptionType_t upgradeOptionType = pOptionInfo->getUpgradeType2();

		// ??? ????
		pItem->changeOptionType( currentOptionType, upgradeOptionType );

		//cout << "Succeed = " << getOptionTypeToString( pItem->getOptionTypeList() );
		
		string optionField;
		setOptionTypeToField( pItem->getOptionTypeList(), optionField );

		//list<OptionType_t> op;
		//setOptionTypeFromField( op, optionField );
		//cout << "Check = " << getOptionTypeToString( op ) << endl;

		// DB????
		char pField[80];
		sprintf(pField, "OptionType='%s'", optionField.c_str());
		pItem->tinysave(pField);

		__END_CATCH
	}
	
		void	processUpgradeOptionType(
					Item* pItem,
					OptionType_t currentOptionType,
					OptionInfo* pOptionInfo)
		throw (Error)
	{
		__BEGIN_TRY

		//cout << "Succeed!" << endl;
		// upgrage?? option
		OptionType_t upgradeOptionType = pOptionInfo->getUpgradeType();

		// ??? ????
		pItem->changeOptionType( currentOptionType, upgradeOptionType );

		//cout << "Succeed = " << getOptionTypeToString( pItem->getOptionTypeList() );
		
		string optionField;
		setOptionTypeToField( pItem->getOptionTypeList(), optionField );

		//list<OptionType_t> op;
		//setOptionTypeFromField( op, optionField );
		//cout << "Check = " << getOptionTypeToString( op ) << endl;

		// DB????
		char pField[80];
		sprintf(pField, "OptionType='%s'", optionField.c_str());
		pItem->tinysave(pField);

		__END_CATCH
	}
	
		void	processUpgradeOptionType3(
					Item* pItem,
					OptionType_t currentOptionType,
					OptionInfo* pOptionInfo)
		throw (Error)
	{
		__BEGIN_TRY

		//cout << "Succeed!" << endl;
		// upgrage?? option
		OptionType_t upgradeOptionType = pOptionInfo->getUpgradeType3();

		// ??? ????
		pItem->changeOptionType( currentOptionType, upgradeOptionType );

		//cout << "Succeed = " << getOptionTypeToString( pItem->getOptionTypeList() );
		
		string optionField;
		setOptionTypeToField( pItem->getOptionTypeList(), optionField );

		//list<OptionType_t> op;
		//setOptionTypeFromField( op, optionField );
		//cout << "Check = " << getOptionTypeToString( op ) << endl;

		// DB????
		char pField[80];
		sprintf(pField, "OptionType='%s'", optionField.c_str());
		pItem->tinysave(pField);

		__END_CATCH
	}

	void	crashItem(Item* pItem, Inventory* pInventory, CoordInven_t invenX, CoordInven_t invenY)
		throw (Error)
	{
		__BEGIN_TRY

		// inventory???? ?????? ????
		pInventory->deleteItem( invenX, invenY );

		// DB???? ????
		pItem->destroy();

		// ?????? ????
		SAFE_DELETE(pItem);

		__END_CATCH
	}

	void	downgradeOptionType(
					Item* pItem,
					OptionType_t currentOptionType,
					OptionInfo* pOptionInfo)
		throw (Error)
	{
		__BEGIN_TRY

		// ???????? ????? ???? ????????.
		OptionType_t previousOptionType = pOptionInfo->getPreviousType();

		//cout << "FailBefore : " << getOptionTypeToString(pItem->getOptionTypeList()) << endl;

		// ??? ????
		// ???? ????? ????? ??? ????? ?????.
		if (previousOptionType==0)
		{
			pItem->removeOptionType( currentOptionType );
			//cout << "Remove = " << getOptionTypeToString( pItem->getOptionTypeList() ) << endl;
		}
		else
		{
			pItem->changeOptionType( currentOptionType, previousOptionType );
			//cout << "Down = " << getOptionTypeToString( pItem->getOptionTypeList() ) << endl;
		}

		// DB????
		string optionField;
		setOptionTypeToField( pItem->getOptionTypeList(), optionField );

		//cout << "FailAfter : " << getOptionTypeToString(pItem->getOptionTypeList()) << endl;

		char pField[80];
		sprintf(pField, "OptionType='%s'", optionField.c_str());
		pItem->tinysave(pField);


		__END_CATCH
	}


	void	sendEnchantImpossible(GamePlayer* pGamePlayer)
		throw (Error)
	{
		__BEGIN_TRY

		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );

		__END_CATCH
	}

	void	sendEnchantOK(GamePlayer* pGamePlayer, DWORD optionChange)
		throw (Error)
	{
		__BEGIN_TRY

		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
		gcAddItemToItemVerify.setParameter( optionChange );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );

		__END_CATCH
	}

	void yellowDropLog(PlayerCreature* pPC, Item* pItem, int ratio, int success) throw(Error)
	{
		__BEGIN_TRY

		__END_CATCH
	}

#ifdef VERSION_THIRD_ENCHANT_1
	void		sendThirdEnchantOK(GamePlayer* pGamePlayer, list<OptionType_t>& changeOption, BYTE enchantClass) 
		throw (Error)
	{
		__BEGIN_TRY

		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK );
		gcAddItemToItemVerify.setThirdOptionType(changeOption);
		gcAddItemToItemVerify.setThirdEnchantType(enchantClass);
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );

		__END_CATCH

	}
	void		sendThirdEnchantFail(GamePlayer* pGamePlayer, BYTE grade) 
		throw (Error)
	{
		__BEGIN_TRY

		GCAddItemToItemVerify gcAddItemToItemVerify;
		gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL );
		gcAddItemToItemVerify.setGrade( grade );
		pGamePlayer->sendPacket( &gcAddItemToItemVerify );

		__END_CATCH

	}

	void	saveThirdOptionType(Item* pItem)
		throw (Error)
	{
		__BEGIN_TRY

		string optionField;
		setOptionTypeToField( pItem->getThirdOptionTypeList(), optionField );

		// DB????
		char pField[80];
		sprintf(pField, "ThirdOptionType='%s'", optionField.c_str());
		pItem->tinysave(pField);
		cout << "pField : " << pField << endl;

		__END_CATCH
	}


	void executeThirdEnchantOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem, OptionType_t addOptionType)
		throw (Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);
		Inventory* 		pInventory  = pPC->getInventory();

		const ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( 
			pItem->getItemClass(), 
			pItem->getItemType() );

		if (pItem->getOptionTypeSize()!=2
			|| pItemInfo->isUnique()
			|| pMouseItem->getNum() != 1
			)
		{
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		int iEnchantClass = addOptionType-1;
		if(iEnchantClass < ThirdEnchantInfo::ENCHANT_CLASS_MELEE || iEnchantClass > ThirdEnchantInfo::ENCHANT_CLASS_MAX)
		{
			filelog("thirdEnchant.log", "addOptionType wrong value");
			Assert(false);
		}

		list<OptionType_t> optionList = pItem->getThirdOptionTypeList();
		list<OptionType_t> changeOptionList;
	
		int successRatio = rand()%100;
		int defenseRatio = rand()%100;
		int bSuccess = false;
		int ibeforeGrade = pItem->getGrade();
		int iAfterGrade = pItem->getGrade();
		
		int ibeforeThirdOptionNum = g_pThirdEnchantInfo->getThirdOptionNumOptionList(iEnchantClass, optionList) + 1;
		int iAfterThirdOptionNum = -1;
		int iClientDeleteCode = 1;
		int AddRatioValue = 0;
		bool Lilith2Series = false;

		if(optionList.size() > 0)
		{
			int iEnchantedClass = g_pThirdEnchantInfo->getEnchantClassThirdOption(optionList);
			if(iEnchantClass != iEnchantedClass)
			{
				cout << "Enchanted class : " << iEnchantedClass << ", EnchantClass : " << iEnchantClass << "different" << endl;
				sendEnchantImpossible( pGamePlayer );
				return;
			}
		}
		
		if(pMouseItem->getItemType() == 60 || pMouseItem->getItemType() == 61 || pMouseItem->getItemType() == 62 )
		{
			ibeforeThirdOptionNum = g_pThirdEnchantInfo->getThirdOptionNumOptionList(iEnchantClass, optionList) + 1;
				
			if(ibeforeThirdOptionNum > 4)
			{
				int iFailCode = 99;
				sendThirdEnchantFail(pGamePlayer, iFailCode);
				return;
			}
			Lilith2Series = true;
			successRatio = 0;
		}
		if(pMouseItem->getItemType() == 70 || pMouseItem->getItemType() == 71 || pMouseItem->getItemType() == 72 )
		{
			ibeforeThirdOptionNum = g_pThirdEnchantInfo->getThirdOptionNumOptionList(iEnchantClass, optionList) + 1;
				
			if(ibeforeThirdOptionNum > 13)
			{
				int iFailCode = 70;
				sendThirdEnchantFail(pGamePlayer, iFailCode);
				return;
			}
			
			successRatio = 0;
		}		
		if(!g_pItemClassInfo->hasEnchantClass(pItem->getItemClass(), (int)addOptionType))
		{
			cout << "item wrong enchant class pItem->getItemClass(): " << pItem->getItemClass() << ", addOptionType : " << (int)addOptionType << endl;
			sendEnchantImpossible( pGamePlayer );
			return;
		}

		if(pInventory->hasEnoughNumItem(Item::ITEM_CLASS_EVENT_STAR, 75, 1) && Lilith2Series == false)
		{	
			pInventory->decreaseNumItem(Item::ITEM_CLASS_EVENT_STAR, 75, 1, pCreature->getPlayer());
			AddRatioValue = 25;
		}
		if(successRatio < (g_pThirdEnchantInfo->getUpdadeRatio(iEnchantClass, optionList) + AddRatioValue))
		{
			if(!g_pThirdEnchantInfo->getNextThirdOptionType(iEnchantClass, optionList, changeOptionList))
			{
				cout << "Function getNextThirdOptionType failed" << endl;
				filelog("thirdEnchant.log", "Function getNextThirdOptionType failed");
				sendEnchantImpossible( pGamePlayer );
				return;
			}
			cout << "ThirdEnchant Succeed!!! " << endl;
			pItem->setThirdOptionType(changeOptionList);
			saveThirdOptionType(pItem);
			ibeforeGrade = pItem->getGrade();
			iAfterGrade = pItem->getGrade();
			bSuccess = true;
			
			iAfterThirdOptionNum = g_pThirdEnchantInfo->getThirdOptionNumOptionList(iEnchantClass, optionList) + 1;
		}
		else
		{
			if(pInventory->hasEnoughNumItem(Item::ITEM_CLASS_EVENT_STAR, 76, 1) && optionList.size() != 0)
			{
				pInventory->decreaseNumItem(Item::ITEM_CLASS_EVENT_STAR, 76, 1, pCreature->getPlayer());
				
				cout<< "FailPenalty Fail :: Lilith Series :: FailPenalty Fail!!" << endl;
				
				if(defenseRatio > 50)
				{
					
					GCAddItemToItemVerify gcAddItemToItemVerify;
					gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_THIRDOPTION_PENALTY_DEFENSE );
					pGamePlayer->sendPacket( &gcAddItemToItemVerify );
					
					pPC->deleteItemFromExtraInventorySlot();
					pMouseItem->destroy();
	
					SAFE_DELETE(pMouseItem);
					
					return;
				}
				else
				{
					iClientDeleteCode = 0;
					iAfterThirdOptionNum = -1;
					
					pItem->clearThirdOption();
					saveThirdOptionType(pItem);
					
				}
			}
			else if(optionList.size() == 0)
			{
				ibeforeGrade = pItem->getGrade();
				if(ibeforeGrade > 1)
					pItem->setGrade(ibeforeGrade - 1);
				iAfterGrade = pItem->getGrade();
				
				cout << "beforeGrade : " << ibeforeGrade << endl;
				cout << "AfterGrade : " << iAfterGrade << endl;

				char buffer[80];
				snprintf(buffer, 80, "Grade=%u", pItem->getGrade() );
				pItem->tinysave(buffer);
				
				ibeforeThirdOptionNum = 0;
				iAfterThirdOptionNum = -1;
			}
			else
			{
				iClientDeleteCode = 0;
				iAfterThirdOptionNum = -1;
				
				pItem->clearThirdOption();
				saveThirdOptionType(pItem);
			}
			
			bSuccess = false;
		}

		Statement* pStmt = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery(
			"INSERT INTO LilithSeriesLog (PlayerID, Name, ReportTime, ItemType, ItemID, BeforeLevel, AfterLevel, BeforeGrade, AfterGrade, TargetItemClass, TargetItemType, TargetItemID, Success)"
			" VALUES('%s','%s',now(),%u, %u, %u, %u, %u, %u, '%s', %u, %u, %d )",
														
			pGamePlayer->getID().c_str(),
			pPC->getName().c_str(),
			pMouseItem->getItemType(),
			pMouseItem->getItemID(),
			ibeforeThirdOptionNum,
			iAfterThirdOptionNum + 1,
			ibeforeGrade,
			iAfterGrade,
			ItemClass2ShortString[ (int)(pItem->getItemClass()) ].c_str(),
			pItem->getItemType(),
			pItem->getItemID(),
			bSuccess
			);
			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt);
		
		{
			pPC->deleteItemFromExtraInventorySlot();

			pMouseItem->destroy();

			SAFE_DELETE(pMouseItem);

			if(bSuccess)
				sendThirdEnchantOK(pGamePlayer, changeOptionList, iEnchantClass);
			else if(iClientDeleteCode == 0)
				sendThirdEnchantFail(pGamePlayer, 0);
			else
				sendThirdEnchantFail(pGamePlayer, iAfterGrade);

		}

		__END_CATCH

	}
#endif // VERSION_THIRD_ENCHANT_1

	void executeGaiZaoAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);


		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getGTwoOptionTypeSize() < 1 
			&& !pItem->isUnique()
			&& pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP  
			&& pItem->getItemClass() != Item::ITEM_CLASS_CUE_OF_ADAM
			&& pItem->getItemClass() != Item::ITEM_CLASS_ETHEREAL_CHAIN
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==95)
		{
			

			OptionType_t targetOption = 0;

			 int ratio = rand() % 100+1;
				    //str+6						
			 int a[5][10]={{88,89,90,91,92,253,254,255,256,257},{93,94,95,96,97,258,259,260,261,262},{98,99,100,101,102,263,264,265,266,267},{207,217,218,219,220,229,230,231,232,233},{169,170,171,172,173,268,269,270,271,272}};

			 int seek ;
			 int seek2 = rand()%5;
			 if(ratio <= 100)
				seek = 0;
			 if(ratio <= 50)
				seek = 1;
			 if(ratio <= 30)
				seek = 2;
			 if(ratio <= 15)
				seek = 3;
			 if(ratio <= 10)
				seek = 4;
			 if(ratio <= 6)
				seek = 5;
			 if(ratio <= 5)
				seek = 6;
			 if(ratio <= 4 )
				seek = 7;
			 if(ratio <= 3)
				seek = 8;
			 if(ratio <= 2)
				seek = 9;

				targetOption = a[seek2][seek];

			pItem->addGTwoOptionType(targetOption);

			string optionField;
			setOptionTypeToField( pItem->getGTwoOptionTypeList(), optionField );
			// DB..
			char pField[80];
			sprintf(pField, "GTwooptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);
			uint optionChange = targetOption;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		
			cout << "ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK ////////// " << optionChange << "//////////////// T_T;;;" << endl;
		}
		
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}

		__END_CATCH

	}
	
	// 20080218 by Kim Sung Yong
	void executeOneGaiZaoAddRandomOption(GamePlayer* pGamePlayer, Item* pMouseItem, Item* pItem) throw(Error)
	{
		__BEGIN_TRY

		Creature*  		pCreature   = pGamePlayer->getCreature();
		PlayerCreature* pPC         = dynamic_cast<PlayerCreature*>(pCreature);

		if (isPossibleOptionItemClass(pItem->getItemClass())
			&& pItem->getGTwoOptionTypeSize() ==1 
			&& !pItem->isUnique()
			&& pItem->getItemClass() != Item::ITEM_CLASS_CORE_ZAP  
			&& pItem->getItemClass() != Item::ITEM_CLASS_CUE_OF_ADAM
			&& pItem->getItemClass() != Item::ITEM_CLASS_ETHEREAL_CHAIN
			&& pMouseItem->getNum()==1
			&& pMouseItem->getItemType()==95)
		{

			list<OptionType_t> optionList = pItem->getGTwoOptionTypeList();
			list<OptionType_t>::const_iterator itr_op = optionList.begin();

			OptionType_t firstOption = *itr_op;


		OptionType_t targetOption = 0;
		uint optionChange = 0;
		int ratio = rand() % 100+1;
			    //str+6						
		 int a[5][10]={{88,89,90,91,92,253,254,255,256,257},{93,94,95,96,97,258,259,260,261,262},{98,99,100,101,102,263,264,265,266,267},{207,217,218,219,220,229,230,231,232,233},{169,170,171,172,173,268,269,270,271,272}};

		 int seek ;
		 int seek2 = rand()%5;
			 if(ratio <= 100)
				seek = 0;
			 if(ratio <= 50)
				seek = 1;
			 if(ratio <= 30)
				seek = 2;
			 if(ratio <= 15)
				seek = 3;
			 if(ratio <= 10)
				seek = 4;
			 if(ratio <= 6)
				seek = 5;
			 if(ratio <= 5)
				seek = 6;
			 if(ratio <= 4 )
				seek = 7;
			 if(ratio <= 3)
				seek = 8;
			 if(ratio <= 2)
				seek = 9;

			targetOption = a[seek2][seek];
		
			pItem->changeGTwoOptionType(firstOption, targetOption);

			string optionField;
			setOptionTypeToField( pItem->getGTwoOptionTypeList(), optionField );
			// DB..
			char pField[80];
			sprintf(pField, "GTwooptionType='%s'", optionField.c_str());
			pItem->tinysave(pField);

			optionChange = targetOption;

			pPC->deleteItemFromExtraInventorySlot();
			pMouseItem->destroy(); 
			SAFE_DELETE(pMouseItem);

			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK );
			gcAddItemToItemVerify.setParameter( optionChange );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );

			cout << "ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK ///////// " << targetOption << "//////////////////" << endl;
		}
		else
		{
			GCAddItemToItemVerify gcAddItemToItemVerify;
			gcAddItemToItemVerify.setCode( ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE );
			pGamePlayer->sendPacket( &gcAddItemToItemVerify );
		}
		__END_CATCH

	}

#endif 
