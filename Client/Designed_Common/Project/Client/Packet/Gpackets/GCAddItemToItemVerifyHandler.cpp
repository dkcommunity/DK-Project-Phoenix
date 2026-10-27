//--------------------------------------------------------------------------------
//
// Filename    : GCAddItemToItemVerifyHandler.cpp
// Written By  : elca, Reiot
// Description :
//
//--------------------------------------------------------------------------------
#include "Client_PCH.h"
// include files
#include "GCAddItemToItemVerify.h"
#include "ClientDef.h"
#include "TempInformation.h"
#include "MInventory.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "ClientFunction.h"
#include "SkillDef.h"
#include "MinTR.H"
#include "UIFunction.h"
#include "ServerInfo.h"
#include "Client.h"

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
	#include "../Client/Packet/Cpackets/CGRequestRankGemData.h"
#endif

#ifdef __GAME_CLIENT__
static bool RequireInventoryItemForAddItemToItemVerify(MItem* pInventoryItem, BYTE code, int tempMode, int value1, int value2)
{
	if (pInventoryItem != NULL)
		return true;

	DEBUG_ADD_FORMAT("[GCAddItemToItemVerify] missing target item: code=%d mode=%d value1=%d value2=%d", code, tempMode, value1, value2);
	UI_UnlockItem();
	if (g_pPlayer != NULL)
		g_pPlayer->ClearItemCheckBuffer();
	UI_PopupMessage(STRING_ERROR_ETC_ERROR);
	return false;
}
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddItemToItemVerifyHandler::execute ( GCAddItemToItemVerify * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	__BEGIN_DEBUG
		
#ifdef __GAME_CLIENT__
	MItem *pInventoryItem = NULL;

	MItem *pItem[2] = { NULL, NULL };
	const int tempMode = g_pTempInformation->GetMode();
	const int tempValue1 = g_pTempInformation->Value1;
	const int tempValue2 = g_pTempInformation->Value2;

	if(tempMode == TempInformation::MODE_ITEM_TO_ITEM)
	{
		pInventoryItem = g_pInventory->GetItem(g_pTempInformation->Value1, g_pTempInformation->Value2);
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		DEBUG_ADD("rallser:MODE_NULL");
	} else if( tempMode == TempInformation::MODE_ITEM_MIXING )
	{
		pItem[0] = g_pInventory->GetItem( g_pTempInformation->Value1, g_pTempInformation->Value2 );
		pItem[1] = g_pInventory->GetItem( g_pTempInformation->Value3, g_pTempInformation->Value4 );

		g_pTempInformation->SetMode( TempInformation::MODE_NULL);
	}		
	else
		DEBUG_ADD("[ERROR] TempInformationMode");


	switch(pPacket->getCode())
	{
		case ADD_ITEM_TO_ITEM_VERIFY_ERROR:
			// Protocol ERROR: server extra'yi cantaya GCCreateItem ile iade eder.
			// Mouse'ta kalan tas duplicate olmasin diye once DropItem.
			// C_VS_UI_MOUSE_POINTER::DropItem item'i dereference etmez; bos mouse no-op.
			UI_PopupMessage( STRING_ERROR_ETC_ERROR );
			UI_DropItem();
			UI_UnlockItem();
			g_pPlayer->ClearItemCheckBuffer();
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE:
			// ui error dialog
			UI_PopupMessage( STRING_MESSAGE_ITEM_TO_ITEM_IMPOSIBLE );
			UI_UnlockItem();
			g_pPlayer->ClearItemCheckBuffer();
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH:
			//----------------------------------------------------
			// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
			//----------------------------------------------------
			if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
				break;
			AddNewInventoryEffect( pInventoryItem->GetID(),
				MAGIC_ENCHANT_REMOVE_ITEM,
				0
				);
			// »ç¿îµå??
			PlaySound(SOUND_XMAS_STAR);
			UI_DropItem();
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE:
			{
				DWORD OptionType = pPacket->getParameter();

				//----------------------------------------------------
				// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
				//----------------------------------------------------
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_NULL,
					0, OptionType
					);
				
				// »ç¿îµå??
				PlaySound(SOUND_XMAS_STAR);
				UI_DropItem();
			}
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK:
			{
				DWORD OptionType = pPacket->getParameter();
				//----------------------------------------------------
				// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
				//----------------------------------------------------

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_PLUS,
					0, OptionType
					);

//				pInventoryItem->SetItemOption(OptionType);
				UI_DropItem();

			}
			break;
