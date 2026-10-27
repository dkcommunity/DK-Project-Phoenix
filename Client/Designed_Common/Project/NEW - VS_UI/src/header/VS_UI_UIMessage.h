#ifndef __VS_UI_UIMESSAGE_H__
#define __VS_UI_UIMESSAGE_H__

// Phoenix: __CONTENTS() macro'sunu tanımlayan ContentsFilter.h gerekli;
// aksi halde enum'daki koşullu üyeler (UI_AUTOATTACK_SET vb.) eksik kalıyor.
#include "ContentsFilter.h"

// message
enum UI_MESSAGE
{
	INVALID_MESSAGE,

	UI_STORAGE_BUY,
	UI_SELECT_STORAGE_SLOT,
	UI_RUNNING_GAMEMENU,
	UI_FINISH_LEVELUP_BUTTON,
	UI_CLICK_BONUS_POINT,
	UI_INFO,

//	UI_MINIMAP_TOGGLE,
	UI_DROP_MONEY,
	UI_EXCHANGE_REQUEST_CANCEL,
	UI_EXCHANGE_ACCEPT,
	UI_EXCHANGE_MONEY,
	UI_WITHDRAW_MONEY,
	UI_ITEM_SELECT_EXCHANGE,	
	UI_DEPOSIT_MONEY,
	UI_CHECK_EXIST_ID,
	UI_RUN_NEWUSER_REGISTRATION,
	UI_INFO_CLOSE,
	UI_GAMEMENU_CONTINUE,
/*
	//
	// PDS
	//
	UI_CHANGE_PCS_CONNECTED_SLOT,		// �̹� ����� �ٸ� ����� click�Ͽ���.
	UI_PLEASE_PCS_CONNECT_ME,			// ����� �ִ� wait room�� click�Ͽ���.
	UI_PCS_CONNECTOR_GRANTED,			// wait room�� �ִ� connector�� ������ �³��Ͽ���.
	UI_QUIT_PCS_ONLINE_MODE,			// PCS online mode���� quit ��ư�� ������.
	UI_END_PCS,								// sending �ϰ� ���� ��, end�� ������.
	UI_SEND_PCS_NUMBER,
	UI_PDS_CLOSED,							// slayer pds�� ������. !���� ������ �����Ѵ�.

	UI_PLEASE_SET_SLAYER_VALUE,		// slayer pds���� gage menu�� �����Ͽ���.
*/
//	UI_LEARN_SLAYER_SKILL,				// skill tree���� ��� �� �ִ� skill�� �����Ͽ���.
//	UI_CLOSE_SKILL_TREE,
//	UI_CLOSE_SKILL_VIEW,

	UI_BACKGROUND_MOUSE_FOCUS,			// Mouse pointer�� UI Window���� �̵����� ���� ��, �� Window�� ����� ��� Client�� ����.
	UI_REMOVE_BACKGROUND_MOUSE_FOCUS,// Game Menu�� ���� topmost Window�� ��µǾ��� ���� background focus�� �����ϴ� ���� ����.

	//
	// ���� Message
	//
	UI_BUY_ITEM,

	UI_SELL_ITEM,
	UI_REPAIR_ITEM,
	UI_SILVERING_ITEM,
	UI_ITEM_SELL_FINISHED,
	UI_ITEM_REPAIR_FINISHED,
	UI_ITEM_SILVERING_FINISHED,

	UI_CLOSE_SHOP,
	UI_CLOSE_STORAGE,
	UI_CLOSE_EXCHANGE,
	UI_CANCEL_EXCHANGE,
	UI_OK_EXCHANGE,

	UI_CHARACTER_MANAGER_FINISHED,	// Characters Window���� �ʱ�ȭ������ �� �� �˷��ش�.
	UI_TERMINATION,						// ����
	UI_LOGIN,								// ����� login ��û
	UI_LOGOUT,								
#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	UI_QUITEXIT,						// ��������
#endif //__080405_FIREST_UI_UPDATE
	UI_NEW_USER_REGISTRATION,			// New account registration request
	UI_CONNECT,							   // Game connection.
	UI_CHAT_RETURN,						// ä��â �Է�.
//	UI_CHAT_SELECT_NAME,				// ä��â���� ���� �ּ� by larosel
	UI_SELECT_SKILL,						// Skill selection.
	UI_CANCEL_SELECT_SKILL,				// Skill ������ ��ҵǾ���.

	//
	// Item message
	//

