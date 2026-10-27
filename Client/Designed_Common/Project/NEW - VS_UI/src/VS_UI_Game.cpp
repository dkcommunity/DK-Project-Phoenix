// VS_UI_Game.cpp

#include "client_PCH.h"
#include "VS_UI_Game.h"
#include "VS_UI_filepath.h"
#include "VS_UI_Message.h"
#include "VS_UI_Mouse_Pointer.h"

#include "MInventory.h"
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "UserOption.h"
#include "VS_UI.h"
#include "crash_trace_tool.h"
#include "UserInformation.h"
#include "VS_UI_GameCommon2.h"
#include "VS_UI_TaxRate.H"
#include "VS_UI_WorldMap.h"
#include "VS_UI_Operator.h"
#include "VS_UI_TIMECOUNT.h"
#include "VS_UI_Usergrade.h"
// 2026-05-12: SkillInfoWindow concrete tip — new/method calls icin.
// Forward decl VS_UI_Game.h'da pointer field icin yeterli, ama RunSkillView
// (new) + CloseSkillView (Finish/Running method) implementation'lari concrete
// tanim ister. Header guard race nedeniyle dolayli include yetersiz kaldi.
#include "VS_UI_SKILL_VIEW.h"


#include "SystemAvailabilities.h"
#include "SkinManager.h"
#include "MGameStringTable.h"
#include "../Client/Packet/Properties.h"
#include "../Client/UIFunction.h"
#include <stdarg.h>
#include <stdio.h>

//#include "ex\DebugInfo.h"

#include "MPlayer.h"
#include "MPriceManager.h"
#include "MilitarySlot.h"
#include "VS_UI_Friend_System.h"

#include "crash_trace_tool.h"

#define	MAX_SOUND_VOLUME		100
#define	MAX_MUSIC_VOLUME		100
#define	MAX_GAMMA_VALUE			100 

namespace
{
static void Phoenix_LogSlayerPortalError(const char *format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	buffer[sizeof(buffer) - 1] = '\0';
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
}
}


//----------------------------------------------------------------------------
// Globals
//
// C_VS_UI_GAME Object�� �����߿� ���� �� ��ȿ�ϴ�.
//----------------------------------------------------------------------------
bool gbl_item_lock; // no static member, 'static GLOBAL!'
						  // �ʱ�ȭ�� C_VS_UI_GAME::Start()���� ���ش�.
bool gbl_gear_lock;

bool gbl_item_trade_lock;

// shop���� ������ �춧�� �칰�ǿ� ���� price�� ���������ϰ�,
// �� ���� ���� �׷����Ѵ�.
bool gbl_sell_running; // ��� ��..
bool gbl_buy_running; // �Ĵ� ��..
bool gbl_repair_running; // ��ġ�� ��...
bool gbl_silvering_running; // ������ �ϴ���
bool gbl_buy_storage_running; // ��ġ�� ��...
bool gbl_exchange_ask_running; // ��ȯ�ҷ�? �� �ִ� ��.. --;		// by sigi
bool gbl_exchange_cancel_running; // ��ȯ����ҷ�? �� �ִ� ��.. --;	// by sigi
bool gbl_party_ask_running; // ��Ƽ�ҷ�? �� �ִ� ��.. --;
bool gbl_party_cancel_running; // ��Ƽ����ҷ�? �� �ִ� ��.. --;
bool gbl_enchant_running; // Enchant�ҷ�? �� �ִ� ��.. --;
bool gbl_use_pet_food_running; // �� ���� ���Ϸ�? �� �ִ���
bool gbl_keep_petitem_running; // �� �±淡? �� �ִ���
bool gbl_get_keep_petitem_running; // �� ã��? �� �ִ���
bool gbl_use_askitem_running; // �������� item ����ҷ�? �� �ִ���
bool gbl_persnalshop_running; // ���λ��� UI ���ִ� ��..
bool gbl_Delegation_Of_Power_running; // �������� item ����ҷ�? �� �ִ���


//bool gbl_option_running; // by sigi
bool gbl_tutorial_exit_ask_running; // ���⼱���Ѱ� �ȹٲٱ� ������? ���ִ���..		//by larosel
bool gbl_no_search_result_running; // �˻���� ���� �� �ִ� ��
bool gbl_deposit_limit_running; // �˻���� ���� �� �ִ� ��
bool gbl_withdraw_limit_running; // �˻���� ���� �� �ִ� ��
bool gbl_exchange_limit_running; // �˻���� ���� �� �ִ� ��
bool gbl_bring_fee_limit_running; // �˻���� ���� �� �ִ� ��
bool gbl_getitem_from_itemlist_running;	// ������ ����Ʈ���� ������ �����ö�
bool gbl_trans_item_running;
bool	gbl_skip_escape = false;

bool gbl_swap_advancement_item_running; // ���� �����۰� ��ȯ��
Race	g_eRaceInterface;

extern int					g_Dimension ;

static C_VS_UI_MONEY_DIALOG *	m_pC_dialog_Modify_Tax = NULL;
static C_VS_UI_MONEY_DIALOG *	m_pC_dialog_Campaign_Help = NULL;
static C_VS_UI_MONEY_DIALOG *	m_pC_dialog_Wedding_Contribution = NULL;
static C_VS_UI_SUMMER_COME_BACK *	m_pC_SummerCommback = NULL;


extern MPlayer*			g_pPlayer;

#if __CONTENTS(__FRIEND_ADDITION)
extern unsigned char message_box_count;
#endif //__FRIEND_ADDITION

void ExecF_Get_Event_Item(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
			gpC_base->SendMessage(UI_REQUEST_EVENT_ITEM, p_this_dialog->GetTempValue());
		break;
	}
}

void ExecF_ModifyTax(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		if(NULL != m_pC_dialog_Modify_Tax)
			gpC_base->SendMessage(UI_MODIFY_TAX, m_pC_dialog_Modify_Tax->GetValue());
		break;
	}
	gC_vs_ui.AcquireChatting();
}

void ExecF_CampaignHelp(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		if(NULL != m_pC_dialog_Campaign_Help)
		{
			gpC_base->SendMessage(UI_CAMPAIGN_HELP, m_pC_dialog_Campaign_Help->GetValue(), m_pC_dialog_Campaign_Help->GetTempValue());
		}
		break;
	}
	gC_vs_ui.AcquireChatting();
}


void ExecF_WeddingContribution(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		if(NULL != m_pC_dialog_Wedding_Contribution)
		{
			gpC_base->SendMessage(UI_WEDDING_CONTRIBUTION, m_pC_dialog_Wedding_Contribution->GetValue(), m_pC_dialog_Wedding_Contribution->GetTempValue());
		}
		break;
	}
	gC_vs_ui.AcquireChatting();
}


// by sigi
void ExecF_ExchangeCancel(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		//case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_EXCHANGE_REQUEST_CANCEL );
			gbl_exchange_cancel_running = false;
		break;
	}

	gC_vs_ui.AcquireChatting();
}

// by sigi
void ExecF_ExchangeAsk(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_EXCHANGE_ACCEPT, TRUE );
			gbl_exchange_ask_running = false;
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
			gpC_base->SendMessage( UI_EXCHANGE_ACCEPT, FALSE );
			gbl_exchange_ask_running = false;
			break;
	}

	gC_vs_ui.AcquireChatting();
}

void ExecF_PartyCancel(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		//case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_PARTY_REQUEST_CANCEL );
			gbl_party_cancel_running = false;
		break;
	}

	gC_vs_ui.AcquireChatting();
}

void ExecF_Enchant(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			{
				MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
				assert(p_cur_item);
				gpC_base->SendMessage(UI_ENCHANT_ACCEPT, C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y, (void *)p_cur_item);
				gbl_enchant_running = false;
			}
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		//case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_ENCHANT_CANCEL );
			gbl_enchant_running = false;
		break;
	}

	gC_vs_ui.AcquireChatting();
}

void ExecF_Third_Enchant(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			{
				MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
				assert(p_cur_item);
				gpC_base->SendMessage(UI_ENCHANT_ACCEPT, C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y, (void *)p_cur_item);
				gbl_enchant_running = false;
			}
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		//case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_ENCHANT_CANCEL );
			gbl_enchant_running = false;
		break;
	}

	gC_vs_ui.AcquireChatting();
}

void ExecF_UsePetFood(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		{
			if(((C_VS_UI_ASK_DIALOG*)p_this_dialog)->GetdwTemporayValue())
			{
				MSubInventory* pSubInventory = (MSubInventory*)((C_VS_UI_ASK_DIALOG*)p_this_dialog)->GetpTemporayValue();
				if(NULL != pSubInventory)
				{
					MItem *p_cur_item = pSubInventory->GetItem(((C_VS_UI_ASK_DIALOG*)p_this_dialog)->GetdwTemporayValue());
					if(NULL != p_cur_item)
					{
						gpC_base->SendMessage(UI_ITEM_USE_SUBINVENTORY, p_cur_item->GetID(), pSubInventory->GetID(), (MItem *)p_cur_item);
					}
				}
			}
			else
			{
				MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
				assert(p_cur_item);
				gpC_base->SendMessage(UI_ITEM_USE, p_cur_item->GetID(), 1, (MItem *)p_cur_item);
	//			gpC_base->SendMessage(UI_ENCHANT_ACCEPT, C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y, (void *)p_cur_item);
			}
			gbl_use_pet_food_running = false;
		}
		break;
		
	case DIALOG_EXECID_CANCEL:
	case DIALOG_EXECID_EXIT:
//		//case DIALOG_EXECID_OK:
		gpC_base->SendMessage( UI_CLOSE_USE_PET_FOOD );
		gbl_use_pet_food_running = false;
		break;
	}
	
	gC_vs_ui.AcquireChatting();
}
void ExecF_Keep_PetItem(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		{
			MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
			if(p_cur_item)
			{
				gpC_base->SendMessage(UI_KEEP_PETITEM, p_cur_item->GetID(), gC_vs_ui.GetPetStorageEmptySlot(), (MItem *)p_cur_item);
				gbl_keep_petitem_running = false;
			}
		}
		break;
		
	case DIALOG_EXECID_CANCEL:
	case DIALOG_EXECID_EXIT:
		gpC_base->SendMessage( UI_CLOSE_KEEP_PETITEM );
		gbl_keep_petitem_running = false;
		break;
	}
	
	gC_vs_ui.AcquireChatting();
}
void ExecF_Get_Keep_PetItem(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		{
			if(g_pStorage)
			{
				const MItem *p_cur_item = g_pStorage->GetItem( gC_vs_ui.GetPetStorageFocusSlot() );
				if(p_cur_item)
				{
					gpC_base->SendMessage(UI_GET_KEEP_PETITEM, p_cur_item->GetID(), gC_vs_ui.GetPetStorageFocusSlot(), (MItem *)p_cur_item);
					
				}
			}
			gbl_get_keep_petitem_running = false;
		}
		break;
		
	case DIALOG_EXECID_CANCEL:
	case DIALOG_EXECID_EXIT:
//		//case DIALOG_EXECID_OK:
		gpC_base->SendMessage( UI_CLOSE_GET_KEEP_PETITEM );
		gbl_get_keep_petitem_running = false;
		break;
	}
	
	gC_vs_ui.AcquireChatting();
}
void	ExecF_TransItem(C_VS_UI_DIALOG * p_this_dialog, id_t id )
{
	switch( id )
	{
	case DIALOG_EXECID_OK :
		{
			MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
			assert( p_cur_item );
			gpC_base->SendMessage( UI_TRANS_ITEM_ACCEPT , C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y, (void*)p_cur_item);
			gbl_enchant_running = false;
		}
		break;
	case DIALOG_EXECID_CANCEL :		
	case DIALOG_EXECID_EXIT :
		gpC_base->SendMessage( UI_TRANS_ITEM_CANCEL );
		gbl_enchant_running = false;
		break;
	}
}

void ExecF_NoSearchResult(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		case DIALOG_EXECID_OK:
			gbl_no_search_result_running = false;
		break;
	}

//	gC_vs_ui.AcquireChatting();
}

void ExecF_DepositLimit(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		case DIALOG_EXECID_OK:
			gbl_deposit_limit_running = false;
		break;
	}

//	gC_vs_ui.AcquireChatting();
}

void ExecF_WithdrawLimit(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		case DIALOG_EXECID_OK:
			gbl_withdraw_limit_running = false;
		break;
	}

//	gC_vs_ui.AcquireChatting();
}

void ExecF_BringFeeLimit(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		case DIALOG_EXECID_OK:
			gbl_bring_fee_limit_running = false;
		break;
	}

//	gC_vs_ui.AcquireChatting();
}


void ExecF_ExchangeLimit(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
		case DIALOG_EXECID_OK:
			gbl_exchange_limit_running = false;
		break;
	}

//	gC_vs_ui.AcquireChatting();
}

void ExecF_PartyAsk(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_PARTY_ACCEPT, TRUE );
			gbl_party_ask_running = false;
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
			gpC_base->SendMessage( UI_PARTY_ACCEPT, FALSE );
			gbl_party_ask_running = false;
			break;
	}

	gC_vs_ui.AcquireChatting();
}

// by sigi
void ExecF_StorageBuy(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			gpC_base->SendMessage( UI_STORAGE_BUY, TRUE );
			gbl_buy_storage_running = false;
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
			gpC_base->SendMessage( UI_STORAGE_BUY, FALSE );
			gbl_buy_storage_running = false;
			break;
	}

	gC_vs_ui.AcquireChatting();
}
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
void ExecF_RepairItemAll(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{	
	switch (id)
	{
		case DIALOG_EXECID_OK:
			gpC_base->SendMessage(UI_REPAIR_ITEM, 0, 0, NULL);
			break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
			gC_vs_ui.UnlockItemTrade();
			break;
	}
}
#endif //__REPAIRSYSTEM_RENEWAL
void ExecF_Use_AskItem(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		{
			MItem *p_cur_item = g_pInventory->GetItem(C_VS_UI_INVENTORY::m_mine_grid_x, C_VS_UI_INVENTORY::m_mine_grid_y);
			MItem* pMouseItem = gpC_mouse_pointer->GetPickUpItem();
			if(p_cur_item)
			{
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EVENT_GIFT_BOX &&
				p_cur_item->GetItemType() >= 22 && p_cur_item->GetItemType() <= 25)
				{
					gpC_base->SendMessage(UI_RUN_NAMING_CHANGE, (int)p_cur_item);
					//gC_vs_ui.RunNamingChange(p_cur_item);
				}
				else
				{
					gpC_base->SendMessage(UI_ITEM_USE, p_cur_item->GetID(), 0, (MItem *)p_cur_item);
				}

				if(p_cur_item->GetItemClass() == ITEM_CLASS_DYE_POTION && p_cur_item->GetItemType() >= 58 && p_cur_item->GetItemType() <= 75) //64)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_DYE_POTION && p_cur_item->GetItemType() >= 80 && p_cur_item->GetItemType() <= 82) //64)
				{
					return ; 
				}
//add by rallser ,summon monster
/*				if(p_cur_item->GetItemClass() == ITEM_CLASS_TRAP_ITEM && p_cur_item->GetItemType() >= 0 && p_cur_item->GetItemType() <= 12) //64)
				{
					return ; 
				}*/
				// by svi
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() >= 0 && p_cur_item->GetItemType() <= 3)
				{
					return ; 
				}

				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() >= 7 && p_cur_item->GetItemType() <= 9)
				{
					return ; 
				}
				
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() >= 20 && p_cur_item->GetItemType() <= 26)
				{
					return ; 
				}

				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() >= 32 && p_cur_item->GetItemType() <= 38) //by svi , 37->38
				{
					return ; 
				}
			
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() >= 39 && p_cur_item->GetItemType() <= 40)
				{
					return ; 
				}

				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 45)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 46)
				{
					return ; 
				}
				//experience stone
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 47)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 85)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 86)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 88)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 89)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 90)
				{
					return ; 
				}
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 87)
				{
			//		gC_vs_ui.RunBulletinBoardWindow(p_cur_item); 
					return ;
				}

				//�� �ð�
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM 
					&& (p_cur_item->GetItemType() >= 4 && p_cur_item->GetItemType() <= 6)
					|| (p_cur_item->GetItemType() >= 10 && p_cur_item->GetItemType() <= 15)  // by svi, modify 12 to 15
					|| (p_cur_item->GetItemType() >= 28 && p_cur_item->GetItemType() <= 30)
					|| (p_cur_item->GetItemType() >= 65 && p_cur_item->GetItemType() <= 73)  //master effects
					)
				{
					return ; 
				}

#if __CONTENTS(__EXTREME_FORCE_SCROLL)
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && p_cur_item->GetItemType() == 54)
				{
					return ; 
				}
#endif //__EXTREME_FORCE_SCROLL

#if __CONTENTS(__PREMIUM_CARD_D)
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (p_cur_item->GetItemType() == 41 && p_cur_item->GetItemType() == 55 ))
				{
					return ; 
				}
#endif //__PREMIUM_CARD_D

#if __CONTENTS(__PREMIUM_CARD_ADAY)
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (p_cur_item->GetItemType() >= 56 && p_cur_item->GetItemType() <= 58 ))
				{
					return ; 
				}
#endif //__PREMIUM_CARD_ADAY
#if __CONTENTS(__PET_MASHIMARO)
				if(p_cur_item->GetItemClass() == ITEM_CLASS_EFFECT_ITEM && (p_cur_item->GetItemType() >= 59 && p_cur_item->GetItemType() <= 64 ))
				{
					return ; 
				}
#endif //__PET_MASHIMARO
				if(pMouseItem != NULL)
				{
					if(pMouseItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM && pMouseItem->GetItemType() == 25)
					{
						gC_vs_ui.RunRemoveOptionFromRareItem( pMouseItem, p_cur_item );
						return ; 
					}
				}
				//if(pMouseItem->GetItemClass() == ITEM_CLASS_PET_ENCHANT_ITEM && pMouseItem->GetItemType() == 21)
					
				//	p_cur_item->GetItemClass() == ITEM_CLASS_PET_ENCHANT_ITEM && p_cur_item->GetItemType() >= 21)
				//{
				//	gbl_use_askitem_running = false;
				//	gC_vs_ui.RunEnchant(1);
				//	return ; 
				//}
				gbl_use_askitem_running = false;
				
			}
		}
		break;
		
	case DIALOG_EXECID_CANCEL:
	case DIALOG_EXECID_EXIT:
		//gpC_base->SendMessage( UI_CLOSE_KEEP_PETITEM );
		gbl_use_askitem_running = false;
		break;
	}
	
	gC_vs_ui.AcquireChatting();
}


void ExecF_Delegation_Of_Power(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	C_VS_UI_ASK_DIALOG*		p_ask_dialog		= static_cast<C_VS_UI_ASK_DIALOG*>(p_this_dialog);
	LPCTSTR	pName = static_cast<LPCTSTR>( p_ask_dialog->GetpTemporayValue() );

	switch (id)
	{
	case DIALOG_EXECID_OK:
		{
			gpC_base->SendMessage(UI_MODIFY_GUILD_MEMBER,
				g_char_slot_ingame.GUILD_ID, 1, (void*)(pName));
			gbl_Delegation_Of_Power_running = false ;	
		}
		break;
		
	case DIALOG_EXECID_CANCEL:
	case DIALOG_EXECID_EXIT:
		 gbl_Delegation_Of_Power_running = false ;	
		break;
	}
	
	gC_vs_ui.AcquireChatting();
}


void ExecF_SummerComeBack(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
	case DIALOG_EXECID_OK:
		if(NULL != m_pC_SummerCommback)
		{
			//gpC_base->SendMessage(Execute_UI_REQUEST_EVENT_ITEM, m_pC_SummerCommback->GetType(), 0);
		}
		break;
	}
	gC_vs_ui.AcquireChatting();
}

/*
// by larosel
void ExecF_TutorialExitAsk(C_VS_UI_DIALOG * p_this_dialog, id_t id)
{
	switch (id)
	{
		case DIALOG_EXECID_OK:
			// �ּ� by sigi
			//gpC_base->SendMessage( UI_TUTORIAL_EXIT_ACCEPT, TRUE );
			gC_vs_ui.RunTutorialExitAccept();	// �ٷ� ���� by sigi
			gbl_tutorial_exit_ask_running = false;
		break;

		case DIALOG_EXECID_CANCEL:
		case DIALOG_EXECID_EXIT:
			gpC_base->SendMessage( UI_STORAGE_BUY, FALSE );
			gbl_tutorial_exit_ask_running = false;
			break;
	}

	gC_vs_ui.AcquireChatting();
}
*/
//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Lock/Unlock Item
//
// Lock�Ǹ� Item ����/����/�ٲٱⰡ �Ұ���������. Unlock�� �װ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::LockItem()
{
	gbl_item_lock = true;
}

void C_VS_UI_GAME::UnlockItem()
{
	gbl_item_lock = false;
}

void C_VS_UI_GAME::LockItemTrade()
{
	gbl_item_trade_lock = true;
}

void C_VS_UI_GAME::UnlockItemTrade()
{
	gbl_item_trade_lock = false;
}

//-----------------------------------------------------------------------------
// Lock/Unlock gear item
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::LockGear()
{
	gbl_gear_lock = true;
}

void C_VS_UI_GAME::UnlockGear()
{
	gbl_gear_lock = false;
}

//-----------------------------------------------------------------------------
// ChangeToSlayerInterface
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ChangeToSlayerInterface()
{
	CT_Done("DIAG: ChangeToSlayerInterface enter");
	CloseDescDialog();

	CloseInfo();
	DeleteNew(m_pC_info);
	DeleteNew(m_pC_other_info);
//	CloseMilitary();
	g_descriptor_manager.Unset();
	CT_Done("DIAG: ChangeToSlayerInterface post Close/DeleteNew");

	g_eRaceInterface = RACE_SLAYER;

	DeleteNew(m_pC_tribe_interface);

	gpC_global_resource->FreeAssemble();
	gpC_global_resource->LoadAssemble();
	CT_Done("DIAG: ChangeToSlayerInterface post FreeAssemble/LoadAssemble");

	CT_Done("DIAG: ChangeToSlayerInterface pre new C_VS_UI_SLAYER");
	m_pC_tribe_interface = new C_VS_UI_SLAYER;
	CT_Done("DIAG: ChangeToSlayerInterface post new C_VS_UI_SLAYER=%p", m_pC_tribe_interface);


	char sz_filename[512],sz_filename2[512],sz_filename3[512];
	wsprintf(sz_filename, "UserSet\\%s.set", g_char_slot_ingame.sz_name.c_str());
	wsprintf(sz_filename2,"UserSet\\%s-%d.set", g_char_slot_ingame.sz_name.c_str(),g_pUserInformation->WorldID);
	wsprintf(sz_filename3,"UserSet\\%s-%d-%d.set", g_char_slot_ingame.sz_name.c_str(),g_Dimension,g_pUserInformation->WorldID);

	ivfstream file(sz_filename3, std::ios::binary);

	if(file.is_open())
	{
		gpC_vs_ui_window_manager->LoadFromFile(file);
		file.close();
	}
	else
	{
		gpC_vs_ui_window_manager->InitDefaultQuickSlot() ;

		file.open(sz_filename2, std::ios::binary);
		if(file.is_open())
		{
			gpC_vs_ui_window_manager->LoadFromFile(file);
			file.close();
		}
		else
		{
			file.open(sz_filename, std::ios::binary);
			if(file.is_open())
			{
				gpC_vs_ui_window_manager->LoadFromFile(file);
				file.close();
			}
		}
	}
	CT_Done("DIAG: ChangeToSlayerInterface post UserSet load");

	m_pC_hotkey = m_pC_tribe_interface;
	CT_Done("DIAG: ChangeToSlayerInterface pre TribeChanged");
	m_pC_chatting->TribeChanged();
	CT_Done("DIAG: ChangeToSlayerInterface pre SetChattingInterface");
	m_pC_tribe_interface->SetChattingInterface(m_pC_chatting);
	CT_Done("DIAG: ChangeToSlayerInterface pre tribe_interface->Start");
	m_pC_tribe_interface->Start();
	CT_Done("DIAG: ChangeToSlayerInterface pre new C_VS_UI_INFO");
	m_pC_info = new C_VS_UI_INFO;
	CT_Done("DIAG: ChangeToSlayerInterface exit, info=%p", m_pC_info);
}