#if __CONTENTS(__2008_FIRST_CHARGE_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_CHANGE_ENCHANT_OK:				//Å©¸®½ºÅ» ¾ÆÀÌÅÛ
			{
				DWORD OptionType = pPacket->getParameter();

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_PLUS,
					0, OptionType
					);
				UI_DropItem();
				break;
			}
#endif//	__2008_FIRST_CHARGE_ITEM
			
#if __CONTENTS(__TUNING_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_TUNING_OK:
			{
				DWORD tType = pPacket->getParameter();

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_PLUS,
					0, tType
					);
				UI_DropItem();
				break;
			}
#endif	//__TUNING_ITEM

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_NEW_OPTION:
			{
				DWORD OptionType = pPacket->getParameter();

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_CLIENT_NEWREINFORCE_OPTION_PLUS,
					0, OptionType
					);
				UI_DropItem();
				break;
			}

		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM:
			if(g_pPlayer->IsSlayer())
				UI_PopupMessage( STRING_MESSAGE_ITEM_TO_ITEM_FAIL_NO_PREMIUM_SLAYER );
			else
				UI_PopupMessage( STRING_MESSAGE_ITEM_TO_ITEM_FAIL_NO_PREMIUM_VAMPIRE );
			UI_UnlockItem();
			g_pPlayer->ClearItemCheckBuffer();
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_TRANS_OK :				// ¼ºÀüÈ¯ ¼º°ø
			{
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID() ,
					MAGIC_TRANS_ITEM_OK,
					0, 0 );
				UI_DropItem();
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_TRANS_IMPOSSIBLE :		// ¼ºÀüÈ¯ ½ÇÆÐ
			{
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK :
			{
				if((pItem[0]!=NULL && pItem[0]->GetItemClass()==ITEM_CLASS_CORE_ZAP) || (pItem[1] !=NULL && pItem[1]->GetItemClass()==ITEM_CLASS_CORE_ZAP)	 )
				{
					UI_OkMixingForgeCoreZap( pPacket->getParameter(), pItem[0], pItem[1] );
					g_pTempInformation->SetMode( TempInformation::MODE_NULL );
					g_pTempInformation->Value1 = 0;
					g_pTempInformation->Value2 = 0;
					g_pTempInformation->Value3 = 0;
					g_pTempInformation->Value4 = 0;
					g_pTempInformation->pValue = NULL;
				}
				else if((pItem[0]!=NULL && pItem[0]->GetItemClass()==ITEM_CLASS_CUE_OF_ADAM) || (pItem[1] !=NULL && pItem[1]->GetItemClass()==ITEM_CLASS_CUE_OF_ADAM)	 )
				{
					UI_OkMixingForgeCoreZap( pPacket->getParameter(), pItem[0], pItem[1] );
					g_pTempInformation->SetMode( TempInformation::MODE_NULL );
					g_pTempInformation->Value1 = 0;
					g_pTempInformation->Value2 = 0;
					g_pTempInformation->Value3 = 0;
					g_pTempInformation->Value4 = 0;
					g_pTempInformation->pValue = NULL;
				}
				else
				{
					UI_OkMixingForge( pPacket->getParameter(), pItem[0], pItem[1] );

					g_pTempInformation->SetMode( TempInformation::MODE_NULL );
					g_pTempInformation->Value1 = 0;
					g_pTempInformation->Value2 = 0;
					g_pTempInformation->Value3 = 0;
					g_pTempInformation->Value4 = 0;
					g_pTempInformation->pValue = NULL;
				}
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK :
			if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
				break;

			UI_OkRemoveOption( pPacket->getParameter(), pInventoryItem );
			UI_DropItem();
			g_pPlayer->ClearItemCheckBuffer();
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_MIXING_FAILED_SAME_OPTION_GROUP :
			UI_PopupMessage( STRING_MESSAGE_MIXING_FORGE_FAILED_SAME_OPTION_GROUP );
			break;
			
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL:
			{
				MItem* pItem = UI_GetMouseItem();
				if(NULL != pItem)
				{
					if(pItem->GetItemClass() == ITEM_CLASS_EVENT_STAR &&
						(pItem->GetItemType() == 22||pItem->GetItemType() == 48
						|| pItem->GetItemType() == 57 || pItem->GetItemType() == 91 ||(pItem->GetItemType() >= 63 && pItem->GetItemType() <= 69)
						)) // ¿»·Î¿ì µå·Ó & Àú±Þ ÀÎÃ¾Æ® ¾ÆÀÌÅÛ
					{
						g_pPlayer->ClearItemCheckBuffer();
						UI_PopupMessage(STRING_MESSAGE_ENCHANT_FAIL);

//						AddNewInventoryEffect( pInventoryItem->GetID() ,
//							MAGIC_ENCHANT_OPTION_NULL,
//							0, pInventoryItem->GetGrade() );

					}
					else
					{
						if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
							break;
						AddNewInventoryEffect( pInventoryItem->GetID() ,
							MAGIC_ENCHANT_OPTION_NULL,
							0, 0 );
					}
					UI_DropItem();
				}
//				AddNewInventoryEffect( pInventoryItem->GetID() ,
//					MAGIC_ENCHANT_OPTION_NULL,
//					0, 0 );
//				UI_DropItem();
			}
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
			{
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;

				DWORD OptionType = pPacket->getParameter();
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_PLUS,
					0, OptionType
					);
				
	//			pInventoryItem->SetCurrentDurability(pPacket->getParameter());
				UI_DropItem();
	//			g_pPlayer->ClearItemCheckBuffer();
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_CLEAR_OPTION_OK:
			{
				DWORD OptionType = pPacket->getParameter();
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
				MAGIC_ENCHANT_OPTION_NULL,
				0, OptionType
				);
				PlaySound(SOUND_XMAS_STAR);
//				pInventoryItem->RemoveItemOption(g_pTempInformation->Value3);

				UI_DropItem();
				UI_PopupMessage( STRING_MESSAGE_SUCCESS_CHANGE );
			}
			break;
			
		case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
			{

				DWORD ItemGrade = pPacket->getParameter();

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				pInventoryItem->SetGrade(ItemGrade);
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_PLUS,
					0, ItemGrade
					);

				UI_DropItem();
				UI_PopupMessage( STRING_MESSAGE_SUCCESS_CHANGE );
			}
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_PET_MAGIC_ENCHANT_OK:
			{
				DWORD MagicRemainSec = pPacket->getParameter();

				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				UI_RemoveEffectStatus(EFFECTSTATUS_CLIENT_MAGICAL_PET_CHANGER) ;
				UI_AddEffectStatus(EFFECTSTATUS_CLIENT_MAGICAL_PET_CHANGER , MagicRemainSec << 4) ;

				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_NULL,
					0, 0
					);

				UI_DropItem();
				
				UI_PopupMessage( STRING_MESSAGE_SUCCESS_CHANGE );
			}
			break;