	// drop
	UI_ITEM_DROP_TO_CLIENT,				// Item�� �ٴڿ� ����߷ȴ�.
	UI_ITEM_DROP_TO_INVENTORY, 		// Inventory�� ���´�.
	//UI_ITEM_DROP_TO_TRADEGRID,			// ��ȯâ �׸��忡 ���´�.
	UI_ITEM_DROP_TO_GEAR,				// Gear�� ���´�.
	UI_ITEM_DROP_TO_QUICKSLOT,			// Quick Slot�� ���´�.

	// pick up
	// Client���� ���� ���� Client -> UI �̴�.
	//UI_ITEM_PICKUP_FROM_TRADEGRID,
	UI_ITEM_PICKUP_FROM_INVENTORY,
	UI_ITEM_PICKUP_FROM_GEAR,
	UI_ITEM_PICKUP_FROM_QUICKSLOT,

	// insert to other Item
	//UI_ITEM_INSERT_FROM_TRADEGRID,
	UI_ITEM_INSERT_FROM_INVENTORY,
	UI_ITEM_INSERT_FROM_GEAR,
	UI_ITEM_INSERT_FROM_QUICKSLOT,  // ���̴� ��� - ���� �������� -

	// use
	UI_ITEM_USE,							// Inventory���� ���.
	UI_ITEM_USE_QUICKSLOT,				// Quick Item Slot���� ���.

	//
	// Character management
	//
	UI_NEW_CHARACTER,
	UI_DELETE_CHARACTER,

	// etc
	UI_CLOSE_BBS,

	// game option
	UI_CHANGE_OPTION,
	UI_CLOSE_OPTION,
	UI_RESET_BUTTON,

	// title option
//	UI_CHANGE_TITLE_OPTION,
//	UI_CLOSE_TITLE_OPTION,	

	// repleace	
	//UI_CHANGE_ITEM_IN_INVENTORY,	// Inventory���� Item�� ��ü�Ǿ���.
	//UI_REMOVE_ITEM_FROM_GEAR,		// Gear���� Item�� ������.

	//tutorial by larosel
	UI_CLOSE_BOOKCASE,
	UI_CLOSE_BRIEFING,
	UI_CLOSE_COMPUTER,
	//UI_CONFIRM_TUTORIAL_EXIT,		// �ּ� by sigi
	UI_CLOSE_TUTORIAL_EXIT,
	//UI_TUTORIAL_EXIT_ACCEPT,		// �ּ� by sigi
	//UI_TUTORIAL_EXIT_SELECT,		// �ּ� by sigi

	UI_CLOSE_DESC_DIALOG,
	UI_CLOSE_ELEVATOR,
	UI_SELECT_ELEVATOR,

	UI_SELL_ALL_ITEM,	//�ذ� ��� �ȱ�
	
//	UI_USE_SERUM,	// ��û����ϱ�

	UI_ITEM_TO_QUICKITEMSLOT, // focus_grid_x, focus_grid_y, (MItem *)p_item

	UI_CLOSE_SLAYER_PORTAL,//�����̾� ��Ż ���
	UI_SLAYER_PORTAL, //�������� left = zoneid right's hiword = x right's LOWORD = y

	UI_FINISH_REQUEST_PARTY_BUTTON,	// ��Ƽ��û �ݱ� left == TRUE : timeout
	UI_PARTY_REQUEST_CANCEL, 
	UI_PARTY_ACCEPT,

	UI_CLOSE_PARTY_MANAGER,
	UI_AWAY_PARTY, // ��Ƽ Ż�� �߹� left :: -1 : Ż�� 0~4 : �߹�

	UI_FINISH_REQUEST_DIE_BUTTON,	// ����-_-��û �ݱ� left == TRUE : timeout

	UI_CLOSE_TEAM_LIST,
	UI_CLOSE_TEAM_INFO,
	UI_CLOSE_TEAM_MEMBER_INFO,
	UI_CLOSE_TEAM_REGIST,
	UI_CLOSE_TEAM_MEMBER_LIST,
	
	UI_REQUEST_GUILD_INFO,
	UI_REQUEST_GUILD_MEMBER_LIST,
	UI_MODIFY_GUILD_MEMBER,

	UI_SELECT_TEAM_MEMBER_LIST,			// void_ptr = CHAR_NAME
	UI_SELECT_READY_TEAM_LIST,		// void_ptr = TEAM_NAME
	UI_SELECT_REGIST_TEAM_LIST,		// void_ptr = TEAM_NAME

	UI_JOIN_READY_TEAM,				// void_ptr = TEAM_NAME
	UI_JOIN_REGIST_TEAM,			// void_ptr = TEAM_NAME

