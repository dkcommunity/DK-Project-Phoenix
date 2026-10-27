#include "VariableManager.h"
#include "DB.h"
#include "Properties.h"
#include "SystemAvailabilitiesManager.h"
#include <stdlib.h>

extern void strlwr(char* str);

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
VariableManager::VariableManager()
	throw(Error)
{
	__BEGIN_TRY

	m_Variables.resize(VARIABLE_MAX);

	m_Variables[STAR_RATIO] = 1000;
	m_Variables[EVENT_ACTIVE] = 1;
	m_Variables[EVENT_RATIO] = 1000;
    m_Variables[ITEM_PROBE_RATIO] = 1;
	m_Variables[EXP_RATIO] = 1;
	m_Variables[COMBAT_BONUS_TIME] = 30;	// 30분
	m_Variables[COMBAT_SLAYER_HP_BONUS_RATIO] = 0;
	m_Variables[COMBAT_VAMPIRE_HP_BONUS_RATIO] = 0;
	m_Variables[COMBAT_SLAYER_DAMAGE_BONUS]= 0;
	m_Variables[COMBAT_VAMPIRE_DAMAGE_BONUS]= 0;
	m_Variables[PREMIUM_EXP_BONUS_PERCENT] = 150; // 150%
	m_Variables[PREMIUM_ITEM_PROBE_PERCENT] = 220; // 220%
	m_Variables[ZONE_GROUP_BALANCING_MINUTE] = 4*7*24*60; // 4주일
	m_Variables[GAMBLE_ITEM_TYPE_RATIO] = 100;	// 100%
	m_Variables[GAMBLE_ITEM_OPTION_RATIO] = 100;	// 100%
	m_Variables[SUMMON_MOTORCYCLE] = 0;
	m_Variables[ENEMY_LIMIT_TIME] = 300; // 5분 
	m_Variables[RARE_ITEM_RATIO] = 100; // 100%
	m_Variables[UNIQUE_ITEM_RATIO] = 100; // 100%
	m_Variables[ACTIVE_MASTER_LAIR] = 0;
	m_Variables[RETRY_MASTER_LAIR] = 0;
	m_Variables[HARVEST_FESTIVAL_ITEM_RATIO] = 100;	// 100%

	// [마스터 흡혈 방식]
	// HP가 70% 이상일때는 흡혈하지 않고
	// HP가 70%일때 10%의 확률로 흡혈하고
	//      50%     20%
	// HP가 30%일때 30%의 확률로 흡혈한다.
	// HP가 30%이하일때는 30%의 확률로 흡혈한다.
	m_Variables[MASTER_BLOOD_DRAIN_START_HP] 	= 70;
	m_Variables[MASTER_BLOOD_DRAIN_START_BD] 	= 10;
	m_Variables[MASTER_BLOOD_DRAIN_END_HP] 	= 30;
	m_Variables[MASTER_BLOOD_DRAIN_END_BD] 	= 30;

	m_Variables[ACTIVE_CHIEF_MONSTER] = 0;
	m_Variables[CHIEF_MONSTER_RARE_ITEM_RATIO] = 0;

	m_Variables[NEWBIE_TRANSPORT_TO_GUILD] = 1;

	// 2002.11.19
	m_Variables[FREE_PLAY_SLAYER_DOMAIN_SUM] = 30;
	m_Variables[FREE_PLAY_VAMPIRE_LEVEL] = 30;

	// 2002.12.10
	m_Variables[LOG_INCOMING_CONNECTION] = false;

	// 2002.12.11
	m_Variables[CHRISTMAS_FIRE_CRACKER_RATIO] = 0;
	m_Variables[CHRISTMAS_TREE_PART_RATIO] = 0;

	// 2002.12.12
	m_Variables[CHRISTMAS_GIFT_BOX_RATIO] = 0;

	// 2002.12.17
	m_Variables[CHRISTMAS_TREE_DECAY_TIME] = 6*60*60*10;

	// 2002.12.31
	m_Variables[MASTER_LAIR_PLAYER_NUM] = 15;
	m_Variables[RANK_EXP_GAIN_PERCENT] 	= 100;	// 100이면 원래 그대로
	m_Variables[ITEM_LUCK_PERCENT] 		= 100;	// 200이면 두배(200%)라는 의미다.

	// 대박 이벤트 용 ( code name : lotto event )
	// 2003. 1. 12 by bezz. Sequoia. dew
	m_Variables[LOTTO_ITEM_RATIO]		= 0;
	m_Variables[LOTTO_ITEM_BONUS_NUM]	= 0;
	m_Variables[LOTTO_SKULL_RATIO]		= 0;
	m_Variables[PREMIUM_HALF_EVENT]		= false;

	// 2003. 1. 20 by bezz, Sequoia
	m_Variables[COMMON_CASTLE_ENTRANCE_FEE]	= 1000;
	m_Variables[GUILD_CASTLE_ENTRANCE_FEE] = 10000;

	m_Variables[COMMON_CASTLE_ITEM_TAX_RATIO] = 110;
	m_Variables[GUILD_CASTLE_ITEM_TAX_RATIO] = 150;
	m_Variables[WAR_REGISTRATION_FEE] 	= 50000000;

	m_Variables[GUILD_WAR_TIME] 	= 1*60*60;	// 1시간
	m_Variables[RACE_WAR_TIME] 		= 3*60*60;	// 3시간

	m_Variables[WAR_PERIOD_WEEK] 	= true;		// 전쟁은 한 주 단위의 주기를 가진다.

	m_Variables[WAR_ACTIVE] 		= false;	// 전쟁 기능이 작동하지 않는다.

	m_Variables[RACE_WAR_TIMEBAND]	= 2;		// 종족 전쟁시 시간

	m_Variables[AUTO_START_RACE_WAR]= false;	// 종족 전쟁을 자동으로 시작하는가?

	m_Variables[ACTIVATE_COUPLE] = false;	// 커플이 성사될 수 있는 기간인가?

	m_Variables[ACTIVE_RACE_WAR_LIMITER] = true;	// 종족 전쟁 참가 인원 제한

	m_Variables[CHIEF_ITEM_BONUS_NUM] = 0;		// 치프 몬스터 추가 아이템 개수

	m_Variables[ACTIVE_EVENT_GIVE_ITEM] = false;		// 아이템 주는 이벤트

	m_Variables[VAMPIRE_REDISTRIBUTE_ATTR_PRICE] = 1000000;	// 능력치 재분배 비용

	m_Variables[ACTIVATE_COUPLE] = true;		// 커플 기능

	m_Variables[CAN_RECOUPLE] = false;			// 재혼(?) 이라기보단 다시 커플이 될 수 있는가

	m_Variables[GUILD_WAR_ACTIVE] = false;		// 길드전을 하는가?

	m_Variables[FREE_PLAY_OUSTERS_LEVEL] = 30;	// 아우스터스로 무료로 플레이할 수 있는 레벨

	m_Variables[CAN_APPLY_QUEST] = true;	// 퀘스트를 신청할 수 있는가
	m_Variables[SEND_QUEST_INFO] = false;	// 퀘스트 정보를 보내줘야 하는가
	m_Variables[CAN_BUY_SHOP] = true;

	m_Variables[PCROOM_FREE_EXP_BONUS] = 120;		// 무료 PC방 추가 경험치 배율
	m_Variables[PCROOM_ITEM_RATIO_BONUS] = 100;

	m_Variables[HOME_EXP_BONUS] = 100;				// 미등록 IP 추가 경험치 배율
	m_Variables[PCROOM_CHARGE_EXP_BONUS] = 150;		// 유료 PC방 추가 경험치 배율

	m_Variables[MONEY_TRACE_LOG_LIMIT] = 5000000;

	m_Variables[PC_ROOM_LOTTO_EVENT] = false;	// 피시방 복권 이벤트 여부
	m_Variables[ACTIVE_FLAG_WAR] = false;	// 깃발뺏기 실행 여부

	m_Variables[KILL_DAEMONCTL] = false;	// daemonctl2 죽이는 거 여부
	m_Variables[EVENT_MOON_CARD] = false;	// daemonctl2 죽이는 거 여부

	m_Variables[FULL_MOON_CARD_RATIO] = 3000;
	m_Variables[HALF_MOON_CARD_RATIO] = 100;
	m_Variables[NEW_MOON_CARD_RATIO] = 200;
	m_Variables[OLD_MOON_CARD_RATIO] = 1000;

	m_Variables[ACTIVE_LEVEL_WAR] = 1000;
	m_Variables[RACE_WAR_HP_BONUS] = 0;
	m_Variables[HEAD_COUNT] = 0;

	m_Variables[HEAD_PRICE_BONUS] = 100;

	m_Variables[LEVEL_WAR_ZONE_FREE_ENTER] = false;
	m_Variables[PET_EXP_RATIO] = 100;

	m_Variables[EVENT_LUCKY_BAG] = false;
	m_Variables[GREEN_LUCKY_BAG_RATIO] = 150;
	m_Variables[BLUE_LUCKY_BAG_RATIO] = 1000;
	m_Variables[GOLD_LUCKY_BAG_RATIO] = 5000;
	m_Variables[RED_LUCKY_BAG_RATIO] = 20000;

	m_Variables[EVENT_GIFT_BOX] = false;
	m_Variables[RED_GIFT_BOX_RATIO] = 10000;
	m_Variables[BLUE_GIFT_BOX_RATIO] = 10000;
	m_Variables[GREEN_GIFT_BOX_RATIO] = 10000;
	m_Variables[YELLOW_GIFT_BOX_RATIO] = 10000;

	m_Variables[NETMARBLE_CARD_RATIO] = 0;
	m_Variables[NETMARBLE_CARD_EVENT] = 0;

	m_Variables[PET_FOOD_EVENT] = 0;

	m_Variables[HIGHER_PET_FOOD_RATIO] = 30;

	m_Variables[PET_FOOD_RATIO] = 125;
	m_Variables[RACE_PET_FOOD_RATIO] = 30;

	m_Variables[SLAYER_HP_RATIO] = 100;
	m_Variables[VAMPIRE_HP_RATIO] = 100;
	m_Variables[OUSTERS_HP_RATIO] = 100;
	m_Variables[MONSTER_HP_RATIO] = 100;
	m_Variables[MONSTER_EXP_RATIO] = 100;

	m_Variables[CLOVER_EVENT] = 0;
	m_Variables[CLOVER_RATIO] = 0;

	m_Variables[MONSTER_DAMAGE_RATIO] = 100;
	m_Variables[RACE_PET_QUEST] = 0;

	m_Variables[NICKNAME_PEN_EVENT] = 0;
	m_Variables[NICKNAME_PEN_RATIO] = 0;
	m_Variables[GOLD_MEDAL_RATIO] = 0;
	m_Variables[PET_DAY_EVENT] = 0;

	m_Variables[OLYMPIC_ITEM_RATIO] = 0;
	m_Variables[OLYMPIC_EVENT] = 0;

	m_Variables[CROWN_PRICE] = 0;
	m_Variables[CHOBO_EVENT] = 0;

	m_Variables[QUIT_GUILD_PENALTY_TERM] = 0;
	m_Variables[GUILD_UNION_MAX] = 0;

	m_Variables[LUCK_CHARM_RATIO] = 0;
	
	m_Variables[GDR_LAIR_PC_LIMIT] = 36;
	m_Variables[GDR_LAIR_REWARD_MAGNIFICATION] = 1;

	m_Variables[DONATION_EVENT_200501] = 0;

	m_Variables[EVENT_NEW_YEAR_2005] = 0;
	m_Variables[BOKJORY_RATIO] = 80000;
	m_Variables[EVENT_RED_RICE_CAKE_SOUP_RATIO] = 1000;
	m_Variables[EVENT_GREEN_RICE_CAKE_SOUP_RATIO] = 1000;
	m_Variables[EVENT_BLUE_RICE_CAKE_SOUP_RATIO] = 1000;
	m_Variables[EVENT_BLACK_RICE_CAKE_SOUP_RATIO] = 80000;
	m_Variables[EVENT_MUGWORT_RICE_CAKE_SOUP_RATIO] = 80000;

	m_Variables[TIME_PERIOD_EXP_2X] = 0;
	m_Variables[COUNT_KILL_MONSTER] = 0;
	m_Variables[DONATION_EVENT_200505] = 0;
	m_Variables[FAMILY_COIN_EVENT] = 0;
	m_Variables[FAMILY_COIN_RATIO] = 200;
	m_Variables[BALLOON_HEADBAND_EVENT] = 0;
	m_Variables[BALLOON_HEADBAND_RATIO] = 0;
	m_Variables[FULL_PARTY_EXP_EVENT] = 0;			// 142

	m_Variables[EVENT_FALL_2005] = 0;
	m_Variables[EVENT_FALL_2005_DEFENSE_RATIO] = 0;
	m_Variables[EVENT_FALL_2005_PROTECTION_RATIO] = 0;
	m_Variables[EVENT_FALL_2005_ATTACK_SPEED_RATIO] = 0;
	m_Variables[EVENT_FALL_2005_DAMAGE_RATIO] = 0;
	m_Variables[EVENT_FALL_2005_LUCK_RATIO] = 0;
	m_Variables[EVENT_FALL_2005_VISION_RATIO] = 0;

	m_Variables[YELLOW_CANDY_RATIO] = 0;

	m_Variables[TIME_PERIOD_MEGAPHONE_RATIO] = 0;
	m_Variables[TIME_PERIOD_TRANSLATOR_RATIO] = 0;
	m_Variables[TIME_PERIOD_TRANSKIT_RATIO] = 0;
	m_Variables[TIME_PERIOD_DRAGON_RATIO] = 0;
	m_Variables[TIME_PERIOD_STORM_RATIO] = 0;
	m_Variables[TIME_PERIOD_WIDE_RATIO] = 0;
	m_Variables[TIME_PERIOD_RUNNING_RATIO] = 0;

	m_Variables[SLAYER_EXP_BONUS_PERCENT] = 0;

	m_Variables[GREEN_HANDSEL_RATIO] = 0;

	m_Variables[PROLOGUE_QUEST_ITEM_RATIO] = 0;

	m_Variables[EVENT_COMEBACK_200604] = 0;
	m_Variables[EVENT_LEVELUP_200604] = 0;

	m_Variables[EVENT_SOCCER_200606_RATIO] = 1;

	m_Variables[EVENT_RICE_CAKE_RATIO] = 0;
	m_Variables[EVENT_RICE_CAKE] = false;

	m_Variables[EVENT_SNOW_MAN_PART_RATIO] = 0;
	m_Variables[EVENT_BOX_ITEM_200612] = false;

	m_Variables[MIKLLIZZ_MAX_PLAYER] = 20;
	m_Variables[NEWBIE_ITEM_EVENT] = false;

	m_Variables[WRITE_EXP_LOG] = false;

	m_Variables[FALLENLEAVES_PART_RATIO] = 500;			//080926 - smallheart
	m_Variables[SAVE_CONTRIBUTE_REWARDCOUNT] = false;	//080926 - smallheart
	
	m_Variables[RESTRICTION_MOTORCYCLE] = false;		//20081002 kim seong yong
	m_Variables[RESTRICTION_GEARSLOT] = false;		//20081007 kim seong yong
	m_Variables[RESTRICTION_WEAR] = false;                  //20081008 kim seong yong
	m_Variables[RESTRICTION_CONNECT_COUPLE] = true;
	
	m_Variables[GIVE_WEEKITEM] = false;
	m_Variables[OPEN_PAYZONE] = false;	//081120 wlzzi
		
	m_Variables[JACKO_LANTERN_EVENT] = 0;
	m_Variables[JACKO_LANTERN_RATIO] = 0;
	
	m_Variables[MENEGROTH_VOUCHER_COUNT] = 1;

	m_Variables[SKILLSPEED_ENABLE_VERIFICATION] = false;
	m_Variables[SKILLSPEED_MAX_ABUSE_ACTION_POINT] = 300;
	m_Variables[SKILLSPEED_SHOW_ABUSE_MESSAGE] = false;
	m_Variables[SKILLSPEED_DISCONNECT_ABUSE_ACTION] = false;
	m_Variables[SKILLSPEED_NETWORK_LATENCY_TIME] = 20;
	// by svi
	m_Variables[DOUBLE_LUCKY_EVENT] = 0;     
	m_Variables[HOURGLASS_DROP_RATIO] = 0 ;
	m_Variables[ALWAYS_DOUBLE_MONSTER] = 0 ;
	m_Variables[FLAGWAR_REWARD_EVENT] = 0 ;
	// AOS Battle is explicitly gated by AttrInfo MOBA_ENABLE.
	m_Variables[MOBA_ENABLE] = 0 ;
	m_Variables[MOBA_REWARD_GOODS_ID] = 450 ;
	resetMobaPlayConfig();

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
	
	m_fRemoveAllGame = false;
	m_fEggDummyDB    = false;

#endif
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
VariableManager::~VariableManager() 
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void VariableManager::init()
	throw(Error)
{
	__BEGIN_TRY

	m_VariableNames.clear();

	for (int i=0; i<VARIABLE_MAX; i++)
	{
		char str[256];
		strcpy(str, VariableType2String[i].c_str());
		strlwr(str);
		m_VariableNames[str] = (VariableType)i;
	}

	load();

	// 특별히 다른 초기값을 설정하는 경우
	setVariable( AUTO_START_RACE_WAR, g_pConfig->getPropertyInt("ActiveRaceWar") );
	setVariable( GUILD_WAR_ACTIVE, g_pConfig->getPropertyInt("ActiveGuildWar") );
	setVariable( ACTIVE_FLAG_WAR, g_pConfig->getPropertyInt("ActiveFlagWar") );
	setVariable( ACTIVE_LEVEL_WAR, g_pConfig->getPropertyInt("ActiveLevelWar") );
	setVariable( HEAD_COUNT, g_pConfig->getPropertyInt("HeadCount") );
	setVariable( KILL_DAEMONCTL, 0 );

	if ( !SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_HOLY_LAND_WAR ) )
		setVariable( WAR_ACTIVE, 0 );

	if ( !SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_GUILD_WAR ) )
		setVariable( GUILD_WAR_ACTIVE, 0 );

	if ( !SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_RACE_WAR ) )
		setVariable( AUTO_START_RACE_WAR, 0 );

	if ( !SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_FLAG_WAR ) )
		setVariable( ACTIVE_FLAG_WAR, 0 );

	if ( !SystemAvailabilitiesManager::getInstance()->isAvailable( SystemAvailabilitiesManager::SYSTEM_MASTER_LAIR ) )
		setVariable( ACTIVE_MASTER_LAIR, 0 );

	__END_CATCH
}