//-----------------------------------------------------------------------------
// ChangeToVampireInterface
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ChangeToVampireInterface()
{
	CT_Done("DIAG: ChangeToVampireInterface enter");
	CloseDescDialog();

	CloseInfo();
	DeleteNew(m_pC_info);
	DeleteNew(m_pC_other_info);

	//CloseMilitary();

	g_descriptor_manager.Unset();
	CT_Done("DIAG: ChangeToVampireInterface post Close/DeleteNew");

	g_eRaceInterface = RACE_VAMPIRE;

	DeleteNew(m_pC_tribe_interface);

	gpC_global_resource->FreeAssemble();
	gpC_global_resource->LoadAssemble();
	CT_Done("DIAG: ChangeToVampireInterface post FreeAssemble/LoadAssemble");

	CT_Done("DIAG: ChangeToVampireInterface pre new C_VS_UI_VAMPIRE");
	m_pC_tribe_interface = new C_VS_UI_VAMPIRE;
	CT_Done("DIAG: ChangeToVampireInterface post new C_VS_UI_VAMPIRE=%p", m_pC_tribe_interface);

	char sz_filename[512];
	char sz_filename2[512];
	char sz_filename3[512];
	wsprintf(sz_filename, "UserSet\\%s.set", g_char_slot_ingame.sz_name.c_str());
	wsprintf(sz_filename2, "UserSet\\%s-%d.set", g_char_slot_ingame.sz_name.c_str(),g_pUserInformation->WorldID);
	wsprintf(sz_filename3, "UserSet\\%s-%d-%d.set", g_char_slot_ingame.sz_name.c_str(),g_Dimension,g_pUserInformation->WorldID);
	
	ivfstream file(sz_filename3, std::ios::binary);

	if(file.is_open())
	{
		gpC_vs_ui_window_manager->LoadFromFile(file);
		file.close();
	}
	else
	{
		gpC_vs_ui_window_manager->InitDefaultQuickSlot() ;
		
		file.open(sz_filename2, std::ios::binary);
		if(file.is_open())
		{
			gpC_vs_ui_window_manager->LoadFromFile(file);
			file.close();
		}
		else
		{
			file.open(sz_filename, std::ios::binary);
			if(file.is_open())
			{
				gpC_vs_ui_window_manager->LoadFromFile(file);
				file.close();
			}
		}
	}
	CT_Done("DIAG: ChangeToVampireInterface post UserSet load");

	m_pC_hotkey = m_pC_tribe_interface;
	CT_Done("DIAG: ChangeToVampireInterface pre TribeChanged");
	m_pC_chatting->TribeChanged();
	CT_Done("DIAG: ChangeToVampireInterface pre SetChattingInterface");
	m_pC_tribe_interface->SetChattingInterface(m_pC_chatting);

	CT_Done("DIAG: ChangeToVampireInterface pre tribe_interface->Start");
	m_pC_tribe_interface->Start();
	CT_Done("DIAG: ChangeToVampireInterface pre new C_VS_UI_INFO");
	m_pC_info = new C_VS_UI_INFO;
	CT_Done("DIAG: ChangeToVampireInterface exit, info=%p", m_pC_info);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME
//
//
//-----------------------------------------------------------------------------
C_VS_UI_GAME::C_VS_UI_GAME()
{	
//	m_pc_military = NULL;
	m_pC_regen_tower_minimap = NULL;
	m_pC_input_name = NULL;
	m_pC_blood_bible_status = NULL;
	m_pC_tribe_interface	= NULL;
	m_pC_shop = NULL;
	m_pC_storage = NULL;
	m_pC_exchange = NULL;
	gbl_sell_running = false;
	gbl_buy_running = false;
	gbl_repair_running = false;
	gbl_silvering_running = false;
	gbl_buy_storage_running = false;
	gbl_exchange_ask_running = false;
	gbl_exchange_cancel_running = false;
	gbl_persnalshop_running = false;
//	gbl_option_running = false;
	m_pC_ousters_skill_info = NULL;
	m_pC_info = new C_VS_UI_INFO;
	m_pC_other_info = NULL;
	m_pC_storage_buy_dialog = NULL;
	m_pC_exchange_ask_dialog = NULL;
	m_pC_exchange_cancel_dialog = NULL;
	m_pC_tutorial_exit_ask_dialog = NULL;	//by larosel
	m_pC_party_ask_dialog = NULL;
	m_pC_party_cancel_dialog = NULL;
	m_pC_enchant_dialog = NULL;
	m_pC_no_search_result_dialog = NULL;
	m_pC_deposit_limit = NULL;
	m_pC_withdraw_limit = NULL;
	m_pC_thirdenchant_dialog = NULL;
	
	m_pC_elevator = NULL;
	m_pC_desc_dialog = NULL;
	m_pC_file_dialog = NULL;
	m_pC_gamemenu = NULL;

	m_pC_skill_view = NULL;

	//Tutorial by larosel
	m_pC_bookcase = NULL;
//	m_pC_briefing = NULL;
	m_pC_computer = NULL;
//	m_pC_tutorial_exit = NULL;
	m_pC_slayer_portal = NULL;
	m_pC_team_member_list = NULL;
	m_pC_team_list = NULL;
	m_pC_team_info = NULL;
	m_pC_team_member_info = NULL;
	m_pC_team_regist = NULL;

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	// PremiumGiveItem
	m_pC_request_premium_give_item = NULL;
#endif //__PREMIUM_GIVE_ITEM_UI
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
	m_pC_RepairItemAllDialog = NULL;
#endif //__REPAIRSYSTEM_RENEWAL
	// party
	m_pC_request_party = NULL;
	m_pC_request_die = NULL;
	m_pC_party_manager = NULL;

	m_pC_chatting = NULL;

	m_pC_trace= NULL;
	m_pC_xmas_card = NULL;
	m_pC_war_list = NULL;

	m_pC_bring_fee = NULL;
	m_pC_bring_fee_limit = NULL;
	m_pC_popup_message = NULL;
	m_pC_lottery_card = NULL;
	m_pC_item_list = NULL;
	m_pC_image_notice = NULL;
	m_pC_bulletin_board = NULL;
	m_pC_request_resurrect = NULL;
	m_pC_trans_item_dialog = NULL;
	m_pC_use_pet_food_dialog = NULL;
	m_pC_mixing_forge = NULL;
	m_pC_remove_option = NULL;
	m_pC_horn = NULL;
	m_pC_mailbox = NULL;
	m_pC_finding_mine = NULL;
	m_pC_arrow_tile = NULL;
	m_pC_crazy_mine = NULL;
	m_pC_quest_inventory= NULL;
	m_pC_status_ctf = NULL;
	m_pC_pet_info = NULL;
	m_pC_help_desc = NULL;
	m_pC_world_map = NULL;
#if __CONTENTS(__PET_MIXINGFORGE)
	m_pC_pet_mixing_forge = NULL;
#endif //__PET_MIXINGFORGE
	m_pC_petstorage = NULL;
	m_pC_keep_petitem_dialog = NULL;
	m_pC_get_keep_petitem_dialog = NULL;

	m_pC_sms_message = NULL;
	m_pC_sms_list = NULL;
	m_pC_sms_record = NULL;
	m_pC_naming = NULL;
	m_pC_naming_change = NULL;
	m_pC_use_askitem_dialog = NULL;
	m_pC_quest_manager	= NULL;
	m_pC_get_event_item = NULL;
	m_pC_Delegation_Of_Power = NULL; 

	m_NickNameList.clear();
//	m_ChangeableNickname = false;
//	m_ChangeablePetNickname = false;

	m_pC_quest_manager = NULL;

	m_pC_dialog_Modify_Tax = NULL;
	m_pC_Ranger_Chat = NULL;
	m_pC_Powerjjang = NULL;

	m_pC_persnalshop = NULL;
	m_pC_persnalshopmessage = NULL;
	gbl_swap_advancement_item_running = false;

	m_pC_dialog_Campaign_Help = NULL;
	m_pC_Quest_Npc_Dialog = NULL;
	m_pC_WebBrowser = NULL;
//	m_pC_SubInventory = NULL;
	m_pC_dialog_Wedding_Contribution = NULL ; 
	m_pC_SummerCommback = NULL ; 
	m_pC_TexRate = NULL ;

	m_pC_operator = NULL;
	m_pC_timer = NULL;
#if __CONTENTS(__USER_GRADE)
	m_pC_usergrade = NULL;
#endif //__USER_GRADE

#if __CONTENTS(__TIPOJYU_CASTLE)	//�δ� ���̾�α� Ŭ���� �ʱ�ȭ
	m_pC_Indoung_Dialog	= NULL;
#endif // __TIPOJYU_CASTLE
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_GAME
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_GAME::~C_VS_UI_GAME()
{
	if(m_pC_mailbox != NULL)
	{
		MString mailFileName;
		mailFileName.Format("UserSet\\%s-%d-%d.mail", g_char_slot_ingame.sz_name.c_str(),
			g_Dimension,g_pUserInformation->WorldID);
		m_pC_mailbox->SaveToFile(mailFileName.GetString());
	}
	if(m_NickNameList.size())
	{
		std::vector<C_VS_UI_NicknameInfo*>::iterator itr = m_NickNameList.begin();
		while(itr != m_NickNameList.end())
		{
			C_VS_UI_NicknameInfo * TempInfo2 = (*itr);
			m_NickNameList.erase(itr);
			DeleteNew(TempInfo2);
		}
		m_NickNameList.clear();
	}
	
//	DeleteNew(m_pc_military);
	DeleteNew(m_pC_persnalshop);
	DeleteNew(m_pC_persnalshopmessage);

	DeleteNew(m_pC_Ranger_Chat);
	DeleteNew(m_pC_Powerjjang);
	DeleteNew(m_pC_dialog_Modify_Tax);

	DeleteNew(m_pC_dialog_Campaign_Help);
	DeleteNew(m_pC_dialog_Wedding_Contribution);
	
	DeleteNew(m_pC_Quest_Npc_Dialog);
	DeleteNew(m_pC_WebBrowser);
//	DeleteNew(m_pC_SubInventory);

	DeleteNew(m_pC_sms_message);
	DeleteNew(m_pC_sms_list);
	DeleteNew(m_pC_sms_record);
	DeleteNew(m_pC_naming);
	DeleteNew(m_pC_naming_change);

	DeleteNew(m_pC_xmas_card);

	DeleteNew(m_pC_quest_manager);

	DeleteNew(m_pC_pet_info);
	DeleteNew(m_pC_petstorage);
	
	DeleteNew(m_pC_regen_tower_minimap);
	DeleteNew(m_pC_ousters_skill_info);
	DeleteNew(m_pC_mixing_forge);
	DeleteNew(m_pC_remove_option);
	DeleteNew(m_pC_request_resurrect);
	DeleteNew(m_pC_input_name);
	DeleteNew(m_pC_blood_bible_status);
	DeleteNew(m_pC_war_list);
	DeleteNew(m_pC_bring_fee_limit);	
	DeleteNew(m_pC_trace);
	DeleteNew(m_pC_info);
	DeleteNew(m_pC_other_info);
	DeleteNew(m_pC_gamemenu);
	DeleteNew(m_pC_shop);
	DeleteNew(m_pC_storage);
	DeleteNew(m_pC_exchange);
	DeleteNew(m_pC_storage_buy_dialog);
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
	DeleteNew(m_pC_RepairItemAllDialog);
#endif //DeleteNew(m_pC_storage_buy_dialog);
#if __CONTENTS(__PET_MIXINGFORGE)
	DeleteNew(m_pC_pet_mixing_forge);
#endif //__PET_MIXINGFORGE
	DeleteNew(m_pC_exchange_ask_dialog);
	DeleteNew(m_pC_exchange_cancel_dialog);
	DeleteNew(m_pC_trans_item_dialog);
	DeleteNew(m_pC_use_pet_food_dialog);
	DeleteNew(m_pC_bookcase);
//	DeleteNew(m_pC_briefing);
	DeleteNew(m_pC_computer);
//	DeleteNew(m_pC_tutorial_exit);
	DeleteNew(m_pC_slayer_portal);
	DeleteNew(m_pC_team_member_list);
	DeleteNew(m_pC_team_list);
	DeleteNew(m_pC_team_info);
	DeleteNew(m_pC_team_member_info);
	DeleteNew(m_pC_team_regist);
//	DeleteNew(m_pC_skill_view);
	DeleteNew(m_pC_desc_dialog);
	DeleteNew(m_pC_file_dialog);
	DeleteNew(m_pC_elevator);
	DeleteNew(m_pC_bulletin_board);

	DeleteNew(m_pC_tribe_interface);

	DeleteNew(m_pC_party_ask_dialog);
	DeleteNew(m_pC_party_cancel_dialog);
	DeleteNew(m_pC_enchant_dialog);
	DeleteNew(m_pC_no_search_result_dialog);
	DeleteNew(m_pC_deposit_limit);
	DeleteNew(m_pC_withdraw_limit);
	DeleteNew(m_pC_thirdenchant_dialog);
	DeleteNew(m_pC_request_die);
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	DeleteNew(m_pC_request_premium_give_item);
#endif //__PREMIUM_GIVE_ITEM_UI
	DeleteNew(m_pC_request_party);
	DeleteNew(m_pC_party_manager);
	DeleteNew(m_pC_chatting);
	DeleteNew(m_pC_bring_fee);	
	DeleteNew(m_pC_popup_message);
	DeleteNew(m_pC_lottery_card);
	DeleteNew(m_pC_item_list);
	DeleteNew(m_pC_image_notice);
	DeleteNew(m_pC_horn);
	DeleteNew(m_pC_mailbox);
	DeleteNew(m_pC_finding_mine);
	DeleteNew(m_pC_arrow_tile);
	DeleteNew(m_pC_crazy_mine);
	DeleteNew(m_pC_quest_inventory);
	DeleteNew(m_pC_status_ctf);
	
	DeleteNew(m_pC_help_desc);
	DeleteNew(m_pC_world_map);

	DeleteNew(m_pC_keep_petitem_dialog);
	DeleteNew(m_pC_get_keep_petitem_dialog);
	
	DeleteNew(m_pC_use_askitem_dialog);
	DeleteNew(m_pC_get_event_item);
	DeleteNew(m_pC_Delegation_Of_Power);

	DeleteNew(m_pC_SummerCommback);

	DeleteNew(m_pC_operator);

	DeleteNew(m_pC_timer);
#if __CONTENTS(__USER_GRADE)
	DeleteNew(m_pC_usergrade);
#endif //__USER_GRADE

#if __CONTENTS(__TIPOJYU_CASTLE)	//�δ� ���̾�α� �޸� ����
	DeleteNew(m_pC_Indoung_Dialog);
#endif // __TIPOJYU_CASTLE

#if __CONTENTS(__FRIEND_ADDITION)
	ReleaseFriendMessagePopup();
#endif //__FRIEND_ADDITION

	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.begin();
	for(; pos != m_SubInventoryMap.end(); ++pos)
		DeleteNew(pos->second);

	m_SubInventoryMap.clear();
	
	gbl_sell_running = false;
	gbl_buy_running = false;
	gbl_repair_running = false;
	gbl_silvering_running = false;
	gbl_buy_storage_running = false;	
	gbl_exchange_ask_running = false;
	gbl_exchange_cancel_running = false;
	gbl_swap_advancement_item_running = false;
	gbl_persnalshop_running = false;
//	gbl_option_running = false;
	g_HISTORY_LINE = 4;
	
	// window_set file saving
	char sz_filename[512];
	wsprintf(sz_filename, "UserSet\\%s-%d-%d.set", g_char_slot_ingame.sz_name.c_str(),
		g_Dimension,g_pUserInformation->WorldID);

	
	std::ofstream file(sz_filename, std::ios::binary);
	
	if(file)
	{
		gpC_vs_ui_window_manager->SaveToFile(file);
		file.close();
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningStorage
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningStorage() const
{
	return (NULL != m_pC_storage);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningPetStorage
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningPetStorage() const
{
	return (NULL != m_pC_petstorage);
}
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningSell
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningSell() const
{
	return gbl_sell_running;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningExchange
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningExchange() const
{
	return (m_pC_exchange != NULL);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningShop
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningShop() const
{
	return (m_pC_shop != NULL);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningSkillView
//
// 2026-05-12: SkillInfoWindow aktivasyonu icin yorumdan acildi.
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningSkillView() const
{
	if (m_pC_skill_view == NULL) return false;
	return m_pC_skill_view->Running();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningBookcase by larosel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningBookcase() const
{
	return (m_pC_bookcase != NULL);
}
/*
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningBriefing by larosel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningBriefing() const
{
	return (m_pC_briefing != NULL);
}
*/
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningComputer by larosel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningComputer() const
{
	return (m_pC_computer != NULL);
}
/*
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningTutorialExit by larosel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTutorialExit() const
{
	return (m_pC_tutorial_exit != NULL);
}
*/
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRepair
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRepair() const
{
	return gbl_repair_running;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningSilvering
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningSilvering() const
{
	return gbl_silvering_running;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningElevator()
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningElevator() const
{
	return (m_pC_elevator!=NULL);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RestoreWhenActivateGame
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RestoreWhenActivateGame()
{
	// FAZ 4 (2026-05-06): m_pC_chatting NULL guard — focus regain
	// game UI henüz tam init olmadıysa (alt-tab login→in-game arası)
	// crash etmesin.
	CT_Done("VSGAME: RestoreWhenActivateGame ENTER m_pC_chatting=%p", (void*)m_pC_chatting);
	if (m_pC_chatting != NULL) {
		m_pC_chatting->RestoreHistoryTemp();
	}
	CT_Done("VSGAME: RestoreWhenActivateGame EXIT");
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::SetHotkey
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::SetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, ACTIONINFO id)
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->SetHotkey(hotkey, id);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetHotkey
//
// 
//-----------------------------------------------------------------------------
int	C_VS_UI_GAME::GetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, C_VS_UI_SKILL::HOTKEY_GRADE grade) const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetHotkey(hotkey, grade);
}

int	C_VS_UI_GAME::GetQuikSlot(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY hotkey) const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetQuikSlotWindow()->GetSlot(hotkey);
}

int	C_VS_UI_GAME::GetQuikSlot(C_VS_UI_SLAYER_QUICKITEM::HOTKEY hotkey) const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetQuikSlotBeltWindow()->GetSlot(hotkey);
}
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetHotkeyType
//
// 
//-----------------------------------------------------------------------------
int	C_VS_UI_GAME::GetHotkeyType(C_VS_UI_SKILL::HOTKEY hotkey) const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetHotkeyType(hotkey);
}
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::FinishLevelUp
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::FinishLevelUp()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->FinishLevelUp();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::LevelUp
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::LevelUp()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->LevelUp();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningLevelUp
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningLevelUp()
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->IsRunningLevelUp();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::HotKey_Skill
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::HotKey_Skill()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Skill();
}
void C_VS_UI_GAME::CloseSkillChooseWindow()
{
	assert(m_pC_hotkey);

	//if (!m_pC_gamemenu)
		m_pC_hotkey->CloseSkillChooseWindow();
}
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::CloseInfo
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::CloseInfo()
{
//	DeleteNew(m_pC_char_info);
	if(m_pC_info)
		m_pC_info->Finish();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunInfo
//
// 
//-----------------------------------------------------------------------------
//void	C_VS_UI_GAME::RunInfo(bool skill_mode)
//{
//	if (gpC_base->EventOccured() == true)
//		return;
//
//	if(m_pC_info == NULL)
//	{
//		DeleteNew(m_pC_info);
//		m_pC_info = new C_VS_UI_INFO();
//	}
//	if(m_pC_info->Running())
//	{
//		m_pC_info->Finish();
//	}
//	m_pC_info->Start(skill_mode);
//}

void	C_VS_UI_GAME::RunInfo(C_VS_UI_INFO::INFO_MODE Mode)
{
	if (gpC_base->EventOccured() == true)
		return;

	if(m_pC_info == NULL)
	{
		m_pC_info = new C_VS_UI_INFO();
	}
	if(m_pC_info->Running())
	{
		m_pC_info->Finish();
	}
	m_pC_info->Start(Mode);	
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunotherInfo
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunOtherInfo(bool isMonster)
{
//	if (gpC_base->EventOccured() == true)
//		return;

	if(m_pC_other_info == NULL)
	{
		DeleteNew(m_pC_other_info);
		m_pC_other_info = new C_VS_UI_OTHER_INFO();
	}
	m_pC_other_info->Start();

	//if ( m_pC_tribe_interface && !isMonster)
	//	m_pC_tribe_interface->OpenGearOther(false);	 //ȡ���鿴װ��
}



void C_VS_UI_GAME::CloseOtherInfo()
{
	if(m_pC_other_info)
		m_pC_other_info->Finish();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningCharInfo
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningCharInfo()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsCharacterMode());
	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningSkillInfo
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningSkillInfo()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsSkillMode());
	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningGrade2Info
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningGrade2Info()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsGrade2Mode());
	return false;
}

bool C_VS_UI_GAME::IsRunningGrade3Info()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsGrade3Mode());
	return false;

}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningGrade1Info
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningGrade1Info()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsGrade1Mode());
	return false;
}


//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRareSkillInfo
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRareSkillInfo()
{
	if(m_pC_info)
		return (m_pC_info->Running() && m_pC_info->IsRareSkillMode());
	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunGameMenu
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ReleaseFinishedGameMenu()
{
	if (m_pC_gamemenu != NULL && !m_pC_gamemenu->Running())
	{
		DeleteNew(m_pC_gamemenu);
	}
}

void	C_VS_UI_GAME::RunGameMenu()
{
	ReleaseFinishedGameMenu();

	if (m_pC_gamemenu)
	{
		DeleteNew(m_pC_gamemenu);
	}
	else
	{
		m_pC_gamemenu = new C_VS_UI_GAMEMENU;
		m_pC_gamemenu->Start();
		gpC_base->SendMessage(UI_RUNNING_GAMEMENU);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::ClosePopupWindow
//
// �ϳ��� ������ true�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::ClosePopupWindow()
{
	//
	// Inventory/Gear�� �ѽ��� Popup Window�� ����. �� ���� ���� �� �ִٸ� ���� ������ ���̴�.
	// �׵��� pinned Window�̹Ƿ� �����Ǿ� �ִ� ���� Interface���� no pinned Window�� ����
	// ��µȴ�. ���� Inventory/Gear -> Other Window ������ close�Ѵ�.
	//
	bool charinfo_closed = false;

#if __CONTENTS(__FRIEND_ADDITION)
	if (CloseFriendMessagePopup())
	{
		charinfo_closed = true;
	}
#endif //__FRIEND_ADDITION

//	if(IsRunningWebBrowser())
//	{
//		CloseWebBrowser();
//		return true;
//	}
	if (IsRunningCharInfo() || IsRunningSkillInfo() ||
		IsRunningGrade1Info()  || IsRunningGrade2Info() ||
		IsRunningGrade3Info() || IsRunningRareSkillInfo())
	{
		CloseInfo();
		charinfo_closed = true;
	}
	
	if(IsRunningPetInfo())
	{
		ClosePetInfo();
		return true;
	}

	if(CountRunningSubInventory() > 0)
	{
		ClearSubInventory();
		return true;
	}

	if(IsRunningSMSRecord())
	{
		CloseSMSRecord();
		return true;
	}
	if(IsRunningSMSList())
	{
		CloseSMSList();
		return true;
	}
	
	if(IsRunningPersnalShop() )
	{
		ClosePersnalShop();
		return true;
	}
	
	if(IsRunningPersnalShopMessage() )
	{
		ClosePersnalShopMessage();
		return true;
	}
	if(IsRunningSMSMessage())
	{
		CloseSMSMessage();
		return true;
	}
	if(IsRunningNamingChange())
	{
		CloseNamingChange();
		return true;
	}
	if(IsRunningNaming())
	{
		CloseNaming();
		return true;
	}
	
	if(IsRunningPowerjjang())
	{
		ClosePowerjjang();
		return true;
	}

	if(IsRunningMailBox() && gC_vs_ui.IsNewMail() == false)
	{
		CloseMailBox();
		return true;
	}

	if(IsRunningOustersSkillInfo())
	{
		CloseOustersSkillInfo();
		return true;
	}

	if(IsRunningRequestShrineMinimap())
	{
		gpC_base->SendMessage(UI_CLOSE_SHRINE_MINIMAP,1 );
		FinishRequestShrineMinimap();
		return true;
	}

	if(IsRunningArrowTile())
	{
		CloseArrowTile();
		return true;
	}

	if(IsRunningCrazyMine() )
	{
		CloseCrazyMine();
		return true;
	}

	if(IsRunningQuestInventory() )
	{
		CloseQuestInventory();
		return true;
	}


	if(IsRunningFindingMine())
	{
		CloseFindingMine();
		return true;
	}

	if(IsRunningHorn())
	{
		CloseHorn();
		return true;
	}
	
	if( IsRunningMixingForge() )
	{
		CloseMixingForge();
		return true;
	}
#if __CONTENTS(__PET_MIXINGFORGE)
	if(IsRunningPetMixingForge())
	{
		ClosePetMixingForge();
		return true;
	}
#endif //__PET_MIXINGFORGE

	if( IsRunningRemoveOptionFromRareItem() )
	{
		CloseRemoveOptionFromRareItem();
		return true;
	}

	if(IsRunningLotteryCard() )
	{
		CloseLotteryCard();
		return true;
	}
	if(IsRunningBulletinBoardWindow() )
	{
		CloseBulletinBoardWindow();
		return true;
	}

	if(IsRunningImageNotice() )
	{
		CloseImageNotice();
		return true;
	}

	if(IsRunningItemListWindow() )
	{
		CloseItemListWindow();
		return true;
	}
	if(IsRunningPopupMessage())
	{
		ClosePopupMessage();
		return true;
	}

	if(IsRunningInputNameWindow())
	{
		CloseInputNameWindow();
		return true;
	}

	if (IsRunningPartyAsk())
	{
		ClosePartyAsk();
		return true;
	}

	if (IsRunningBringFeeWindow())
	{
		CloseBringFeeWindow();
		return true;
	}

	if(IsRunningTraceWindow())
	{
	//	CloseTraceWindow();
		gpC_base->SendMessage(UI_CLOSE_TRACE_WINDOW);
		return true;
	}
	if(IsRunningWarListWindow())
	{
		CloseWarListWindow();
		return true;
	}
	if(IsRunningXmasCardWindow())
	{
		CloseXmasCardWindow();
//		gpC_base->SendMessage(UI_CLOSE_TRACE_WINDOW);
		return true;
	}
//	if (IsRunningSkillView())
//	{
//		HotKey_SkillInfo();
//		gpC_base->SendMessage(UI_CLOSE_SKILL_VIEW);
//		charinfo_closed = true;
//	}
	
	if (IsRunningOtherInfo())
	{
		CloseOtherInfo();
		return true;
	}

	if (IsRunningPartyManager())
	{
//		gpC_base->SendMessage(UI_CLOSE_SKILL_VIEW);
		ClosePartyManager();
		charinfo_closed = true;
	}

	if(m_pC_tribe_interface->IsRunningMark())
	{
		m_pC_tribe_interface->CloseMark();
		charinfo_closed = true;
	}

	if(IsRunningSlayerPortal())
	{
		CloseSlayerPortal();
		return true;
	}

	if(IsRunningTeamRegist())
	{
		CloseTeamMemberList();
		gpC_base->SendMessage(UI_CLOSE_TEAM_MEMBER_LIST);
		return true;
	}

	if(IsRunningTeamInfo())
	{
		CloseTeamInfo();
		gpC_base->SendMessage(UI_CLOSE_TEAM_INFO);
		return true;
	}

	if(IsRunningTeamMemberList())
	{
		CloseTeamMemberList();
		gpC_base->SendMessage(UI_CLOSE_TEAM_MEMBER_LIST);
		return true;
	}

	if(IsRunningTeamList())
	{
		CloseTeamList();
		// �� �޽����� UnSetLockInputPCTalk�� ȣ���ϴ� �ٶ���
		// �������� ������ ���� ������ �Ǵ�, �ϴ� �ּ� ó�� ����
		//gpC_base->SendMessage(UI_CLOSE_TEAM_LIST);
		return true;
	}

	if ( m_pC_tribe_interface->GetGearOtherOpenState() )
	{
		m_pC_tribe_interface->GetGearOtherWindow()->Finish();
		return true;
	}

	if ( m_pC_tribe_interface->GetTMallOpenState() )
	{
		m_pC_tribe_interface->GetTMallWindow()->Finish();
		return true;
	}

	if ( m_pC_tribe_interface->GetPayEventState() )
	{
		m_pC_tribe_interface->GetPayEventWindow()->Finish();
		return true;
	}

	if ( m_pC_tribe_interface->GetHollyWarOpenState() )
	{
		m_pC_tribe_interface->GetHollyWarWindow()->Finish();
		return true;
	}

	if ( m_pC_tribe_interface->GetMilitaryOpenState() )
	{
		m_pC_tribe_interface->GetMilitaryWindow()->Finish();
		return true;
	}

	if (m_pC_tribe_interface->CloseInventoryGearWindow())
		return true;

#if __CONTENTS(__QUEST_RENEWAL)	// C_VS_UI_GAME::ClosePopupWindow()		CloseQuestStatusWindow();
#else
	if(IsRunningQuestStatusWindow())
	{
		CloseQuestStatusWindow();
		return true;
	}
#endif	// 	__QUEST_RENEWAL
	// by sigi
	// å �а� ������ å �ݱ�
	if (IsRunningBookcase() && m_pC_bookcase->IsReadingBook())
	{
		m_pC_bookcase->CloseBook();

		return true;
	}
	// �ƴϸ�.. bookcase �ݱ�
	else if (IsRunningBookcase())
	{
//		gpC_base->SendMessage(UI_CLOSE_BOOKCASE);		
		CloseBookcase();

		return true;
	}
//	else if (IsRunningBriefing())
//	{
//		gpC_base->SendMessage(UI_CLOSE_BRIEFING);		
//
//		return true;
//	}
	else if (IsRunningComputer())
	{
		#ifdef _LIB
//				DEBUG_ADD("closePopupWindow-UI_CLOSE_COMPUTER");
		#endif
		gpC_base->SendMessage(UI_CLOSE_COMPUTER);		

		return true;
	}
//	else if (IsRunningTutorialExit())
//	{
//		gpC_base->SendMessage(UI_CLOSE_TUTORIAL_EXIT);		
//
//		return true;
//	}
	else if (IsRunningDescDialog())
	{
//		gpC_base->SendMessage(UI_CLOSE_DESC_DIALOG);		
//		gC_vs_ui.HotKey_Help();
		gC_vs_ui.CloseDescDialog();

		return true;
	}
	else if (IsRunningFileDialog())
	{
//		gpC_base->SendMessage(UI_CLOSE_FILE_DIALOG);		
		gC_vs_ui.CloseFileDialog();

		return true;
	}
	else if (IsRunningElevator())
	{
		gpC_base->SendMessage(UI_CLOSE_ELEVATOR);		

		return true;
	}
	if (gbl_sell_running)
	{
		gpC_base->SendMessage(UI_ITEM_SELL_FINISHED);
//		gbl_sell_running = false;
		return true;
	}
	else if(gbl_swap_advancement_item_running)
	{
		gpC_base->SendMessage(UI_CLOSE_SWAPADVANCEMENTITEM);
	}
	else if (gbl_repair_running)
	{
		gpC_base->SendMessage(UI_ITEM_REPAIR_FINISHED);
//		gbl_repair_running = false;
		return true;
	}
	else if (gbl_silvering_running)
	{
		gpC_base->SendMessage(UI_ITEM_SILVERING_FINISHED);
//		gbl_silvering_running = false;
		return true;
	}
	else if (gbl_buy_running)
	{
		gpC_base->SendMessage(UI_CLOSE_SHOP);
//		gbl_buy_running = false;
		return true;
	}
	else if (gbl_buy_storage_running)
	{
		gpC_base->SendMessage(UI_STORAGE_BUY, FALSE);
		gbl_buy_storage_running = false;
		return true;
	}
	else if (gbl_exchange_ask_running)
	{
		gpC_base->SendMessage(UI_EXCHANGE_ACCEPT, FALSE);
		gbl_exchange_ask_running = false;
		return true;
	}
	else if (gbl_exchange_cancel_running)
	{
		gpC_base->SendMessage(UI_EXCHANGE_REQUEST_CANCEL);
		gbl_exchange_cancel_running = false;
		return true;
	}
	else if (gbl_trans_item_running )
	{
		gpC_base->SendMessage( UI_TRANS_ITEM_CANCEL );
		gbl_trans_item_running = false;
		return true;
	}
//	else if (gbl_option_running)
//	{
//		gpC_base->SendMessage(UI_CLOSE_GAME_OPTION);
//		gbl_option_running = false;
//		return true;
//	}
//	else if(gC_vs_ui.IsRunningOption())
//	{
//	//	gC_vs_ui.CloseOption();
//		gpC_base->SendMessage(UI_CLOSE_OPTION);
//		return true;
//	}
	if (IsRunningExchange())
	{
		gpC_base->SendMessage(UI_CLOSE_EXCHANGE, FALSE);		
		return true;
	}
//	else if (m_pC_tribe_interface->SkillTreeRunning())
//	{
//		gpC_base->SendMessage(UI_CLOSE_SKILL_TREE);
//		return true;
//	}
	else if (IsRunningStorage())
	{
		gpC_base->SendMessage(UI_CLOSE_STORAGE);
		return true;
	} 
	else if(IsRunningPetStorage())
	{
		gpC_base->SendMessage(UI_CLOSE_PETSTORAGE);
		return true;
	}
//	else if (gC_vs_ui.IsRunningExchange())
//	{
//		gpC_base->SendMessage(UI_CANCEL_EXCHANGE);
//		gC_vs_ui.CloseExchange();
//		return true;
//	}
//	if (gC_vs_ui.IsRunningDescDialog())
//	{
//		gpC_base->SendMessage(UI_CLOSE_DESC_DIALOG);
//		gC_vs_ui.CloseDescDialog();
//		return true;
//	}

	if(IsRunningQuestManager())
	{
		if(IsRunningQuestList())
		{
			charinfo_closed = true;
			CloseQuestList();
		}
		if(IsRunningQuestDetail())
		{
			charinfo_closed = true;
			CloseQuestDetail();
		}
		if(IsRunningQuestMission())
		{
			charinfo_closed = true;
			CloseQuestMission();
		}
		if(IsRunningQuestItem())
		{
			charinfo_closed = true;
			CloseQuestItem();
		}
		
	}

	
	if(IsRunningHelpMailDesc())
	{
		CloseHelpDesc();
		return true;
	}
	
	if(IsRunTaxRate())
	{
		CloseTexRate();
		return true;
	}

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	if(IsRunningRequest_PremiumGiveItem())
	{
		bool isClose = m_pC_request_premium_give_item->ClosePremiumGiveItemDialog();
		
		if(g_pPremiumGiveItemInfoList->GetItemNum()<=0)
		{
			FinishRequest_PremiumGiveItem();
		}

		if(isClose)
			return true;
	}
#endif //__PREMIUM_GIVE_ITEM_UI

	return charinfo_closed;

	//return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::AcquireChatting
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::AcquireChatting()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->AcquireChatting();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsEmptyChatting
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsEmptyChatting()
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->IsEmptyChatting();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::AddChatToHistory
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::AddChatToHistory(char * str, char * sz_id, CHAT_LINE_CONDITION condition, DWORD color)
{ 
	assert(m_pC_tribe_interface != NULL);

//	if (guild)
//	{
//		m_C_chatting.AddChatToHistory(str, sz_id, condition, guild);
//	}
//	else
	{
		// �Ϲ�ä��â����...
		m_pC_chatting->AddChatToHistory(str, sz_id, condition, color);
	}
}
/*
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::DisconnectPCS
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::DisconnectPCS(int slot)
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DisconnectPCS(slot);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::ConnectionFailed
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ConnectionFailed()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->ConnectionFailed();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GrantPCSWaitRoomToDuplex
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::GrantPCSWaitRoomToDuplex(int slot)
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->GrantPCSWaitRoomToDuplex(slot);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetSendPossibleSlot
//
// 
//-----------------------------------------------------------------------------
int	C_VS_UI_GAME::GetSendPossibleSlot() const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetSendPossibleSlot();
}
*/
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::CloseGameMenu
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseGameMenu()
{
	DeleteNew(m_pC_gamemenu);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetInventoryPosition
//
// 
//-----------------------------------------------------------------------------
Point C_VS_UI_GAME::GetInventoryPosition() const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetInventoryPosition();
}



Point C_VS_UI_GAME::GetInventoryGetWidthAndHeight()
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetInventoryGetWidthAndHeight();
}


//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetInventoryGridPosition
//
// 
//-----------------------------------------------------------------------------
Point	C_VS_UI_GAME::GetInventoryGridPosition(int grid_x, int grid_y) const
{
	assert(m_pC_tribe_interface != NULL);

	return m_pC_tribe_interface->GetInventoryGridPosition(grid_x, grid_y);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::ReplaceItemInGear
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_GAME::ReplaceItemInGear(MItem* pItem, int slot, MItem*& pOldItem)
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->ReplaceItem(pItem, (MSlayerGear::GEAR_SLAYER)slot, pOldItem);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->ReplaceItem(pItem, (MVampireGear::GEAR_VAMPIRE)slot, pOldItem);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->ReplaceItem(pItem, (MOustersGear::GEAR_OUSTERS)slot, pOldItem);
		break;	
	}

	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RemoveItemInGear
//
// 
//-----------------------------------------------------------------------------
MItem * C_VS_UI_GAME::RemoveItemInGear(int slot)
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->RemoveItem((MSlayerGear::GEAR_SLAYER)slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->RemoveItem((MVampireGear::GEAR_VAMPIRE)slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->RemoveItem((MOustersGear::GEAR_OUSTERS)slot);
		break;
	}

	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::CanReplaceItemInGear
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::CanReplaceItemInGear(MItem* pItem, int slot, MItem*& pOldItem)
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->CanReplaceItem(pItem, (MSlayerGear::GEAR_SLAYER)slot, pOldItem);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->CanReplaceItem(pItem, (MVampireGear::GEAR_VAMPIRE)slot, pOldItem);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->CanReplaceItem(pItem, (MOustersGear::GEAR_OUSTERS)slot, pOldItem);
		break;		
	}	

	return false;
}

bool C_VS_UI_GAME::CanReplaceItemInMilitarySlot(MItem* pItem, int slot, MItem*& pOldItem)
{
	return g_pMilitarySlot->CanReplaceItem(pItem, slot, pOldItem);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetGearItem
//
// 
//-----------------------------------------------------------------------------
const MItem * C_VS_UI_GAME::GetGearItem(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetItem((MSlayerGear::GEAR_SLAYER)slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetItem((MVampireGear::GEAR_VAMPIRE)slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetItem((MOustersGear::GEAR_OUSTERS)slot);
		break;
	}

	return false;
}

const MItem * C_VS_UI_GAME::GetGearOtherItem(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGearOther->GetItem((MSlayerGear::GEAR_SLAYER)slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGearOther->GetItem((MVampireGear::GEAR_VAMPIRE)slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGearOther->GetItem((MOustersGear::GEAR_OUSTERS)slot);
		break;
	}

	return false;
}

const MItem* C_VS_UI_GAME::GetMilitarySlot(int slot) const
{
	return g_pMilitarySlot->GetItem(slot);
}


#if __CONTENTS(__GEAR_SWAP_CHANGE)	//VS_UI_GAME::GeatGearID()
DWORD	C_VS_UI_GAME::GetGearID()
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetAccepGearID();
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetAccepGearID();
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetAccepGearID();
		break;
	}

	return false;
}
#endif //__GEAR_SWAP_CHANGE


#if __CONTENTS(__QUEST_RENEWAL)
	void	C_VS_UI_GAME::GpsToBringQuestInfo()
	{
		if(m_pC_world_map != NULL)
		{
			m_pC_world_map->GpsToBringQuestInfo();
		}
	}
#endif

const MItem * C_VS_UI_GAME::GetGearItem_PickUp(int &slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetGearItem_PickUp(slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetGearItem_PickUp(slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetGearItem_PickUp(slot);
		break;
	}

	return NULL;
}

const MItem * C_VS_UI_GAME::GetGearCoreZapItem(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetGearCoreZapItem(slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetGearCoreZapItem(slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetGearCoreZapItem(slot);
		break;
	}

	return NULL;
}

const MItem * C_VS_UI_GAME::GetGearOtherCoreZapItem(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGearOther->GetGearCoreZapItem(slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGearOther->GetGearCoreZapItem(slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGearOther->GetGearCoreZapItem(slot);
		break;
	}

	return NULL;
}

const MItem * C_VS_UI_GAME::GetGearCoreZapedItem(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetGearCoreZapedItem(slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetGearCoreZapedItem(slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetGearCoreZapedItem(slot);
		break;
	}

	return NULL;
}

const bool C_VS_UI_GAME::IsHasAllCoreZap(int CoreZapType) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->IsHasAllCoreZap(CoreZapType);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->IsHasAllCoreZap(CoreZapType);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->IsHasAllCoreZap(CoreZapType);
		break;
	}

	return false;
}


const bool C_VS_UI_GAME::IsHasAllCue(OUT bool CueOption[]) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->IsHasAllCue(CueOption);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->IsHasAllCue(CueOption);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->IsHasAllCue(CueOption);
		break;
	}

	return false;
}

const bool C_VS_UI_GAME::IsCloseBloodBibleSlot(int slot) const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->IsCloseBloodBibleSlot((MSlayerGear::GEAR_SLAYER)slot);
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->IsCloseBloodBibleSlot((MVampireGear::GEAR_VAMPIRE)slot);
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->IsCloseBloodBibleSlot((MOustersGear::GEAR_OUSTERS)slot);
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::GetGearSize
//
// 
//-----------------------------------------------------------------------------
const int C_VS_UI_GAME::GetGearSize() const
{
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		return g_pSlayerGear->GetSize();
		break;

	case RACE_VAMPIRE:
		return g_pVampireGear->GetSize();
		break;

	case RACE_OUSTERS:
		return g_pOustersGear->GetSize();
		break;
	}

	return false;
}


/*
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::AcquireQuitPCSOnlineModeMessage
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::AcquireQuitPCSOnlineModeMessage()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->AcquireQuitPCSOnlineModeMessage();
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::PleaseConnectPCS
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::PleaseConnectPCS(char * sz_name, long pcs_number, int slot)
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->PleaseConnectPCS(sz_name, pcs_number, slot);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::OnLinePCS
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::OnLinePCS(char * sz_name, long pcs_number)
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->OnLinePCS(sz_name, pcs_number);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::AcquirePDSClosedMessage
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::AcquirePDSClosedMessage()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->AcquirePDSClosedMessage();
}
*/
/*
//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunSwordSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunSwordSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_SWORD);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunVampireSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunVampireSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_VAMPIRE);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunEnchantSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunEnchantSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_ENCHANT);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunHealSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunHealSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_HEAL);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunGunSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunGunSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_GUN);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::RunBladeSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::RunBladeSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->RunSkillTree(SKILLTREE_BLADE);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::CloseSkillTree
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAME::CloseSkillTree()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->CloseSkillTree(SKILLTREE_ANY);

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
*/
//-----------------------------------------------------------------------------
// FinishItemSelling
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishItemSelling()
{
	if (gbl_sell_running == false)
		return;

	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->FinishItemSelling();
	gbl_sell_running = false;
}

//void C_VS_UI_GAME::OpenGameHook()
//{
//	assert(m_pC_tribe_interface != NULL);
//
//	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
//	//m_pC_tribe_interface->OpenGameHook(false);
//
//}
//
//void C_VS_UI_GAME::CloseGameHook()
//{
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
//	m_pC_tribe_interface->CloseGameHook();
//}


void C_VS_UI_GAME::ToggleMilitary()
{
	assert(m_pC_tribe_interface != NULL);
	C_VS_UI_MILITARYDLG* pTMilitaryWindow = m_pC_tribe_interface->GetMilitaryWindow();
	if ( pTMilitaryWindow )
		pTMilitaryWindow->ToggleMilitary();	
}

void C_VS_UI_GAME::OpenMilitary()
{
	assert(m_pC_tribe_interface != NULL);

	//m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenMilitary(false);

}

void C_VS_UI_GAME::CloseMilitary()
{
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->CloseMilitary();
}

void C_VS_UI_GAME::InvalidateMilitarySkillLists()
{
	if (m_pC_tribe_interface == NULL)
		return;
	m_pC_tribe_interface->InvalidateMilitarySkillLists();
}

bool C_VS_UI_GAME::IsRunningMilitary() const
{
	if (m_pC_tribe_interface == NULL)
		return false;
	return m_pC_tribe_interface->GetMilitaryOpenState();
}

void C_VS_UI_GAME::UpdateMilitarySlotData(int nSlotID, int nType, int nGrade)
{
	m_pC_tribe_interface->UpdateMilitarySlotData(nSlotID, nType, nGrade);
}


void C_VS_UI_GAME::ToggleTMall()
{
	assert(m_pC_tribe_interface != NULL);

	//m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	C_VS_UI_TMALL* pTMallWindow = m_pC_tribe_interface->GetTMallWindow();
	if ( pTMallWindow )
		pTMallWindow->ToggleTMall();

}

void C_VS_UI_GAME::OpenTMall()
{
	assert(m_pC_tribe_interface != NULL);

	//m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenTMall(false);

}

void C_VS_UI_GAME::CloseTMall()
{
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->CloseTMall();
}

bool C_VS_UI_GAME::AddItemToTMall(C_VS_UI_TMALL::ItemList pItem)
{
	return m_pC_tribe_interface->AddItemToTMall( pItem );
}

bool C_VS_UI_GAME::IsRunningPayEvent()
{
	return m_pC_tribe_interface->GetPayEventState();
}

void C_VS_UI_GAME::OpenPayEvent()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->OpenPayEvent(false);

}

void C_VS_UI_GAME::ClosePayEvent()
{
	m_pC_tribe_interface->ClosePayEvent();
}

bool C_VS_UI_GAME::AddItemToPayEvent(C_VS_UI_PAYEVENT::ItemData pItem)
{
	return m_pC_tribe_interface->AddItemToPayEvent( pItem );
}

void C_VS_UI_GAME::SetPayEventCount(int nLevel, int nCount)
{
	m_pC_tribe_interface->SetPayEventCount(nLevel, nCount);
}

void C_VS_UI_GAME::SetPayEventStautus(int nLevel, int nStautus)
{
	m_pC_tribe_interface->SetPayEventStautus(nLevel, nStautus);
}


void C_VS_UI_GAME::OpenHollyWar()
{
	assert(m_pC_tribe_interface != NULL);

	//m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenHollyWar(false);

}

void C_VS_UI_GAME::CloseHollyWar()
{
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->CloseHollyWar();
}

void C_VS_UI_GAME::AddHollyScore(C_VS_UI_HOLYWAR::PlayerScore score, int nRace)
{
	assert( m_pC_tribe_interface );
	m_pC_tribe_interface->AddHollyScore( score, nRace );
}

void C_VS_UI_GAME::AddHollyRace(C_VS_UI_HOLYWAR::RaceData * data)
{
    assert( m_pC_tribe_interface );
	m_pC_tribe_interface->AddHollyRace(data);
}

void C_VS_UI_GAME::AddPlayerData(C_VS_UI_HOLYWAR::PlayerData data)
{
	assert( m_pC_tribe_interface );
	m_pC_tribe_interface->AddPlayerData(data);
}
//-----------------------------------------------------------------------------
// OpenInventoryToSell
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::OpenInventoryToSell()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenInventoryToSell();
	gbl_sell_running = true;
}

//-----------------------------------------------------------------------------
// FinishItemRepairing
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishItemRepairing()
{
	if (gbl_repair_running == false)
		return;

	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->FinishItemRepairing();
	gbl_repair_running = false;
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
	g_pPriceManager->RepairPriceListeraseAll();
#endif //__REPAIRSYSTEM_RENEWAL
}

//-----------------------------------------------------------------------------
// FinishItemSilvering
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishItemSilvering()
{
	if (gbl_silvering_running == false)
		return;

	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->FinishItemSilvering();
	gbl_silvering_running = false;
}

//-----------------------------------------------------------------------------
// OpenInventoryToRepair
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::OpenInventoryToRepair()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenInventoryToRepair();
	gbl_repair_running = true;
}