	UI_REGIST_GUILD_MEMBER,			// void_ptr = introduction max:150byte â �ݾ��ٰ�!
	UI_REGIST_GUILD_TEAM,			// left = TEAM_NAME, void_ptr = introduction max:150byte â �ݾ��ٰ�!

	UI_CLOSE_FILE_DIALOG,			// left = mode, void_ptr = file_name

	UI_CONNECT_SERVER,				// left == true : group false : server right : id
									// request_server_list �Ⱥ����Ƿ� �����ϸ� list�����ٰ�
	UI_CLOSE_SERVER_SELECT,			// left == true : group false : server

	UI_NEWCHARACTER_CHECK,				// void_ptr : name

	UI_ENCHANT_ACCEPT,
	UI_ENCHANT_CANCEL,
	
	UI_MESSAGE_BOX,					// left = �޽��� ����  STRING_MESSAGE_CANNOT_REPAIR

	//UI_OTHER_INFO,					// Other Player Info Box
	UI_CLOSE_OTHER_INFO,			//            //
	UI_MODIFY_TEAM_INFO,			// �Ұ��� ����  left = guild id, ptr = intro
	UI_MODIFY_GUILD_MEMBER_INTRO,	// �� �Ұ� �� ���� left = , ptr = intro

	UI_CLOSE_TRACE_WINDOW,			// TRACE â �ݱ�.
	UI_SEND_NAME_FOR_SOUL_CHAIN,	// TRACE �̸� ������ ptr ��

	// �ݸ����� ����
	UI_RUN_CONNECT,
	UI_SELECT_GRADE_SKILL,			// Grade Skill ����. left = id

	UI_USE_XMAS_TREE,				// left = ItemPointer, void_ptr = message
	UI_CLOSE_XMAS_CARD_WINDOW,
	UI_SEND_BRING_FEE,				// left = money				to client
	UI_CLOSE_BRING_FEE_WINDOW,		// close window

	UI_CLOSE_WAR_LIST_WINDOW,

	UI_CLOSE_BLOOD_BIBLE_STATUS,

	UI_SEND_NAME_FOR_COUPLE,		// left = type(Ŀ�� ��û, ����) void_ptr = name
	UI_CLOSE_INPUT_NAME,
	
	UI_ITEM_USE_GEAR,							// Gear���� ���.	left : focus slot    void_ptr : item_ptr
	UI_GO_BILING_PAGE,						// ������������ ����!

	UI_CLOSE_POPUP_MESSAGE,
	UI_CLOSE_QUEST_STATUS,
	UI_CLOSE_LOTTERY_CARD,
	UI_LOTTERY_CARD_STATUS,				// left = step, right = GiftID
	UI_FINISH_SCRATCH_LOTTERY,
	
	UI_CLOSE_IMAGE_NOTICE,
	
	UI_SELECT_ITEM_FROM_SHOP,
	UI_CLOSE_ITEM_LIST,
	UI_CLOSE_BULLETIN_BOARD_WINDOW,
	
	UI_TRANS_ITEM_CANCEL,
	UI_TRANS_ITEM_ACCEPT,

	UI_FINISH_REQUEST_RESURRECT_BUTTON,		// ��Ȱ

	UI_USE_MIXING_FORGE,
	UI_CLOSE_MIXING_FORGE,

	UI_CLOSE_REMOVE_OPTION,
	UI_SEND_REMOVE_OPTION,

	UI_CLOSE_OUSTERS_SKILL_INFO,
	UI_OUSTERS_LEARN_SKILL,
	
	UI_RUN_LEVELUP,
	
	UI_CLOSE_HORN,
	UI_OUSTERS_DOWN_SKILL,
	
	UI_CLOSE_FINDING_MINE,
	UI_CLOSE_NEMONEMO,
	UI_CLOSE_PUSHPUSH,
	UI_CLOSE_CRAZY_MINE,
	UI_CLOSE_ARROW_TILE,
	UI_CLEAR_STAGE,					// left = (minigame|stage) right = time
	UI_CLEAR_ALL_STAGE,
	UI_REQUEST_FORCE_DIE,			// ������ �׾�!
	UI_ADD_ITEM_TO_CODE_SHEET,		// left = x , right = y, ptr = item
	
	UI_SEND_BUG_REPORT,				// left = line, ptr = string
	UI_GO_TO_BEGINNER_ZONE,

	UI_POPUP_MESSAGE_OK,			    // left = ID, right = selectedID
	UI_CLOSE_SHRINE_MINIMAP,		// left ?
	UI_WARP_REGEN_TOWER,			// left id num;

	UI_CLOSE_MAILBOX,
	