void VariableManager::load()
	throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;
	Result*    pResult = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT MAX(attrID) FROM AttrInfo");

		if (pResult->getRowCount() <= 0)
		{
			SAFE_DELETE(pStmt);
			throw Error("VariableManager::load(): attrID MAX값을 구할 수 없습니다.");
		}

		pResult->next();
		int maxAttr = pResult->getInt(1);
		m_Variables.clear();
		m_Variables.resize( max((maxAttr+1), (int)VARIABLE_MAX) );

		pResult = pStmt->executeQuery("SELECT attrID, attr1, attr2 FROM AttrInfo");

		if(pResult->getRowCount() <= 0)
		{
			SAFE_DELETE(pStmt);
			throw Error("VariableManager::load(): 값이 하나도 존재하지 않습니다");
		}

		while(pResult->next())
		{
			VariableType    attrID = (VariableType)pResult->getInt(1);
			int    			attr1  = pResult->getInt(2);
			//int    			attr2  = pResult->getInt(3);


			setVariable(attrID, attr1);
			cout << toString(attrID).c_str() << endl;
		}
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt);

	loadMobaPlayConfig();

	__END_CATCH
}

void VariableManager::resetMobaPlayConfig()
{
	m_MobaPlayConfig = MobaPlayConfig();
}