//-----------------------------------------------------------------------------
// OpenInventoryToSilvering
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::OpenInventoryToSilvering()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenInventoryToSilvering();
	gbl_silvering_running = true;
}

//-----------------------------------------------------------------------------
// SetShop
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetShop(MShop* pShop)
{
	if (m_pC_shop)
		m_pC_shop->SetShop(pShop);
}

//-----------------------------------------------------------------------------
// SetStorage
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetStorage(MStorage* pStorage)
{
	if (m_pC_storage)
		m_pC_storage->SetStorage(pStorage);
}
//-----------------------------------------------------------------------------
// SetStorage
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetPetStorage(MStorage* pStorage)
{
	if (m_pC_petstorage)
		m_pC_petstorage->SetPetStorage(pStorage);
}
//-----------------------------------------------------------------------------
// CloseShop
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseShop()
{
	if (gbl_buy_running == false)
	{
		assert(m_pC_shop == NULL);
		return;
	}

	DeleteNew(m_pC_shop);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	gbl_buy_running = false;
}

//-----------------------------------------------------------------------------
// CloseStorage
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseStorage()
{
	if (NULL == m_pC_storage)
		return;

	DeleteNew(m_pC_storage);
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
//-----------------------------------------------------------------------------
// ClosePetStorage
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ClosePetStorage()
{
	if (NULL ==m_pC_petstorage)
		return;
	
	DeleteNew(m_pC_petstorage);
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
//-----------------------------------------------------------------------------
// CloseExchange
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseExchange()
{
	if (!m_pC_exchange)
		return;

	DeleteNew(m_pC_exchange);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// CloseExchangeAsk
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseExchangeAsk()
{
	if (!m_pC_exchange_ask_dialog)
		return;

	DeleteNew(m_pC_exchange_ask_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_exchange_ask_running = false;
	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// CloseExchangeCancel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseExchangeCancel()
{
	if (!m_pC_exchange_cancel_dialog)
		return;

	DeleteNew(m_pC_exchange_cancel_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_exchange_cancel_running = false;
	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// CloseGameMenuOption
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
/*
void C_VS_UI_GAME::CloseOption()
{
	if (!m_pC_option)
		return;

	DeleteNew(m_pC_option);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

//	gbl_option_running = false;
	gC_vs_ui.AcquireChatting();
}
*/

//-----------------------------------------------------------------------------
// RunShop
//
// Shop�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunShop()
{
	if (m_pC_shop == NULL)
	{
		m_pC_shop = new C_VS_UI_SHOP;

		assert(m_pC_tribe_interface != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		// Shop Window�� �Բ� ��� �͵��� �����Ѵ�.
		m_pC_tribe_interface->ShopRunningAnnounced();
		m_pC_shop->Start();
		gbl_buy_running = true;
	}
}


void	C_VS_UI_GAME::Inventory_Item_Position_Remove()
{
	if(m_pC_tribe_interface != NULL)
		m_pC_tribe_interface->Inventory_Item_Position_Remove();
}


//-----------------------------------------------------------------------------
// RunStorage
//
// Storage�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunStorage()
{
	if (m_pC_storage == NULL)
	{
		m_pC_storage = new C_VS_UI_STORAGE();

		assert(m_pC_tribe_interface != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_tribe_interface->StorageRunningAnnounced();
		m_pC_storage->Start();
	}
}
//-----------------------------------------------------------------------------
// RunPetStorage
//
// Storage�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunPetStorage()
{
	if (m_pC_petstorage == NULL)
	{
		m_pC_petstorage = new C_VS_UI_PETSTORAGE();

		assert(m_pC_tribe_interface != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_tribe_interface->StorageRunningAnnounced();
		m_pC_petstorage->Start();
	}
}
//-----------------------------------------------------------------------------
// RunStorageBuy
//
// Storage�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunStorageBuy(int price)
{
	if (m_pC_storage_buy_dialog != NULL)
	{
		DeleteNew(m_pC_storage_buy_dialog);
	}

	// center
	m_pC_storage_buy_dialog = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_StorageBuy, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_STORAGE_BUY, price);	// by sigi

	assert(m_pC_storage_buy_dialog != NULL);

	gbl_buy_storage_running = true;

	m_pC_storage_buy_dialog->Start();	
}
/*
//-----------------------------------------------------------------------------
// RunOption
//
// �ɼ�â ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunOption()
{
	//gC_vs_ui.CloseGameMenu();

	if (m_pC_option!= NULL)
	{
		DeleteNew(m_pC_option);
	}

	// center
	m_pC_option = new C_VS_UI_OPTION;

	assert(m_pC_option != NULL);

//	gbl_option_running = true;

	m_pC_option->Start();	
}
*/
//-----------------------------------------------------------------------------
// RunSkillView //by larosel
//
// 2026-05-12: Uncomment + null safety. K hotkey skill window'unu acmiyordu.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunSkillView()
{
	if (m_pC_skill_view == NULL)
	{
		m_pC_skill_view = new C_VS_UI_SKILL_VIEW();
		assert(m_pC_skill_view != NULL);

		if (m_pC_tribe_interface != NULL)
			m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	}
	m_pC_skill_view->Start();
}


//-----------------------------------------------------------------------------
// CloseSkillView
//
// Already-finished'sa no-op.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseSkillView()
{
	if (m_pC_skill_view == NULL)
		return;

	m_pC_skill_view->Finish();
	DeleteNew(m_pC_skill_view);

	if (m_pC_tribe_interface != NULL)
	{
		// SkillView referansta aktif degil; kapatma eski SkillInfo yolunu
		// tekrar tetiklememeli. Sadece event kilidini geri al.
		m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	}
}

//-----------------------------------------------------------------------------
// RunDescDialog //by larosel
//
// ����â�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunDescDialog(int type, void *ptr, void *ptr2)
{
	if (m_pC_desc_dialog == NULL)
	{
		m_pC_desc_dialog = new C_VS_UI_DESC_DIALOG(type, ptr, ptr2);

		assert(m_pC_desc_dialog != NULL);

//		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_desc_dialog->Start();
	}
}


//-----------------------------------------------------------------------------
// CloseDescDialog
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseDescDialog()
{
	if (!m_pC_desc_dialog)
		return;

	DeleteNew(m_pC_desc_dialog);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->ResetOpenedHelpIdx();

//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// RunFileDialog //by larosel
//
// ����â�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunFileDialog(C_VS_UI_FILE_DIALOG::MODE Mode, char *type)
{
//	if (gpC_base->EventOccured() == true)
//		return;

	if(m_pC_file_dialog == NULL)
	{
		DeleteNew(m_pC_file_dialog);

		m_pC_file_dialog = new C_VS_UI_FILE_DIALOG(Mode);
	}
	m_pC_file_dialog->Start(type);
}


//-----------------------------------------------------------------------------
// CloseFileDialog
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseFileDialog()
{
	if (!m_pC_file_dialog)
		return;

//	if(m_pC_file_dialog)
//		m_pC_file_dialog->Finish();

	DeleteNew(m_pC_file_dialog);

//	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// RunElevator //by larosel
//
// ���������� �������̽��� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunElevator()
{
	if (m_pC_elevator == NULL)
	{
		m_pC_elevator = new C_VS_UI_ELEVATOR();

		assert(m_pC_elevator != NULL);

//		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_elevator->Start();
	}
}


//-----------------------------------------------------------------------------
// CloseElevator
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseElevator()
{
	if (!m_pC_elevator)
		return;

	DeleteNew(m_pC_elevator);

	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
void C_VS_UI_GAME::RunRepairItemAll(DWORD objectID, DWORD price)
{
	if(objectID == 0)
	{
		if (m_pC_RepairItemAllDialog != NULL)
		{
			DeleteNew(m_pC_RepairItemAllDialog);
		}
		// center
		m_pC_RepairItemAllDialog = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_RepairItemAll, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_REPAIRITEMALL, price);	// by sigi
		assert(m_pC_RepairItemAllDialog != NULL);
		m_pC_RepairItemAllDialog->Start();
	}
}
#endif //__REPAIRSYSTEM_RENEWAL

//-----------------------------------------------------------------------------
// RunBookcase //by larosel
//
// å���� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunBookcase()
{
	if (m_pC_bookcase == NULL)
	{
		m_pC_bookcase = new C_VS_UI_BOOKCASE;

		assert(m_pC_bookcase != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_bookcase->Start();
	}
}


//-----------------------------------------------------------------------------
// CloseBookcase
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseBookcase()
{
	if (!m_pC_bookcase)
		return;

	DeleteNew(m_pC_bookcase);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}
/*
//-----------------------------------------------------------------------------
// RunBriefing //by larosel
//
// �긮���� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunBriefing()
{
	if (m_pC_briefing == NULL)
	{
		m_pC_briefing = new C_VS_UI_BRIEFING;

		assert(m_pC_briefing != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_briefing->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseBriefing //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseBriefing()
{
	if (!m_pC_briefing)
		return;

	DeleteNew(m_pC_briefing);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}
*/
//-----------------------------------------------------------------------------
// RunComputer //by larosel
//
// ��ǻ�͸� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunComputer()
{
	if (m_pC_computer == NULL)
	{
		m_pC_computer = new C_VS_UI_COMPUTER;

		assert(m_pC_computer != NULL);
		
		if(m_pC_computer == NULL)
		{
			CloseComputer();
			return;
		}

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		if (m_pC_computer==NULL)
		{
		}
		else
		{

			m_pC_computer->Start();
		}

	}
}


//-----------------------------------------------------------------------------
// CloseComputer //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseComputer()
{

	if (!m_pC_computer)
		return;

	DeleteNew(m_pC_computer);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

}
/*
//-----------------------------------------------------------------------------
// RunTutorialExit //by larosel
//
// Ʃ�丮�� �����⸦ �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTutorialExit()
{
	if (m_pC_tutorial_exit == NULL)
	{
		m_pC_tutorial_exit = new C_VS_UI_TUTORIAL_EXIT;

		assert(m_pC_tutorial_exit != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_tutorial_exit->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTutorialExit //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTutorialExit()
{
	if (!m_pC_tutorial_exit)
		return;

	// ask�� ���� �� �� �ݾ��ش� -_-; by sigi
	CloseTutorialExitAsk();

	DeleteNew(m_pC_tutorial_exit);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// RunTutorialExitAsk	// by larosel
//
// Ʃ�丮�� ������Ȯ���� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTutorialExitAsk(const int select, const char* pName)
{
	if (m_pC_tutorial_exit_ask_dialog != NULL)
	{
		DeleteNew(m_pC_tutorial_exit_ask_dialog);
	}

	// center
	m_pC_tutorial_exit_ask_dialog = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_TutorialExitAsk, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_TUTORIAL_EXIT, select, (void *)pName);

	assert(m_pC_tutorial_exit_ask_dialog != NULL);

	gbl_tutorial_exit_ask_running = true;

	m_pC_tutorial_exit_ask_dialog->Start();	
}

void C_VS_UI_GAME::RunTutorialExitAccept()
{
	assert(m_pC_tutorial_exit);
	m_pC_tutorial_exit->Accept();
}

//-----------------------------------------------------------------------------
// CloseTutorialExitAsk
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTutorialExitAsk()
{
	if (!m_pC_tutorial_exit_ask_dialog)
		return;

	DeleteNew(m_pC_tutorial_exit_ask_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_tutorial_exit_ask_running = false;
	gC_vs_ui.AcquireChatting();
}
*/

//-----------------------------------------------------------------------------
// RunHorn //by larosel
//
// �����̾� ��Ż�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunHorn(int currentZoneID)
{
	if (m_pC_horn == NULL)
	{
		m_pC_horn = new C_VS_UI_HORN(currentZoneID);
		
		assert(m_pC_horn != NULL);
		
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
		
		m_pC_horn->Start();
		
	}
}


//-----------------------------------------------------------------------------
// CloseHorn //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseHorn()
{
	if (!m_pC_horn)
		return;
	
	DeleteNew(m_pC_horn);
	
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningHorn
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningHorn() const
{
	return (m_pC_horn != NULL);
}

//-----------------------------------------------------------------------------
// RunMailBox //by larosel
//
// ������
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunMailBox()
{
	m_pC_mailbox->Start();
}


//-----------------------------------------------------------------------------
// CloseMailBox //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseMailBox()
{
	if (m_pC_mailbox == NULL)
		return;

	m_pC_mailbox->Finish();
	
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}
void C_VS_UI_GAME::CloseMailBoxDialogBox()
{
	if (m_pC_mailbox == NULL)
		return;

	m_pC_mailbox->CloseMailBoxDialogBox();

}
//-----------------------------------------------------------------------------
// IsRunningMailBox
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningMailBox() const
{
	if(m_pC_mailbox == NULL)
		return false;

	return m_pC_mailbox->Running();
}

void	C_VS_UI_GAME::AddMail(int tab_id, DWORD id, SIZE windowSize, const char * sender, const char * title, DWORD date, bool open, int guildid, int guildtype)
{
	if(m_pC_mailbox)
		m_pC_mailbox->AddMail((C_VS_UI_MAILBOX::TAB_ID)tab_id, id, windowSize, sender, title, date, open, guildid, guildtype);
}

void	C_VS_UI_GAME::AddMailContents(int tab_id, DWORD id, const char * contents)
{
	if(m_pC_mailbox)
		m_pC_mailbox->AddMailContents((C_VS_UI_MAILBOX::TAB_ID)tab_id, id, contents);
}

void	C_VS_UI_GAME::DeleteMail(int tab_id, DWORD id)
{
	if(m_pC_mailbox)
		m_pC_mailbox->DeleteMail((C_VS_UI_MAILBOX::TAB_ID)tab_id, id);
}

bool	C_VS_UI_GAME::IsNewMail()
{ 
	if(m_pC_mailbox)
		return m_pC_mailbox->IsNewMail();
	return false; 
}
void	C_VS_UI_GAME::AddHelpMail(DWORD id,  bool open)
{
	if(m_pC_mailbox)
		m_pC_mailbox->AddHelpMail(id,  open);
	
}

//-----------------------------------------------------------------------------
// RunFindingMine //by larosel
//
// �����̾� ��Ż�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunFindingMine()
{
	if (m_pC_finding_mine == NULL)
	{
		m_pC_finding_mine = new C_VS_UI_FINDING_MINE();
		
		assert(m_pC_finding_mine != NULL);
		
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
		
		m_pC_finding_mine->Start();
		
	}
}


//-----------------------------------------------------------------------------
// CloseFindingMine //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseFindingMine()
{
	if (!m_pC_finding_mine)
		return;
	
	DeleteNew(m_pC_finding_mine);
	
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningFindingMine
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningFindingMine() const
{
	return (m_pC_finding_mine != NULL);
}

//-----------------------------------------------------------------------------
// RunSlayerPortal //by larosel
//
// �����̾� ��Ż�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunSlayerPortal( int nExtendLevel )
{
	if (m_pC_slayer_portal == NULL)
	{
		m_pC_slayer_portal = new C_VS_UI_SLAYER_PORTAL( nExtendLevel );

		assert(m_pC_slayer_portal != NULL);
		if (m_pC_slayer_portal == NULL || !m_pC_slayer_portal->IsReady())
		{
			Phoenix_LogSlayerPortalError("[SlayerPortal] creation failed. extend=%d portal=%p", nExtendLevel, m_pC_slayer_portal);
			DeleteNew(m_pC_slayer_portal);
			UI_PopupStrMessage("Slayer portal data is unavailable.");
			return;
		}

		if (m_pC_tribe_interface == NULL)
		{
			Phoenix_LogSlayerPortalError("[SlayerPortal] tribe interface is NULL. extend=%d", nExtendLevel);
			DeleteNew(m_pC_slayer_portal);
			UI_PopupStrMessage("Slayer portal interface is unavailable.");
			return;
		}

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_slayer_portal->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseSlayerPortal //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseSlayerPortal()
{
	if (!m_pC_slayer_portal)
		return;

	DeleteNew(m_pC_slayer_portal);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningSlayerPortal
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningSlayerPortal() const
{
	return (m_pC_slayer_portal != NULL);
}

//-----------------------------------------------------------------------------
// RunTeamList //by larosel
//
// �����̾� ��Ż�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTeamList(bool ready, bool IsUnion)
{
	if (m_pC_team_list == NULL)
	{
		m_pC_team_list = new C_VS_UI_TEAM_LIST(ready, IsUnion);

		assert(m_pC_team_list != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_team_list->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTeamList //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTeamList()
{
	if (!m_pC_team_list)
		return;

	DeleteNew(m_pC_team_list);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningTeamList
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTeamList(bool IsUnion) const
{
	if( m_pC_team_list == NULL )
		return false;
	if(IsUnion)
	{
		if(m_pC_team_list->IsUnionInfo())
			return true;
		else
			return false;

	}
	return true;
//	return (m_pC_team_list != NULL);
}

//-----------------------------------------------------------------------------
// RunTeamMemberList //by larosel
//
// �����̾� ��Ż�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTeamMemberList()
{
	if (m_pC_team_member_list == NULL)
	{
		m_pC_team_member_list = new C_VS_UI_TEAM_MEMBER_LIST;

		assert(m_pC_team_member_list != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_team_member_list->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTeamMemberList //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTeamMemberList()
{
	if (!m_pC_team_member_list)
		return;

	DeleteNew(m_pC_team_member_list);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningTeamMemberList
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTeamMemberList() const
{
	return (m_pC_team_member_list != NULL);
}

//-----------------------------------------------------------------------------
// RunTeamInfo //by larosel
//
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTeamInfo(bool ready, void *info, bool IsUnion)
{
	if (m_pC_team_info == NULL)
	{
		m_pC_team_info = new C_VS_UI_TEAM_INFO(ready, info, IsUnion);

		assert(m_pC_team_info != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_team_info->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTeamInfo //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTeamInfo()
{
	if (!m_pC_team_info)
		return;

	DeleteNew(m_pC_team_info);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}
void C_VS_UI_GAME::CloseTeamInfoDialogBox()
{
	if (!m_pC_team_info)
		return;

	m_pC_team_info->CloseTeamInfoDialogBox();
}

//-----------------------------------------------------------------------------
// IsRunningTeamInfo
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTeamInfo() const
{
	return (m_pC_team_info != NULL);
}

//-----------------------------------------------------------------------------
// RunTeamMemberInfo //by larosel
//
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTeamMemberInfo(C_VS_UI_TEAM_MEMBER_INFO::MEMBER_INFO *info)
{
	if (m_pC_team_member_info == NULL)
	{
		m_pC_team_member_info = new C_VS_UI_TEAM_MEMBER_INFO(info);

		assert(m_pC_team_member_info != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_team_member_info->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTeamMemberInfo //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTeamMemberInfo()
{
	if (!m_pC_team_member_info)
		return;

	DeleteNew(m_pC_team_member_info);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningTeamInfo
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTeamMemberInfo() const
{
	return (m_pC_team_member_info != NULL);
}

//-----------------------------------------------------------------------------
// RunTeamRegist //by larosel
//
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunTeamRegist(bool member, int reg_fee, int rank, char *date, char *team_name, int guild_id)
{
	if (m_pC_team_regist == NULL)
	{
		m_pC_team_regist = new C_VS_UI_TEAM_REGIST(member, reg_fee, rank, date, team_name, guild_id);

		assert(m_pC_team_regist != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_team_regist->Start();

	}
}


//-----------------------------------------------------------------------------
// CloseTeamRegist //by larosel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseTeamRegist()
{
	if (!m_pC_team_regist)
		return;

	DeleteNew(m_pC_team_regist);

	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// IsRunningTeamRegist
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningTeamRegist() const
{
	return (m_pC_team_regist != NULL);
}

//-----------------------------------------------------------------------------
// RunExchangeAsk ( ��ȯ�ҷ��� ��� �̸� )
//
// ��ȯ�Ұųİ� ���´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunExchangeAsk(const char* pName)
{
	if (m_pC_exchange_ask_dialog != NULL)
	{
		DeleteNew(m_pC_exchange_ask_dialog);
	}

	// center
	m_pC_exchange_ask_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 2, 0, ExecF_ExchangeAsk, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_EXCHANGE, 0, (void*)pName);	// by sigi

	assert(m_pC_exchange_ask_dialog != NULL);

	gbl_exchange_ask_running = true;

	m_pC_exchange_ask_dialog->Start();	
}

//-----------------------------------------------------------------------------
// RunExchangeCancel
//
// ��ȯ�Ұųİ� ���´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunExchangeCancel(const char* pName)
{
	if (m_pC_exchange_cancel_dialog != NULL)
	{
		DeleteNew(m_pC_exchange_cancel_dialog);
	}

	// center
	m_pC_exchange_cancel_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 2, 0, ExecF_ExchangeCancel, DIALOG_CANCEL,// | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_EXCHANGE_CANCEL, 0, (void*)pName);	// by sigi

	assert(m_pC_exchange_cancel_dialog != NULL);

	gbl_exchange_cancel_running = true;

	m_pC_exchange_cancel_dialog->Start();	
}

//-----------------------------------------------------------------------------
// RunExchange
//
// ��ȯâ�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunExchange()
{
	if (m_pC_exchange == NULL)
	{
		if(IsRunningGameMenu())CloseGameMenu();
		if(gC_vs_ui.IsRunningOption())gC_vs_ui.CloseOption();
		m_pC_exchange = new C_VS_UI_EXCHANGE;

		assert(m_pC_tribe_interface != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_tribe_interface->ExchangeRunningAnnounced();
		m_pC_exchange->Start();
	}
}


void	C_VS_UI_GAME::RunQuickItemSlot()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->RunQuickItemSlot();
}

void	C_VS_UI_GAME::CloseQuickItemSlot()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->CloseQuickItemSlot();
}

bool	C_VS_UI_GAME::IsRunningQuickItemSlot()
{
	if(m_pC_tribe_interface)
		return m_pC_tribe_interface->IsRunningQuickItemSlot();
	return false;
}

void	C_VS_UI_GAME::RunMinimap()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->RunMinimap();
}

void	C_VS_UI_GAME::CloseMinimap()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->CloseMinimap();
}

bool	C_VS_UI_GAME::IsRunningMinimap()
{
	if(m_pC_tribe_interface)
		return m_pC_tribe_interface->IsRunningMinimap();
	return false;
}

void	C_VS_UI_GAME::RunTraceWindow()
{
	if(m_pC_trace == NULL )
	{
		m_pC_trace = new C_VS_UI_TRACE;

		assert(m_pC_trace != NULL);		
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
		m_pC_trace->Start();
	}	
}

void	C_VS_UI_GAME::CloseTraceWindow()
{
	gC_vs_ui.AcquireChatting();

	if(m_pC_trace == NULL)
		return;
	
	DeleteNew(m_pC_trace);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

bool	C_VS_UI_GAME::IsStartTrace()
{
	if(m_pC_trace == NULL) return false;
	return m_pC_trace->IsStartTrace();
}

void	C_VS_UI_GAME::SetCannotTrace()
{
	if(m_pC_trace == NULL) return;
	m_pC_trace->SetCannotTrace();
}

bool	C_VS_UI_GAME::IsRunningTraceWindow() const
{
	if(m_pC_trace!=NULL)
		return m_pC_trace->Running();

	return false;
}

void	C_VS_UI_GAME::RunXmasCardWindow(const MItem *pItem)
{
	if(m_pC_xmas_card == NULL )
	{
		m_pC_xmas_card = new C_VS_UI_XMAS_CARD(pItem);

		assert(m_pC_xmas_card != NULL);		
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();		
		m_pC_xmas_card->Start();
	}	
}

void	C_VS_UI_GAME::CloseXmasCardWindow()
{
	gC_vs_ui.AcquireChatting();

	if(m_pC_xmas_card == NULL)
		return;
	
	DeleteNew(m_pC_xmas_card);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

bool	C_VS_UI_GAME::IsRunningXmasCardWindow() const
{
	if(m_pC_xmas_card!=NULL)
		return m_pC_xmas_card->Running();

	return false;
}

void	C_VS_UI_GAME::RunWarListWindow()
{
	if( m_pC_war_list == NULL)
	{
		m_pC_war_list = new C_VS_UI_WAR_LIST;

		assert(m_pC_war_list != NULL);
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
		m_pC_war_list->Start();		
	}
}
void	C_VS_UI_GAME::CloseWarListWindow()
{
	if(m_pC_war_list== NULL)
		return;

	DeleteNew(m_pC_war_list);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
bool	C_VS_UI_GAME::IsRunningWarListWindow() const
{
	if(m_pC_war_list!=NULL)
		return m_pC_war_list->Running();

	return false;
}

void	C_VS_UI_GAME::AddWarList(C_VS_UI_WAR_LIST::WarInfo &war_info)
{
	if(m_pC_war_list== NULL )
		return;

	m_pC_war_list->AddWarList(war_info);
}
void	C_VS_UI_GAME::ClearWarList()
{
	if(m_pC_war_list == NULL )
		return;

	m_pC_war_list->ClearWarList();
}

void	C_VS_UI_GAME::RunBloodBibleStatusWindow()
{
	if( m_pC_blood_bible_status == NULL)
	{
		m_pC_blood_bible_status = new C_VS_UI_BLOOD_BIBLE_STATUS;

		assert(m_pC_blood_bible_status != NULL);

		m_pC_blood_bible_status->Start();
	}
}

void	C_VS_UI_GAME::CloseBloodBibleStatus()
{
	if(m_pC_blood_bible_status == NULL)
		return;
	
	m_pC_blood_bible_status->Finish();
	DeleteNew(m_pC_blood_bible_status);
}

bool	C_VS_UI_GAME::IsRunningBloodBibleStatus()
{
	if(m_pC_blood_bible_status != NULL)
		return m_pC_blood_bible_status->Running();

	return false;
}

void	C_VS_UI_GAME::SetBloodBibleStatus(int blood_bible_id, C_VS_UI_BLOOD_BIBLE_STATUS::BLOOD_BIBLE_STATUS &bbs)
{
	if(m_pC_blood_bible_status != NULL)
		m_pC_blood_bible_status->SetBloodBible(blood_bible_id,bbs);
}

void	C_VS_UI_GAME::UnSetBloodBibleStatus(int blood_bible_id)
{
	if(m_pC_blood_bible_status != NULL)
		m_pC_blood_bible_status->UnSetBloodBible(blood_bible_id);
}

void	C_VS_UI_GAME::RunPopupMessage(const char *str, C_VS_UI_POPUP_MESSAGE::POPUP_TYPE type )
{
	if(str == NULL ) return;
	if( m_pC_popup_message != NULL )
		delete m_pC_popup_message;

	m_pC_popup_message = new C_VS_UI_POPUP_MESSAGE(str, type);
	assert( m_pC_popup_message != NULL );
	m_pC_popup_message->Start();
}

void	C_VS_UI_GAME::RunPopupMessage( C_VS_UI_POPUP_MESSAGE::PopupWindowStyle& style)
{
	if( m_pC_popup_message != NULL )
		delete m_pC_popup_message;

	m_pC_popup_message = new C_VS_UI_POPUP_MESSAGE( style );
	assert( m_pC_popup_message != NULL );
	m_pC_popup_message->Start();
}

void	C_VS_UI_GAME::ClosePopupMessage()
{
	if(m_pC_popup_message == NULL)
		return;
	m_pC_popup_message->Finish();
	DeleteNew(m_pC_popup_message);
}

bool	C_VS_UI_GAME::IsRunningPopupMessage()
{
	if(m_pC_popup_message != NULL)
		return m_pC_popup_message->Running();
	return false;
}

void	C_VS_UI_GAME::RunLotteryCard( int& step )
{	
	if( m_pC_lottery_card != NULL )
		CloseLotteryCard();

	m_pC_lottery_card = new C_VS_UI_LOTTERY_CARD( step );
	assert( m_pC_lottery_card != NULL );
	m_pC_lottery_card->Start();
}

void	C_VS_UI_GAME::CloseLotteryCard()
{
	if(m_pC_lottery_card == NULL)
		return;
	m_pC_lottery_card->Finish();
	DeleteNew(m_pC_lottery_card);
}

bool	C_VS_UI_GAME::IsRunningLotteryCard()
{
	if(m_pC_lottery_card != NULL)
		return m_pC_lottery_card->Running();
	return false;
}


//-----------------------------------------------------------------------------
// GetInventoryOpenState
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_GAME::GetInventoryOpenState() const
{
	if (m_pC_tribe_interface)
		return m_pC_tribe_interface->GetInventoryOpenState();

	return false;
}

bool C_VS_UI_GAME::GetGearOpenState() const
{
	if (m_pC_tribe_interface)
		return m_pC_tribe_interface->GetGearOpenState();

	return false;
}

//-----------------------------------------------------------------------------
// GetZoneID
//
// 
//-----------------------------------------------------------------------------
int C_VS_UI_GAME::GetZoneID()
{
	if (m_pC_tribe_interface != NULL)
	{
		return m_pC_tribe_interface->GetZoneID();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// SetZoneName
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetZone(int zone_id)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetZone(zone_id);
	}

	C_VS_UI_WORLD_MAP::SetNowZoneID(zone_id);
}

void	C_VS_UI_GAME::SetBlock(int x, int y )
{
	if( m_pC_tribe_interface != NULL )
	{
		m_pC_tribe_interface->SetBlock(x,y);
	}
}

//-----------------------------------------------------------------------------
// SetTime
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetTime(const char * str)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetTime(str);
	}
}

//-----------------------------------------------------------------------------
// SetXY
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetXY(int x, int y)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetXY(x, y);
	}
	
	if(m_pC_world_map)
	{
		m_pC_world_map->SetPlayerXY(x, y);
	}
}

//-----------------------------------------------------------------------------
// SetSize
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetSize(SIZE size)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetSize(size);
	}
}

//-----------------------------------------------------------------------------
// SetSafetyZone
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetSafetyZone(RECT rect, bool my_zone)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetSafetyZone(rect, my_zone);
	}
}

//-----------------------------------------------------------------------------
// SetNPC
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetNPC(MINIMAP_NPC npc)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetNPC(npc);
	}
}

void C_VS_UI_GAME::ClearNPC()
{
	if( m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->ClearNPC();
	}
}

//-----------------------------------------------------------------------------
// SetPortal
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetPortal(RECT rect, int id)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetPortal(rect, id);
	}
}

//-----------------------------------------------------------------------------
// SetDate
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::SetDate(const char * str)
{
	if (m_pC_tribe_interface != NULL)
	{
		m_pC_tribe_interface->SetDate(str);
	}
}



//-----------------------------------------------------------------------------
// ToggleESC4UI
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ToggleESC4UI()
{
	if (m_pC_gamemenu)
	{
		DeleteNew(m_pC_gamemenu);
	}
	else
	{
		ClosePopupWindow();
		RunGameMenu();
	}

	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		PlaySound(SOUND_SLAYER_BUTTON);
		break;
		
	case RACE_VAMPIRE:
		PlaySound(SOUND_VAMPIRE_BUTTON);
		break;
		
	case RACE_OUSTERS:
		PlaySound(SOUND_VAMPIRE_BUTTON);
		break;
	}

}

//-----------------------------------------------------------------------------
// ResetSlayerQuickItemSize
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ResetSlayerQuickItemSize()
{
	m_pC_tribe_interface->ResetSlayerQuickItemSize();
}

void C_VS_UI_GAME::ResetOustersQuickItemSize()
{
	m_pC_tribe_interface->ResetOustersQuickItemSize();
}

// 2004, 5, 17 sobeit add start
void C_VS_UI_GAME::HotKey_SummonPet()
{
	assert(m_pC_hotkey);
	
	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_SummonPet();
}
//2004, 5, 17 sobeit add end
void C_VS_UI_GAME::HotKey_MailBox()
{
	assert(m_pC_hotkey);
	
	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_MailBox();
}

void C_VS_UI_GAME::HotKey_PetInfo()
{
	assert(m_pC_hotkey);
	
	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_PetInfo();
}

void C_VS_UI_GAME::HotKey_Sysrq()		// printscreen sysrq
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Sysrq();

}

void C_VS_UI_GAME::HotKey_Slash()		// ctrl + /
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Slash();

}

void C_VS_UI_GAME::HotKey_Where()			// ä�ø��ɾ� /���
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Where();
}

void C_VS_UI_GAME::HotKey_Background_Music()		// ä�ø��ɾ� /�����
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Background_Music();

}

void C_VS_UI_GAME::HotKey_Effect_Music()		// ä�ø��ɾ� /ȿ����
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Effect_Music();

}

void C_VS_UI_GAME::HotKey_Chat_Help()		// ä�ø��ɾ� /ä�õ���
{
	assert(m_pC_hotkey);
	
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Chat_Help();

}

void C_VS_UI_GAME::HotKey_Party()
{
	assert(m_pC_hotkey);

	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Party();
}

void C_VS_UI_GAME::HotKey_Mark()
{
	assert(m_pC_hotkey);

	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Mark();
}

void C_VS_UI_GAME::HotKey_Inventory(bool IsCheckSubInventory)
{
	assert(m_pC_hotkey);
	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Inventory(IsCheckSubInventory);
}

void C_VS_UI_GAME::HotKey_Gear()
{
	assert(m_pC_hotkey);
	if (!m_pC_gamemenu)
	{ 
		m_pC_hotkey->HotKey_Gear();
		m_pC_hotkey->HotKey_Inventory();
	}

}

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// C_VS_UI_GAME::HotKey_GearChange()
void C_VS_UI_GAME::HotKey_GearChange()
{
	assert(m_pC_hotkey);

	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_GearChange();
}

void C_VS_UI_GAME::HotKey_GearChangeShow()
{
	assert(m_pC_hotkey);

	//if (!gpC_window_manager->GetShowState(m_pC_gamemenu))
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_GearChangeShow();
}
#endif	// __GEAR_SWAP_CHANGE

void C_VS_UI_GAME::HotKey_QuickItemSlot()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_QuickItemSlot();
}

void C_VS_UI_GAME::HotKey_CharInfo()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_CharInfo();
}

void C_VS_UI_GAME::HotKey_SkillInfo()
{
	assert(m_pC_hotkey);

	if (m_pC_gamemenu)
		return;

	// 2026-05-23: VS_UI_SKILL_VIEW referansta aktif degil ve Phoenix'te
	// skillwindow.spk frame eslemesiyle overlap uretiyor. Skill hotkey yolu
	// referansin GameMenu skill davranisiyla ayni aktif pencereye yonlenir.
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:  PlaySound(SOUND_SLAYER_BUTTON);     break;
	case RACE_VAMPIRE: PlaySound(SOUND_VAMPIRE_BUTTON);    break;
	case RACE_OUSTERS: PlaySound(SOUND_OUSTERS_INTERFACE); break;
	}

	if (IsRunningSkillView())
		CloseSkillView();

	if (m_pC_tribe_interface != NULL)
		m_pC_tribe_interface->HotKey_Military();
}

void C_VS_UI_GAME::HotKey_Grade3Info()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Grade3Info();
}

void C_VS_UI_GAME::HotKey_Grade2Info()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Grade2Info();
}

void C_VS_UI_GAME::HotKey_Grade1Info()
{
	assert(m_pC_hotkey);

	const int hasGameMenu = (m_pC_gamemenu != NULL) ? 1 : 0;
	const int rankBonusAvailable = (g_pSystemAvailableManager != NULL && g_pSystemAvailableManager->IsAvailableRankBonusSystem()) ? 1 : 0;
	CT_HOTKEY("C_VS_UI_GAME::HotKey_Grade1Info entry m_pC_hotkey=%p m_pC_gamemenu=%p hasGameMenu=%d systemMgr=%p rankBonusAvailable=%d",
		(void*)m_pC_hotkey, (void*)m_pC_gamemenu, hasGameMenu, (void*)g_pSystemAvailableManager, rankBonusAvailable);

	if (!m_pC_gamemenu && g_pSystemAvailableManager->IsAvailableRankBonusSystem() )
	{
		CT_HOTKEY("C_VS_UI_GAME::HotKey_Grade1Info forwarding to tribe hotkey");
		m_pC_hotkey->HotKey_Grade1Info();
	}
	else
	{
		CT_HOTKEY("C_VS_UI_GAME::HotKey_Grade1Info blocked hasGameMenu=%d rankBonusAvailable=%d",
			hasGameMenu, rankBonusAvailable);
	}
}

void C_VS_UI_GAME::HotKey_Help()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Help();
}

void C_VS_UI_GAME::HotKey_WorldMap()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_WorldMap();
}

void C_VS_UI_GAME::HotKey_Minimap()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Minimap();
}

void C_VS_UI_GAME::HotKey_Chat_Up()			// ctrl + PgUp
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Chat_Up();
}

void C_VS_UI_GAME::HotKey_Chat_Dn()			// ctrl + PgDn
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Chat_Dn();
}

void C_VS_UI_GAME::HotKey_Tribe_Prior()			// alt + PgUp
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Tribe_Prior();
}

void C_VS_UI_GAME::HotKey_Tribe_Next()			// alt + PgDn
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Tribe_Next();
}

void C_VS_UI_GAME::HotKey_Guild_Info()			// ctrl + l
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Guild_Info();
}

void C_VS_UI_GAME::HotKey_Guild_Member_List()	// ctrl + v
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Guild_Member_List();
}

void C_VS_UI_GAME::HotKey_Coer_Attack()			// ctrl + o
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Coer_Attack();
}

// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
void C_VS_UI_GAME::HotKey_Auto_Attack()			// ctrl + j
{
	assert(m_pC_hotkey);
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Auto_Attack();
}
#endif  //__AUTO_ATTACT
void C_VS_UI_GAME::HotKey_ZoneChat()		// ctrl + z
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_ZoneChat();
}

void C_VS_UI_GAME::HotKey_GuildChat()		// ctrl + g
{
	assert(m_pC_hotkey);
// end
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_GuildChat();
}

void C_VS_UI_GAME::HotKey_UnionChat()		// ctrl + d
{
	assert(m_pC_hotkey);
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_UnionChat();
}
void C_VS_UI_GAME::HotKey_Whisper()		// ctrl + w
{
	assert(m_pC_hotkey);
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_Whisper();
}
void C_VS_UI_GAME::HotKey_ExtendChat()		// ctrl + e
{
	assert(m_pC_hotkey);
	if (!m_pC_gamemenu)
		m_pC_hotkey->HotKey_ExtendChat();
}

void C_VS_UI_GAME::HotKey_ESC()
{
	

	//
	// ESCŰ�� �ۿ�
	//
	// (1) Window�� �̵����ΰ�? -> Window�� ������Ų��.
	// (2) ��ư�� ������ �ִ°�? -> ������ �����Ѵ�.
	// (3) Popup Window�� �� �ִ°�? -> Window�� �ݴ´�.
	// (3-2) event lock�� �����Ѵ�.
	// (4) Item�� ��� �ִ°�? -> GameMenu�� ȣ���Ѵ�. �̶��� ��� Mouse pointer�� Item�� �ƴ�
	//										������ ������ ���Ѵ�.
	//

	if( gbl_skip_escape )
	{
		gbl_skip_escape = false;
		return;
	}

	Window * p_window = gpC_window_manager->GetMovingWindow();
	if (p_window != NULL) // (1)
	{
		p_window->ClearInputState();
		gpC_window_manager->CancelPushStateOfCurrentPushedWindow(); // !�߿�
	}
	else if (gpC_window_manager->CancelPushStateOfCurrentPushedWindow() == false) // (2)
	{
		//
		// Popup Window�� ���� ���� �켱 MessageBox�� �����Ѵ�. �׵��� topmost Window��
		// ������� ó���Ǳ� ���Ѵ�. �̰��� hot-key ��å(VS_UI.cpp ����)�� ���� ���̴�.
		// �׷��ϱ� escape���� �� Window�� �´� ó���� �ϰ��ϱ� ���ؼ� Window�� message��
		// ������(topmost Window��).
		//
		Window * p_first_window = gpC_window_manager->GetFirstPriorityWindow();
		if (p_first_window != NULL)
			if (p_first_window->GetAttributes()->topmost == true)
			{
				if (p_first_window != m_pC_gamemenu)
				{
					p_first_window->KeyboardControl(WM_KEYDOWN, VK_ESCAPE, 0);
					return;
				}
			}
/*
		//
		// Inventory/Gear�� �ѽ��� Popup Window�� ����. �� ���� ���� �� �ִٸ� ���� ������ ���̴�.
		// �׵��� pinned Window�̹Ƿ� �����Ǿ� �ִ� ���� Interface���� no pinned Window�� ����
		// ��µȴ�. ���� Inventory/Gear -> Other Window ������ close�Ѵ�.
		//
		if (m_pC_tribe_interface->CloseInventoryGearWindow() == false)
		{
			if (gbl_sell_running)
			{
				gC_vs_ui.TerminateItemSelling();
			}
			else if (gbl_buy_running)
			{
				gpC_base->SendMessage(UI_CLOSE_SHOP);
			}
			else if (m_pC_tribe_interface->SkillTreeRunning())
			{
				CloseSkillTree();
			}
			else
			{
				if (gpC_mouse_pointer->GetPickUpItem() != NULL)
				{
				}
				
				if (m_pC_gamemenu)
				{
					DeleteNew(m_pC_gamemenu);
				}
				else
				{
					RunGameMenu();
				}
			}
		}*/

		if (ClosePopupWindow() == false)
		{
			if(IsRunningGameMenu())
				CloseGameMenu();
			else
				RunGameMenu();
			
			switch(g_eRaceInterface)
			{
			case RACE_SLAYER:
				PlaySound(SOUND_SLAYER_BUTTON);
				break;

			case RACE_VAMPIRE:
				PlaySound(SOUND_VAMPIRE_BUTTON);
				break;

			case RACE_OUSTERS:
				PlaySound(SOUND_VAMPIRE_BUTTON);
				break;
			}
		}
	}
}

