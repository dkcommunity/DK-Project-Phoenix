//-----------------------------------------------------------------------------
// UIMessageManager.h
//-----------------------------------------------------------------------------

#ifndef __UIMESSAGEMANAGER_H__
#define __UIMESSAGEMANAGER_H__

#include <Windows.h>
#include "VS_UI_UIMessage.h"

class UIMessageManager {
	public :
		typedef	void (*UI_MESSAGE_FUNCTION)(int, int, void *);
		typedef std::vector<std::string> STRING_VEC;

	public :
		UIMessageManager();
		~UIMessageManager();

		//-----------------------------------------------------------
		// ÃÊ±âÈ­ - ÇÊ¼ö~~
		//-----------------------------------------------------------
		void			Init();

		//-----------------------------------------------------------
		// UI Message Ã³¸®
		//-----------------------------------------------------------
		void			Execute(DWORD message, int left, int right, void* void_ptr);

	protected :
		//-----------------------------------------------------------
		// ½ÇÁ¦·Î messageÃ³¸®ÇÏ´Â ÇÔ¼öµé
		//-----------------------------------------------------------
		static void	Execute_UI_NEW_CHARACTER(int left, int right, void* void_ptr);
		static void	Execute_UI_RUN_NEWUSER_REGISTRATION(int left, int right, void* void_ptr);
		static void	Execute_UI_CHECK_EXIST_ID(int left, int right, void* void_ptr);
		static void	Execute_UI_INFO_CLOSE(int left, int right, void* void_ptr);		
		static void	Execute_UI_DELETE_CHARACTER(int left, int right, void* void_ptr);
		static void	Execute_UI_TERMINATION(int left, int right, void* void_ptr);
		static void	Execute_UI_NEW_USER_REGISTRATION(int left, int right, void* void_ptr);
		static void	Execute_UI_LOGIN(int left, int right, void* void_ptr);
		static void	Execute_UI_CHARACTER_MANAGER_FINISHED(int left, int right, void* void_ptr);
		static void	Execute_UI_CONNECT(int left, int right, void* void_ptr);
		static void	Execute_UI_CHAT_RETURN(int left, int right, void* void_ptr);
		//static void	Execute_UI_CHAT_SELECT_NAME(int left, int right, void* void_ptr);
		static void	Execute_UI_LOGOUT(int left, int right, void* void_ptr);
	
	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
		static void	Execute_UI_QUITEXIT(int left, int right, void* void_ptr);
	#endif //__080405_FIREST_UI_UPDATE