bool VariableManager::loadMobaPlayConfig()
{
	__BEGIN_TRY

	resetMobaPlayConfig();

	Statement* pStmt = NULL;
	Result* pResult = NULL;

	try
	{
		// 2026-09-04 - BEGIN_DB/END_DB kaldirildi: END_DB pStmt'yi delete edip
		// exception'i rethrow ediyordu; asagidaki catch'in SAFE_DELETE'i ayni
		// pointer'i IKINCI kez siliyordu (tablo/sorgu hatasinda double delete
		// = GS crash riski). SAFE_DELETE NULL'a cektigi icin tek catch yeterli
		// (PartyExpBonusTable::load ile ayni guvenli kalip).
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery(
				"SELECT `Enable`, MobaPlayTime, MobaMaxPC, MobaTeamSize, MobaWaveSpawnTime, "
				"MobaWaitOutTime, MobaGoodsIDReward, MobaMaxDailyPlays, MobaMinAdvancementLevel, "
				"ExitZoneID, ExitSlayerX, ExitSlayerY, ExitVampireX, ExitVampireY, ExitOustersX, ExitOustersY "
				"FROM MobaPlayConfig WHERE ConfigID=1"
			);

			if (pResult == NULL || pResult->getRowCount() <= 0)
			{
				SAFE_DELETE(pStmt);
				filelog("Moba.log", "[MobaPlayConfig] ConfigID=1 missing; AOS Battle disabled");
				return false;
			}

			pResult->next();
			MobaPlayConfig config;
			config.Enable = pResult->getInt(1);
			config.MobaPlayTime = pResult->getInt(2);
			config.MobaMaxPC = pResult->getInt(3);
			config.MobaTeamSize = pResult->getInt(4);
			config.MobaWaveSpawnTime = pResult->getInt(5);
			config.MobaWaitOutTime = pResult->getInt(6);
			config.MobaGoodsIDReward = pResult->getInt(7);
			config.MobaMaxDailyPlays = pResult->getInt(8);
			config.MobaMinAdvancementLevel = pResult->getInt(9);
			config.ExitZoneID = pResult->getInt(10);
			config.ExitSlayerX = pResult->getInt(11);
			config.ExitSlayerY = pResult->getInt(12);
			config.ExitVampireX = pResult->getInt(13);
			config.ExitVampireY = pResult->getInt(14);
			config.ExitOustersX = pResult->getInt(15);
			config.ExitOustersY = pResult->getInt(16);
			SAFE_DELETE(pStmt);

			bool valid = true;
			if (config.Enable < 0 || config.Enable > 1) valid = false;
			if (config.MobaPlayTime <= 0) valid = false;
			if (config.MobaMaxPC <= 1 || config.MobaMaxPC > 200) valid = false;
			if (config.MobaTeamSize <= 0) valid = false;
			if (config.MobaMaxPC != config.MobaTeamSize * 2) valid = false;
			if (config.MobaWaveSpawnTime <= 0) valid = false;
			if (config.MobaWaitOutTime < 0) valid = false;
			if (config.MobaGoodsIDReward <= 0) valid = false;
			if (config.MobaMaxDailyPlays <= 0) valid = false;
			if (config.MobaMinAdvancementLevel < 0) valid = false;
			if (config.ExitZoneID <= 0) valid = false;
			if (config.ExitSlayerX < 0 || config.ExitSlayerX > 255 || config.ExitSlayerY < 0 || config.ExitSlayerY > 255) valid = false;
			if (config.ExitVampireX < 0 || config.ExitVampireX > 255 || config.ExitVampireY < 0 || config.ExitVampireY > 255) valid = false;
			if (config.ExitOustersX < 0 || config.ExitOustersX > 255 || config.ExitOustersY < 0 || config.ExitOustersY > 255) valid = false;

			if (!valid)
			{
				filelog("Moba.log", "[MobaPlayConfig] invalid values enable=%d play=%d maxPC=%d team=%d wave=%d wait=%d reward=%d daily=%d minAdv=%d exit=%d S(%d,%d) V(%d,%d) O(%d,%d); AOS Battle disabled",
					config.Enable, config.MobaPlayTime, config.MobaMaxPC, config.MobaTeamSize, config.MobaWaveSpawnTime,
					config.MobaWaitOutTime, config.MobaGoodsIDReward, config.MobaMaxDailyPlays, config.MobaMinAdvancementLevel,
					config.ExitZoneID, config.ExitSlayerX, config.ExitSlayerY, config.ExitVampireX, config.ExitVampireY,
					config.ExitOustersX, config.ExitOustersY);
				return false;
			}

			config.Valid = true;
			m_MobaPlayConfig = config;
			filelog("Moba.log", "[MobaPlayConfig] loaded enable=%d play=%d maxPC=%d team=%d wave=%d wait=%d reward=%d daily=%d minAdv=%d exit=%d",
				m_MobaPlayConfig.Enable, m_MobaPlayConfig.MobaPlayTime, m_MobaPlayConfig.MobaMaxPC,
				m_MobaPlayConfig.MobaTeamSize, m_MobaPlayConfig.MobaWaveSpawnTime, m_MobaPlayConfig.MobaWaitOutTime,
				m_MobaPlayConfig.MobaGoodsIDReward, m_MobaPlayConfig.MobaMaxDailyPlays,
				m_MobaPlayConfig.MobaMinAdvancementLevel, m_MobaPlayConfig.ExitZoneID);
			return true;
		}
	}
	catch (Throwable& t)
	{
		SAFE_DELETE(pStmt);
		filelog("Moba.log", "[MobaPlayConfig] load failed: %s; AOS Battle disabled", t.toString().c_str());
		return false;
	}

	return false;

	__END_CATCH
}