void C_VS_UI_GAME::HotKey_F1()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F1();
}

void C_VS_UI_GAME::HotKey_F2()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F2();
}

void C_VS_UI_GAME::HotKey_F3()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F3();
}

void C_VS_UI_GAME::HotKey_F4()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F4();
}

void C_VS_UI_GAME::HotKey_F5()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F5();
}

void C_VS_UI_GAME::HotKey_F6()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F6();
}

void C_VS_UI_GAME::HotKey_F7()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F7();
}

void C_VS_UI_GAME::HotKey_F8()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F8();
}

void C_VS_UI_GAME::HotKey_F9()				
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F9();
}

void C_VS_UI_GAME::HotKey_F10()			
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F10();
}

void C_VS_UI_GAME::HotKey_F11()			
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_F11();
}

void C_VS_UI_GAME::HotKey_F12()
{
	// 2026-05-18 ROOT CAUSE FIX: F12 crash — Release build assert() no-op.
	// m_pC_hotkey NULL ise virtual call AV. CT_Done log + NULL guard.
	// Root cause TODO: m_pC_hotkey neden NULL? (initialize chain incele)
	CT_Done("HotKey_F12 ENTER m_pC_hotkey=%p", (void*)m_pC_hotkey);
	assert(m_pC_hotkey);
	if (m_pC_hotkey == NULL) {
		CT_Done("HotKey_F12 SKIP — m_pC_hotkey NULL (Release assert no-op bypass)");
		return;
	}
	m_pC_hotkey->HotKey_F12();
}

void C_VS_UI_GAME::Hotkey_Function(int FKey)
{
	assert(m_pC_hotkey);

	m_pC_hotkey->Hotkey_Function(FKey) ; 
	//if (m_pC_game)
	//	m_pC_game->Hotkey_Function(FKey);
}

// hyungony
void C_VS_UI_GAME::HotKey_PartyChat()
{
	assert(m_pC_hotkey);
	m_pC_hotkey->HotKey_PartyChat();
}
void C_VS_UI_GAME::HotKey_NormalChat()
{
	assert(m_pC_hotkey);
	m_pC_hotkey->HotKey_NormalChat();
}

void C_VS_UI_GAME::HotKey_Quest()
{
	assert(m_pC_hotkey);

	m_pC_hotkey->HotKey_Quest();
}

void C_VS_UI_GAME::ResetHotKey()
{
//	if(m_C_skill != NULL)
//		m_C_skill.ResetSkillSet();
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->ResetSkillSet();
}
//void C_VS_UI_GAME::HotKey_EveryInventoryOpen()
//{
//	MItemClassFinder itemFinder(ITEM_CLASS_SUB_INVENTORY);
//	ITEM_LIST FindItem = ((MItemManager*)g_pInventory)->FindItemList(itemFinder);
//	ITEM_LIST::iterator listpos = FindItem.begin()
//	
//	for(; listpos != FindItem.end(); ++listpos)
//	{
//		CGUseItemFromInventory _CGUseItemFromInventory;
//		_CGUseItemFromInventory.setObjectID( pos->GetID );
//		_CGUseItemFromInventory.setX( pos->GetGridX() );
//		_CGUseItemFromInventory.setY( pos->GetGridY() );
//		
//		g_pSocket->sendPacket( &_CGUseItemFromInventory );
//		
//		g_pPlayer->SetItemCheckBuffer( this, MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY);
//	}
//}
//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::Start()
{
	PI_Processor::Start();

//	if(bl_load_set)
//	{
//		// window_set file loading
//		char sz_filename[512];
//		wsprintf(sz_filename, "UserSet\\%s.set", g_char_slot_ingame.sz_name.c_str());
//		
//		ifstream file(sz_filename, ios::binary | ios::nocreate);
//		
//		if(file)
//		{
//			gpC_vs_ui_window_manager->LoadFromFile(file);
//			file.close();
//		}
//	}

	gbl_item_lock	= false;
	gbl_gear_lock	= false;
	gbl_item_trade_lock = false;

	m_pC_hotkey = NULL;

	if(m_pC_chatting != NULL)
	{
		DeleteNew(m_pC_chatting);
		m_pC_chatting = NULL;
	}
	m_pC_chatting = new C_VS_UI_CHATTING;

	if(m_pC_mailbox != NULL)
	{
		DeleteNew(m_pC_mailbox);
		m_pC_mailbox = NULL;
	}
	m_pC_mailbox = new C_VS_UI_MAILBOX;
	
	MString mailFileName;
	mailFileName.Format("UserSet\\%s-%d-%d.mail", g_char_slot_ingame.sz_name.c_str(),g_Dimension,g_pUserInformation->WorldID);

	m_pC_mailbox->LoadFromFile(mailFileName.GetString());

	bool IsTestServer = false;
	//ifstream IsTestServerFile(FILE_INFO_TESTSERVER, ios::binary);
	//IsTestServerFile.read((char *)&IsTestServer, 1);
	//IsTestServerFile.close();
	
// 	if(!_access(FILE_INFO_TESTSERVER, 0))
// 	{
		Properties TestServerConfig;
		TestServerConfig.load(FILE_INFO_TESTSERVER);			
		IsTestServer = TestServerConfig.getPropertyInt("TestServer") != 0;
//	}

	g_pUserInformation->IsTestServer = IsTestServer;
//	for(int zone = 0; zone < zoneNum; zone++)

	
	// default: Slayer interface
	CT_Done("DIAG: C_VS_UI_GAME::Start race switch (race=%d)", (int)g_eRaceInterface);
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		ChangeToSlayerInterface();
		break;

	case RACE_VAMPIRE:
		ChangeToVampireInterface();
		break;

	case RACE_OUSTERS:
		ChangeToOustersInterface();
	}
	CT_Done("DIAG: C_VS_UI_GAME::Start post race switch");

	// Client ���� Inventory data structure.
	CT_Done("DIAG: C_VS_UI_GAME::Start pre g_pInventory->Init g_pInventory=%p", g_pInventory);
	g_pInventory->Init(C_VS_UI_INVENTORY::GRID_X, C_VS_UI_INVENTORY::GRID_Y);
	CT_Done("DIAG: C_VS_UI_GAME::Start pre g_pSlayerGear->Init g_pSlayerGear=%p", g_pSlayerGear);
	g_pSlayerGear->Init();
	CT_Done("DIAG: C_VS_UI_GAME::Start pre g_pVampireGear->Init g_pVampireGear=%p", g_pVampireGear);
	g_pVampireGear->Init();
	CT_Done("DIAG: C_VS_UI_GAME::Start pre g_pOustersGear->Init g_pOustersGear=%p", g_pOustersGear);
	g_pOustersGear->Init();
	CT_Done("DIAG: C_VS_UI_GAME::Start pre g_pMilitarySlot->Init g_pMilitarySlot=%p", g_pMilitarySlot);
	g_pMilitarySlot->Init(MilitarySlot::L_MAX);
	CT_Done("DIAG: C_VS_UI_GAME::Start post all gear/inv inits");

	g_descriptor_manager.Unset();
//	MHelpMessageManager::Instance().LoadHelpMessageRpk("helpmessage.txt");
	CT_Done("DIAG: C_VS_UI_GAME::Start exit");
}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::Process()
{
	ReleaseFinishedGameMenu();
}

void C_VS_UI_GAME::UnselectSkill()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->UnselectSkill();
}

void C_VS_UI_GAME::ChangeSkillWheel(bool direct)
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->ChangeSkillWheel(direct);
}

void C_VS_UI_GAME::SelectSkill(int id)
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->SelectSkill(id);
}


//-----------------------------------------------------------------------------
// RequestDie
//
// ����-_-��û? ������ ���
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RequestDie(DWORD timer, bool enforce)
{
	if(m_pC_request_die)
	{
		DeleteNew(m_pC_request_die);
		m_pC_request_die = NULL;
	}

	m_pC_request_die = new C_VS_UI_REQUEST_DIE(timer, enforce);
	m_pC_request_die->Start();

}