		static void	Execute_UI_SELECT_SKILL(int left, int right, void* void_ptr);
		static void	Execute_UI_CANCEL_SELECT_SKILL(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_CLIENT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_INVENTORY(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_QUICKSLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_GEAR(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_MILITARYSLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DELETE_FROM_MILITARYSLOT(int left, int right, void* void_ptr);

		static void	Execute_UI_NEW_OPTION(int left, int right, void* void_ptr);
		

		static void	Execute_UI_ITEM_PICKUP_FROM_QUICKSLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_PICKUP_FROM_INVENTORY(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_PICKUP_FROM_GEAR(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_QUICKSLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_INSERT_FROM_INVENTORY(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_INSERT_FROM_GEAR(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_INSERT_FROM_QUICKSLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_SHOP(int left, int right, void* void_ptr);
		static void	Execute_UI_BUY_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_SELL_FINISHED(int left, int right, void* void_ptr);
		static void	Execute_UI_SELL_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_SELL_ALL_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_REMOVE_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr);
		static void	Execute_UI_BACKGROUND_MOUSE_FOCUS(int left, int right, void* void_ptr);
//		static void	Execute_UI_CLOSE_SKILL_VIEW(int left, int right, void* void_ptr);
//		static void	Execute_UI_LEARN_SLAYER_SKILL(int left, int right, void* void_ptr);
		//static void	Execute_UI_PDS_CLOSED(int left, int right, void* void_ptr);
		//static void	Execute_UI_PLEASE_SET_SLAYER_VALUE(int left, int right, void* void_ptr);
		//static void	Execute_UI_SEND_PCS_NUMBER(int left, int right, void* void_ptr);
		//static void	Execute_UI_END_PCS(int left, int right, void* void_ptr) ;
		//static void	Execute_UI_QUIT_PCS_ONLINE_MODE(int left, int right, void* void_ptr);
		//static void	Execute_UI_CHANGE_PCS_CONNECTED_SLOT(int left, int right, void* void_ptr);
		//static void	Execute_UI_PLEASE_PCS_CONNECT_ME(int left, int right, void* void_ptr);
		static void	Execute_UI_GAMEMENU_CONTINUE(int left, int right, void* void_ptr);
		//static void	Execute_UI_MINIMAP_TOGGLE(int left, int right, void* void_ptr);
		static void	Execute_UI_DROP_MONEY(int left, int right, void* void_ptr);
		static void	Execute_UI_CLICK_BONUS_POINT(int left, int right, void* void_ptr);
		static void	Execute_UI_INFO(int left, int right, void* void_ptr);
		static void	Execute_UI_REPAIR_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_SILVERING_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_REPAIR_FINISHED(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_SILVERING_FINISHED(int left, int right, void* void_ptr);
		static void	Execute_UI_FINISH_LEVELUP_BUTTON(int left, int right, void* void_ptr);
		static void	Execute_UI_RUNNING_GAMEMENU(int left, int right, void* void_ptr);
		static void	Execute_UI_STORAGE_BUY(int left, int right, void* void_ptr);
		static void	Execute_UI_SELECT_STORAGE_SLOT(int left, int right, void* void_ptr);
		static void	Execute_UI_DEPOSIT_MONEY(int left, int right, void* void_ptr);
		static void	Execute_UI_WITHDRAW_MONEY(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_STORAGE(int left, int right, void* void_ptr);
		static void	Execute_UI_EXCHANGE_REQUEST_CANCEL(int left, int right, void* void_ptr);
		static void	Execute_UI_EXCHANGE_ACCEPT(int left, int right, void* void_ptr);
		static void	Execute_UI_EXCHANGE_MONEY(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_SELECT_EXCHANGE(int left, int right, void* void_ptr);
		//static void	Execute_UI_ITEM_PICKUP_FROM_TRADEGRID(int left, int right, void* void_ptr);
		//static void	Execute_UI_ITEM_INSERT_FROM_TRADEGRID(int left, int right, void* void_ptr);
		//static void	Execute_UI_ITEM_DROP_TO_TRADEGRID(int left, int right, void* void_ptr);
		static void	Execute_UI_OK_EXCHANGE(int left, int right, void* void_ptr);
		static void	Execute_UI_CANCEL_EXCHANGE(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_EXCHANGE(int left, int right, void* void_ptr);
//		static void	Execute_UI_CHANGE_GAME_OPTION(int left, int right, void* void_ptr);
//		static void	Execute_UI_CLOSE_GAME_OPTION(int left, int right, void* void_ptr);
		static void	Execute_UI_CHANGE_OPTION(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_OPTION(int left, int right, void* void_ptr);
		static void	Execute_UI_APPLY_RESOLUTION(int left, int right, void* void_ptr);	// O3
		static void	Execute_UI_RESET_BUTTON(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_BOOKCASE(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_BRIEFING(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_COMPUTER(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_TUTORIAL_EXIT(int left, int right, void* void_ptr);
		//static void	Execute_UI_SELECT_EXPLOSIVE(int left, int right, void* void_ptr);
		//static void	Execute_UI_CLOSE_SELECT_EXPLOSIVE(int left, int right, void* void_ptr);

		static void	Execute_UI_CLOSE_DESC_DIALOG(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_ELEVATOR(int left, int right, void* void_ptr);
		static void	Execute_UI_SELECT_ELEVATOR(int left, int right, void* void_ptr);

		//static void	Execute_UI_SELECT_SERVER(int left, int right, void* void_ptr);
		//static void	Execute_UI_REQUEST_SERVER_LIST(int left, int right, void* void_ptr);

		static void	Execute_UI_ITEM_TO_QUICKITEMSLOT(int left, int right, void* void_ptr);
		
		static void	Execute_UI_CLOSE_SLAYER_PORTAL(int left, int right, void* void_ptr);
		static void	Execute_UI_SLAYER_PORTAL(int left, int right, void* void_ptr);

		static void	Execute_UI_FINISH_REQUEST_PARTY_BUTTON(int left, int right, void* void_ptr);
		static void	Execute_UI_PARTY_REQUEST_CANCEL(int left, int right, void* void_ptr);
		static void	Execute_UI_PARTY_ACCEPT(int left, int right, void* void_ptr);	
		static void	Execute_UI_CLOSE_PARTY_MANAGER(int left, int right, void* void_ptr);
		static void	Execute_UI_AWAY_PARTY(int left, int right, void* void_ptr);

		static void	Execute_UI_FINISH_REQUEST_DIE_BUTTON(int left, int right, void* void_ptr);

		static void	Execute_UI_CONNECT_SERVER(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_SERVER_SELECT(int left, int right, void* void_ptr);

		static void	Execute_UI_NEWCHARACTER_CHECK(int left, int right, void* void_ptr);

		static void Execute_UI_CLOSE_TEAM_LIST(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_TEAM_INFO(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_TEAM_MEMBER_INFO(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_TEAM_REGIST(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_TEAM_MEMBER_LIST(int left, int right, void *void_ptr);
	
		static void Execute_UI_REQUEST_GUILD_INFO(int left, int right, void *void_ptr);
		static void Execute_UI_REQUEST_GUILD_MEMBER_LIST(int left, int right, void *void_ptr);
		static void Execute_UI_MODIFY_GUILD_MEMBER(int left, int right, void *void_ptr);

		static void Execute_UI_SELECT_TEAM_MEMBER_LIST(int left, int right, void *void_ptr);		// void_ptr = MEMBER_NAME
		static void Execute_UI_SELECT_READY_TEAM_LIST(int left, int right, void *void_ptr);		// void_ptr = TEAM_NAME
		static void Execute_UI_SELECT_REGIST_TEAM_LIST(int left, int right, void *void_ptr);		// void_ptr = TEAM_NAME

		static void Execute_UI_JOIN_READY_TEAM(int left, int right, void *void_ptr);				// void_ptr = TEAM_NAME
		static void Execute_UI_JOIN_REGIST_TEAM(int left, int right, void *void_ptr);			// void_ptr = TEAM_NAME

		static void Execute_UI_REGIST_GUILD_MEMBER(int left, int right, void *void_ptr);			// void_ptr = introduction max:150byte 
		static void Execute_UI_REGIST_GUILD_TEAM(int left, int right, void *void_ptr);			// left = TEAM_NAME, void_ptr = introduction max:150byte 

		static void Execute_UI_CLOSE_FILE_DIALOG(int left, int right, void *void_ptr);

		static void	Execute_UI_ENCHANT_ACCEPT(int left, int right, void* void_ptr);
		static void	Execute_UI_ENCHANT_CANCEL(int left, int right, void* void_ptr);

		static void Execute_UI_MESSAGE_BOX(int left, int right, void* void_ptr);
		static void Execute_UI_CLOSE_OTHER_INFO(int left, int right, void* void_ptr);

		static void Execute_UI_MODIFY_TEAM_INFO(int left, int right, void* void_ptr);
		static void Execute_UI_MODIFY_GUILD_MEMBER_INTRO(int left, int right, void* void_ptr);

		static void Execute_UI_SEND_NAME_FOR_SOUL_CHAIN(int left, int right, void* void_ptr);
		static void Execute_UI_CLOSE_TRACE_WINDOW(int left, int right, void* void_ptr);

	
		static void Execute_UI_RUN_CONNECT(int left, int right, void* void_ptr);
		static void Excute_UI_SELECT_GRADE_SKILL(int left, int right, void* void_ptr);

		static void Excute_UI_USE_XMAS_TREE(int left, int right, void* void_ptr);				// left = to, right = from, void_ptr = message
		static void Excute_UI_CLOSE_XMAS_CARD_WINDOW(int left, int right, void* void_ptr);


		// 2003.1.21
		static void	Excute_UI_SEND_BRING_FEE(int left,int right, void * void_ptr);
		static void Excute_UI_CLOSE_BRING_FEE_WINDOW(int left,int right, void * void_ptr);

		// 2003.1.24
		static void Excute_UI_CLOSE_WAR_LIST(int left, int right, void *void_ptr);

		static void	Execute_UI_CLOSE_BLOOD_BIBLE_STATUS(int left, int right, void *void_ptr);
		static void	Execute_UI_SEND_NAME_FOR_COUPLE(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_INPUT_NAME_WINDOW(int left, int right, void *void_ptr);

		static void Execute_UI_ITEM_USE_GEAR(int left, int right, void *void_ptr);
		static void Execute_GO_BILING_PAGE(int left, int right, void *void_ptr);
		
		static void Execute_UI_CLOSE_POPUP_MESSAGE(int left, int right, void *void_ptr);
		static void	Execute_UI_CLOSE_QUEST_STATUS( int left, int right, void *void_ptr);

		static void Execute_UI_CLOSE_LOTTERY_CARD(int left, int right, void *void_ptr);
		static void Execute_UI_LOTTERY_CARD_STATUS(int left, int right, void *void_ptr);
		static void Execute_UI_FINISH_SCRATCH_LOTTERY(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_IMAGE_NOTICE(int left,int right, void *void_ptr);
		static void Execute_UI_SELECT_ITEM_FROM_SHOP(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_ITEM_LIST(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_BULLETIN_BOARD(int left, int right, void *void_ptr);
		static void	Execute_UI_TRANS_ITEM_CANCEL(int left, int right, void *void_ptr);
		static void Execute_UI_TRANS_ITEM_ACCEPT(int left, int right, void *void_ptr);
		static void Execute_UI_REQUEST_RESURRECT(int left, int right, void *void_ptr);

		static void Execute_UI_CLOSE_MIXING_FORGE(int left, int right, void *void_ptr);
		static void Execute_UI_MIXING_FORGE(int left, int right, void* void_ptr);
		
		static void	Execute_UI_CLOSE_REMOVE_OPTION(int left, int right, void *void_ptr);
		static void Execute_UI_SEND_REMOVE_OPTION(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_OUSTERS_SKILL_INFO(int left, int right, void *void_ptr);
		static void Execute_UI_LEARN_OUSTERS_SKILL(int left, int right, void *void_ptr);

		static void Execute_UI_RUN_LEVELUP(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_HORN(int left, int right, void *void_ptr);
		static void Execute_UI_OUSTERS_DOWN_SKILL(int left, int right, void *void_ptr);
		static void Execute_UI_CLEAR_ALL_STAGE(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_FINDING_MINE(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_NEMONEMO(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_PUSHPUSH(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_CRAZY_MINE(int left, int right, void *void_ptr);
		static void Execute_UI_CLOSE_ARROW_TILE(int left, int right, void *void_ptr);
		static void Execute_UI_CLEAR_STAGE(int left, int right, void *void_ptr);
		static void Execute_UI_FORCE_DIE(int left, int right, void *void_ptr);
		static void Execute_UI_ADD_ITEM_TO_CODE_SHEET(int left, int right, void *void_ptr);
		
		static void Execute_UI_SEND_BUG_REPORT(int left,int right, void *void_ptr);
		static void Execute_UI_GO_BEGINNER_ZONE(int left, int right, void* void_ptr);
		static void Execute_UI_POPUP_MESSAGE_OK(int left, int right, void* void_ptr);
		static void Execute_UI_CLOSE_SHRINE_MINIMAP(int left, int right, void* void_ptr);
		static void Execute_UI_WARP_TO_REGEN_TOWER(int left, int right, void* void_ptr);
		static void Execute_UI_CLOSE_MAILBOX(int left, int right, void* void_ptr);
		static void	Execute_UI_CLOSE_PET_INFO(int left, int right, void* void_ptr);		
		static void	Execute_UI_PET_GAMBLE(int left, int right, void* void_ptr);		
		static void	Execute_UI_CLOSE_USE_PET_FOOD(int left, int right, void* void_ptr);	
		
		static void	 Execute_UI_CLOSE_PETSTORAGE(int left, int right, void* void_ptr);	
		static void	 Execute_UI_CLOSE_KEEP_PETITEM(int left, int right, void* void_ptr);	
		static void	 Execute_UI_CLOSE_GET_KEEP_PETITEM(int left, int right, void* void_ptr);
		static void	 Execute_UI_KEEP_PETITEM(int left, int right, void* void_ptr);	
		static void	 Execute_UI_GET_KEEP_PETITEM(int left, int right, void* void_ptr);

		static void	 Execute_UI_CLOSE_SMS_MESSAGE(int left, int right, void* void_ptr);
		static void	 Execute_UI_SEND_SMS_MESSAGE(int left, int right, void* void_ptr);
		static void	 Execute_UI_SMS_OPEN_LIST(int left, int right, void* void_ptr);
		static void	 Execute_UI_SMS_RECORD(int left, int right, void* void_ptr);
		static void	 Execute_UI_SMS_DELETE(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_SMS_LIST(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_SMS_RECORD(int left, int right, void* void_ptr);
		static void	 Execute_UI_SMS_ADD_SEND_LIST(int left, int right, void* void_ptr);

		static void	 Execute_UI_CLOSE_NAMING(int left, int right, void* void_ptr);
		static void	 Execute_UI_CHANGE_CUSTOM_NAMING(int left, int right, void* void_ptr);
		static void	 Execute_UI_SELECT_NAMING(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_NAMING_CHANGE(int left, int right, void* void_ptr);
		static void	 Execute_UI_RUN_NAMING_CHANGE(int left, int right, void* void_ptr);

		static void	 Execute_UI_CLOSE_QUEST_MANAGER(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_QUEST_LIST(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_QUEST_DETAIL(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_QUEST_MISSION(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_QUEST_ITEM(int left, int right, void* void_ptr);
		static void	 Execute_UI_CLOSE_QUEST_ICON(int left, int right, void* void_ptr);
		static void	 Execute_UI_GQUEST_ACCEPT(int left, int right, void* void_ptr);
		static void	 Execute_UI_GQUEST_GIVEUP(int left, int right, void* void_ptr);
		
		static void	 Execute_UI_ITEM_USE_GQUEST_ITEM(int left, int right, void* void_ptr);

		static void	Execute_UI_ITEM_USE_REQUEST_GUILD_LIST(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_UNION_INFO(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_REQUEST_UNION(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_QUIT(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_EXPER(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_UNION_ACCEPT(int left, int right, void* void_ptr);	
		static void	Execute_UI_ITEM_USE_UNION_DENY(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_USE_UNION_QUIT_ACCEPT(int left, int right, void* void_ptr);		
		static void	Execute_UI_ITEM_USE_UNION_QUIT_DENY(int left, int right, void* void_ptr);

		static void	Execute_UI_REQUEST_TMALL_LIST(int left, int right, void* void_ptr);

		// 2026-08-20 PayPal Cash Charge
		static void	Execute_UI_REQUEST_CASH_CHARGE(int left, int right, void* void_ptr);

		static void	Execute_UI_REQUEST_PAY_EVENT(int left, int right, void* void_ptr);

		static void	Execute_UI_PAY_GET_ITEM(int left, int right, void* void_ptr); 


		static void Execute_UI_BUY_TMALL_ITEM(int left, int right, void* void_ptr);

		static void Execute_UI_REQUEST_MILITARY_SLOTDATA(int left, int right, void* void_ptr);

		static void	 Execute_UI_RECALL_BY_NAME(int left, int right, void* void_ptr);

		static void	 Execute_UI_UI_MODIFY_TAX(int left, int right, void* void_ptr);

		static void	 Execute_UI_APPOINT_SUBMASTER(int left, int right, void* void_ptr);

		static void	Execute_UI_DISPLAY_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_UNDISPLAY_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_STORE_SIGN(int left, int right, void* void_ptr);
		static void	Execute_UI_STORE_OPEN(int left, int right, void* void_ptr);
		static void	Execute_UI_STORE_CLOSE(int left, int right, void* void_ptr);
		static void	Execute_UI_REQUEST_STORE_INFO(int left, int right, void* void_ptr);
		static void	Execute_UI_MY_STORE_INFO(int left, int right, void* void_ptr);
		static void	Execute_UI_OTHER_STORE_INFO(int left, int right, void* void_ptr);
		static void	Execute_UI_BUY_STORE_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_REMOVE_STORE_ITEM(int left, int right, void* void_ptr);
		static void	Execute_UI_ADD_STORE_ITEM(int left, int right, void* void_ptr);		
		static void Execute_UI_CLOSE_PERSNALSHOP(int left, int right, void* void_ptr);		


		static void	 Execute_UI_CLOSE_POWER_JJANG(int left, int right, void* void_ptr);
		static void	 Execute_UI_REQUEST_POWER_JJANG_POINT(int left, int right, void* void_ptr);
		static void	 Execute_UI_POWER_JJANG_GAMBLE(int left, int right, void* void_ptr);

		static void	 Execute_UI_CLOSE_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr);
		static void	 Execute_UI_SWAPADVANCEMENTITEM(int left, int right, void* void_ptr);
		//2005.1.5
		static void	 Execute_UI_LEARN_ADVANCE_SKILL(int left, int right, void* void_ptr);	
		
		static void	 Execute_UI_CAMPAIGN_HELP(int left, int right, void* void_ptr);
		static void	Execute_UI_RUN_NEXT_GQUEST_EXCUTE_ELEMENT(int left, int right, void* void_ptr);

		static void	Execute_UI_GQUEST_SET_ACTION(int left, int right, void* void_ptr);
		static void	Execute_UI_GQUEST_ENDING_EVENT(int left, int right, void* void_ptr);

		static void	Execute_UI_REQUEST_EVENT_ITEM(int left, int right, void* void_ptr);

		static void	Execute_UI_CLOSE_INVENTORY_SUB(int left, int right, void* void_ptr);
		static void	Execute_UI_ITEM_DROP_TO_INVENTORY_SUB(int left, int right, void* void_ptr);
		static void Execute_UI_ITEM_INSERT_TO_INVENTORY_SUB(int left, int right, void* void_ptr);//20070330 by diesirace
		static void	Execute_UI_ITEM_PICKUP_FROM_INVENTORY_SUB(int left, int right, void* void_ptr);

		static void	Execute_UI_ITEM_USE_SUBINVENTORY(int left, int right, void* void_ptr);
		static void	Execute_UI_WEDDING_CONTRIBUTION(int left, int right, void* void_ptr);
		
		static void Execute_UI_NETMARBLE_AGREEMENT( int left, int right, void* void_ptr );
		static void Execute_UI_BLOOD_BURST( int left, int right, void* void_ptr );	
		static void Execute_UI_CLIENT_REMOVE_EFFECT_STATUS( int left, int right, void* void_ptr );	
		static void Execute_UI_MARKET_ACCOUNT( int left, int right, void* void_ptr );	
		
		static void Execute_UI_NONPK_SERVER_USE( int left, int right, void* void_ptr );	

		static void Execute_UI_PLAYER_PETINFO( int left, int right, void* void_ptr );	
		static void Execute_UI_FORCEATTACK_SET( int left, int right, void* void_ptr );
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
		static void Execute_UI_AUTOATTACK_SET( int left, int right, void* void_ptr );
#endif //__AUTO_ATTACT
		static void Execute_UI_SCREENSHOT( int left, int right, void* void_ptr );

		static void Execute_UI_UPDATE_AVAILABLE_SKILLS( int left, int right, void* void_ptr );

		static void Execute_UI_PLAYER_ACTION_TO_SEND_PACKET( int left, int right, void* void_ptr );	

		static void Execute_UI_RELOAD_INFO_FILE( int left, int right, void* void_ptr );
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//UIMessageManager		 ÀåÂøÃ¢À» ±³Ã¼ ÇÒ¶§ UIÀÇ ¸Þ½ÃÁö ÇÔ¼ö¸¦ ¸¸µç´Ù.
		static void Execute_UI_GEAR_CHANGE_SEND_PACKET(int left, int right, void* void_ptr);		
#endif	//__GEAR_SWAP_CHANGE

#if __CONTENTS(__TIPOJYU_CASTLE)		//UIMessageManager		 ÀåÂøÃ¢À» ±³Ã¼ ÇÒ¶§ UIÀÇ ¸Þ½ÃÁö ÇÔ¼ö¸¦ ¸¸µç´Ù.
		static void Execute_UI_TIPOJYU_MOVE_SEND_PACKET(int left, int right, void* void_ptr);		
#endif	//__TIPOJYU_CASTLE
		
#if __CONTENTS(__ITEM_SHOW)				//¾ÆÀÌÅÛ »ý¼º ½Ã½ºÅÛ
		static void Execute_UI_ITEM_SHOW_AND_CREATE_ITEM(int left, int right, void* void_ptr);
		static void Execute_UI_ITEM_SHOW_AND_CLEAR_ITEM(int left, int right, void* void_ptr);
#endif	//__ITEM_SHOW

#if __CONTENTS(__MOVE_ZONE)
		static void Execute_UI_MOVE_ZONE(int left, int right, void* void_ptr);
#endif //__MOVE_ZONE

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)	//¼±ÅÃµÈ ÇÁ¸®¹Ì¾ö ¾ÆÀÌÅÛ Áö±Þ ¿äÃ»
		static void Execute_UI_SELECT_WEEKITEM(int left, int right, void* void_ptr);
		static void Execute_UI_SELECT_ALL_WEEKITEM(int left, int right, void* void_ptr);
		static void Execute_UI_FINISH_PREMIUM_GIVEITEM_BUTTON(int left, int right, void* void_ptr);
#endif	//__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)	//¼±ÅÃµÈ ÇÁ¸®¹Ì¾ö ¾ÆÀÌÅÛ Áö±Þ ¿äÃ»
		static void Execute_UI_REQUEST_REPAIRINFO(int left, int right, void* void_ptr);
#endif	//__REPAIRSYSTEM_RENEWAL
#if __CONTENTS(__PET_MIXINGFORGE)
		static void Execute_UI_USE_PET_MIXING_FORGE(int left, int right, void* void_ptr);
		static void Execute_UI_CLOSE_PET_MIXING_FORGE(int left, int right, void* void_ptr);
#endif //__PET_MIXINGFORGE
		static void Execute_UI_DESTROY(int left, int right, void* void_ptr);
#if __CONTENTS(__FRIEND_ADDITION)
		static void Execute_UI_FRIEND_INFO_DATA(int left, int right, void* void_ptr);
		static void Execute_UI_FRIEND_SYSTEM_OPTION(int left, int right, void* void_ptr);
		static void Execute_UI_FRIEND_INFO_OPTION(int left, int right, void* void_ptr);
		static void Execute_UI_FRIEND_ADD(int left, int right, void* void_ptr);
		static void Execute_UI_FRIEND_DEL(int left, int right, void* void_ptr);
		static void Execute_UI_FRIEND_RES_ADD(int left, int right, void* void_ptr);
#endif //__FRIEND_ADDITION

		static void Execute_UI_REMOVE_ITEM(int left, int right, void* void_ptr);

		
	protected :
		UI_MESSAGE_FUNCTION				m_UIMessageFunction[MAX_UI_MESSAGE];
		static void ChatCommand_Where(STRING_VEC& ChatCommandStr, LPCTSTR pOrgMsg, int right);
		static void ChatCommand_Whisper(STRING_VEC& ChatCommandStr, LPCTSTR pOrgMsg, int right);
	
};

extern UIMessageManager*	g_pUIMessageManager;

#endif