VariableType     
VariableManager::getVariableType(const string& variableName) const
{
	char str[256];
	strcpy(str, variableName.c_str());
	strlwr(str);

	VARIABLE_NAME::const_iterator itr = m_VariableNames.find( str );

	if (itr==m_VariableNames.end())
	{
		return VARIABLE_MAX;
	}

	return itr->second;
}

void 
VariableManager::setVariable(VariableType vt, const string& strVariable)
{
	int value = atoi(strVariable.c_str());

	if (value==0)
	{
		if (strcasecmp(strVariable.c_str(), "on")==0)
		{
			setVariable(vt, 1);
		}
		else if (strcasecmp(strVariable.c_str(), "off")==0)
		{
			setVariable(vt, 0);
		}
		else
		{
			setVariable(vt, 0);
		}
	}
	else
	{
		setVariable(vt, value);
	}
}

void 
VariableManager::setVariable(VariableType vt, int value)
{ 
	try {

		// RACE_WAR_TIMEBAND 범위 체크
		if ( vt == RACE_WAR_TIMEBAND )
		{
			if ( value < 0 || value > 3 )
				return;
		}

		m_Variables[vt] = value; 

		Statement* pStmt = NULL;
		//Result*    pResult = NULL;
		
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery("UPDATE AttrInfo SET attr1=%d WHERE attrID=%d", value, (int)vt);

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
		
	} catch (Throwable& t){
		filelog("variableManagerError.log", "%s", t.toString().c_str());
	}
}