//-----------------------------------------------------------------------------
// FinishRequestDie
//
// ��Ƽ��û ������ ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishRequestDie()
{
	if(m_pC_request_die)
	{
		DeleteNew(m_pC_request_die);
		m_pC_request_die = NULL;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRequestDie
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRequestDie() const
{
	if(m_pC_request_die == NULL)return false;
	return m_pC_request_die->Running();
}

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
//-----------------------------------------------------------------------------
// RequestPremiumGiveItem
//
// �����̾� ���� ������ ���
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::Request_PremiumGiveItem()
{
	if(g_pPremiumGiveItemInfoList->GetItemNum()<=0)
	{
		FinishRequest_PremiumGiveItem();
		return;
	}

	if(m_pC_request_premium_give_item!=NULL)
	{
		m_pC_request_premium_give_item->Update_Item_List();
		return;
	}

	m_pC_request_premium_give_item = new C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM();
	m_pC_request_premium_give_item->Start();
}

//-----------------------------------------------------------------------------
// FinishRequestPremiumGiveItem
//
// �����̾� ���� ������ ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishRequest_PremiumGiveItem()
{
	if(m_pC_request_premium_give_item)
	{
		DeleteNew(m_pC_request_premium_give_item);
		m_pC_request_premium_give_item = NULL;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRequestDie
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRequest_PremiumGiveItem() const
{
	if(m_pC_request_premium_give_item == NULL) return false;
	return m_pC_request_premium_give_item->Running();
}
#endif //__PREMIUM_GIVE_ITEM_UI

// party
//-----------------------------------------------------------------------------
// RequestParty
//
// ��Ƽ��û ������ ���
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RequestParty(const char *name, DWORD timer)
{
	if(m_pC_request_party)
	{
		DeleteNew(m_pC_request_party);
		m_pC_request_party = NULL;
	}

	m_pC_request_party = new C_VS_UI_REQUEST_PARTY(name, timer);
	m_pC_request_party->Start();

}

//-----------------------------------------------------------------------------
// FinishRequestParty
//
// ��Ƽ��û ������ ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishRequestParty()
{
	if(m_pC_request_party)
	{
		DeleteNew(m_pC_request_party);
		m_pC_request_party = NULL;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRequestParty
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRequestParty() const
{
	if(m_pC_request_party == NULL)return false;
	return m_pC_request_party->Running();
}

bool	C_VS_UI_GAME::IsRunningPartyAsk() const
{
	if(m_pC_party_ask_dialog == NULL||gbl_party_ask_running==false)
		return false;

	return m_pC_party_ask_dialog->Running();
}

//-----------------------------------------------------------------------------
// RunPartyAsk ( ��û�� ��� �̸� )
//
// ��Ƽ�� �����Ұųİ� ���´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunPartyAsk(const char* pName, C_VS_UI_REQUEST_PARTY::REQUEST_PARTY type)
{
	if (m_pC_party_ask_dialog != NULL)
	{
		DeleteNew(m_pC_party_ask_dialog);
	}
//	OutputDebugString("party 1\n");

	// center
	if(type == C_VS_UI_REQUEST_PARTY::INVITE)
	{
		m_pC_party_ask_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 2, 0, ExecF_PartyAsk, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_PARTY_INVITE, 0, (void*)pName);	// by sigi
	}
	else if(type == C_VS_UI_REQUEST_PARTY::REQUEST)
	{
		m_pC_party_ask_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 2, 0, ExecF_PartyAsk, DIALOG_OK|DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_PARTY_REQUEST, 0, (void*)pName);	// by sigi
	}
	assert(m_pC_party_ask_dialog != NULL);


	gbl_party_ask_running = true;
	m_pC_party_ask_dialog->Start();	
}

//-----------------------------------------------------------------------------
// RunPartyCancel
//
// ��Ƽ�� �����ϴ°� ��Ѹ�����..
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunPartyCancel(const char* pName)
{
	if (m_pC_party_cancel_dialog != NULL)
	{
		DeleteNew(m_pC_party_cancel_dialog);
	}

	// center
	m_pC_party_cancel_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 2, 1, ExecF_PartyCancel, DIALOG_CANCEL,// | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_PARTY_CANCEL, 0, (void*)pName);	// by sigi

	assert(m_pC_party_cancel_dialog != NULL);

	gbl_party_cancel_running = true;

	m_pC_party_cancel_dialog->Start();
}

//-----------------------------------------------------------------------------
// RunEnchant
//
// Enchant�ҷ� �ϰ� ������...
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunEnchant(int value)
{
	if (m_pC_enchant_dialog != NULL)
	{
		DeleteNew(m_pC_enchant_dialog);
	}

	// center 50 , 30 
	m_pC_enchant_dialog = new C_VS_UI_ASK_DIALOG(-1, 200 , 3, 0, ExecF_Enchant, DIALOG_CANCEL | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_ENCHANT, value);	// by sigi

	assert(m_pC_enchant_dialog != NULL);

	gbl_enchant_running = true;

	m_pC_enchant_dialog->Start();	
}

//-----------------------------------------------------------------------------
// CloseEnchant
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseEnchant()
{
	if (!m_pC_enchant_dialog)
		return;

	DeleteNew(m_pC_enchant_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_enchant_running = false;

	gC_vs_ui.AcquireChatting();
}

#if __CONTENTS(__TUNING_ITEM)
void C_VS_UI_GAME::RunTuningConfirm1(int value, char *name1, char *name2)	// Ʃ�� ����
{
	if (m_pC_enchant_dialog != NULL)
	{
		DeleteNew(m_pC_enchant_dialog);
	}

	char Name[256];
	sprintf((char *)Name,"%s*%s",name1,name2);

	m_pC_enchant_dialog = new C_VS_UI_ASK_DIALOG(-1, 200 , 3, 0, ExecF_Enchant, DIALOG_CANCEL | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_ENCHANT, value, (void *)Name);	// by cheon

	assert(m_pC_enchant_dialog != NULL);

	gbl_enchant_running = true;
	m_pC_enchant_dialog->Start();	
}

void C_VS_UI_GAME::RunTuningConfirm2(int value, char *name1, char *name2)	// Ʃ�� �Ұ���
{
	if (m_pC_enchant_dialog != NULL)
	{
		DeleteNew(m_pC_enchant_dialog);
	}

	char Name[256];
	sprintf((char *)Name,"%s*%s",name1,name2);
	
	m_pC_enchant_dialog = new C_VS_UI_ASK_DIALOG(-1, 200 , 3, 0, ExecF_Enchant, DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_ENCHANT, value, (void *)Name);	// by cheon

	assert(m_pC_enchant_dialog != NULL);

	gbl_enchant_running = true;
	m_pC_enchant_dialog->Start();
}

void C_VS_UI_GAME::RunConfirmError(char *message) // �Ұ��� �޼���
{
	if (m_pC_enchant_dialog != NULL)
	{
		DeleteNew(m_pC_enchant_dialog);
	}
	
	m_pC_enchant_dialog = new C_VS_UI_ASK_DIALOG(-1, 200 , 3, 0, ExecF_Enchant, DIALOG_CANCEL, 
												C_VS_UI_ASK_DIALOG::ASK_ERROR_MESSAGE, 0, (void *)message);	// by cheon

	assert(m_pC_enchant_dialog != NULL);

	gbl_enchant_running = true;
	m_pC_enchant_dialog->Start();
}
#endif //__TUNING_ITEM

void C_VS_UI_GAME::RunThirdEnchant(int value)
{
	if (m_pC_thirdenchant_dialog != NULL)
	{
		DeleteNew(m_pC_thirdenchant_dialog);
	}

	// center 50 , 30 
	m_pC_thirdenchant_dialog = new C_VS_UI_ASK_DIALOG(-1, 200 , 3, 0, ExecF_Third_Enchant, DIALOG_CANCEL | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_THIRD_ENCHANT, value);	// by sigi

	assert(m_pC_thirdenchant_dialog != NULL);

	gbl_enchant_running = true;

	m_pC_thirdenchant_dialog->Start();
}
void C_VS_UI_GAME::CloseThirdEnchant()
{
	if (!m_pC_thirdenchant_dialog)
		return;

	DeleteNew(m_pC_thirdenchant_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_enchant_running = false;

	gC_vs_ui.AcquireChatting();

}



//-----------------------------------------------------------------------------
// RunUsePetFood
//
// �� ���� ���Ϸ� �ϰ� ������...
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunUsePetFood(DWORD UsingObjectID, MItem* SubInventory)
{
	if (m_pC_use_pet_food_dialog != NULL)
	{
		DeleteNew(m_pC_use_pet_food_dialog);
	}
	
	// center
	m_pC_use_pet_food_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 5, 1, ExecF_UsePetFood, DIALOG_CANCEL | DIALOG_OK, 
		C_VS_UI_ASK_DIALOG::ASK_USE_PET_FOOD, UsingObjectID, SubInventory);	// by sigi
	
	assert(m_pC_use_pet_food_dialog != NULL);
	
	gbl_use_pet_food_running = true;
	
	m_pC_use_pet_food_dialog->Start();	
}

//-----------------------------------------------------------------------------
// CloseUsePetFood
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseUsePetFood()
{
	if (!m_pC_use_pet_food_dialog)
		return;
	
	DeleteNew(m_pC_use_pet_food_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
	
	gbl_use_pet_food_running = false;
	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// RunKeepPetItemFood
//
// ���� ���� �ҷ�? �ϰ� ������...
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunKeepPetItemDialog()
{
	if (m_pC_keep_petitem_dialog != NULL)
	{
		DeleteNew(m_pC_keep_petitem_dialog);
	}
	
	// center
	m_pC_keep_petitem_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 5, 1, ExecF_Keep_PetItem, DIALOG_CANCEL | DIALOG_OK, 
		C_VS_UI_ASK_DIALOG::ASK_KEEP_PETITEM, 0);	// by sigi
	
	assert(m_pC_keep_petitem_dialog != NULL);
	
	gbl_keep_petitem_running = true;
	
	m_pC_keep_petitem_dialog->Start();	
}

//-----------------------------------------------------------------------------
// CloseKeepPetItemDialog
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseKeepPetItemDialog()
{
	if (!m_pC_keep_petitem_dialog)
		return;
	
	DeleteNew(m_pC_keep_petitem_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
	
	gbl_keep_petitem_running = false;
	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// RunGetKeepPetItemDialog
//
// ���� ���� �ҷ�? �ϰ� ������...
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunGetKeepPetItemDialog()
{
	if (m_pC_get_keep_petitem_dialog != NULL)
	{
		DeleteNew(m_pC_get_keep_petitem_dialog);
	}
	
	// center
	m_pC_get_keep_petitem_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 5, 1, ExecF_Get_Keep_PetItem, DIALOG_CANCEL | DIALOG_OK, 
		C_VS_UI_ASK_DIALOG::ASK_GET_KEEP_PETITEM, 0);	// by sigi
	
	assert(m_pC_get_keep_petitem_dialog != NULL);
	
	gbl_get_keep_petitem_running = true;
	
	m_pC_get_keep_petitem_dialog->Start();	
}

//-----------------------------------------------------------------------------
// CloseKeepPetItemDialog
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseGetKeepPetItemDialog()
{
	if (!m_pC_get_keep_petitem_dialog)
		return;
	
	DeleteNew(m_pC_get_keep_petitem_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
	
	gbl_get_keep_petitem_running = false;
	gC_vs_ui.AcquireChatting();
}
//-----------------------------------------------------------------------------
// RunNoSearchResult
//
// �˻���� ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunNoSearchResult()
{
	if (m_pC_no_search_result_dialog != NULL)
	{
		DeleteNew(m_pC_no_search_result_dialog);
	}

	// center
	m_pC_no_search_result_dialog = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_NoSearchResult, DIALOG_OK,// | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_NO_SEARCH_RESULT, 0);	// by sigi

	assert(m_pC_no_search_result_dialog != NULL);

	gbl_no_search_result_running = true;

	m_pC_no_search_result_dialog->Start();	
}

//-----------------------------------------------------------------------------
// RunDepositLimit
//
// �˻���� ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunDepositLimit()
{
	if (m_pC_deposit_limit != NULL)
	{
		DeleteNew(m_pC_deposit_limit);
	}

	// center
	m_pC_deposit_limit = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_DepositLimit, DIALOG_OK,// | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_DEPOSIT_LIMIT, 0);	// by sigi

	assert(m_pC_deposit_limit != NULL);

	gbl_deposit_limit_running = true;

	m_pC_deposit_limit->Start();	
}

void	C_VS_UI_GAME::RunBringFeeLimit()
{
	if(m_pC_bring_fee_limit != NULL)
		DeleteNew(m_pC_bring_fee_limit);

	m_pC_bring_fee_limit = new C_VS_UI_ASK_DIALOG(-1,-1,2,0,ExecF_BringFeeLimit,DIALOG_OK,
		C_VS_UI_ASK_DIALOG::ASK_BRINGFEE_LIMIT, 0);

	assert(m_pC_bring_fee_limit != NULL);

	gbl_bring_fee_limit_running = true;

	m_pC_bring_fee_limit->Start();
}

//-----------------------------------------------------------------------------
// RunWithdrawLimit
//
// �˻���� ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunWithdrawLimit()
{
	if (m_pC_withdraw_limit != NULL)
	{
		DeleteNew(m_pC_withdraw_limit);
	}

	// center
	m_pC_withdraw_limit = new C_VS_UI_ASK_DIALOG(-1, -1, 2, 0, ExecF_WithdrawLimit, DIALOG_OK,// | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_WITHDRAW_LIMIT, 0);	// by sigi

	assert(m_pC_withdraw_limit != NULL);

	gbl_withdraw_limit_running = true;

	m_pC_withdraw_limit->Start();	
}


//-----------------------------------------------------------------------------
// ClosePartyAsk
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ClosePartyAsk()
{
	if (!m_pC_party_ask_dialog)
		return;

	DeleteNew(m_pC_party_ask_dialog);

//	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_party_ask_running = false;

	gC_vs_ui.AcquireChatting();

}

//-----------------------------------------------------------------------------
// ClosePartyCancel
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ClosePartyCancel()
{
	if (!m_pC_party_cancel_dialog)
		return;

	DeleteNew(m_pC_party_cancel_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_party_cancel_running = false;

	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// CloseNoSearchResult
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseNoSearchResult()
{
	if (!m_pC_no_search_result_dialog)
		return;

	DeleteNew(m_pC_no_search_result_dialog);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_no_search_result_running = false;

//	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// CloseDepositLimit
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseDepositLimit()
{
	if (!m_pC_deposit_limit)
		return;

	DeleteNew(m_pC_deposit_limit);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_deposit_limit_running = false;

//	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// CloseWithdrawLimit
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseWithdrawLimit()
{
	if (!m_pC_withdraw_limit)
		return;

	DeleteNew(m_pC_withdraw_limit);
	
	//assert(m_pC_tribe_interface != NULL);
	//m_pC_tribe_interface->DoCommonActionAfterEventOccured();	

	gbl_withdraw_limit_running = false;

//	gC_vs_ui.AcquireChatting();
}


//-----------------------------------------------------------------------------
// RunPartyManager //by larosel
//
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunPartyManager()
{
	if (m_pC_party_manager == NULL)
	{
		m_pC_party_manager = new C_VS_UI_PARTY_MANAGER();

		assert(m_pC_party_manager != NULL);

//		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

	}
	m_pC_party_manager->Start();
}


//-----------------------------------------------------------------------------
// ClosePartyManager
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ClosePartyManager()
{
	if (!m_pC_party_manager)
		return;


	m_pC_party_manager->Finish();
//	DeleteNew(m_pC_skill_view);

	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->HotKey_SkillInfo();
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningPartyManager
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningPartyManager() const
{
	if(m_pC_party_manager == NULL)return false;
	return m_pC_party_manager->Running();
}


void	C_VS_UI_GAME::RunBringFeeWindow(UINT totalfee, UINT bringfee)
{
	if(m_pC_bring_fee != NULL)
		DeleteNew(m_pC_bring_fee);

	m_pC_bring_fee = new C_VS_UI_BRING_FEE(totalfee,bringfee);
	
	assert(m_pC_bring_fee != NULL);

	m_pC_bring_fee->Start();
}

void	C_VS_UI_GAME::CloseBringFeeWindow()
{
	if(m_pC_bring_fee == NULL)
		return;

	m_pC_bring_fee->Finish();
	DeleteNew(m_pC_bring_fee);
}

bool	C_VS_UI_GAME::IsRunningBringFeeWindow()
{
	if(m_pC_bring_fee == NULL)
		return false;

	return m_pC_bring_fee->Running();
}

void	C_VS_UI_GAME::SetTotalFee(UINT totalfee)
{
	if(m_pC_bring_fee == NULL)
		return;

	m_pC_bring_fee->SetTotalFee(totalfee);
}

UINT		C_VS_UI_GAME::GetWillBringFee()
{
	if(m_pC_bring_fee == NULL)
		return 0;

	return m_pC_bring_fee->GetBringFee();
}
void	C_VS_UI_GAME::SetBringFee(UINT	fee)
{
	if(m_pC_bring_fee == NULL)
		return;

	m_pC_bring_fee->SetBringFee(fee);
}

UINT		C_VS_UI_GAME::GetTotalFee()
{
	if(m_pC_bring_fee == NULL)
		return 0;

	return m_pC_bring_fee->GetTotalFee();
}

void	C_VS_UI_GAME::RunInputNameWindow(C_VS_UI_INPUT_NAME::INPUT_NAME_MODE_LIST mode)
{
	if(m_pC_input_name != NULL)
		DeleteNew(m_pC_input_name);

	m_pC_input_name = new C_VS_UI_INPUT_NAME(mode);
	
	assert(m_pC_input_name != NULL);
	
	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_input_name->Start();

}

void	C_VS_UI_GAME::CloseInputNameWindow()
{
	if(m_pC_input_name == NULL)
		return;

	m_pC_input_name->Finish();
	DeleteNew(m_pC_input_name);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();		

	gC_vs_ui.AcquireChatting();
}

bool	C_VS_UI_GAME::IsRunningInputNameWindow()
{
	if(m_pC_input_name == NULL)
		return false;

	return m_pC_input_name->Running();
}

const char * C_VS_UI_GAME::GetCurrentInputName()
{
	if(m_pC_input_name == NULL)
		return NULL;

	return m_pC_input_name->GetCurrentName();
}

void	C_VS_UI_GAME::SetInputNameWindow(const char* szName)
{
	if(m_pC_input_name != NULL)
		m_pC_input_name->SetName(szName);
}

void	C_VS_UI_GAME::SetEnableInputName()
{
	if(m_pC_input_name != NULL)
		m_pC_input_name->SetEnable();
}

void	C_VS_UI_GAME::SetDisableInputName()
{
	if(m_pC_input_name != NULL)
		m_pC_input_name->SetDisable();
}

void	C_VS_UI_GAME::SetQuestStatus(C_VS_UI_QUEST_STATUS::QUEST_STATUS &status)
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->SetQuestStatusWindow( status );
}

void	C_VS_UI_GAME::SetQuestStatusInit()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->UnSetQuestStatusWindow();
}

DWORD	C_VS_UI_GAME::GetQuestStatusID()
{
	if(m_pC_tribe_interface)
		return m_pC_tribe_interface->GetQuestStatusID();
	return 0xffffffff;
}


void	C_VS_UI_GAME::RunQuestStatusWindow()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->OpenQuestStatusWindow();
}

void	C_VS_UI_GAME::CloseQuestStatusWindow()
{
	if(m_pC_tribe_interface)
		m_pC_tribe_interface->CloseQuestStatusWindow();
}

bool	C_VS_UI_GAME::IsRunningQuestStatusWindow()
{
	if( m_pC_tribe_interface )
		return m_pC_tribe_interface->IsRunningQuestStatusWindow();

	return false;
}

void	C_VS_UI_GAME::RunItemListWindow()
{
	if(m_pC_item_list != NULL)
		CloseItemListWindow();

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_item_list = new C_VS_UI_ITEM_LIST;
	m_pC_item_list->Start();
}

bool	C_VS_UI_GAME::IsRunningItemListWindow()
{
	if(m_pC_item_list != NULL)
		return m_pC_item_list->Running();

	return false;
}

void	C_VS_UI_GAME::CloseItemListWindow()
{
	if(m_pC_item_list != NULL)
	{
		m_pC_item_list->Finish();
		delete  m_pC_item_list;

		m_pC_item_list = NULL;

		m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	}
}

bool	C_VS_UI_GAME::AddItemToItemList(C_VS_UI_ITEM_LIST::ItemList pItem)
{
	if(m_pC_item_list != NULL)
		return m_pC_item_list->AddItem( pItem );
	return false;
}

bool	C_VS_UI_GAME::RemoveItemInItemList( DWORD ID )
{
	if(m_pC_item_list != NULL)
		return m_pC_item_list->DeleteItem( ID );

	return false;
}

void	C_VS_UI_GAME::RunImageNotice()
{
	if( m_pC_image_notice != NULL)
		CloseImageNotice();

	m_pC_image_notice = new C_VS_UI_IMAGE_NOTICE();
	m_pC_image_notice->Start();
}

void	C_VS_UI_GAME::CloseImageNotice()
{
	if(m_pC_image_notice == NULL)
		return;
	m_pC_image_notice->Finish();
	delete m_pC_image_notice;
	m_pC_image_notice = NULL;
}

bool	C_VS_UI_GAME::IsRunningImageNotice()
{
	if(m_pC_image_notice != NULL)
		return m_pC_image_notice->Running();
	return false;
}

void	C_VS_UI_GAME::AddNotice(const char *name, DWORD& id)
{
	if( m_pC_image_notice == NULL)
		RunImageNotice();

	m_pC_image_notice->AddNotice(name, id);
}

void	C_VS_UI_GAME::RunBulletinBoardWindow(const MItem *pItem)
{
	if(m_pC_bulletin_board == NULL )
	{
		m_pC_bulletin_board = new C_VS_UI_BULLETIN_BOARD(pItem);

		assert(m_pC_bulletin_board != NULL);		
		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();		
		m_pC_bulletin_board->Start();
	}	
}

void	C_VS_UI_GAME::CloseBulletinBoardWindow()
{
	gC_vs_ui.AcquireChatting();

	if(m_pC_bulletin_board == NULL)
		return;
	
	DeleteNew(m_pC_bulletin_board);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();	
}

bool	C_VS_UI_GAME::IsRunningBulletinBoardWindow() const
{
	if(m_pC_bulletin_board!=NULL)
		return m_pC_bulletin_board->Running();

	return false;
}

void	C_VS_UI_GAME::RunTransItem()
{
	if (m_pC_trans_item_dialog != NULL)
	{
		DeleteNew(m_pC_trans_item_dialog);
	}

	// center
	m_pC_trans_item_dialog = new C_VS_UI_ASK_DIALOG(-1, 50, 3, 0, ExecF_TransItem, DIALOG_CANCEL | DIALOG_OK, 
												C_VS_UI_ASK_DIALOG::ASK_TRANS_ITEM, 0);	// by sigi

	assert(m_pC_trans_item_dialog != NULL);

	gbl_trans_item_running = true;

	m_pC_trans_item_dialog->Start();	
}

void	C_VS_UI_GAME::CloseTransItem()
{
	if (!m_pC_trans_item_dialog)
		return;

	DeleteNew(m_pC_trans_item_dialog);
	
	gbl_trans_item_running = false;

	gC_vs_ui.AcquireChatting();
}

//-----------------------------------------------------------------------------
// RequestResurrect
//
// ����-_-��û? ������ ���
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RequestResurrect(bool f1, bool f2, bool eternity, bool IsSiegeAttacker, bool IsSkillFromOther)
{
	if(m_pC_request_resurrect)
	{
		DeleteNew(m_pC_request_resurrect);
		m_pC_request_resurrect = NULL;
	}

	m_pC_request_resurrect = new C_VS_UI_REQUEST_RESURRECT(f1,f2,eternity,IsSiegeAttacker,IsSkillFromOther);
	m_pC_request_resurrect->Start();

}

void C_VS_UI_GAME::RequestResurrectSetButton(int resurrectMode, bool enable)
{
	if(IsRunningRequestResurrect())
	{
		m_pC_request_resurrect->RequestResurrectSetButton(resurrectMode, enable);
	}
	else
	{
		bool enableArray[5] = { false, };

		enableArray[resurrectMode] = enable;

		RequestResurrect(
			enableArray[0], enableArray[1], enableArray[2],
			enableArray[3], enableArray[4]);
	}
}

void C_VS_UI_GAME::SetDelayRequestResurrect( C_VS_UI_REQUEST_RESURRECT::RESURRECT_MODE &mode, int &delay )
{
	if( m_pC_request_resurrect != NULL )
	{
		m_pC_request_resurrect->SetDelay( mode, delay );
	}
}

//-----------------------------------------------------------------------------
// FinishRequestResurrect
//
// ��Ƽ��û ������ ����
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::FinishRequestResurrect()
{
	if(m_pC_request_resurrect)
	{
		DeleteNew(m_pC_request_resurrect);
		m_pC_request_resurrect = NULL;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningRequestResurrect
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningRequestResurrect() const
{
	if(m_pC_request_resurrect == NULL)return false;
	return m_pC_request_resurrect->Running();
}


//-----------------------------------------------------------------------------
// ChangeToOustersInterface
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ChangeToOustersInterface()
{
	CT_Done("DIAG: ChangeToOustersInterface enter");
	CloseDescDialog();

	CloseInfo();
	DeleteNew(m_pC_info);
	DeleteNew(m_pC_other_info);

	//CloseMilitary();
//	DeleteNew(m_pc_military);

//	CloseSkillView();
//	DeleteNew(m_pC_skill_view);

	g_descriptor_manager.Unset();
	CT_Done("DIAG: ChangeToOustersInterface post Close/DeleteNew");

//	gbl_vampire_interface = true;
	g_eRaceInterface = RACE_OUSTERS; // ousters interface eklendi

	DeleteNew(m_pC_tribe_interface);

	// change skin data
	gpC_global_resource->FreeAssemble();
	gpC_global_resource->LoadAssemble();
	CT_Done("DIAG: ChangeToOustersInterface post FreeAssemble/LoadAssemble");

	CT_Done("DIAG: ChangeToOustersInterface pre new C_VS_UI_OUSTERS");
	m_pC_tribe_interface = new C_VS_UI_OUSTERS;
	CT_Done("DIAG: ChangeToOustersInterface post new C_VS_UI_OUSTERS=%p", m_pC_tribe_interface);

	char sz_filename[512];
	char sz_filename2[512];
	char sz_filename3[512];
	wsprintf(sz_filename, "UserSet\\%s.set", g_char_slot_ingame.sz_name.c_str());
	wsprintf(sz_filename2, "UserSet\\%s-%d.set", g_char_slot_ingame.sz_name.c_str(),g_pUserInformation->WorldID);
	wsprintf(sz_filename3, "UserSet\\%s-%d-%d.set", g_char_slot_ingame.sz_name.c_str(),g_Dimension,g_pUserInformation->WorldID);
	
	ivfstream file(sz_filename3, std::ios::binary);

	if(file.is_open())
	{
		gpC_vs_ui_window_manager->LoadFromFile(file);
		file.close();
	}
	else
	{
		gpC_vs_ui_window_manager->InitDefaultQuickSlot() ;
		
		file.open(sz_filename2, std::ios::binary);
		if(file.is_open())
		{
			gpC_vs_ui_window_manager->LoadFromFile(file);
			file.close();
		}
		else
		{
			file.open(sz_filename, std::ios::binary);
			if(file.is_open())
			{
				gpC_vs_ui_window_manager->LoadFromFile(file);
				file.close();
			}
		}
	}
	CT_Done("DIAG: ChangeToOustersInterface post UserSet load");

	m_pC_hotkey = m_pC_tribe_interface;
	CT_Done("DIAG: ChangeToOustersInterface pre TribeChanged");
	m_pC_chatting->TribeChanged();
	CT_Done("DIAG: ChangeToOustersInterface pre SetChattingInterface");
	m_pC_tribe_interface->SetChattingInterface(m_pC_chatting);

	CT_Done("DIAG: ChangeToOustersInterface pre tribe_interface->Start");
	m_pC_tribe_interface->Start();
	CT_Done("DIAG: ChangeToOustersInterface pre new C_VS_UI_INFO");
	m_pC_info = new C_VS_UI_INFO;
	//m_pc_military = new C_VS_UI_MILITARYDLG;
	CT_Done("DIAG: ChangeToOustersInterface exit, info=%p", m_pC_info);
}


//-----------------------------------------------------------------------------
// RunMixingForge
//
// MixingForge�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunMixingForge(C_VS_UI_MIXING_FORGE::FORGE_CLASS forge_class, C_VS_UI_MIXING_FORGE::FORGE_TYPE forge_type)
{
	if (m_pC_mixing_forge == NULL)
	{
		if(m_pC_tribe_interface != NULL)
		{
			m_pC_tribe_interface->CloseInventory();
			m_pC_tribe_interface->CloseGear();
		}
		
		m_pC_mixing_forge = new C_VS_UI_MIXING_FORGE( forge_class, forge_type );	
		m_pC_mixing_forge->Start();
	}
}


//-----------------------------------------------------------------------------
// CloseMixingForge
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseMixingForge()
{
	if (!m_pC_mixing_forge)
		return;

	DeleteNew(m_pC_mixing_forge);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningMixingForge
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningMixingForge() const
{
	if(m_pC_mixing_forge)
		return m_pC_mixing_forge->Running();

	return false;
}

void	C_VS_UI_GAME::RunRemoveOptionFromRareItem(const MItem* pItem, const MItem* pItem2)
{
	if( m_pC_remove_option )
		return;

	m_pC_remove_option = new C_VS_UI_REMOVE_OPTION( pItem, pItem2 );

	m_pC_remove_option->Start();
}

void	C_VS_UI_GAME::CloseRemoveOptionFromRareItem()
{
	if( m_pC_remove_option )
	{
		m_pC_remove_option->Finish();
		DeleteNew( m_pC_remove_option );
	}
}

bool	C_VS_UI_GAME::IsRunningRemoveOptionFromRareItem() const
{
	if( m_pC_remove_option )
		return m_pC_remove_option->Running();

	return false;
}

/////////////////////////////////////////////////////////////////////////
//
// Ousters Skill Info
//
/////////////////////////////////////////////////////////////////////////
void	C_VS_UI_GAME::RunOustersSkillInfo(int skillID, int window_x, int window_y, bool flag)
{
	if( m_pC_ousters_skill_info )
	{
		m_pC_ousters_skill_info->SetSkillID(skillID);
		return;
	}
	
	m_pC_ousters_skill_info = new C_VS_UI_OUSTERS_SKILL_INFO(skillID, window_x, window_y, flag);
	
	m_pC_ousters_skill_info->Start();
}

void	C_VS_UI_GAME::CloseOustersSkillInfo()
{
	if( m_pC_ousters_skill_info )
	{
		m_pC_ousters_skill_info->Finish();
		DeleteNew( m_pC_ousters_skill_info );
	}
}

bool	C_VS_UI_GAME::IsRunningOustersSkillInfo() const
{
	if( m_pC_ousters_skill_info )
		return m_pC_ousters_skill_info->Running();
	
	return false;
}

void	C_VS_UI_GAME::SetOustersDownSkill()
{
	//if( m_pC_info != NULL )
//		m_pC_info->SetOustersDownSkill();
	//if(m_pc_military != NULL)
	//	m_pc_military->SetOustersDownSkill();
	assert(m_pC_tribe_interface != NULL);
		m_pC_tribe_interface->DownSkillOuster();
}

void	C_VS_UI_GAME::RunArrowTile()
{
	if ( m_pC_arrow_tile != NULL )
		CloseArrowTile();
	
	m_pC_arrow_tile = new C_VS_UI_ARROW_TILE;

	assert(m_pC_tribe_interface);
	assert(m_pC_arrow_tile);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_arrow_tile->Start();
}

void	C_VS_UI_GAME::CloseArrowTile()
{
	if( m_pC_arrow_tile == NULL )
		return;

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_arrow_tile->Finish();
	DeleteNew(m_pC_arrow_tile);
}

bool	C_VS_UI_GAME::IsRunningArrowTile()	const
{
	if( m_pC_arrow_tile )
		return m_pC_arrow_tile->Running();

	return false;
}

void	C_VS_UI_GAME::RunCrazyMine()
{
	if ( m_pC_crazy_mine != NULL )
		CloseCrazyMine();
	
	m_pC_crazy_mine = new C_VS_UI_CRAZY_MINE;

	assert(m_pC_tribe_interface);
	assert(m_pC_crazy_mine);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_crazy_mine->Start();
}

void	C_VS_UI_GAME::CloseCrazyMine()
{
	if( m_pC_crazy_mine == NULL )
		return;

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_crazy_mine->Finish();
	DeleteNew(m_pC_crazy_mine);
}

bool	C_VS_UI_GAME::IsRunningCrazyMine()	const
{
	if( m_pC_crazy_mine )
		return m_pC_crazy_mine->Running();

	return false;
}

void	C_VS_UI_GAME::SetMiniGameScore(BYTE& Type, std::string& topname, WORD& topscore, WORD& mybestscore)
{
	switch(Type)
	{
	case 0 :			// mine
		if( m_pC_crazy_mine )
			m_pC_crazy_mine->SetScore( topname, topscore, mybestscore );
		break;
	case 3 :			// arrow
		if( m_pC_arrow_tile )
			m_pC_arrow_tile->SetScore( topname, topscore, mybestscore );
		break;
	}
}


void	C_VS_UI_GAME::RunQuestInventory(const MItem* pItem)
{
	if( m_pC_quest_inventory )
		CloseQuestInventory();

//	if(m_pC_tribe_interface != NULL)
//	{
//		m_pC_tribe_interface->CloseInventory();
//		m_pC_tribe_interface->CloseGear();
//	}

	m_pC_quest_inventory = new C_VS_UI_QUEST_INVENTORY(pItem);
	m_pC_quest_inventory->Start();
}

bool	C_VS_UI_GAME::IsRunningQuestInventory() const
{
	if( m_pC_quest_inventory )
		return m_pC_quest_inventory->Running();

	return false;
}

void	C_VS_UI_GAME::CloseQuestInventory()
{
	if( m_pC_quest_inventory == NULL)
		return;

	m_pC_quest_inventory->Finish();
	delete m_pC_quest_inventory;
	m_pC_quest_inventory = NULL;
}

void	C_VS_UI_GAME::RunCTFStatusWindow()
{
	if( m_pC_status_ctf != NULL )
		CloseCTFStatusWindow();

	m_pC_status_ctf = new C_VS_UI_STATUS_CTF;
	m_pC_status_ctf->Start();
}

void	C_VS_UI_GAME::CloseCTFStatusWindow()
{
	if( m_pC_status_ctf != NULL )
	{
		m_pC_status_ctf->Finish();
		DeleteNew( m_pC_status_ctf );
	}
}

bool	C_VS_UI_GAME::IsRunningCTFStatusWindow()
{
	if( m_pC_status_ctf != NULL )
		return m_pC_status_ctf->Running();

	return false;
}

void	C_VS_UI_GAME::RunPetInfo(PETINFO *petInfo)
{
	if(IsRunningPetInfo())
		ClosePetInfo();

	m_pC_pet_info = new C_VS_UI_PET_INFO(petInfo);
	if( m_pC_pet_info != NULL )
		m_pC_pet_info->Start();
}

void	C_VS_UI_GAME::ClosePetInfo()
{
	if( m_pC_pet_info != NULL )
	{
		m_pC_pet_info->Finish();
		DeleteNew( m_pC_pet_info );
	}
}

bool	C_VS_UI_GAME::IsRunningPetInfo()
{
	if( m_pC_pet_info != NULL )
		return m_pC_pet_info->Running();

	return false;
}

void C_VS_UI_GAME::RunHelpDesc(const char *detail,const char *title,DWORD id)
{
	if(m_pC_help_desc != NULL)
	{
		CloseHelpDesc();
	}
	m_pC_help_desc = new C_VS_UI_HELPDESC(detail,title, id);
	m_pC_help_desc->Start();
}

void	C_VS_UI_GAME::CloseHelpDesc()
{
	if(m_pC_help_desc != NULL)
	{
		m_pC_help_desc->Finish();
		DeleteNew(m_pC_help_desc);
	}
}

bool	C_VS_UI_GAME::IsRunningHelpDesc()
{
	if( m_pC_help_desc != NULL )
		return m_pC_help_desc->Running();
	
	return false;
}


void C_VS_UI_GAME::RunWorldMap()
{
	WORD	wZoneID = g_pZone->GetID();
	if(wZoneID	!= 1801		&&		wZoneID	!= 1802		&&
		wZoneID	!= 1803		&&		wZoneID	!= 1804)
	{
		if(!m_pC_world_map)
		{
			m_pC_world_map = new C_VS_UI_WORLD_MAP();
		}

		if(IsRunningWorldMap())
			CloseWorldMap();

#if __CONTENTS(__QUEST_RENEWAL)
		m_pC_world_map->GpsToBringQuestInfo();
#endif
		m_pC_world_map->Start();
		PlaySound(C_VS_UI_WORLD_MAP::WINDOW_APPEAR_SOUND);
	}
}


#if __CONTENTS(__TIPOJYU_CASTLE)	//RunIndounNpcDlg
void C_VS_UI_GAME::RunIndounNpcDlg(S_INDOUN_DLG_INFO Indoun_Npc_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect)
{

	if(g_pPlayer == NULL)
		return;
	BYTE btActionType	= g_pPlayer->GetAction();
	BYTE btWingType		= g_pPlayer->GetWingType();	//�����̾� �̵�����	1 or 2�̸� Ÿ�� �ִٰ� ��. 1�� ���� 2�� �ű� �̵�����

	BOOL	blIndounRun	= FALSE;
	switch(g_pPlayer->GetRace())	//�̵� ������ ����ϰ� �ִ� �߿��� Ƽ���� NPC â�� �� �� ����.
	{
		case RACE_SLAYER:
			if(	btActionType	!= ACTION_SLAYER_MOTOR_STAND		&&	btActionType	!= ACTION_SLAYER_MOTOR_MOVE )
				blIndounRun	= TRUE;
			break;

		case RACE_VAMPIRE:
			if(btWingType		== 0 )
				blIndounRun	= TRUE;
			break;

		case RACE_OUSTERS:
			if( btActionType	!= ACTION_OUSTERS_FAST_MOVE_STAND	&&	btActionType	!= ACTION_OUSTERS_FAST_MOVE_MOVE	&&
				btActionType	!= ACTION_OUSTERS_UNICORN_STAND		&&	btActionType	!= ACTION_OUSTERS_UNICORN_MOVE		&&
				btActionType	!= ACTION_OUSTERS_WING_STAND		&&	btActionType	!= ACTION_OUSTERS_WING_MOVE	)
				blIndounRun	= TRUE;
			break;
	}
	if(blIndounRun)
	{
		if(!m_pC_Indoung_Dialog)
			m_pC_Indoung_Dialog = new C_VS_UI_INDOUN_DIALOG(Indoun_Npc_Dlg_Info, etIndounSelect);
		else
			m_pC_Indoung_Dialog->SetIndounNpcDlgInfo(Indoun_Npc_Dlg_Info, etIndounSelect);
		m_pC_Indoung_Dialog->Start();
	}

}
#endif // __TIPOJYU_CASTLE;


void C_VS_UI_GAME::CloseWorldMap()
{
	if(m_pC_world_map)
	{
		m_pC_world_map->Finish();	
	}
}


#if __CONTENTS(__TIPOJYU_CASTLE)	//CloseIndounNpcDlg
void C_VS_UI_GAME::CloseIndounNpcDlg()
{
	if(m_pC_Indoung_Dialog)
	{
		m_pC_Indoung_Dialog->Finish();
	}
}
#endif // __TIPOJYU_CASTLE;


void C_VS_UI_GAME::ToggleWorldMap()
{
	if(IsRunningWorldMap())
		CloseWorldMap();
	else
		RunWorldMap();	
}


bool C_VS_UI_GAME::IsRunningWorldMap()
{
	if(m_pC_world_map)
		return m_pC_world_map->Running();
	return false;
}


void	C_VS_UI_GAME::SetCTFStatus(DWORD &endtime, int &flag_s, int &flag_v, int &flag_o)
{
	if( m_pC_status_ctf == NULL )
		RunCTFStatusWindow();

	if( m_pC_status_ctf )
		m_pC_status_ctf->SetStatus( endtime, flag_s, flag_v, flag_o );
}

void	C_VS_UI_GAME::RequestShrineMinimap(DWORD timer)
{
	if( m_pC_regen_tower_minimap != NULL )
		FinishRequestShrineMinimap();

	m_pC_regen_tower_minimap = new C_VS_UI_REGEN_TOWER_MINIMAP( timer );
	m_pC_regen_tower_minimap->Start();
}

void	C_VS_UI_GAME::FinishRequestShrineMinimap()
{
	if( m_pC_regen_tower_minimap == NULL )
		return;

	m_pC_regen_tower_minimap->Finish();
	DeleteNew( m_pC_regen_tower_minimap );
}

bool	C_VS_UI_GAME::IsRunningRequestShrineMinimap() const
{
	if( m_pC_regen_tower_minimap == NULL )
		return false;

	return m_pC_regen_tower_minimap->Running();
}

//-------------------------------------------------------------------------------------------------
// C_VS_UI_GAMEMENU
//-------------------------------------------------------------------------------------------------


/*-----------------------------------------------------------------------------
- C_VS_UI_GAMEMENU
-
-----------------------------------------------------------------------------*/
C_VS_UI_GAMEMENU::C_VS_UI_GAMEMENU()
{
	g_RegisterWindow(this);

	AttrTopmost(true);
	//AttrKeyboardControl(true);
	m_pC_gamemenu_spk = NULL;
	m_pC_gamemenu_spk = new C_SPRITE_PACK(SPK_SLAYER_GAMEMENU);
	Set(g_pUserInformation->iResolution_x/2-m_pC_gamemenu_spk->GetWidth()/2, g_pUserInformation->iResolution_y/2-m_pC_gamemenu_spk->GetHeight()/2, m_pC_gamemenu_spk->GetWidth(), m_pC_gamemenu_spk->GetHeight());

	m_pC_button_group = new ButtonGroup(this);

	int bx = 0;
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+bx,60,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_GEAR,this,BUTTON_1));//װ��
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68+bx,60,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_SKILL,this,BUTTON_1));   //����
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*2+bx,60,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_QUEST,this,BUTTON_1)); //����
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*3+bx,60,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_PET,this,BUTTON_1));	 //����


	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+bx,60+63,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_GUILD,this,BUTTON_1));//����
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68+bx,60+63,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_GUILD_LIST,this,BUTTON_1));   //����Ŀ¼
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*2+bx,60+63,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_WATING_GUILD,this,BUTTON_1));   //	�ȴ�����
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*3+bx,60+63,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_GUILD_MEMBER,this,BUTTON_1));   //�����Ա

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+bx,60+63*2,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_FRIEND,this,BUTTON_1));   //����
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68+bx,60+63*2,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_PARTY,this,BUTTON_1));	//���

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+bx,60+63*3,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_TMALL,this,BUTTON_1));// ̳?	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68+bx,60+63*3,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_SHOP,this,BUTTON_1));//��̯
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68+bx,60+63*3,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_SHOP,this,BUTTON_1));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*2+bx,60+63*3,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_GOODLIST,this,BUTTON_1));//���ﳵ

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+bx,60+63*4,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_HELP,this,BUTTON_1));//����

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*2+bx,50+63*5,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_OPTION,this,BUTTON_1));//ѡ��
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*3+bx,50+63*5,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_LOGOUT,this,BUTTON_1));//ע��
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*4+bx,50+63*5,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_CLOSEGAME,this,BUTTON_1));//�˳���Ϸ
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(75+68*5+bx,50+63*5,m_pC_gamemenu_spk->GetWidth(BUTTON_1),m_pC_gamemenu_spk->GetHeight(BUTTON_1),Button_CLOSE,this,BUTTON_1));//�ر�

}

/*-----------------------------------------------------------------------------
- ~C_VS_UI_GAMEMENU
-
-----------------------------------------------------------------------------*/
C_VS_UI_GAMEMENU::~C_VS_UI_GAMEMENU()
{
//	if(m_bHangul)
//	{
//		gC_ci.SetEngInput(true);
//	}
	g_UnregisterWindow(this);

	DeleteNew(m_pC_button_group);
	DeleteNew(m_pC_gamemenu_spk);
}

void	C_VS_UI_GAMEMENU::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
{
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAMEMENU::ShowButtonWidget
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_GAMEMENU::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	//if (p_button->GetFocusState())// && p_button->GetPressState())
	//	m_pC_gamemenu_spk->BltLocked(x+p_button->x, y+p_button->y, p_button->m_image_index);
	if(p_button->GetFocusState())
	{			
		if(p_button->GetPressState())
			m_pC_gamemenu_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index+2);
		else
			m_pC_gamemenu_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index+1);
	}
	else
		m_pC_gamemenu_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index);
}

//-----------------------------------------------------------------------------
// AcquireDisappear
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAMEMENU::AcquireDisappear()
{
}

//-----------------------------------------------------------------------------
// IsPixel
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_GAMEMENU::IsPixel(int _x, int _y)
{
	return m_pC_gamemenu_spk->IsPixel(SCR2WIN_X(_x), SCR2WIN_Y(_y));
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAMEMENU::WindowEventReceiver(id_t event)
{

}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAMEMENU::Process()
{
	m_pC_button_group->Process();
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAMEMENU::Start()
{
	PI_Processor::Start();
	m_pC_button_group->Init();

	gpC_window_manager->AppearWindow(this);
}

void C_VS_UI_GAMEMENU::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);
}