#if __CONTENTS(__EXPERT_PET_CHANGER)
		case ADD_ITEM_TO_ITEM_VERIFY_PET_EXPERT_ENCHANT_OK:
			{
				DWORD ExpertRemainSec = pPacket->getParameter();


				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				UI_RemoveEffectStatus(EFFECTSTATUS_CLIENT_EXPERT_PET_CHANGER) ;
				UI_AddEffectStatus(EFFECTSTATUS_CLIENT_EXPERT_PET_CHANGER , ExpertRemainSec << 4) ;

				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_OPTION_NULL,
					0, 0
					);

				UI_DropItem();
				
				UI_PopupMessage( STRING_MESSAGE_SUCCESS_CHANGE );
			}
			break;
#endif //__EXPERT_PET_CHANGER
		case ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_LEVEL_FAIL:
			{
				g_pPlayer->ClearItemCheckBuffer();

				UI_PopupMessage( UI_STRING_MESSAGE_PET_ENCHANT_LEVEL_FAIL );
			}
			break;

		case ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_DUPLICATE_FAIL:
			{
				g_pPlayer->ClearItemCheckBuffer();

				UI_PopupMessage( UI_STRING_MESSAGE_ENCHANT_DUPLICATE_FAIL );
			}
			break;
			
		case ADD_ITEM_TO_ITEM_VERIFY_PET_ENCHANT_OK_MESSAGE:
			{
				UI_DropItem();
				g_pPlayer->ClearItemCheckBuffer();

				UI_PopupMessage( UI_STRING_MESSAGE_PET_ENCHANT_OK );
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_FAIL:
			{
				BYTE itemGrade = pPacket->getGrade();
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				if(itemGrade && itemGrade != 99)
				{
					AddNewInventoryEffect( pInventoryItem->GetID() ,
						MAGIC_ENCHANT_OPTION_PLUS,
						0, itemGrade);
					UI_PopupMessage( UI_STRING_MESSAGE_THIRD_ENCHANT_DOWN_GRADE );
				}
				else if(itemGrade == 99)
				{
					UI_PopupMessage(UI_STRING_MESSAGE_THIRD_ENCHANT_2_OVERLEVEL);
					g_pPlayer->ClearItemCheckBuffer();
					break;
				}
				else
				{
					std::list<OptionType_t>* thirdOptionType = &pPacket->getThirdOptionType();
					AddNewInventoryEffect( pInventoryItem->GetID() ,
						MAGIC_ENCHANT_THIRD_OPTION_NULL,
						0, reinterpret_cast<DWORD> (thirdOptionType));
					UI_PopupMessage( UI_STRING_MESSAGE_THIRD_ENCHANT_FAIL );
					g_pPlayer->ClearItemCheckBuffer();
				}
				UI_DropItem();
			}
			break;


		case ADD_ITEM_TO_ITEM_VERIFY_THIRD_ENCHANT_OK:
			{	
				tag_ThirdOptionData ThirdOptionData;
				
				ThirdOptionData.thirdOptionList = &pPacket->getThirdOptionType();
				ThirdOptionData.thirdOptionType = pPacket->getThirdEnchantType();

				//----------------------------------------------------
				// ±â¼ú »ç¿ë ½Ãµµ µ¿ÀÛ
				//----------------------------------------------------
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				AddNewInventoryEffect( pInventoryItem->GetID(),
					MAGIC_ENCHANT_THIRD_OPTION_PLUS,
					0, reinterpret_cast<DWORD> (&ThirdOptionData)
					);

				g_pPlayer->ClearItemCheckBuffer();
				UI_PopupMessage( UI_STRING_MESSAGE_THIRD_ENCHANT_OK );

				UI_DropItem();
			}
			break;
#if __CONTENTS(__ENCHANT_REQUIRE_LEVE)
		case ADD_ITEM_TO_ITEM_VERIFY_NEED_LEVEL_FAIL:
			{
				UI_PopupMessage(STRING_MESSAGE_ITEM_TO_ITEM_IMPOSIBLE);
				g_pPlayer->ClearItemCheckBuffer();
			}
			break;
#endif
		case ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_OK:
			{
				if( pItem[0] == NULL || pItem[1] == NULL )
				{
					DEBUG_ADD_FORMAT("[GCAddItemToItemVerify] missing mixing item: code=%d mode=%d value1=%d value2=%d", pPacket->getCode(), tempMode, tempValue1, tempValue2);
					UI_UnlockItem();
					g_pPlayer->ClearItemCheckBuffer();
					UI_PopupMessage(STRING_ERROR_ETC_ERROR);
					break;
				}
				UI_OKPetMixingForge(pItem[1]);

				g_pTempInformation->SetMode( TempInformation::MODE_NULL );
				g_pTempInformation->Value1 = 0;
				g_pTempInformation->Value2 = 0;
				g_pTempInformation->Value3 = 0;
				g_pTempInformation->Value4 = 0;
				g_pTempInformation->pValue = NULL;
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_PET_MIXING_FAIL:
			{
				UI_ClosePetMixingForge();
				UI_PopupMessage(UI_STRING_MESSAGE_DESC_PETMIXING_FAIL);
			}
			break;
#if __CONTENTS(__TREASURE_BOX)
		case 	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_OK:// »óÀÚ ¿ÀÇÂ ¼º°ø
			{
				g_pPlayer->ClearItemCheckBuffer();
				UI_DropItem();
			}
			break;
		case 	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NOT_ENOUGH_SPACE:// ÀÎº¥Åä¸® °ø°£ÀÌ ºÎÁ·ÇÔ 
			{
				UI_PopupMessage(UI_STRING_MESSAGE_OPEN_CASE_FAIL_NOT_ENOUGH_SPACE);
				g_pPlayer->ClearItemCheckBuffer();
			}
			break;
		case 	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_NO_ITEM:// »óÀÚ¿¡ ¾ÆÀÌÅÛÀÌ ¾øÀ½ 
			{
				UI_PopupMessage(UI_STRING_MESSAGE_OPEN_CASE_FAIL_NO_ITEM);
				g_pPlayer->ClearItemCheckBuffer();
			}
			break;
		case 	ADD_ITEM_TO_ITEM_VERIFY_OPEN_CASE_FAIL_INCORRECT_KEY:// »óÀÚ¿¡ ¸ÂÁö ¾Ê´Â ¿­¼è
			{
				UI_PopupMessage(UI_STRING_MESSAGE_OPEN_CASE_FAIL_INCORRECT_KEY);
				g_pPlayer->ClearItemCheckBuffer();
			}
			break;

#endif //__TREASURE_BOX
#if __CONTENTS(__IMI_LOW_LEVEL_ITEM)
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_DOWN_GRADE:
			{
				DWORD ItemGrade = pPacket->getParameter();
				if (!RequireInventoryItemForAddItemToItemVerify(pInventoryItem, pPacket->getCode(), tempMode, tempValue1, tempValue2))
					break;
				pInventoryItem->SetGrade(ItemGrade);
				UI_PopupMessage( UI_STRING_MESSAGE_THIRD_ENCHANT_DOWN_GRADE );
				UI_DropItem();
				g_pPlayer->ClearItemCheckBuffer();
			}
			break;
#endif //__IMI_NEW_ENCHANT_ITEM
		case ADD_ITEM_TO_RANKGEM_ERROR:
			{
				g_pUIDialog->PopupFreeMessageDlg("¸Ã²å²Û²»ÄÜÏâÇ¶Õâ¸ö·ûÎÄ£¡");
				UI_UnlockItem();
				g_pPlayer->ClearItemCheckBuffer();
				
			}
			break;
		case ADD_ITEM_TO_RANKGEM_OK:
			{
				g_pPlayer->ClearItemCheckBuffer();
				UI_DropItem();
				g_pUIDialog->PopupFreeMessageDlg("¹§Ï²ÄãÏâÇ¶³É¹¦£¡");
				CGRequestRankGemData _CGRequestRankGemData;
				g_pSocket->sendPacket(&_CGRequestRankGemData); 
			}
			break;

		case ADD_ITEM_TO_RANKGEM_REMOVE_OK:
			{
				g_pUIDialog->PopupFreeMessageDlg("ÄúÒÑÕª³ý³É¹¦£¡");
				CGRequestRankGemData _CGRequestRankGemData;
				g_pSocket->sendPacket(&_CGRequestRankGemData); 
			}
			break;

		case ADD_REQUEST_PING_OK:
			{
				g_PingReceivTime = timeGetTime();
				g_Ping = g_PingReceivTime - g_PingSendTime;
			}
			break;
		case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_THIRDOPTION_PENALTY_DEFENSE:
			{
				g_pUIDialog->PopupFreeMessageDlg("ÀòÀòË¿±£»¤ÉúÐ§£¡");
				g_pPlayer->ClearItemCheckBuffer();
				UI_DropItem();
			//	UI_UnlockItem();
			}
			break;
				// ¾ÆÀÌÅÛÀÌ »ç¶óÁö´Â °ÍÀÌ ¹æ¾î µÇ¾ú½À´Ï´Ù.
		}
				// ¾ÆÀÌÅÛÀÌ ±Þ¼ö³ª ¿É¼ÇÀÌ Ãë¼Ò µÇ´Â °ÍÀÌ ¹æ¾î µÇ¾ú½À´Ï´Ù.

				// ¾ÆÀÌÅÛÀÌ ±Þ¼ö³ª ¿É¼ÇÀÌ Ãë¼Ò µÇ´Â °ÍÀÌ ¹æ¾î µÇ¾ú½À´Ï´Ù.
#endif //__IMI_MIXNGFORGE



	__END_DEBUG
	__END_CATCH
}