string   
VariableManager::toString(VariableType vt) const
{
	StringStream msg;

	if ( vt >= VARIABLE_MAX )
	{
		msg << "알수 없는 설정변수: VariableType=" << (int)vt;

		return msg.toString();
	}

	int attr1 = m_Variables[vt];

	switch (vt)
	{
		case EVENT_ACTIVE:
			msg << "Event발생여부: " << attr1;
			break;
		case EVENT_RATIO:
			msg << "Event발생빈도: " << attr1;
			break;
		case ITEM_PROBE_RATIO:
			msg << "Item추가발생확률:  " << attr1;
			break;
		case EXP_RATIO:
			msg << "경험치 추가 발생확률: " << attr1;
			break;
		case COMBAT_BONUS_TIME:
			msg << "전쟁 승리 유지시간: " << attr1;
			break;

		case COMBAT_SLAYER_HP_BONUS_RATIO:
			msg << "슬레이어 전쟁 HP 보너스: " << attr1;
			break;

		case COMBAT_VAMPIRE_HP_BONUS_RATIO:
			msg << "뱀파이어 전쟁 HP 보너스: " << attr1;
			break;

		case PREMIUM_EXP_BONUS_PERCENT:
			msg << "프리미엄 사용자의 경험치 보너스: " << attr1;
			break;

		case PREMIUM_ITEM_PROBE_PERCENT:
			msg << "프리미엄 존의 아이템 루팅 확률: " << attr1;
			break;

		case ZONE_GROUP_BALANCING_MINUTE:
			msg << "ZoneGroupBalancing 시간 주기: " << attr1 << "분";
			break;

		case GAMBLE_ITEM_TYPE_RATIO:
			msg << "갬블 ItemType 확률: " << attr1;
			break;

		case GAMBLE_ITEM_OPTION_RATIO:
			msg << "갬블 ItemOption 확률: " << attr1;
			break;

		case SUMMON_MOTORCYCLE:
			msg << "오토바이 소환기능: " << (attr1? "ON":"OFF");
			break;

		case ENEMY_LIMIT_TIME:
			msg << "몬스터 연속 적 설정 시간: " << attr1 << "초"; 
			break;

		case COMBAT_SLAYER_DAMAGE_BONUS:
			msg << "슬레이어 전쟁 Damage 보너스: " << attr1;
			break;

		case COMBAT_VAMPIRE_DAMAGE_BONUS:
			msg << "뱀파이어 전쟁 Damage 보너스: " << attr1;
			break;

		case RARE_ITEM_RATIO:
			msg << "레어 아이템 루팅 확률: " << attr1;
			break;

		case UNIQUE_ITEM_RATIO:
			msg << "유니크 아이템 루팅 확률: " << attr1; 
			break;

		case ACTIVE_MASTER_LAIR:
			msg << "마스터 레어 작동: " << (attr1? "ON":"OFF");
			break;

		case RETRY_MASTER_LAIR:
			msg << "마스터 레어에 죽고 다시 들어가기: " << (attr1? "ON":"OFF");
			break;


		case HARVEST_FESTIVAL_ITEM_RATIO:
			if (attr1==0)
			{
				msg << "추석 송편 아이템 확률 : 0";
			}
			else
			{
				msg << "추석 송편 아이템 확률 : 1/" << attr1;
			}
			break;

		case MASTER_BLOOD_DRAIN_START_HP:
			msg << "마스터 흡혈 시작 HP : " << attr1 << "%";
			break;
		case MASTER_BLOOD_DRAIN_START_BD:
			msg << "마스터 흡혈 시작 BD : " << attr1 << "%";
			break;
		case MASTER_BLOOD_DRAIN_END_HP:
			msg << "마스터 흡혈 끝 HP : " << attr1 << "%";
			break;
		case MASTER_BLOOD_DRAIN_END_BD:
			msg << "마스터 흡혈 끝 BD : " << attr1 << "%";
			break;

		case ACTIVE_CHIEF_MONSTER:
			msg << "치프 몬스터 등장 " << (attr1? "ON":"OFF");
			break;
		case CHIEF_MONSTER_RARE_ITEM_RATIO:
			msg << "치프 몬스터 레어 아이템 루팅 확률 : " << attr1 << "%";
			break;

		case NEWBIE_TRANSPORT_TO_GUILD:
			msg << "능력치 40된 애들 길드로 이동 : " << (attr1? "ON":"OFF");
			break;

		case FREE_PLAY_SLAYER_DOMAIN_SUM:
			msg << "슬레이어 무료 사용 가능 도메인레벨합: " << attr1;
			break;

		case FREE_PLAY_VAMPIRE_LEVEL:
			msg << "뱀파이어 무료 사용 가능 레벨: " << attr1;
			break;

		case LOG_INCOMING_CONNECTION:
			msg << "IncomingConnection관련된 Log: " << (attr1? "ON":"OFF");
			break;

		case CHRISTMAS_FIRE_CRACKER_RATIO:
			msg << "크리스마스 폭죽 루팅 확률 : " << attr1 << " / 10000 ";
			break;

		case CHRISTMAS_TREE_PART_RATIO:
			msg << "크리스마스 트리 조각 루팅 확률 : " << attr1 << " / 10000 ";
			break;

		case CHRISTMAS_GIFT_BOX_RATIO :
			msg << "크리스마스 선물 상자 루팅 확률: " << attr1 << " / 10000 ";
			break;

		case CHRISTMAS_TREE_DECAY_TIME :
			msg << "크리스마스 트리 지속 시간: " << attr1/600 << "분";
			break;

		case MASTER_LAIR_PLAYER_NUM :
			msg << "마스터 레어 출입 가능 인원 수: " << attr1 << "명";
			break;

		case RANK_EXP_GAIN_PERCENT :
			msg << "계급 경험치는 기본 경험치의 " << attr1 << "%";
			break;

		case ITEM_LUCK_PERCENT :
			msg << "Luck 보너스는 기본 확률의 " << attr1 << "%";
			break;

		case LOTTO_ITEM_RATIO :
			msg << "대박 아이템 확률 : " << attr1 << " / 10000";
			break;

		case LOTTO_ITEM_BONUS_NUM:
			msg << "대박 아이템 추가 아이템 개수 : " << attr1;
			break;

		case LOTTO_SKULL_RATIO :
			msg << "대박 해골 확률 : " << attr1 << " / 10000";
			break;

		case PREMIUM_HALF_EVENT :
			msg << "유료 사용자 앰플, 혈청 반값 이벤트 : " << (attr1?"ON":"OFF");
			break;

		case COMMON_CASTLE_ENTRANCE_FEE :
			msg << "공용성 입장료 : " << attr1;
			break;

		case GUILD_CASTLE_ENTRANCE_FEE :
			msg << "길드성 입장료 : " << attr1;
			break;

		case COMMON_CASTLE_ITEM_TAX_RATIO :
			msg << "공용성 아이템 세금 : " << attr1;
			break;

		case GUILD_CASTLE_ITEM_TAX_RATIO :
			msg << "길드성 아이템 세금 : " << attr1;
			break;

		case WAR_REGISTRATION_FEE :
			msg << "전쟁 신청금 : " << attr1;
			break;

		case GUILD_WAR_TIME :
			msg << "길드 전쟁 시간 : " << attr1 << " 초 == " << attr1/60 << "분";
			break;

		case RACE_WAR_TIME :
			msg << "종족 전쟁 시간 : " << attr1 << " 초 == " << attr1/60 << "분";
			break;

		case WAR_PERIOD_WEEK :
			msg << "전쟁주기 : " << (attr1? "주 단위":"일 단위");
			break;

		case WAR_ACTIVE :
			msg << "아담의 성지 전쟁 기능 : " << (attr1?"ON":"OFF");
			break;

		case RACE_WAR_TIMEBAND:
			msg << "종족 전쟁시 Timeband : ";
			if ( attr1 == 0 )
				msg << "새벽";
			else if ( attr1 == 1 )
				msg << "낮";
			else if ( attr1 == 2 )
				msg << "저녁";
			else if ( attr1 == 3 )
				msg << "밤";
			else
				msg << "알 수 없음";
			break;

		case AUTO_START_RACE_WAR:
			msg << "종족 전쟁 자동 시작 기능: " << (attr1?"ON":"OFF");
			break;

		case ACTIVE_RACE_WAR_LIMITER :
			msg << "종족 전쟁 참가 인원 제한: " << (attr1?"ON":"OFF");
			break;

		case CHIEF_ITEM_BONUS_NUM:
			msg << "치프 몬스터 추가 아이템 개수: " << attr1 << " 개";
			break;

		case ACTIVE_EVENT_GIVE_ITEM :
			msg << "아이템 주는 이벤트: " << (attr1?"ON":"OFF");
			break;

		case VAMPIRE_REDISTRIBUTE_ATTR_PRICE:
			msg << "뱀파이어 능력치 재분배 비용: " << attr1;
			break;

		case ACTIVATE_COUPLE:
			msg << "커플 성사 기능: " << (attr1?"ON":"OFF");
			break;

		case CAN_RECOUPLE:
			msg << "헤어진 적 있는 사람이 또 커플이 될 수 있는가: " << (attr1?"Y":"N");
			break;

		case GUILD_WAR_ACTIVE:
			msg << "길드 전쟁을 하는가?: " << (attr1?"Yes":"No");
			break;

		case FREE_PLAY_OUSTERS_LEVEL:
			msg << "아우스터스 무료 사용 가능 레벨: " << attr1;
			break;

		case CAN_APPLY_QUEST:
			msg << "퀘스트를 신청할 수 있는가?: " << (attr1?"Yes":"No");
			break;

		case SEND_QUEST_INFO:
			msg << "퀘스트 정보를 보내줘야 하는가?: " << (attr1?"Yes":"No");
			break;

		case CAN_BUY_SHOP:
			msg << "아이템 상점 기능이 켜져있는가: " << (attr1?"Yes":"No");
			break;

		case HOME_EXP_BONUS:
			msg << "미등록 IP 경험치 보너스: " << attr1;
			break;
		case PCROOM_FREE_EXP_BONUS:
			msg << "무료 PC방 경험치 보너스: " << attr1;
			break;
		case PCROOM_CHARGE_EXP_BONUS:
			msg << "유료 PC방 경험치 보너스: " << attr1;
			break;

		case PCROOM_ITEM_RATIO_BONUS:
			msg << "PC방 아이템 획득 확률 보너스: " << attr1;
			break;
		case MONEY_TRACE_LOG_LIMIT:
			msg << "돈 로그 남기는 최소 금액: " << attr1;
			break;
		case PC_ROOM_LOTTO_EVENT:
			msg << "피씨방 복권 이벤트: " << attr1;
			break;
		case ACTIVE_FLAG_WAR:
			msg << "깃발뺏기 게임 실행: " << (attr1?"yes":"no");
			break;
		case KILL_DAEMONCTL:
			msg << "DAEMONCTL 죽이는 여부: " << (attr1?"yes":"no");
			break;

		case EVENT_MOON_CARD:
			if ( attr1 )
				msg << "달카드 이벤트가 진행중입니다.";
			else
				msg << "달카드 이벤트를 중단했습니다.";
			break;

		case FULL_MOON_CARD_RATIO:
			msg << "보름달 카드 나올 확률: 1 / " << attr1;
			break;

		case HALF_MOON_CARD_RATIO:
			msg << "반달 카드 나올 확률: 1 / " << attr1;
			break;

		case NEW_MOON_CARD_RATIO:
			msg << "초승달 카드 나올 확률: 1 / " << attr1;
			break;

		case OLD_MOON_CARD_RATIO:
			msg << "그믐달(없는) 카드 나올 확률: 1 / " << attr1;
			break;

		case ACTIVE_LEVEL_WAR:
			msg << "레벨별 전쟁 진행 여부: " << (attr1?"yes":"no");
			break;

		case RACE_WAR_HP_BONUS:
			msg << "종족전시 HP 보너스: " << attr1;
			break;

		case HEAD_COUNT:
			msg << "머리 갯수 카운트하기: " << (attr1?"yes":"no");
			break;

		case HEAD_PRICE_BONUS:
			msg << "머리 가격 보너스 : " << attr1;
			break;

		case LEVEL_WAR_ZONE_FREE_ENTER:
			msg << "레벨별 전쟁 존에 자유롭게 들어갈 수 있는가? : " << (attr1?"yes":"no");
			break;

		case PET_EXP_RATIO:
			msg << "펫 경험치 비율 : " << attr1;
			break;

		case EVENT_LUCKY_BAG:
			if ( attr1 )
				msg << "복주머니 이벤트가 진행중입니다.";
			else
				msg << "복주머니 이벤트가 진행중이지 않습니다.";
			break;

		case GREEN_LUCKY_BAG_RATIO:
			msg << "초록색 복주머니 나올 확률 : 1 / " << attr1;
			break;

		case BLUE_LUCKY_BAG_RATIO:
			msg << "파란색 복주머니 나올 확률 : 1 / " << attr1;
			break;

		case GOLD_LUCKY_BAG_RATIO:
			msg << "황금색 복주머니 나올 확률 : 1 / " << attr1;
			break;

		case RED_LUCKY_BAG_RATIO:
			msg << "빨간색 복주머니 나올 확률 : 1 / " << attr1;
			break;

		case EVENT_GIFT_BOX:
			if ( attr1 )
				msg << "선물상자 교환 이벤트가 진행중입니다.";
			else
				msg << "선물상자 교환 이벤트가 진행중이지 않습니다.";
			break;

		case RED_GIFT_BOX_RATIO:
			msg << "빨간색 선물상자 나올 확률 : 1 / " << attr1;
			break;

		case BLUE_GIFT_BOX_RATIO:
			msg << "파란색 선물상자 나올 확률 : 1 / " << attr1;
			break;

		case GREEN_GIFT_BOX_RATIO:
			msg << "초록색 선물상자 나올 확률 : 1 / " << attr1;
			break;

		case YELLOW_GIFT_BOX_RATIO:
			msg << "노란색 선물상자 나올 확률 : 1 / " << attr1;
			break;

		case NETMARBLE_CARD_RATIO:
			msg << "넷마블 카드 나올 확률 : " << attr1 << " / 100000";
			break;

		case NETMARBLE_CARD_EVENT:
			msg << "넷마블 카드 이벤트 : " << ((attr1)?"Yes":"No");
			break;

		case PREMIUM_TRIAL_EVENT:
			msg << "프리미엄 체험 이벤트 : " << ((attr1)?"Yes":"No");
			break;

		case PET_FOOD_EVENT:
			msg << "펫 먹이 이벤트 : " << ((attr1)?"Yes":"No");
			break;

		case HIGHER_PET_FOOD_RATIO:
			msg << "고단계 펫 먹이 확률 : " << attr1 << "%";
			break;

		case RACE_PET_FOOD_RATIO:
			msg << "종족별 펫 먹이 확률 : " << attr1 << "%";
			break;

		case PET_FOOD_RATIO:
			msg << "펫 먹이 루팅 확률 : " << attr1 << " / 100000";
			break;

		case REVIVAL_SET_RATIO:
			msg << "리바이벌 셋 확률 : " << attr1 << "%";
			break;

		case TODAY_IS_HOLYDAY:
			if ( attr1 ) msg << "오늘은 국경일 입니다.";
			else msg << "오늘은 국경일이 아닙니다.";
			break;

		case SLAYER_HP_RATIO:
			msg << "슬레이어 HP비율 : " << attr1 << "%";
			break;
		case VAMPIRE_HP_RATIO:
			msg << "뱀파이어 HP비율 : " << attr1 << "%";
			break;
		case OUSTERS_HP_RATIO:
			msg << "아우스터즈 HP비율 : " << attr1 << "%";
			break;
		case MONSTER_HP_RATIO:
			msg << "몬스터 HP비율 : " << attr1 << "%";
			break;

		case MONSTER_EXP_RATIO:
			msg << "몬스터 EXP비율 : " << attr1 << "%";
			break;

		case CLOVER_EVENT:
			msg << "네잎클로버 이벤트 : " << ((attr1)?"on":"off");
			break;

		case CLOVER_RATIO:
			msg << "네잎클로버 확률 : " << attr1 << "/100000";
			break;

		case MONSTER_DAMAGE_RATIO:
			msg << "몬스터 Damage 비율 : " << attr1 << "%";
			break;

		case RACE_PET_QUEST:
			msg << "2차 펫 퀘스트 : " << ((attr1)?"on":"off");
			break;

		case NICKNAME_PEN_EVENT:
			msg << "네이밍펜 이벤트 : " << ((attr1)?"on":"off");
			break;

		case NICKNAME_PEN_RATIO:
			msg << "네이밍펜 확률 : " << attr1 << "/100000";
			break;

		case GOLD_MEDAL_RATIO:
			msg << "금메달 확률 : " << attr1 << "/100000";
			break;

		case PET_DAY_EVENT:
			msg << "복날 이벤트 : " << ((attr1)?"on":"off");
			break;

		case OLYMPIC_ITEM_RATIO:
			msg << "올림픽 이벤트 아이템 루팅 확률 : " << attr1 << "/100000";
			break;

		case OLYMPIC_EVENT:
			msg << "다덴 올림픽 이벤트 : " << ((attr1)?"on":"off");
			break;

		case CROWN_PRICE:
			msg << "월계관 가격 : " << attr1;
			break;

		case CHOBO_EVENT:
			msg << "초보자 이벤트 : " << ((attr1)?"on":"off");
			break;

		case QUIT_GUILD_PENALTY_TERM:
			msg << "길드 탈퇴시 재가입 불가 기간 : " << attr1;
			break;

		case GUILD_UNION_MAX:
			msg << "연합 길드 갯수 제한 : " << attr1;
			break;

		case LUCK_CHARM_RATIO:
			msg << "수능 대박 아이템 확률 : " << attr1;
			break;

		case GDR_LAIR_PC_LIMIT:
			msg << "질드레 레어 인원 제한 : " << attr1;
			break;
			
		case GDR_LAIR_REWARD_MAGNIFICATION :
			msg << "질드레 레어 보상 배수 : " << attr1;
			break;

		case DONATION_EVENT_200501:
			msg << "기부 이벤트 2005.01 : " << ((attr1)?"on":"off");
			break;

		case EVENT_NEW_YEAR_2005:
			msg << "2005년 새해 맞이 이벤트 : " << ((attr1)?"on":"off");
			break;

		case BOKJORY_RATIO:
			msg << "복조리 아이템 확률 : 1 / " << attr1;
			break;
			
		case EVENT_RED_RICE_CAKE_SOUP_RATIO:
			msg << "이벤트 빨간 떡국 아이템 확률 : 1 / " << attr1;
			break;
			
		case EVENT_GREEN_RICE_CAKE_SOUP_RATIO:
			msg << "이벤트 녹색 떡국 아이템 확률 : 1 / " << attr1;
			break;
			
		case EVENT_BLUE_RICE_CAKE_SOUP_RATIO:
			msg << "이벤트 파란 떡국 아이템 확률 : 1 / " << attr1;
			break;
			
		case EVENT_BLACK_RICE_CAKE_SOUP_RATIO:
			msg << "이벤트 검은 떡국 아이템 확률 : 1 / " << attr1;
			break;
			
		case EVENT_MUGWORT_RICE_CAKE_SOUP_RATIO:
			msg << "이벤트 쑥색 떡국 아이템 확률 : 1 / " << attr1;
			break;

		case TIME_PERIOD_EXP_2X:
			msg << "시간대별 경험치 2배 이벤트 : " << ((attr1)?"on":"off");
			break;

		case COUNT_KILL_MONSTER:
			msg << "죽인 몬스터 숫자 세기 : " << ((attr1)?"on":"off");
			break;

		case DONATION_EVENT_200505:
			msg << "결혼 축의금 이벤트 : " << ((attr1)?"on":"off");
			break;

		case FAMILY_COIN_EVENT:
			msg << "패밀리 코인 이벤트 : " << ((attr1)?"on":"off");
			break;

		case FAMILY_COIN_RATIO:
			msg << "패밀리 코인 아이템 확률 : 1 / " << attr1;
			break;

		case BALLOON_HEADBAND_EVENT:
			msg << "풍선 머리띠 이벤트 : " << ((attr1)?"on":"off");
			break;

		case BALLOON_HEADBAND_RATIO:
			msg << "풍선 머리띠 아이템 확률 : 1 / " << attr1;
			break;

		case FULL_PARTY_EXP_EVENT:
			msg << "풀 파티 경험치 이벤트 : " << ((attr1)?"on":"off");
			break;

		case EVENT_FALL_2005:
			msg << "2005년 추석 이벤트 : " << ((attr1)?"on":"off");
			break;

		case EVENT_FALL_2005_DEFENSE_RATIO:
			msg << "몸놀림이 잽싸지는 송편(Defense) SP 확률 : 1 / " << attr1;
			break;

		case EVENT_FALL_2005_PROTECTION_RATIO:
			msg << "피부가 단단해지는 송편(Protection) SP 확률 : 1 / " << attr1;
			break;

		case EVENT_FALL_2005_ATTACK_SPEED_RATIO:
			msg << "손짓이 빨라지는 송편(Attack Speed) SP 확률 : 1 / " << attr1;
			break;

		case EVENT_FALL_2005_DAMAGE_RATIO:
			msg << "맞으면 더 아픈 송편(Damage) SP 확률 : 1 / " << attr1;
			break;

		case EVENT_FALL_2005_LUCK_RATIO:
			msg << "운발이 오르는 송편(Luck) SP 확률 : 1 / " << attr1;
			break;

		case EVENT_FALL_2005_VISION_RATIO:
			msg << "눈이 맑아지는 송편(Vision) SP 확률 : 1 / " << attr1;
			break;

		case YELLOW_CANDY_RATIO:
			msg << "노란사탕 확률 : " << attr1 << " / 100000";
			break;

		case TIME_PERIOD_MEGAPHONE_RATIO:
			msg << "TIME_PERIOD_MEGAPHONE_RATIO : " << attr1;
			break;

		case TIME_PERIOD_TRANSLATOR_RATIO:
			msg << "TIME_PERIOD_TRANSLATOR_RATIO : " << attr1;
			break;

		case TIME_PERIOD_TRANSKIT_RATIO:
			msg << "TIME_PERIOD_TRANSKIT_RATIO : " << attr1;
			break;

		case TIME_PERIOD_DRAGON_RATIO:
			msg << "TIME_PERIOD_DRAGON_RATIO : " << attr1;
			break;

		case TIME_PERIOD_STORM_RATIO:
			msg << "TIME_PERIOD_STORM_RATIO : " << attr1;
			break;

		case TIME_PERIOD_WIDE_RATIO:
			msg << "TIME_PERIOD_WIDE_RATIO : " << attr1;
			break;

		case TIME_PERIOD_RUNNING_RATIO:
			msg << "TIME_PERIOD_RUNNING_RATIO : " << attr1;
			break;

		case SLAYER_EXP_BONUS_PERCENT:
			msg << "SLAYER_EXP_BONUS_PERCENT : " << attr1;
			break;

		case GREEN_HANDSEL_RATIO:
			msg << "GREEN_HANDSEL_RATIO : " << attr1;
			break;

		case PROLOGUE_QUEST_ITEM_RATIO:
			msg << "Prologue Quest Item Ratio : 1 / " << attr1;
			break;

		case EVENT_COMEBACK_200604 :
			msg << "Comeback Event 2006.4 : " << (attr1 ? "on" : "off");
			break;

		case EVENT_LEVELUP_200604:
			msg << "Level UP Event 2006.4 : " << (attr1 ? "on" : "off");
			break;

		case EVENT_SOCCER_200606_RATIO:
			msg << "Soccer Event Ratio : " << attr1;
			break;

		case EVENT_RICE_CAKE_RATIO:
			msg << "Rice Cake Event Ratio : 1 / " << attr1;
			break;

		case EVENT_RICE_CAKE:
			msg << "Rice Cake Event 2006 : " << (attr1 ? "on" : "off");
			break;

		case EVENT_SNOW_MAN_PART_RATIO:
			msg << "Snowman Part Ratio : " << attr1 << " / 10000";
			break;

		case EVENT_BOX_ITEM_200612:
			msg << "2006 12 Event Box : " << (attr1 ? "on" : "off"); 
			break;

		case MIKLLIZZ_MAX_PLAYER:
			msg << "Mikllizz Max Pass Player : " << attr1 << " Player";
			break;

		case NEWBIE_ITEM_EVENT:
			msg << "Newbie Give Item Event : " << (attr1 ? "on" : "off");
			break;
//		case NEW_ACCOUNT_FIRST_GOAL_80_EVENT:
//			msg << "New Account First Goal Level 80 Give Item Event : " << (attr1 ? "on" : "off");
//			break;
		default:
			msg << VariableType2String[vt] << ": " << attr1;
			break;
	}

	return msg.toString();
}

VariableManager* g_pVariableManager = NULL;
bool g_bRunning = true;