//-----------------------------------------------------------------------------
// Run
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_GAMEMENU::Run(id_t id)
{
	// 2026-05-20 REVERT: Finish() RESTORE (Yol A — stabil 2026-05-11 davranis).
	// Bu seansta Finish() removal (Yol B — Phoenix modern) yan etki yarattigi
	// icin geri alindi: kullanici testi gosterdi ki QUEST window GameMenu'nun
	// arkasinda kaliyordu (z-order yanlis), GUILD/GOODLIST window onunde ama
	// GameMenu kapanmiyordu. Eski 2026-05-11 davranis: yeni window acilan
	// butonlarda Finish() ile menu kapansin, yeni window onde temiz.
	// LOGOUT/CLOSE Finish'siz korundu (confirm dialog veya explicit close).
	// CLOSEGAME Finish'li (explicit quit).
	//
	// Tarihce:
	//   2026-05-11 (Bug 5): tum butonlara Finish() eklendi — "GameMenu arkada kaliyor" sorununa
	//   2026-05-15: SKILL HotKey_SkillInfo bypass + C_VS_UI_INFO::Start early-return
	//   2026-05-20 try B: Finish removal — broken handlers exposed (GEAR/PARTY/PET/TMALL/SHOP/FRIEND/GUILD_INFO/GUILD_MEMBER 9 buton)
	//   2026-05-20 revert A: Finish restore — broken butonlar gizli ama UX temiz
	//
	// AYRI IS (ileri seans): 9 broken buton C_VS_UI_GAME::HotKey_X body audit
	// (HELP HotKey_Help calisir, GEAR HotKey_Gear calismaz — context check farki).
	switch (id)
	{
	case Button_GEAR:
		gpC_base->SendMessage(UI_NEW_OPTION,1);
		Finish();
		break;
	case Button_SKILL:
		// 2026-05-23: UI_NEW_OPTION,2 referans Skill/Rune yolu olan
		// ToggleMilitary() dispatch'ine gider. Direkt HotKey_SkillInfo()
		// kullanilmaz; VS_UI_SKILL_VIEW overlap uretiyor.
		gpC_base->SendMessage(UI_NEW_OPTION,2);
		Finish();
		break;
	case Button_QUEST:
		gpC_base->SendMessage(UI_NEW_OPTION,3);
		Finish();
		break;
	case Button_PET:
		gpC_base->SendMessage(UI_NEW_OPTION,4);
		Finish();
		break;
	case Button_GUILD:
		// 2026-05-15: GuildGrade=4 (guildless) durumunda eski kod Finish() cagirmiyordu,
		// GameMenu acik kaliyordu. Her durumda Finish() — guild yoksa sessizce kapanir.
		if(g_pUserInformation->GuildGrade != 4)
		{
			gpC_base->SendMessage(UI_REQUEST_GUILD_INFO, 0, 0, NULL);
		}
		Finish();
		break;
	case Button_GUILD_LIST:
		gpC_base->SendMessage(UI_REQUEST_UNION_REQUEST_GUILD_LIST, 1,0,NULL);
		Finish();
		break;
	case Button_WATING_GUILD:
		gpC_base->SendMessage(UI_REQUEST_UNION_REQUEST_GUILD_LIST, 0,0,NULL);
		Finish();
		break;
	case Button_GUILD_MEMBER:
		// 2026-05-15: Ayni ghost menu pattern (Button_GUILD ile).
		if(g_pUserInformation->GuildGrade != 4)
		{
			gC_vs_ui.RunTeamMemberList();
			gpC_base->SendMessage(UI_REQUEST_GUILD_MEMBER_LIST);
		}
		Finish();
		break;
	case Button_FRIEND:
		 gpC_base->SendMessage(UI_NEW_OPTION,5);
		 Finish();
		break;
	case Button_PARTY:
		gpC_base->SendMessage(UI_NEW_OPTION,6);
		Finish();
		break;
	case Button_TMALL:
		gpC_base->SendMessage(UI_NEW_OPTION,7);
		Finish();
		break;
	case Button_SHOP:
		gpC_base->SendMessage(UI_REQUEST_STORE_INFO,0,1,NULL);
		Finish();
		break;
	case Button_GOODLIST:
		gpC_base->SendMessage(UI_MARKET_ACCOUNT ,  1, 0) ;
		Finish();
		break;
	case Button_HELP:
		gpC_base->SendMessage(UI_NEW_OPTION,9);
		Finish();
		break;
	case Button_OPTION:
		 // 2026-05-15: Login (Title) ile ayni Option dialog — IsTitle=true.
		 // Onceki: SendMessage(UI_NEW_OPTION,8) -> Execute_UI_NEW_OPTION -> RunOption() default(false)
		 // Bu yol race-specific SPK_OPTION_SLAYER/VAMPIRE/OUSTERS layout aciyordu.
		 // Yeni: RunOption(true) Login Title'daki SPK_TITLE_OPTION + OTHER tab + tek-sprite layout'u acar.
		 gC_vs_ui.RunOption(true);
		 Finish();
		 break;
	case Button_LOGOUT:
		 // Logout confirmation dialog acabilir, Finish() eklenmedi (2026-05-11 doğru karar).
		 gpC_base->SendMessage(UI_LOGOUT, 0, 0);
		 break;
	case Button_CLOSEGAME:
		 // Quit dialog. Finish() ile menu kapanmali (explicit close).
		 gpC_base->SendMessage(UI_QUITEXIT, 0, 0);
		 Finish();
		 break;
	case Button_CLOSE:
		 // Explicit close butonu — CloseGameMenu zaten kapatir.
		 gC_vs_ui.CloseGameMenu();
		 break;
	}
}

/*-----------------------------------------------------------------------------
- MouseControl
-
-----------------------------------------------------------------------------*/
bool C_VS_UI_GAMEMENU::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x; _y-=y;
	m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}

/*-----------------------------------------------------------------------------
- KeyboardControl
-
-----------------------------------------------------------------------------*/
void C_VS_UI_GAMEMENU::KeyboardControl(UINT message, UINT key, long extra)
{
	// ���� IME toggle�Ǿ� ���� ��츦 ����Ͽ� scan code�� �Ѵ�.
//
//	id_t id = INVALID_ID;
//	if (message == WM_KEYDOWN)
//	{
//		switch (SCAN_CODE(extra))
//		{
////			case SCANCODE_B: // To 'B'oard
////				id = TO_BOARD;
////				break;
//
//			case SCANCODE_L: // 'L'ogout
//				id = LOGOUT;
//				break;
//
//			case SCANCODE_O: // 'O'ption
//				id = OPTION;
//				break;
//
//	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
//			case SCANCODE_X:
//				id = QUITEXIT;
//				break;
//	#endif //__080405_FIREST_UI_UPDATE
//
////			case SCANCODE_ESC:
//			case SCANCODE_C: // 'C'ancel
//				id = CONTINUE;
//				break;
//
//				//gC_vs_ui.ToggleESC4UI();
////				break;
//
//			default:
//				id = INVALID_ID;
//		}
//	}
//
//	Run(id);
}

bool C_VS_UI_GAME::IsRunningOtherInfo()
{	
	if(m_pC_other_info!=NULL)
		return m_pC_other_info->Running();

	return false;
}