	UI_CLOSE_PET_INFO,
	UI_PET_GAMBLE,
	UI_CLOSE_USE_PET_FOOD,
	
	// 2004, 5, 11 sobeit add start - �� ������ ����
	UI_CLOSE_PETSTORAGE,
	UI_CLOSE_KEEP_PETITEM,
	UI_CLOSE_GET_KEEP_PETITEM,
	UI_KEEP_PETITEM,
	UI_GET_KEEP_PETITEM,
	// 2004, 5, 11 sobeit add end

	// 2004, 5, 27, sobeit add start - sms ����
	UI_CLOSE_SMS_MESSAGE,
	UI_SEND_SMS_MESSAGE,
	UI_SMS_OPEN_LIST,
	UI_SMS_RECORD,
	UI_SMS_DELETE,
	UI_CLOSE_SMS_LIST,
	UI_CLOSE_SMS_RECORD,
	UI_SMS_ADD_SEND_LIST,
	// 2004, 5, 27, sobeit add end - sms ����

	// 2004, 6, 15 sobeit add start- naming ����
	UI_CLOSE_NAMING,
	UI_CHANGE_CUSTOM_NAMING,
	UI_SELECT_NAMING,
	UI_CLOSE_NAMING_CHANGE,
	UI_RUN_NAMING_CHANGE,
	// 2004, 6, 15 sobeit add end
	
	// 2004, 7, 5, sobeit add start
	UI_CLOSE_QUEST_MANAGER,
	UI_CLOSE_QUEST_LIST,
	UI_CLOSE_QUEST_DETAIL,
	UI_CLOSE_QUEST_MISSION,
	UI_CLOSE_QUEST_ITEM,
	UI_CLOSE_QUEST_ICON,
	UI_GQUEST_ACCEPT,
	UI_GQUEST_GIVEUP,
	
	// 2004, 7, 5, sobeit add end

	UI_ITEM_USE_GQUEST_ITEM,

	UI_REQUEST_UNION,
	UI_REQUEST_UNION_ACCEPT,
	UI_REQUEST_UNION_DENY,
	UI_REQUEST_UNION_REQUEST_DENY,
	UI_REQUEST_UNION_REQUEST_INFO,
	UI_REQUEST_UNION_EXPERGUILD,
	UI_REQUEST_UNION_QUIT,
	UI_REQUEST_UNION_QUIT_ACCEPT,
	UI_REQUEST_UNION_QUIT_DENY,

	UI_REQUEST_UNION_REQUEST_GUILD_LIST,

	UI_RECALL_BY_NAME,
	
	UI_MODIFY_TAX,
	UI_APPOINT_SUBMASTER,
	
	UI_CLOSE_PERSNALSHOP,
	UI_SELECT_PERSNALSHOP_SLOT,
	UI_CLOSE_PERSNALSHOP_MESSAGE,
	UI_INPUT_PERSNALSHOP_MESSAGE,
	
	UI_DISPLAY_ITEM,                     // 465  ���λ����� ������ ����
    UI_UNDISPLAY_ITEM,                   // 466  ������ ������ ������
    UI_STORE_SIGN,                       // 467  ���λ��� ���� ���ڿ�
    UI_STORE_OPEN,                       // 468  ���λ��� ����
    UI_STORE_CLOSE,                      // 469  ���λ��� �ݱ�	
	UI_REQUEST_STORE_INFO,               // 470  ���� ���� ���� ��û	
	UI_MY_STORE_INFO,                    // 471  �ڱ� ���λ��� ���� �����ֱ�	
    UI_OTHER_STORE_INFO,                 // 472  �ٸ���� ���λ��� ���� �����ֱ�	
	
    UI_BUY_STORE_ITEM,                   // 473  �ٸ� ��� �������� ������ �����ϱ�	
    UI_REMOVE_STORE_ITEM,                // 474  �������� �������� ���ŵ�	
    UI_ADD_STORE_ITEM,                   // 475  ������ �������� �߰���	

	// 2004, 11, 30, sobeit add start
	UI_CLOSE_POWER_JJANG,
	UI_REQUEST_POWER_JJANG_POINT,
	UI_POWER_JJANG_GAMBLE,
	// 2004, 11, 30, sobeit add end
	
	UI_CLOSE_SWAPADVANCEMENTITEM,
	UI_SWAPADVANCEMENTITEM,
	UI_LEARN_ADVANCE_SKILL,

	UI_CAMPAIGN_HELP, // �ҿ��̿����� ���� �Է� ok

	UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT, // ���� ����Ʈ �޼��� �����ֱ� 
	