/*-----------------------------------------------------------------------------
- Show
-
-----------------------------------------------------------------------------*/
void C_VS_UI_GAMEMENU::Show()
{
	CT_Done("WnSh: C_VS_UI_GAMEMENU::Show ENTER pthis=%p", (void*)this);
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		m_pC_gamemenu_spk->BltLocked(x, y, MAIN);
		
		m_pC_button_group->Show();

		m_pC_gamemenu_spk->BltLocked(x+75+9, y+60+9, IMAGE_INFO);
		m_pC_gamemenu_spk->BltLocked(x+75+68+9-1, y+60+9+1, IMAGE_SKILL);
		m_pC_gamemenu_spk->BltLocked(x+75+68*2+9+1, y+60+9, IMAGE_QUEST);
		m_pC_gamemenu_spk->BltLocked(x+75+68*3+9+1, y+60+9+2, IMAGE_PET);

		m_pC_gamemenu_spk->BltLocked(x+75+9+1, y+60+9+2+63, IMAGE_GUILD);
		m_pC_gamemenu_spk->BltLocked(x+75+68+9+1, y+60+9+2+63, IMAGE_GUILD_LIST);
		m_pC_gamemenu_spk->BltLocked(x+75+68*2+9+1, y+60+9+2+63, IMAGE_WATING_GUILD);
		m_pC_gamemenu_spk->BltLocked(x+75+68*3+9+1, y+60+9+2+63, IMAGE_GUILD_MEMBER);

		m_pC_gamemenu_spk->BltLocked(x+75+9+1, y+60+9+2+63*2, IMAGE_FRIEND);
		m_pC_gamemenu_spk->BltLocked(x+75+68+4+1, y+60+9+2+63*2, IMAGE_PARTY);

		m_pC_gamemenu_spk->BltLocked(x+75+9+1, y+60+9+4+63*3, IMAGE_TMALL);
		m_pC_gamemenu_spk->BltLocked(x+75+68+9+1, y+60+9+4+63*3, IMAGE_SHOP);
		m_pC_gamemenu_spk->BltLocked(x+75+68*2+9+3, y+60+9+5+63*3, IMAGE_GOODLIST);

		m_pC_gamemenu_spk->BltLocked(x+75+9+11, y+60+9+6+63*4, IMAGE_HELP);

	   m_pC_gamemenu_spk->BltLocked(x+75+68*2+8,y+50+63*5+12, IMAGE_OPTION);
	   m_pC_gamemenu_spk->BltLocked(x+75+68*3+15,y+50+63*5+11, IMAGE_LOGOUT);
	   m_pC_gamemenu_spk->BltLocked(x+75+68*4+14,y+50+63*5+14, IMAGE_CLOSEGAME);
	   m_pC_gamemenu_spk->BltLocked(x+75+68*5+15,y+50+63*5+15, IMAGE_CLOSE);


		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	char sz_temp[100];

	wsprintf(sz_temp,"") ; //Game Menu
	g_PrintColorStr(x+12, y+15, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);

	wsprintf(sz_temp,"") ; //Information
	g_PrintColorStr(x+5, y+80, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Guild
	g_PrintColorStr(x+5, y+80+63, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);
	wsprintf(sz_temp," ") ; //Social
	g_PrintColorStr(x+5, y+80+63*2, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);
	wsprintf(sz_temp," ") ; //Shop
	g_PrintColorStr(x+5, y+80+63*3, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Assistance
	g_PrintColorStr(x+5, y+80+63*4, sz_temp, gpC_base->m_desc_menu_pi, RGB_WHITE);

	wsprintf(sz_temp,"") ; //Equipment Window
	int maxW = (53-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
	int maxH = 53-g_GetStringHeight(sz_temp,gpC_base->m_chatting_pi.hfont)-10;
	g_PrintColorStr(x+75+maxW, y+60+maxH, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Skills Window
	g_PrintColorStr(x+75+maxW+68, y+60+maxH, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Quest Window
	g_PrintColorStr(x+75+maxW+68*2, y+60+maxH, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Pet Window
	g_PrintColorStr(x+75+maxW+68*3, y+60+maxH, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);

	wsprintf(sz_temp,"") ; //Guild Info Window
	int maxW2 = (53-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
	g_PrintColorStr(x+75+maxW2, y+60+maxH+63, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Guild List Window
	g_PrintColorStr(x+75+maxW2+68, y+60+maxH+63, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Guild Waiting List Window
	g_PrintColorStr(x+75+maxW2+68*2, y+60+maxH+63, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Guild members Window
	g_PrintColorStr(x+75+maxW2+68*3, y+60+maxH+63, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);

	wsprintf(sz_temp,"") ; //Friends Window
	g_PrintColorStr(x+75+maxW, y+60+maxH+63*2, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Team Window
	g_PrintColorStr(x+75+maxW+68, y+60+maxH+63*2, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);

	wsprintf(sz_temp," ") ; //Ship Window
	g_PrintColorStr(x+75+maxW, y+60+maxH+63*3, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Open Shop Window
	g_PrintColorStr(x+75+maxW+68, y+60+maxH+63*3, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"") ; //Basket Window
	g_PrintColorStr(x+75+maxW+68*2-2, y+60+maxH+63*3, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);

	wsprintf(sz_temp,"") ; //Promotion Window
	g_PrintColorStr(x+75+maxW2, y+60+maxH+63*4, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);

	wsprintf(sz_temp,"Options") ; //Options Button(In-game)
	g_PrintColorStr(x+55+maxW+68*2, y+50+maxH+63*5, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"LogOut") ; //LogOut Button
	g_PrintColorStr(x+55+maxW+68*3, y+50+maxH+63*5, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"Exit Game") ; //Exit Game Button
	g_PrintColorStr(x+47+maxW+68*4, y+50+maxH+63*5, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);
	wsprintf(sz_temp,"Close Window") ; //Close Window Button
	g_PrintColorStr(x+40+maxW+68*5, y+50+maxH+63*5, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE);


	g_FL2_ReleaseDC();

	SHOW_WINDOW_ATTR;
}

PETINFO* 
C_VS_UI_GAME::GetPetInfo()
{
	if(m_pC_pet_info != NULL)return m_pC_pet_info->GetPetInfo();
	return NULL;
}

PETINFO* 
C_VS_UI_GAME::GetMyPetInfo()
{
	if(m_pC_info != NULL)return m_pC_info->GetPetInfo();
	return NULL;
}

int C_VS_UI_GAME::GetPetStorageEmptySlot()
{
	if(m_pC_petstorage)
		return m_pC_petstorage->GetEmptySlot();
	return -1;
}

int C_VS_UI_GAME::GetPetStorageFocusSlot()
{
	if(m_pC_petstorage)
		return m_pC_petstorage->GetFocusSlot();
	return -1;
}

// 2004, 5, 27 sobeit add start
//-----------------------------------------------------------------------------
// Run SMS Message
//
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunSMSMessage()
{
	if (m_pC_sms_message == NULL)
	{
		m_pC_sms_message = new C_VS_UI_SMS_MESSAGE();

		assert(m_pC_sms_message != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_sms_message->Start();

	}
}
bool	C_VS_UI_GAME::IsRunningSMSMessage() const
{
	return (NULL != m_pC_sms_message);
}

void	C_VS_UI_GAME::CloseSMSMessage() 
{
	if (NULL ==m_pC_sms_message)
		return;
	
	DeleteNew(m_pC_sms_message);
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}

void C_VS_UI_GAME::RunSMSList()
{
	if (m_pC_sms_list == NULL)
	{
		m_pC_sms_list = new C_VS_UI_SMS_LIST();

		assert(m_pC_sms_list != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_sms_list->Start();

	}
}
bool	C_VS_UI_GAME::IsRunningSMSList() const
{
	return (NULL != m_pC_sms_list);
}

void	C_VS_UI_GAME::CloseSMSList() 
{
	if (NULL ==m_pC_sms_list)
		return;
	
	DeleteNew(m_pC_sms_list);
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}

void C_VS_UI_GAME::RunSMSRecord()
{
	if (m_pC_sms_record == NULL)
	{
		m_pC_sms_record = new C_VS_UI_SMS_RECORD();

		assert(m_pC_sms_record!= NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_sms_record->Start();

	}
}
bool	C_VS_UI_GAME::IsRunningSMSRecord() const
{
	return (NULL != m_pC_sms_record);
}

void	C_VS_UI_GAME::CloseSMSRecord() 
{
	if (NULL ==m_pC_sms_record)
		return;
	
	DeleteNew(m_pC_sms_record);
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
void	C_VS_UI_GAME::SetSMSAddressList(void* pVoid) 
{
	if (NULL ==m_pC_sms_list)
		return;
	m_pC_sms_list->SetSMSList(pVoid);
}
void	C_VS_UI_GAME::AddSMSAddressList(DWORD element, char* name, char* id, char* num) 
{
	if (NULL ==m_pC_sms_list)
		return;
	m_pC_sms_list->AddList(element, name, id, num);

}
void	C_VS_UI_GAME::DeleteSMSAddressList(int id) 
{
	if (NULL ==m_pC_sms_list)
		return;
	m_pC_sms_list->DeleteList(id);

}
void	C_VS_UI_GAME::AddSMSSendList(char* str) 
{
	if (NULL ==m_pC_sms_message)
		return;
	m_pC_sms_message->AddSendList(str);
}

void	C_VS_UI_GAME::AcquireSMS()
{
	if (NULL ==m_pC_sms_message)
		return;
	m_pC_sms_message->AcquireSMS();

}

void	C_VS_UI_GAME::SetSMSCharage(int i)
{
	g_char_slot_ingame.m_SMS_Charge = i;
}
// 2004, 5, 27 sobeit add end


// 2004, 6, 14 sobeit add start- naming
void C_VS_UI_GAME::RunNaming()
{
	if (m_pC_naming == NULL)
	{
		m_pC_naming = new C_VS_UI_NAMING(m_NickNameList);

		assert(m_pC_naming!= NULL);

//		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_naming->Start();

	}
}
bool	C_VS_UI_GAME::IsRunningNaming() const
{
	return (NULL != m_pC_naming);
}

void	C_VS_UI_GAME::CloseNaming() 
{
	if (NULL ==m_pC_naming)
		return;
	
	DeleteNew(m_pC_naming);
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}


bool	C_VS_UI_GAME::IsRunAskUseItemDialog()
{
	if(gbl_use_askitem_running) return	true ; 
	return false; 
}


void	C_VS_UI_GAME::SetUseAskIitemRunning(bool bRunning)
{
	gbl_use_askitem_running = bRunning  ; 
}


//-----------------------------------------------------------------------------
// RunUseSMSItemDialog
//
// sms �������� ��� �ҷ�?? �ϰ� ������...
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunAskUseItemDialog(int AskType ,  int value, void* pValue)
{
	if (m_pC_use_askitem_dialog != NULL)
	{
		DeleteNew(m_pC_use_askitem_dialog);
	}
	
	// center
	if(AskType >= C_VS_UI_ASK_DIALOG::MAX_ASK_DIALOG_TYPE)
		return;
	m_pC_use_askitem_dialog = new C_VS_UI_ASK_DIALOG(-1, -1, 3, 0, ExecF_Use_AskItem, DIALOG_CANCEL | DIALOG_OK, 
		(C_VS_UI_ASK_DIALOG::TYPE_ASK_DIALOG)AskType, value, pValue) ; //0);	// by sigi
	
	assert(m_pC_use_askitem_dialog != NULL);
	
	gbl_use_askitem_running = true;
	
	m_pC_use_askitem_dialog->Start();	
}

//-----------------------------------------------------------------------------
// CloseKeepPetItemDialog
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::CloseAskUseItemDialog()
{
	if (!m_pC_use_askitem_dialog)
		return;
	
	DeleteNew(m_pC_use_askitem_dialog);
	
	
	gbl_use_askitem_running = false;
	gC_vs_ui.AcquireChatting();
}

// 2004, 6, 14 sobeit end
// 2004, 6, 15 sobeit add start
bool	C_VS_UI_GAME::IsRunningGearWindow() const
{
	if(m_pC_tribe_interface&&gpC_window_manager)
		return gpC_window_manager->GetShowState(m_pC_tribe_interface->GetGearWindow());
	return false;
}
void C_VS_UI_GAME::Change_Custom_Naming(int nID, char* szName)
{
	assert(m_pC_tribe_interface != NULL);
	if(m_NickNameList.size())
	{
		std::vector<C_VS_UI_NicknameInfo*>::iterator itr = m_NickNameList.begin();
		while(itr != m_NickNameList.end())
		{
			C_VS_UI_NicknameInfo * TempInfo = (*itr);
			if(TempInfo->getNicknameID() == nID)
			{
				TempInfo->setNickname(szName);
				break;
			}
			itr ++;
		}
	}
	if(m_pC_naming)
	{
		m_pC_naming->Change_Custom_Naming(nID, szName);
	}

}
// 2004, 6, 15 sobeit end

// 2004, 6, 16 sobeit add start
void C_VS_UI_GAME::SetNickNameList(void* pData)
{
	if(NULL == pData)
		return;
	if(m_NickNameList.size())
	{
		std::vector<C_VS_UI_NicknameInfo*>::iterator itr = m_NickNameList.begin();
		while(itr != m_NickNameList.end())
		{
			C_VS_UI_NicknameInfo * TempInfo2 = (*itr);
			m_NickNameList.erase(itr);
			DeleteNew(TempInfo2);
		}
		m_NickNameList.clear();
	}

	std::vector<C_VS_UI_NicknameInfo*>	*TempList = (std::vector<C_VS_UI_NicknameInfo*>*)pData;
	
	if(TempList->size())
	{
		WORD TempID = 0;
		WORD TempIndex = 0;
		BYTE TempType = 0;
		std::vector<C_VS_UI_NicknameInfo*>::iterator itr = TempList->begin();

		// ����Ʈ�� �� ÷�� �г��� ���ִ� �� �߰��Ѵ�. - start
		C_VS_UI_NicknameInfo * FirstInfo = new C_VS_UI_NicknameInfo;
		FirstInfo->setNicknameID(0xffff);
		FirstInfo->setNicknameType(C_VS_UI_NicknameInfo::NICK_CUSTOM);
		FirstInfo->setNickname((*g_pNickNameStringTable)[0].GetString());
		m_NickNameList.push_back(FirstInfo);
		// ����Ʈ�� �� ÷�� �г��� ���ִ� �� �߰��Ѵ�. -end

		while(itr != TempList->end())
		{
			C_VS_UI_NicknameInfo * TempInfo2 = (*itr);
			C_VS_UI_NicknameInfo * TempInfo = new C_VS_UI_NicknameInfo;
			
			TempID	   = TempInfo2->getNicknameID();
			TempIndex = TempInfo2->getNicknameIndex();
			TempType  = TempInfo2->getNicknameType();
			
			TempInfo->setNicknameID(TempID);
			TempInfo->setNicknameIndex(TempIndex);
			TempInfo->setNicknameType(TempType);
			
			// Ŀ���� �г��� �϶�
			if(TempType == C_VS_UI_NicknameInfo::NICK_CUSTOM_FORCED)
			{
				TempInfo->setNickname(TempInfo2->getNickname());
			}
			else if(TempType == C_VS_UI_NicknameInfo::NICK_CUSTOM)
			{
//				if(0 == TempInfo2->getNickname().size())
//					TempInfo->setNickname((*g_pGameStringTable)[UI_STRING_MESSAGE_AVABLE_CHANGE_NICK].GetString());
//				else
					TempInfo->setNickname(TempInfo2->getNickname());
			}
			else // �г��� �ε����� ���� ��
			{
				if(TempIndex >= g_pNickNameStringTable->GetSize())
					TempIndex = 0;
				TempInfo->setNickname((*g_pNickNameStringTable)[TempIndex].GetString());
			}
			m_NickNameList.push_back(TempInfo);
			itr ++;
		}
	}
	if (NULL!=m_pC_naming)
		m_pC_naming->SetNameList(m_NickNameList);//SetNameList(m_NickNameList);
}
void C_VS_UI_GAME::AddNickNameList(void* pData)
{
	if(NULL == pData)
		return;
	C_VS_UI_NicknameInfo *TempInfo2 = (C_VS_UI_NicknameInfo*)pData;
	C_VS_UI_NicknameInfo * TempInfo = new C_VS_UI_NicknameInfo;

	WORD TempID	   = TempInfo2->getNicknameID();
	WORD TempIndex = TempInfo2->getNicknameIndex();
	BYTE TempType  = TempInfo2->getNicknameType();
	
	TempInfo->setNicknameID(TempID);
	TempInfo->setNicknameIndex(TempIndex);
	TempInfo->setNicknameType(TempType);
	
	// Ŀ���� �г��� �϶�
	if(TempType == C_VS_UI_NicknameInfo::NICK_CUSTOM_FORCED ||
		TempType == C_VS_UI_NicknameInfo::NICK_CUSTOM)
	{
		TempInfo->setNickname(TempInfo2->getNickname());
	}
	else // �г��� �ε����� ���� ��
	{
		if(TempIndex >= g_pNickNameStringTable->GetSize())
			TempIndex = 0;
		TempInfo->setNickname((*g_pNickNameStringTable)[TempIndex].GetString());
	}

	m_NickNameList.push_back(TempInfo);
	if (NULL!=m_pC_naming)
		m_pC_naming->AddNameList(TempInfo);//SetNameList(m_NickNameList);
}
// 2004, 6, 16 sobeit add end

// 2004, 6, 21 sobiet add start
void	C_VS_UI_GAME::RunNamingChange(MItem* pItem , char* szCustom)
{
	if (m_pC_naming_change == NULL)
	{
		m_pC_naming_change = new C_VS_UI_NAMING_CHANGE(pItem, szCustom);

		assert(m_pC_naming_change!= NULL);

//		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();

		m_pC_naming_change->Start();

	}
}

bool	C_VS_UI_GAME::IsRunningNamingChange() const
{
	return (NULL != m_pC_naming_change);
}

void	C_VS_UI_GAME::CloseNamingChange() 
{
	if (NULL ==m_pC_naming_change)
		return;
	
	DeleteNew(m_pC_naming_change);
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
// 2004, 6, 21 sobeit add end
// 2004, 6, 30 sobiet add start
void	C_VS_UI_GAME::RunQuestManager()
{
	if (m_pC_quest_manager == NULL)
	{
		m_pC_quest_manager = new C_VS_UI_QUEST_MANAGER();

		assert(m_pC_quest_manager!= NULL);
	}
	else
	{
		m_pC_quest_manager->ToggleWindow();
	}
}

bool	C_VS_UI_GAME::IsRunningQuestManager() const
{
	return (NULL != m_pC_quest_manager);
}

void	C_VS_UI_GAME::CloseQuestManager() 
{
	if (NULL == m_pC_quest_manager)
		return;

	DeleteNew(m_pC_quest_manager);
	assert(m_pC_tribe_interface != NULL);
//	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}

bool	C_VS_UI_GAME::SetQuestManagerInfo(void* pVoid)
{
	if (NULL == m_pC_quest_manager)
		return false;

	return m_pC_quest_manager->SetQuestManagerInfo(pVoid);
}
bool	C_VS_UI_GAME::ModifyQuestManagerInfo(void* pVoid, int nType)
{
	if (NULL == m_pC_quest_manager)
		return false;

	return m_pC_quest_manager->ModifyQuestManagerInfo(pVoid, nType);
}
void	C_VS_UI_GAME::SetQuestManagerSubWindow(DWORD dwID)
{
	if (NULL == m_pC_quest_manager)
		return ;

	m_pC_quest_manager->SetSubWindowInfo(dwID);
}
void	C_VS_UI_GAME::SendQuestIntention(bool bIntention)
{
	if(NULL == m_pC_quest_manager)
		return;
	m_pC_quest_manager->SendQuestIntention(bIntention);
}
void C_VS_UI_GAME::ReleaseQuestXML()
{
	if(NULL == m_pC_quest_manager)
		return;
	m_pC_quest_manager->ReleaseQuestXML();
}
// 2004, 6, 30 sobeit add end

// 2004, 7, 9 sobiet add start

void	C_VS_UI_GAME::RunQuestList()
{
	if (m_pC_quest_manager == NULL)
		return;
	m_pC_quest_manager->RunQuestList();
}

bool	C_VS_UI_GAME::IsRunningQuestList() const
{
	if (m_pC_quest_manager != NULL)
	{
		return m_pC_quest_manager->IsRunningQuestList();
	}
	
	return false;
}

void	C_VS_UI_GAME::CloseQuestList() 
{
	if (NULL == m_pC_quest_manager)
		return;
	m_pC_quest_manager->CloseQuestList();
}

void	C_VS_UI_GAME::RunQuestDetail()
{
	if (m_pC_quest_manager == NULL)
		return;
	m_pC_quest_manager->RunQuestDetail();
}

bool	C_VS_UI_GAME::IsRunningQuestDetail() const
{
	if (m_pC_quest_manager != NULL)
	{
		return m_pC_quest_manager->IsRunningQuestDetail();
	}
	return false;
}

void	C_VS_UI_GAME::CloseQuestDetail() 
{
	if (m_pC_quest_manager == NULL)
		return;
	m_pC_quest_manager->CloseQuestDetail();
}
void	C_VS_UI_GAME::RunQuestMission()
{
	if (m_pC_quest_manager == NULL)
		return;

	m_pC_quest_manager->RunQuestMission();
}

bool	C_VS_UI_GAME::IsRunningQuestMission() const
{
	if (m_pC_quest_manager != NULL)
	{
		return m_pC_quest_manager->IsRunningQuestMission();
	}
	return false;
}

void	C_VS_UI_GAME::CloseQuestMission() 
{
	if (NULL ==m_pC_quest_manager)
		return;
	m_pC_quest_manager->CloseQuestMission();
}

void	C_VS_UI_GAME::RunQuestItem()
{
	if (m_pC_quest_manager == NULL)
		return;

	m_pC_quest_manager->RunQuestItem();
}

bool	C_VS_UI_GAME::IsRunningQuestItem() const
{
	if (m_pC_quest_manager != NULL)
	{
		return m_pC_quest_manager->IsRunningQuestItem();
	}
	return false;
}

void	C_VS_UI_GAME::CloseQuestItem() 
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->CloseQuestItem();
}

void	C_VS_UI_GAME::CloseQuestIcon(int JustClose) 
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->CloseQuestIcon(JustClose);
}
void	C_VS_UI_GAME::UpdateQuestItemInfo(std::vector<MItem*>& vItem)
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->UpdateQuestItemInfo(vItem);

}

void	C_VS_UI_GAME::PushGQuestExcuteElement(DWORD qID, BYTE bCondition, WORD wIndex)
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->PushGQuestExcuteElement(qID,bCondition,wIndex);
}

void	C_VS_UI_GAME::RunNextGQuestExcuteElement()
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->RunNextGQuestExcuteElement();
}

// 2004, 7, 9 sobeit add end
void	C_VS_UI_GAME::DeleteQuestItem(int nSlot)
{
	if (NULL ==m_pC_quest_manager)
		return;

	m_pC_quest_manager->DeleteQuestItem(nSlot);
}

// 2004, 10, 25, sobeit add start - ���� ����
void	C_VS_UI_GAME::RunModifyTax()
{
	if(m_pC_dialog_Modify_Tax)
		DeleteNew(m_pC_dialog_Modify_Tax);

	m_pC_dialog_Modify_Tax = new C_VS_UI_MONEY_DIALOG(400, 300, 2, 0, ExecF_ModifyTax, DIALOG_OK|DIALOG_CANCEL, 10, 
		C_VS_UI_MONEY_DIALOG::MODIFY_TAX);	// by sigi
	m_pC_dialog_Modify_Tax->Start();
}

void	C_VS_UI_GAME::RunRangerChat()
{
	if (m_pC_Ranger_Chat == NULL)
	{
		m_pC_Ranger_Chat = new C_VS_UI_RANGER_CHAT();
		m_pC_Ranger_Chat->Start();

	}
}
bool	C_VS_UI_GAME::IsRunngingRangerChat()
{
	return (NULL != m_pC_Ranger_Chat);
}
void	C_VS_UI_GAME::CloseRangerChat()
{
	if (NULL == m_pC_Ranger_Chat)
		return;

	DeleteNew(m_pC_Ranger_Chat);
	assert(m_pC_tribe_interface != NULL);
}

void	C_VS_UI_GAME::SetRangerChatString(char* str)
{
	if (m_pC_Ranger_Chat != NULL)
	{
		m_pC_Ranger_Chat->SetRangerChatString(str);
	}
}
// 2004, 10, 25, sobeit add end

bool	C_VS_UI_GAME::IsRunningPersnalShopMessage() const
{
	return (m_pC_persnalshopmessage != NULL);
}


void    C_VS_UI_GAME::RunPersnalShopMessage()
{
	if(m_pC_persnalshopmessage == NULL)
	{
		m_pC_persnalshopmessage = new C_VS_UI_PERSNALSHOP_MESSAGE;
		m_pC_persnalshopmessage->Start();
	}
		

}
void	C_VS_UI_GAME::ClosePersnalShopMessage()
{
	if(m_pC_persnalshopmessage== NULL)
		return;
	
	m_pC_persnalshopmessage->Finish();
	DeleteNew(m_pC_persnalshopmessage);

//	assert(m_pC_persnalshopmessage!=NULL);
}


bool	C_VS_UI_GAME::IsRunningPersnalShop() const
{
	return (NULL != m_pC_persnalshop);
}
    
void C_VS_UI_GAME::RunPersnalShop()
{
	if( m_pC_persnalshop == NULL)	
	{
		m_pC_persnalshop = new C_VS_UI_PERSNALSHOP();
		
		assert(m_pC_tribe_interface != NULL);

		m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
		m_pC_tribe_interface->StorageRunningAnnounced();
		gC_vs_ui.setinventory_mode(1);
		m_pC_persnalshop->Start();
	}
}

void C_VS_UI_GAME::ClosePersnalShop()
{
	if (NULL == m_pC_persnalshop)
		return;

	DeleteNew(m_pC_persnalshop);
	assert(m_pC_tribe_interface != NULL);
	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
}
void C_VS_UI_GAME::SetPersnalShop(MStorage* pStorage)
{
	if (m_pC_persnalshop)
		m_pC_persnalshop->SetPersnalShop(pStorage);
}	

// 2004, 11, 30, sobeit add start
void	C_VS_UI_GAME::RunPowerjjang()
{
	if (m_pC_Powerjjang == NULL)
	{
		m_pC_Powerjjang = new C_VS_UI_POWER_JJANG();
		m_pC_Powerjjang->Start();

	}
}
bool	C_VS_UI_GAME::IsRunningPowerjjang()
{
	return (NULL != m_pC_Powerjjang);
}
void	C_VS_UI_GAME::ClosePowerjjang()
{
	if (NULL == m_pC_Powerjjang)
		return;

	DeleteNew(m_pC_Powerjjang);
	assert(m_pC_tribe_interface != NULL);
}

void	C_VS_UI_GAME::PowerjjangGambleResult(BYTE bItemCode)
{
	if (NULL == m_pC_Powerjjang)
		return;

	m_pC_Powerjjang->PowerjjangGambleResult(bItemCode);
}
// 2004, 11, 30, sobeit add end

// 2005, 1, 3, sobeit add start
void C_VS_UI_GAME::OpenInventoryToSwapAdvanceItem()
{
	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionBeforeEventOccured();
	m_pC_tribe_interface->OpenInventoryToSwapAdvanceItem();
	gbl_swap_advancement_item_running = true;
}

void C_VS_UI_GAME::FinishSwapAdvancementItem()
{
	if (gbl_swap_advancement_item_running == false)
		return;

	assert(m_pC_tribe_interface != NULL);

	m_pC_tribe_interface->DoCommonActionAfterEventOccured();
	m_pC_tribe_interface->FinishItemSelling();
	gbl_swap_advancement_item_running = false;
}

bool	C_VS_UI_GAME::IsRunningSwapAdvancementItem() const
{
	return gbl_swap_advancement_item_running;
}
// 2005, 1, 3, sobeit add end


// 2005, 1, 11, sobeit add start - �ҿ��̿����� â
void	C_VS_UI_GAME::Run_Campaign_Help_Unfortunate_Neighbors(int value)
{
	if(m_pC_dialog_Campaign_Help)
		DeleteNew(m_pC_dialog_Campaign_Help);

	m_pC_dialog_Campaign_Help = new C_VS_UI_MONEY_DIALOG(250, 300, 2, 0, ExecF_CampaignHelp, DIALOG_OK|DIALOG_CANCEL, 10, 
		C_VS_UI_MONEY_DIALOG::MONEY_CAMPAIGN_HELP);	// by sigi
	m_pC_dialog_Campaign_Help->Start();
	m_pC_dialog_Campaign_Help->SetTempValue(value);

}
// 2005, 1, 11, sobeit add end


// 2005, 1, 17, sobeit add start - ����Ʈ ����
void	C_VS_UI_GAME::SetQuestNpcDialog(void* pVoid)
{
	if(NULL == pVoid) 
		return;
	if(NULL == m_pC_Quest_Npc_Dialog)
	{
		m_pC_Quest_Npc_Dialog = new C_VS_UI_QUEST_NPC_DIALOG;	
	}
	m_pC_Quest_Npc_Dialog->SetQuestNpcDialog((C_VS_UI_NPC_DIALOG*)pVoid);
	
}
// 2005, 1, 17, sobeit add end - ����Ʈ ����
	

// 2005, 1, 24, sobeit add start - ������ �ޱ� �̺�Ʈ
void	C_VS_UI_GAME::Run_Confirm_GetItemEvent(int value)
{
	if (m_pC_get_event_item != NULL)
	{
		DeleteNew(m_pC_get_event_item);
	}
	
	m_pC_get_event_item = new C_VS_UI_ASK_DIALOG(-1, 50, 5, 1, ExecF_Get_Event_Item, DIALOG_CANCEL | DIALOG_OK, 
		C_VS_UI_ASK_DIALOG::ASK_GET_EVENT_ITEM, 0);	
	
	assert(m_pC_get_event_item != NULL);
	m_pC_get_event_item->SetTempValue(value);
	
	m_pC_get_event_item->Start();	

}
// 2005, 1, 24, sobeit add end

// 2005, 2, 1, sobeit add start
void	C_VS_UI_GAME::RunWebBrowser(HWND hWnd, char* szURL, HINSTANCE hInst) //void* pWebOjbect)
{
	if(IsRunningWebBrowser())
		CloseWebBrowser();
	if (m_pC_WebBrowser == NULL)
	{
		m_pC_WebBrowser = new C_VS_UI_WEBBROWSER();
		
		if(false == m_pC_WebBrowser->Start(hWnd, szURL, hInst)) // pWebOjbect))
		{
			gpC_base->SendMessage(UI_MESSAGE_BOX, UI_STRING_MESSAGE_FAIL_OPEN_WEBPAGE, 0, 	NULL);
			DeleteNew(m_pC_WebBrowser);
		}
	}
}
bool	C_VS_UI_GAME::IsRunningWebBrowser()
{
	return (NULL != m_pC_WebBrowser);
}
void	C_VS_UI_GAME::CloseWebBrowser()
{
	if (NULL == m_pC_WebBrowser)
		return;

	DeleteNew(m_pC_WebBrowser);
	assert(m_pC_tribe_interface != NULL);
}
int	C_VS_UI_GAME::GetMouseCursorInfo()
{
	if (NULL == m_pC_WebBrowser)
		return 0;
	return m_pC_WebBrowser->GetMouseCursorInfo();
	
}
bool C_VS_UI_GAME::IsInRectPointWebBrowser(int X, int Y)
{
	if (NULL == m_pC_WebBrowser)
		return false;
	return m_pC_WebBrowser->IsInRectPoint(X, Y);
}
// 2005, 2, 1, sobeit add end

// 2005, 2, 24, sobeit add start
void	C_VS_UI_GAME::RunSubInventory(MItem* pItem)
{
	TYPE_OBJECTID itemID = pItem->GetID();
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.find(itemID);

	C_VS_UI_INVENTORY_SUB* pSubInventory = NULL;

	if(pos == m_SubInventoryMap.end())
	{
		pSubInventory = new C_VS_UI_INVENTORY_SUB(pItem);

		m_SubInventoryMap.insert(
			SUBINVENTORY_MAP::value_type(itemID, pSubInventory));
	}
	else
	{
		pSubInventory = pos->second;
	}

	if(pSubInventory)
		pSubInventory->Start();
}

int		C_VS_UI_GAME::CountRunningSubInventory()
{
	return m_SubInventoryMap.size();
}


bool	C_VS_UI_GAME::IsRunningSubInventory(TYPE_OBJECTID SubInventory)
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.find(SubInventory);

	if(pos != m_SubInventoryMap.end())
	{
		C_VS_UI_INVENTORY_SUB* pSubInventory = pos->second;

		if(pSubInventory)
		{
			return pSubInventory->Running();
		}
	}

	return false;
}


bool	C_VS_UI_GAME::ExistSubInventory(TYPE_OBJECTID SubInventory)
{
	return m_SubInventoryMap.find(SubInventory) != m_SubInventoryMap.end();
}


bool	C_VS_UI_GAME::AddItemToSubInventory(TYPE_OBJECTID SubInventory, MItem* pItem, int X, int Y)
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.find(SubInventory);

	if(pos != m_SubInventoryMap.end())
	{
		C_VS_UI_INVENTORY_SUB* pSubInventory = pos->second;

		if(pSubInventory)
		{
			return pSubInventory->AddItemToSubInventory(pItem, X, Y);
		}
	}	

	return false;
}

bool	C_VS_UI_GAME::ReplaceSubInventoryItem(TYPE_OBJECTID SubInventory, MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem)
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.find(SubInventory);

	if(pos != m_SubInventoryMap.end())
	{
		C_VS_UI_INVENTORY_SUB* pSubInventory = pos->second;

		if(pSubInventory)
		{
			return pSubInventory->ReplaceSubInventoryItem(pItem, x, y, pOldItem);
		}
	}	
	
	return false;
}

void	C_VS_UI_GAME::CloseSubInventory(TYPE_OBJECTID SubInventory)
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.find(SubInventory);

	if(pos != m_SubInventoryMap.end())
	{
		C_VS_UI_INVENTORY_SUB* pSubInventory = pos->second;

		if(pSubInventory)
		{
			pSubInventory->Finish();
		}
	}

//	if (NULL == m_pC_SubInventory)
//		return;

//	DeleteNew(m_pC_SubInventory);
	assert(m_pC_tribe_interface != NULL);
}

void	C_VS_UI_GAME::CloseAllSubInventory()
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.begin();

	for(; pos != m_SubInventoryMap.end(); ++pos)
	{
		C_VS_UI_INVENTORY_SUB* pSubInventory = pos->second;

		if(pSubInventory)
		{
			pSubInventory->Finish();
		}
	}
}

#if __CONTENTS(__ITEM_SHOW)
void	C_VS_UI_GAME::CloseInventory()
{
	assert(m_pC_hotkey);

	if (!m_pC_gamemenu)
		m_pC_hotkey->CloseInventory();
}
#endif //__ITEM_SHOW

void	C_VS_UI_GAME::ClearSubInventory()
{
	SUBINVENTORY_MAP::iterator pos = m_SubInventoryMap.begin();

	for(; pos != m_SubInventoryMap.end(); ++pos)
		DeleteNew(pos->second);

	m_SubInventoryMap.clear();
}

// 2005, 2, 25, sobeit add end

void	C_VS_UI_GAME::AutoMoveInventoryItemToQuickSlot(BYTE inven_grid_x, BYTE inven_grid_y, BYTE quick_slot_num)
{
	gC_vs_ui.AutoMoveInventoryItemToQuickSlot(inven_grid_x, inven_grid_y, quick_slot_num);
}


//2005, 4, 21, sjheon add start - ��ȥ���Ǳ�  â
void	C_VS_UI_GAME::Run_Wedding_Contribution_Unfortunate_Neighbors(int value)
{
	if(m_pC_dialog_Wedding_Contribution)
		DeleteNew(m_pC_dialog_Wedding_Contribution);

	m_pC_dialog_Wedding_Contribution = new C_VS_UI_MONEY_DIALOG(250, 300, 3, 0, ExecF_WeddingContribution, DIALOG_OK|DIALOG_CANCEL, 6, 
		C_VS_UI_MONEY_DIALOG::MONEY_WEDDING_CONTRIBUTION);	// by sjheon
	m_pC_dialog_Wedding_Contribution->Start();
	m_pC_dialog_Wedding_Contribution->SetTempValue(value);

}
//2005, 4, 21, sjheon add end - ��ȥ���Ǳ� â


//2005, 7, 14, sjheon add  - ComboSkill Add
void	C_VS_UI_GAME::SetComboCnt(int ComboCnt)
{
	m_pC_tribe_interface->SetComboCnt(ComboCnt) ; 
}

//2005, 7, 14, sjheon add  - SelectedComboSkill Add

void	C_VS_UI_GAME::SetSelectedAttackComboSkill(bool	bAttackSkill)
{
	m_pC_tribe_interface->SetSelectedAttackComboSkill(bAttackSkill) ;
}

//2005, 7, 14, sjheon add  - SelectedComboSkill End


//2005, 7, 18, sjheon add  - PopupSummerComBack Add
void	C_VS_UI_GAME::PopupSummerComBack(int value)
{
	if(m_pC_SummerCommback)
		DeleteNew(m_pC_SummerCommback);

	if(value == 0) 
		m_pC_SummerCommback = new C_VS_UI_SUMMER_COME_BACK(250, 300, 3, 0, ExecF_SummerComeBack, DIALOG_OK , C_VS_UI_SUMMER_COME_BACK::COME_BACK_LOGIN_AFTER) ;
	else if(value == 1)
		m_pC_SummerCommback = new C_VS_UI_SUMMER_COME_BACK(250, 300, 3, 0, ExecF_SummerComeBack, DIALOG_OK , C_VS_UI_SUMMER_COME_BACK::COME_BACK_SETTLEMENT_AFTER) ;
	else if(value == 2)
		m_pC_SummerCommback = new C_VS_UI_SUMMER_COME_BACK(250, 300, 3, 0, ExecF_SummerComeBack, DIALOG_OK , C_VS_UI_SUMMER_COME_BACK::COME_BACK_RECOMMENDER) ;

	else if(value == 3)
		m_pC_SummerCommback = new C_VS_UI_SUMMER_COME_BACK(250, 300, 2, 0, ExecF_SummerComeBack, DIALOG_OK , C_VS_UI_SUMMER_COME_BACK::COME_BACK2006_LOGIN_AFTER) ;
	else if(value == 4)
		m_pC_SummerCommback = new C_VS_UI_SUMMER_COME_BACK(250, 300, 3, 0, ExecF_SummerComeBack, DIALOG_OK , C_VS_UI_SUMMER_COME_BACK::COME_BACK2006_SETTLEMENT_AFTER) ;

	m_pC_SummerCommback->Start();
}
//2005, 7, 18, sjheon add  - PopupSummerComBack End


void	C_VS_UI_GAME::SetBloodBurstAttackGage()
{
	m_pC_tribe_interface->SetBloodBurstAttackGage() ; 
}

void	C_VS_UI_GAME::SetBloodBurstDefenseGage()
{
	m_pC_tribe_interface->SetBloodBurstDefenseGage() ; 
}

void	C_VS_UI_GAME::SetBloodBurstPartyGage()
{
	m_pC_tribe_interface->SetBloodBurstPartyGage() ; 
}


bool	C_VS_UI_GAME::GetBloodBurstAttackGageFull()
{
	return	m_pC_tribe_interface->GetBloodBurstAttackGageFull();
}

bool	C_VS_UI_GAME::GetBloodBurstDefenseGageFull()
{
		return	m_pC_tribe_interface->GetBloodBurstDefenseGageFull();
}

bool	C_VS_UI_GAME::GetBloodBurstPartyGageFull()
{
		return	m_pC_tribe_interface->GetBloodBurstPartyGageFull();
}

int		C_VS_UI_GAME::GetSelectedSkillID()
{
	if(m_pC_tribe_interface)
		return	m_pC_tribe_interface->GetSelectedSkillID();
	else return	 -1 ;
}

void	C_VS_UI_GAME::RunTaxRate(int	TexRate) //void* pWebOjbect)
{
	if(IsRunTaxRate())	return ;
	//	CloseWebBrowser();
	if (m_pC_TexRate == NULL)
	{
		m_pC_TexRate = new VS_UI_TAXRATE(TexRate)	;
		
		m_pC_TexRate->Start() ; 
		
		//if(false == m_pC_WebBrowser->Start(hWnd, szURL, hInst)) // pWebOjbect))
		
	}

//	if(m_pC_dialog_Wedding_Contribution)
//		DeleteNew(m_pC_dialog_Wedding_Contribution);
//
//	m_pC_dialog_Wedding_Contribution = new C_VS_UI_MONEY_DIALOG(250, 300, 3, 0, ExecF_WeddingContribution, DIALOG_OK|DIALOG_CANCEL, 6, 
//		C_VS_UI_MONEY_DIALOG::MONEY_WEDDING_CONTRIBUTION);	// by sjheon
//	m_pC_dialog_Wedding_Contribution->Start();
//	m_pC_dialog_Wedding_Contribution->SetTempValue(value);


	//if (m_pC_game)
	//	m_pC_game->RunTaxRate() ; //pWebOjbect);
}

bool	C_VS_UI_GAME::IsRunTaxRate()
{
	return (NULL != m_pC_TexRate);
}

void	C_VS_UI_GAME::CloseTexRate()
{
	if (NULL == m_pC_TexRate)
		return;

	DeleteNew(m_pC_TexRate);
}


void	C_VS_UI_GAME::RunDelegation_Of_Power(const char* pName)
{
	if (m_pC_Delegation_Of_Power != NULL)
	{
		DeleteNew(m_pC_Delegation_Of_Power);
	}

	if (m_pC_Delegation_Of_Power == NULL)
	{
		m_pC_Delegation_Of_Power = new C_VS_UI_ASK_DIALOG(-1, 50, 3, 1, ExecF_Delegation_Of_Power, DIALOG_CANCEL | DIALOG_OK, 
											C_VS_UI_ASK_DIALOG::ASK_DELEGATION_OF_POWER, 0,
											(void*)pName );	// by sigi
		m_pC_Delegation_Of_Power->Start() ; 
		gbl_Delegation_Of_Power_running = true;
	}

}

bool	C_VS_UI_GAME::IsRunningDelegation_Of_Power()
{
	if(gbl_Delegation_Of_Power_running) return	true ; 
	return false; 
}

void	C_VS_UI_GAME::CloseDelegation_Of_Power()
{
	if (NULL == m_pC_Delegation_Of_Power)
		return;
	DeleteNew(m_pC_Delegation_Of_Power);
	gbl_Delegation_Of_Power_running = false ;
}


void C_VS_UI_GAME::RunOperator()
{
	if(IsRunningOperator())
		CloseOperator();

	m_pC_operator = new C_VS_UI_OPERATOR();
	m_pC_operator->Start();

}


void C_VS_UI_GAME::CloseOperator()
{
	if(m_pC_operator != NULL)
	{
		m_pC_operator->Finish();
		DeleteNew(m_pC_operator);
	}
}


void C_VS_UI_GAME::ToggleOperator()
{
	if(IsRunningOperator())
		CloseOperator();
	else
		RunOperator();	
}


bool C_VS_UI_GAME::IsRunningOperator()
{
	return m_pC_operator != NULL;
}


void C_VS_UI_GAME::RunTimeCount(DWORD time, LPCTSTR desc)
{
	if(IsRuningTimeCount())
		CloseTimeCount();

	m_pC_timer = new C_VS_UI_TIME_COUNT(time, desc);
	m_pC_timer->Start();

}


void C_VS_UI_GAME::CloseTimeCount()
{
	if(m_pC_timer != NULL)
	{
		m_pC_timer->Finish();
		DeleteNew(m_pC_timer);
	}
}


bool C_VS_UI_GAME::IsRuningTimeCount()
{
	return m_pC_timer != NULL;
}

#if __CONTENTS(__USER_GRADE)
void C_VS_UI_GAME::ToggleUserGrade()
{
	if(IsRuningUserGrade())
	{
		m_pC_usergrade->Finish();
		DeleteNew(m_pC_usergrade);
	}
	else
	{
		m_pC_usergrade = new C_VS_UI_USER_GRADE();
		m_pC_usergrade->Start();
	}
}

bool C_VS_UI_GAME::IsRuningUserGrade()
{
	return m_pC_usergrade != NULL;
}
#endif	//__USER_GRADE
#if __CONTENTS(__PET_MIXINGFORGE)
//-----------------------------------------------------------------------------
// RunMixingForge
//
// MixingForge�� �����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::RunPetMixingForge()
{
	if (m_pC_pet_mixing_forge == NULL)
	{
		if(m_pC_tribe_interface != NULL)
		{
			m_pC_tribe_interface->CloseInventory();
			m_pC_tribe_interface->CloseGear();
		}
		
		m_pC_pet_mixing_forge = new C_VS_UI_PET_MIXING_FORGE();	
		m_pC_pet_mixing_forge->Start();
	}
}


//-----------------------------------------------------------------------------
// CloseMixingForge
//
// �̹� finish�� �����̸� �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_GAME::ClosePetMixingForge()
{
	if (!m_pC_pet_mixing_forge)
		return;

	DeleteNew(m_pC_pet_mixing_forge);
}

//-----------------------------------------------------------------------------
// C_VS_UI_GAME::IsRunningMixingForge
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_GAME::IsRunningPetMixingForge() const
{
	if(m_pC_pet_mixing_forge)
		return m_pC_pet_mixing_forge->Running();

	return false;
}
#endif //__PET_MIXINGFORGE

#if __CONTENTS(__FRIEND_ADDITION)
/*
	ģ�� UI ���� â �ݱ�
*/
bool C_VS_UI_GAME::CloseFriendMessagePopup()
{
	// ������ ������ ���� ������ �θ� ���ϰ�����, �ϴ� ������ ������ ������ �� ���� ������
	// ������ ���Ÿ� �ϵ��� �Ѵ�.

	bool close_result = false;
	
	// �޴� �˾�â
	Window* pWindow = gpC_window_manager->GetWindowbyName("PlayerPopupMenu");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}

	// ģ�� �ý��� ���� ������
	pWindow = gpC_window_manager->GetWindowbyName("FriendSystem");
	if (pWindow)
	{// �̰� ������ ����� �ȵǰ�, ������..���ο� ����� �����Ͱ� �ֱ� ������...
		if (pWindow->Running())
		{
			pWindow->Finish();
			close_result = true;
		}
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendInfo");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendWaitList");
	if (pWindow)
	{// �̰� ������ ����� �ȵǰ�, ������..���ο� ����� �����Ͱ� �ֱ� ������...
		if (pWindow->Running())
		{
			pWindow->Finish();
			close_result = true;
		}
	}	

	// �޽��� �˾�â ����
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddIsPlayer");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddInput");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendDelIsPlayer");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendDelNotSelect");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendOption");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendOptionConfirm");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}

	// ������ ����Ǹ� �� �ִ� �޽��� �ڽ� ��� ����
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddOkPopupMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendRequestAddMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendResultAddMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
		close_result = true;
	}

	// ģ�� �߰� Ȯ��â�� �������� ������ �����Ƿ� ������ ��ŭ ���� �Ѵ�.
	char window_name[128] = {0};
	for (int i = 0; i < message_box_count; ++i)
	{
		sprintf(window_name,"FriendResName-%d",i);
		pWindow = gpC_window_manager->GetWindowbyName(window_name);
		if (pWindow)
		{
			pWindow->Finish();
			DeleteNew(pWindow);
			close_result = true;
		}
	}

	return close_result;
}

/*
	�޽��� �ڽ� �˾� UI ���� �Լ�
*/
void C_VS_UI_GAME::ReleaseFriendMessagePopup()
{
	// ������ ������ ���� ������ �θ� ���ϰ�����, �ϴ� ������ ������ ������ �� ���� ������
	// ������ ���Ÿ� �ϵ��� �Ѵ�.
	
	// �޴� �˾�â
	Window* pWindow = gpC_window_manager->GetWindowbyName("PlayerPopupMenu");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}

	// ģ�� �ý��� ���� ������
	pWindow = gpC_window_manager->GetWindowbyName("FriendSystem");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendWaitList");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}	
	pWindow = gpC_window_manager->GetWindowbyName("FriendInfo");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}

	// �޽��� �˾�â ����
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddIsPlayer");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddInput");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendDelIsPlayer");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendDelNotSelect");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendOption");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendOptionConfirm");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}

	// ������ ����Ǹ� �� �ִ� �޽��� �ڽ� ��� ����
	pWindow = gpC_window_manager->GetWindowbyName("FriendAddOkPopupMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendRequestAddMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}
	pWindow = gpC_window_manager->GetWindowbyName("FriendResultAddMessage");
	if (pWindow)
	{
		pWindow->Finish();
		DeleteNew(pWindow);
	}

	// ģ�� �߰� Ȯ��â�� �������� ������ �����Ƿ� ������ ��ŭ ���� �Ѵ�.
	char window_name[128] = {0};
	for (int i = 0; i < message_box_count; ++i)
	{
		sprintf(window_name,"FriendResName-%d",i);
		pWindow = gpC_window_manager->GetWindowbyName(window_name);
		if (pWindow)
		{
			pWindow->Finish();
			DeleteNew(pWindow);
		}
	}
}
#endif //__FRIEND_ADDITION