	UI_GQUEST_SET_ACTION,		// ����Ʈ �� Ư�� ������ ������� �� ��� 
	UI_GQUEST_ENDING_EVENT,		// ����Ʈ ����� ���� �̺�Ʈ�� ���� ���
	
	UI_REQUEST_EVENT_ITEM,		// �޸� ���� ������ �ޱ� �̺�Ʈ �� ������ ���� ��û�� �Ѵ�.

	UI_CLOSE_INVENTORY_SUB,		// sub inventory �� �ݴ´�.
	UI_ITEM_DROP_TO_INVENTORY_SUB, // sub inventory �� ������ ���
	UI_ITEM_PICKUP_FROM_INVENTORY_SUB,// sub inventory �� ������ �Ⱦ�
	UI_ITEM_INSERT_TO_INVENTORY_SUB,// sub inventory ������ �μ�Ʈ 20070330

	UI_ITEM_USE_SUBINVENTORY,
	UI_WEDDING_CONTRIBUTION , //���Ǳ� �Է� ok
	UI_NETMARBLE_AGREEMENT, // �ݸ��� ����� ��� ���� 
	UI_BLOOD_BURST,  // ���� ��ư ó��( 0 : ����  ,  1 : ��� , 2 : ��Ƽ )  
	UI_CLIENT_REMOVE_EFFECT_STATUS,
	UI_MARKET_ACCOUNT,   //( 0 : Market 1 : Bag  2 : HomePage) 
	UI_NONPK_SERVER_USE,
	
	UI_PLAYER_PETINFO,
	UI_FORCEATTACK_SET,//���� ���� ó��
// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
	UI_AUTOATTACK_SET,
#endif //__AUTO_ATTACT
// end
	UI_SCREENSHOT,//��ũ���� ���ɾ� ó��

	UI_UPDATE_AVAILABLE_SKILLS,			// ��� ������ ��ų�� ������Ʈ �Ѵ�.

	UI_PLAYER_ACTION_TO_SEND_PACKET,	// �÷��̾ ����� �׼��� ������ �����Ѵ�.
	
	UI_RELOAD_INFO_FILE,				// info ���� �ٽ� �б�

	UI_ITEM_USE_GEAR_CHANGE,			// Gear���� ���.	left : focus slot    void_ptr : item_ptr
	
	UI_TIPOYU_MOVE,

	UI_ITEM_SHOW_AND_CREATE_ITEM,		// ItemShow ���� ���
	UI_ITEM_SHOW_AND_CLEAR_ITEM,

	UI_SELECT_WEEKITEM,					// ���õ� �����̾� ������ ���� ��û
	UI_SELECT_ALL_WEEKITEM,				// �����̾� ������ ��� ���� ��û
	UI_FINISH_PREMIUM_GIVEITEM_BUTTON,	// �����̾� ���� â ���� ������ ������

	UI_REQUEST_REPAIRINFO,

	UI_USE_PET_MIXING_FORGE,			// �����ձ� ���
	UI_CLOSE_PET_MIXING_FORGE,			// �����ձ� �ݱ�

	UI_MOVE_ZONE,						// MoveZone ���� ���

	UI_DESTROY,
#if __CONTENTS(__FRIEND_ADDITION)
	UI_FRIEND_INFO_DATA,
	UI_FRIEND_SYSTEM_OPTION,
	UI_FRIEND_INFO_OPTION,
	UI_FRIEND_ADD,
	UI_FRIEND_RES_ADD,
	UI_FRIEND_DEL,
#endif //__FRIEND_ADDITION

	UI_REMOVE_ITEM,

	UI_REQUEST_TMALL_LIST,

	UI_REQUEST_PAY_EVENT,

	UI_PAY_GET_ITEM,

	UI_BUY_TMALL_ITEM,

	UI_REQUEST_MILITARY_SLOTDATA,

	UI_ITEM_DROP_TO_MILITARYSLOT,
	UI_ITEM_DELETE_FROM_MILITARYSLOT,
	UI_NEW_OPTION,

	// 2026-08-20 PayPal Cash Charge — TMall "Recharge" butonu bu mesaji yollar;
	// UIMessageManager CGCashChargeRequest (549) gonderir.
	UI_REQUEST_CASH_CHARGE,

	// 2026-09-02 O3 — Reborn Option: canli cozunurluk (left=w, right=h).
	// Kuyruklu dispatch pencere iterasyonu disinda calisir; handler
	// PhoenixResolutionLive zincirini + title/menu yeniden kurulumunu yapar.
	UI_APPLY_RESOLUTION,

	MAX_UI_MESSAGE
};

#endif
