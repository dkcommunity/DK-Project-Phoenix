//-----------------------------------------------------------------------------
// ContentsFilter.h
//-----------------------------------------------------------------------------
// define을 사용하여 컨텐츠를 필터링한다.
// 실행 시간에 필터링하는 것은 해킹의 위험이 있기 때문에
// 컴파일 시간에 필터링되도록 한다.
//-----------------------------------------------------------------------------

#pragma once

#define __ON	(1)
#define __OFF	(0)

//////////////////////////////////////////////////////////////////////////
// Korea

#ifdef __DESIGNED_KOREA
#define __DESIGN_NAME "Designed Korea"

#	define __ADVANCEMENT_CHARACTER			__ON
#	define __LANGUAGE_CHANGE				__ON	//by viva

//#	define __POWER_JJANG_ID					__OFF
#	define __USER_GRADE						__ON
#	define __SMS_SERVICES					__OFF
#	define __RARE_SKILL						__ON

#	define __INTERNATIONAL_PREMIUM_SYSTEM	__OFF
#	define __NEW_SYSTEM_AVAILABILITIES		__OFF
#	define __ENCHANT_REQUIRE_LEVE			__OFF
#	define __ID_PASSWORD					__OFF

#	define __CHAR_DELETE					__OFF
#	define __INTERNATIONAL_UI				__OFF
#	define __ENGLISH_STRFILTER				__OFF
#	define __CRYPT							__OFF
#	define __CONECT_LIMITER					__OFF
#	define __PERSNALSHOP_NAMING				__OFF
#	define __HERO_SYSTEM					__ON
#	define __CONTRIBUTE_SYSTEM				__ON
#	define __LEVEL_WAR_RENEWAL				__ON
#	define __SERVER_SELECT_REVEWAL			__ON
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__ON
#	define __2008_FIRST_CHARGE_ITEM			__ON	// 2008 1차 유료 아이템
#	define __DSG_EFFECT						__ON
#	define __GEAR_SWAP_CHANGE				__ON	//								본섭,	테섭,	개발섭
#	define __2008_WHITEDAY_LOVECANDY		__ON	// 2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__ON	// 2008 계급경험의 모래시계				테섭,	개발섭
#	define __PAYZONE_PASS_TICKET			__ON	// 유료존 입장권
#	define __QUEST_RENEWAL					__OFF	//										테섭,	개발섭
#	define __EFFECT_RENEWAL					__ON	//								본섭,	테섭,	개발섭
#	define __SECOND_RARE_SKILL				__ON	//										테섭,	개발섭
#	define __080405_FIREST_UI_UPDATE		__ON	//										테섭,	개발섭
#	define __LIMITED_ITEM_UNISEX			__ON	
#	define __FAST_TRANSFORTER				__ON	
#	define __SECOND_TRANSFORTER				__ON	//by viva  add new 
#	define __CLOVER_EVENT					__ON
#	define __QUEST_RENEWAL2					__OFF	// __QUEST_RENEWAL은 미리 패치 되었고 __QUEST_RENEWAL2은 수정 중에 있는 내용이다.
#	define __GORE_BLOOD_BIBLE				__ON	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__ON	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정

#	define __TIPOJYU_CASTLE					__ON
#	define __2008_JUNE_EVENT_EX_STONE		__ON	// 경험의 돌 이벤트(13일의 금요일)
#	define __NEW_ADVANCEMENT_ITEM			__ON	// 새로운 승직아이템
#	define __TUNING_ITEM					__ON	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__OFF	// 일본 아이템 역개발
#	define __CHINESE_UI						__OFF	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__OFF	// 일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__ON	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND				__ON	// 신규이동수단 2차
#	define __ADD_MAGICPROTECTION			__ON	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)
#	define __AUTO_ITEM_ROOTING				__OFF	// 아이템 자동 루팅 코드(테스트 코드의 가까움)
#	define __LOCALIZING_LONGNAME			__OFF	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__ON	// OX이벤트
#	define __ITEM_SHOW						__ON	// 아이템 보기

#	define __BLITZ_COUPON_EVENT				__ON	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__ON	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__ON	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF	// 
#	define __PREMIUM_GIVE_ITEM_UI			__ON	// 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트			2008.10.1 패치
#	define __2008_OCTOBER_MAKETITEM			__ON	// 10월 유료 아이템
#	define __PREMIUM_CARD					__ON	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__ON	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__ON	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__ON	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__ON	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__ON	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__ON	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__OFF	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__ON	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__ON	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__ON	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__ON	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__ON	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__ON	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__ON	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__ON	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.

#	define __HERO_SYSTEM_ITEM				__ON	
#	define __CONTRIBUTE_SYSTEM_ITEM			__ON	
#	define __LEVEL_WAR_RENEWAL_ITEM			__ON	
#	define __MENEGROTH_ITEM					__ON
#	define __LC_SELECT_WORLD_ERROR			__ON	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__ON	// 기능성 펫푸드
#	define __PCITEMINFO						__ON	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흩어져 있던 녀석들을 통합

#	define __REPAIRSYSTEM_RENEWAL			__ON	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__ON	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__ON	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__ON	// 스마트 푸더
#	define __VISION_AMPLE					__ON	// 비전 앰플
#	define __THIRD_ENCHANT_2				__ON	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__OFF	// NProtect 넣고 빼기	//byviva
#	define __PREMIUM_CARD_ADAY				__ON	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__ON	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__ON	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__ON	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__ON	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__ON	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__ON	// PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자
#	define __ATTACK_SPEED					__ON	// 어택 스피드 서버에 종속화
#	define __ONIBLA							__ON	// 오니블라 개발
#	define __FRIEND_ADDITION				__ON	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__ON	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__ON	// by svi // 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__OFF	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__OFF   // 다덴마켓 끄기
#	define __CREDIT							__ON	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__OFF	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__ON	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03
#	define __IMI_SENDGLASS					__ON	// IMI용 모래시계
#	define __PET_MASHIMARO					__ON	// 마시마로
#	define __TREASURE_BOX					__ON	// 보물상자
#	define __PET_CATS						__ON	// 신규 일본 마켓 아이템 고양이 3마리 - 2009.08.14
#	define __MASTER_EFFECTS					__ON	// 기한제 승직/비승직 이펙트 아이템 - 2009.08.24
#	define __IMI_NEW_ENCHANT_ITEM			__ON	// IMI용 신규 인첸트 아이템 - 2009.08.25
#	define __MOONLIGHT_FESTIVAL_EVENT		__ON	// 달빛 축제 이벤트 - 2009.09.15
#	define __TREBLE_PET_SANDGLASS			__ON	// 3배 혜택의 펫 경험의 모래시계 - 2009.09.21
#	define __IMI_MIXNGFORGE					__ON	// IMI용 믹싱 포지 - 2009.09.21
#	define __NONE_ADVANCEMENT_SANDGLASS		__ON	// 비승직 범용 경험의 모래시계 - 2009.09.22
#	define __PET_CATS_UPGRADED				__ON	// 일본 마켓 업그레이드 고양이 12마리 - 2009.09.21
#	define __UPGRADE_MONSTER_PET			__ON	// 업그레이드 몬스터 펫 - 2009.10.09
#	define __20091028_ADD_ITEM				__ON	// 새로운 아이템 추가 - 2009.10.28	
#	define __TAIWAN_INTERFACE				__OFF	// 대만 인터페이스
#	define __CRIMSON_EVENT					__ON	// 크림슨 이벤트 추가 - 2009.11.09
#	define __20091116_ADD_RARE_SKILL		__ON	// 레어스킬 추가 - 2009.11.16	
#	define __20091119_ADD_ENCHANT_ITEM		__ON	// 신규 인첸트 아이템 - 2009.11.19
#endif	// DESIGNED_KOREA


#ifdef __DESIGNED_CHS
#define __DESIGN_NAME "Designed CHS"
#	define __ADVANCEMENT_CHARACTER			__ON
#	define __LANGUAGE_CHANGE				__ON	//by viva
#	define __USER_GRADE						__ON
#	define __SMS_SERVICES					__OFF	//by viva
#	define __RARE_SKILL						__ON
#	define __INTERNATIONAL_PREMIUM_SYSTEM	__OFF
#	define __NEW_SYSTEM_AVAILABILITIES		__OFF
#	define __ENCHANT_REQUIRE_LEVE			__OFF
#	define __ID_PASSWORD					__OFF
#	define __CHAR_DELETE					__OFF
#	define __INTERNATIONAL_UI				__OFF
#	define __ENGLISH_STRFILTER				__OFF
#	define __CRYPT							__OFF
#	define __CONECT_LIMITER					__OFF
#	define __PERSNALSHOP_NAMING				__OFF
#	define __HERO_SYSTEM					__ON
#	define __CONTRIBUTE_SYSTEM				__ON
#	define __LEVEL_WAR_RENEWAL				__ON
#	define __SERVER_SELECT_REVEWAL			__ON
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__ON
#	define __2008_FIRST_CHARGE_ITEM			__ON	// 2008 1차 유료 아이템
#	define __DSG_EFFECT						__ON
#	define __GEAR_SWAP_CHANGE				__ON	//								본섭,	테섭,	개발섭
#	define __2008_WHITEDAY_LOVECANDY		__ON	// 2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__ON	// 2008 계급경험의 모래시계				테섭,	개발섭
#	define __PAYZONE_PASS_TICKET			__ON	// 유료존 입장권
#	define __QUEST_RENEWAL					__OFF	//										테섭,	개발섭
#	define __EFFECT_RENEWAL					__ON	//								본섭,	테섭,	개발섭
#	define __SECOND_RARE_SKILL				__ON	//										테섭,	개발섭
#	define __080405_FIREST_UI_UPDATE		__ON	//										테섭,	개발섭
#	define __LIMITED_ITEM_UNISEX			__ON	
#	define __FAST_TRANSFORTER				__ON	
#	define __SECOND_TRANSFORTER				__ON	//by viva  add new 
#	define __CLOVER_EVENT					__ON
#	define __QUEST_RENEWAL2					__OFF	// __QUEST_RENEWAL은 미리 패치 되었고 __QUEST_RENEWAL2은 수정 중에 있는 내용이다.
#	define __GORE_BLOOD_BIBLE				__ON	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__ON	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정
#	define __TIPOJYU_CASTLE					__ON
#	define __2008_JUNE_EVENT_EX_STONE		__ON	// 경험의 돌 이벤트(13일의 금요일)
#	define __NEW_ADVANCEMENT_ITEM			__ON	// 새로운 승직아이템
#	define __TUNING_ITEM					__ON	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__OFF	// 일본 아이템 역개발
#	define __CHINESE_UI						__OFF	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__OFF	// 일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__ON	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND				__ON	// 신규이동수단 2차
#	define __ADD_MAGICPROTECTION			__ON	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)
#	define __AUTO_ITEM_ROOTING				__OFF	// 아이템 자동 루팅 코드(테스트 코드의 가까움)
#	define __LOCALIZING_LONGNAME			__OFF	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__ON	// OX이벤트
#	define __ITEM_SHOW						__ON	// 아이템 보기
#	define __BLITZ_COUPON_EVENT				__ON	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__ON	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__ON	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF	// by rallser
#	define __PREMIUM_GIVE_ITEM_UI			__ON	// by svi     // 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트			2008.10.1 패치
#	define __2008_OCTOBER_MAKETITEM			__ON	// 10월 유료 아이템
#	define __PREMIUM_CARD					__ON	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__ON	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__ON	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__ON	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__ON	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__ON	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__ON	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__OFF	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__ON	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__ON	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__ON	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__ON	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__ON	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__ON	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__ON	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__ON	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.
#	define __HERO_SYSTEM_ITEM				__ON	
#	define __CONTRIBUTE_SYSTEM_ITEM			__ON	
#	define __LEVEL_WAR_RENEWAL_ITEM			__ON	
#	define __MENEGROTH_ITEM					__ON
#	define __LC_SELECT_WORLD_ERROR			__ON	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__ON	// 기능성 펫푸드
#	define __PCITEMINFO						__ON	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흩어져 있던 녀석들을 통합
#	define __REPAIRSYSTEM_RENEWAL			__ON	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__ON	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__ON	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__ON	// 스마트 푸더
#	define __VISION_AMPLE					__ON	// 비전 앰플
#	define __THIRD_ENCHANT_2				__ON	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__OFF	// NProtect 넣고 빼기	//byviva
#	define __PREMIUM_CARD_ADAY				__ON	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__ON	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__ON	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__ON	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__ON	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__ON	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__ON	// PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자
#	define __ATTACK_SPEED					__ON	// 어택 스피드 서버에 종속화
#	define __ONIBLA							__ON	// 오니블라 개발
#	define __FRIEND_ADDITION				__ON	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__ON	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__ON	// by svi // 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__OFF	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__OFF   // 다덴마켓 끄기
#	define __CREDIT							__ON	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__ON	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__ON	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03
#	define __IMI_SENDGLASS					__ON	// IMI용 모래시계
#	define __PET_MASHIMARO					__ON	// 마시마로
#	define __TREASURE_BOX					__ON	// 보물상자
#	define __PET_CATS						__ON	// 신규 일본 마켓 아이템 고양이 3마리 - 2009.08.14
#	define __MASTER_EFFECTS					__ON	// 기한제 승직/비승직 이펙트 아이템 - 2009.08.24
#	define __IMI_NEW_ENCHANT_ITEM			__ON	// IMI용 신규 인첸트 아이템 - 2009.08.25
#	define __MOONLIGHT_FESTIVAL_EVENT		__ON	// 달빛 축제 이벤트 - 2009.09.15
#	define __TREBLE_PET_SANDGLASS			__ON	// 3배 혜택의 펫 경험의 모래시계 - 2009.09.21
#	define __IMI_MIXNGFORGE					__ON	// IMI용 믹싱 포지 - 2009.09.21
#	define __NONE_ADVANCEMENT_SANDGLASS		__ON	// 비승직 범용 경험의 모래시계 - 2009.09.22
#	define __PET_CATS_UPGRADED				__ON	// 일본 마켓 업그레이드 고양이 12마리 - 2009.09.21
#	define __UPGRADE_MONSTER_PET			__ON	// 업그레이드 몬스터 펫 - 2009.10.09
#	define __20091028_ADD_ITEM				__ON	// 새로운 아이템 추가 - 2009.10.28	
#	define __TAIWAN_INTERFACE				__OFF	// 대만 인터페이스
#	define __CRIMSON_EVENT					__ON	// 크림슨 이벤트 추가 - 2009.11.09
#	define __20091116_ADD_RARE_SKILL		__ON	// 레어스킬 추가 - 2009.11.16	
#	define __20091119_ADD_ENCHANT_ITEM		__ON	// 신규 인첸트 아이템 - 2009.11.19
#	define __KimString		__OFF	// 신규 인첸트 아이템 - 2009.11.19
#endif	// __DESIGNED_CHS



//////////////////////////////////////////////////////////////////////////
// Internation

#ifdef __DESIGNED_INTERNATION
#define __DESIGN_NAME "Designed Internation"

#	define __ADVANCEMENT_CHARACTER			__OFF
#	define __LANGUAGE_CHANGE				__OFF

//#	define __POWER_JJANG_ID					__OFF
#	define __USER_GRADE						__OFF
#	define __SMS_SERVICES					__OFF
#	define __RARE_SKILL						__ON
#	define __INTERNATIONAL_PREMIUM_SYSTEM	__ON

#	define __NEW_SYSTEM_AVAILABILITIES		__ON
#	define __ENCHANT_REQUIRE_LEVE			__ON
#	define __ID_PASSWORD					__ON

#	define __CHAR_DELETE					__ON
#	define __INTERNATIONAL_UI				__ON
#	define __ENGLISH_STRFILTER				__ON
#	define __CRYPT							__ON
#	define __CONECT_LIMITER					__ON
#	define __PERSNALSHOP_NAMING				__OFF

#	define __HERO_SYSTEM					__OFF	//-- 인포 파일을 만들땐 ON 실행 파일은 OFF
#	define __CONTRIBUTE_SYSTEM				__ON	//-- 인포 파일을 만들땐 ON 실행 파일은 OFF//by viva
#	define __LEVEL_WAR_RENEWAL				__OFF	//-- 인포 파일을 만들땐 ON 실행 파일은 OFF
#	define __SERVER_SELECT_REVEWAL			__OFF
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__OFF
#	define __2008_FIRST_CHARGE_ITEM			__OFF	// 2008 1차 유료 아이템
#	define __DSG_EFFECT						__OFF
#	define __GEAR_SWAP_CHANGE				__OFF	// 이중 장착 창
#	define __2008_WHITEDAY_LOVECANDY		__OFF	// 2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__OFF	// 2008 계급경험의 모래시계
#	define __PAYZONE_PASS_TICKET			__OFF
#	define __QUEST_RENEWAL					__OFF	// 퀘스트 리뉴얼
#	define __EFFECT_RENEWAL					__OFF
#	define __SECOND_RARE_SKILL				__OFF
#	define __080405_FIREST_UI_UPDATE		__OFF
#	define __LIMITED_ITEM_UNISEX			__OFF
#	define __FAST_TRANSFORTER				__ON	//by viva
#	define __CLOVER_EVENT					__OFF
#	define __QUEST_RENEWAL2					__OFF
#	define __GORE_BLOOD_BIBLE				__OFF	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__OFF	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정
#	define __TIPOJYU_CASTLE					__ON	//by viva
#	define __HERO_CONTRINBUTE_LEVEL			__OFF	// 메네그로스는 기여도, 영웅, 레벨전이 이펙트가 섞여 있어 Info  파일을 만들때엔 ON 실행 파일은 OFF로 한다.
#	define __2008_JUNE_EVENT_EX_STONE		__OFF	// 경험의 돌 이벤트(13일의 금요일)
#	define __CREATURE_INDEX_BLANK_SUCCESS	__ON	// 크리쳐 컨텐츠 두명이 동시 작업할경우 빈 인덱스 에러 해결.
#	define __NEW_ADVANCEMENT_ITEM			__OFF	// 새로운 승직아이템
#	define __TUNING_ITEM					__OFF	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__OFF	// 일본 아이템 역개발
#	define __CHINESE_UI						__OFF	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__OFF	// 일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__OFF	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND_TRANSFORTER				__ON	// 신규이동수단 2차	//by viva
#	define __ADD_MAGICPROTECTION			__OFF	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)
#	define __LOCALIZING_LONGNAME			__OFF	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__OFF	// OX이벤트
#	define __ITEM_SHOW						__ON	// 아이템 보기
#	define __BLITZ_COUPON_EVENT				__OFF	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__OFF	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__OFF	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF
#	define __PREMIUM_GIVE_ITEM_UI			__OFF	// 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트
#	define __2008_OCTOBER_MAKETITEM			__OFF	// 10월 유료 아이템
#	define __PREMIUM_CARD					__OFF	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__OFF	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__OFF	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__OFF	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__OFF	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__OFF	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__OFF	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__OFF	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__OFF	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__OFF	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__OFF	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__OFF	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__OFF	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__OFF	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__OFF	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__OFF	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.

#	define __HERO_SYSTEM_ITEM				__ON	
#	define __CONTRIBUTE_SYSTEM_ITEM			__OFF	
#	define __LEVEL_WAR_RENEWAL_ITEM			__ON	
#	define __MENEGROTH_ITEM					__ON
#	define __LC_SELECT_WORLD_ERROR			__OFF	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__OFF	// 기능성 펫푸드
#	define __PCITEMINFO						__OFF	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흐텨져 있던 녀석들을 통합
#	define __REPAIRSYSTEM_RENEWAL			__OFF	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__OFF	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__OFF	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__OFF	// 스마트 푸더
#	define __VISION_AMPLE					__OFF	// 비전 앰플
#	define __THIRD_ENCHANT_2				__OFF	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__OFF	// NProtect 넣고 빼기
#	define __PREMIUM_CARD_ADAY				__OFF	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__OFF	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__OFF	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__OFF	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__OFF	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__OFF	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__ON   // PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자//by viva
#	define __ATTACK_SPEED					__ON	// 어택 스피드 서버에 종속화//by viva
#	define __ONIBLA							__OFF	// 오니블라 개발
#	define __FRIEND_ADDITION				__OFF	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__OFF	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__OFF	// 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__OFF	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__OFF   // 다덴마켓 끄기
#	define __CREDIT							__OFF	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__OFF	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__OFF	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03
#	define __IMI_SENDGLASS					__OFF	// IMI용 모래시계
#	define __PET_MASHIMARO					__OFF	// 마시마로
#	define __TREASURE_BOX					__OFF	// 보물상자
#endif	// DESIGNED_INTERNATION



//////////////////////////////////////////////////////////////////////////
// China

#ifdef __DESIGNED_CHINA
#define __DESIGN_NAME "Designed China"

#	define __ADVANCEMENT_CHARACTER			__ON
#	define __LANGUAGE_CHANGE				__ON

//#	define __POWER_JJANG_ID					__OFF
#	define __USER_GRADE						__OFF
#	define __SMS_SERVICES					__OFF
#	define __RARE_SKILL						__ON
#	define __INTERNATIONAL_PREMIUM_SYSTEM	__OFF

#	define __NEW_SYSTEM_AVAILABILITIES		__OFF
#	define __ENCHANT_REQUIRE_LEVE			__OFF
#	define __ID_PASSWORD					__OFF

#	define __CHAR_DELETE					__ON
#	define __INTERNATIONAL_UI				__OFF
#	define __ENGLISH_STRFILTER				__OFF
#	define __CRYPT							__OFF
#	define __CONECT_LIMITER					__OFF
#	define __PERSNALSHOP_NAMING				__ON

#	define __HERO_SYSTEM					__ON
#	define __CONTRIBUTE_SYSTEM				__ON
#	define __LEVEL_WAR_RENEWAL				__ON
#	define __SERVER_SELECT_REVEWAL			__ON
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__ON
#	define __2008_FIRST_CHARGE_ITEM			__ON	//2008 1차 유료 아이템
#	define __DSG_EFFECT						__ON
#	define __GEAR_SWAP_CHANGE				__ON	//이중 장착 창
#	define __2008_WHITEDAY_LOVECANDY		__ON	//2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__ON	//2008 계급경험의 모래시계
#	define __PAYZONE_PASS_TICKET			__ON
#	define __QUEST_RENEWAL					__OFF	//퀘스트 리뉴얼
#	define __EFFECT_RENEWAL					__ON
#	define __SECOND_RARE_SKILL				__ON
#	define __080405_FIREST_UI_UPDATE		__ON
#	define __LIMITED_ITEM_UNISEX			__ON
#	define __FAST_TRANSFORTER				__ON
#	define __CLOVER_EVENT					__ON
#	define __QUEST_RENEWAL2					__ON
#	define __GORE_BLOOD_BIBLE				__OFF	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__OFF	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정

#	define __TIPOJYU_CASTLE					__OFF
#	define __2008_JUNE_EVENT_EX_STONE		__OFF	// 경험의 돌 이벤트(13일의 금요일)
#	define __NEW_ADVANCEMENT_ITEM			__OFF	// 새로운 승직아이템
#	define __TUNING_ITEM					__OFF	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__OFF	// 일본 아이템 역개발
#	define __CHINESE_UI						__ON	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__OFF	//일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__OFF	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND_TRANSFORTER				__OFF	// 신규이동수단 2차
#	define __ADD_MAGICPROTECTION			__OFF	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)
#	define __LOCALIZING_LONGNAME			__OFF	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__OFF	//OX이벤트
#	define __ITEM_SHOW						__OFF	// 아이템 보기
#	define __BLITZ_COUPON_EVENT				__OFF	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__OFF	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__OFF	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF
#	define __PREMIUM_GIVE_ITEM_UI			__OFF	// 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트
#	define __2008_OCTOBER_MAKETITEM			__OFF	// 10월 유료 아이템
#	define __PREMIUM_CARD					__OFF	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__OFF	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__OFF	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__OFF	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__OFF	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__OFF	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__OFF	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__OFF	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__ON	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__OFF	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__OFF	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__OFF	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__OFF	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__OFF	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__OFF	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__OFF	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.

#	define __HERO_SYSTEM_ITEM				__OFF	
#	define __CONTRIBUTE_SYSTEM_ITEM			__OFF	
#	define __LEVEL_WAR_RENEWAL_ITEM			__OFF	
#	define __MENEGROTH_ITEM					__OFF
#	define __LC_SELECT_WORLD_ERROR			__OFF	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__OFF	// 기능성 펫푸드
#	define __PCITEMINFO						__OFF	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흐텨져 있던 녀석들을 통합
#	define __REPAIRSYSTEM_RENEWAL			__OFF	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__OFF	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__OFF	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__OFF	// 스마트 푸더
#	define __VISION_AMPLE					__OFF	// 비전 앰플
#	define __THIRD_ENCHANT_2				__OFF	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__OFF	// NProtect 넣고 빼기
#	define __PREMIUM_CARD_ADAY				__OFF	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__OFF	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__OFF	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__OFF	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__OFF	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__OFF	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__OFF   // PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자
#	define __ATTACK_SPEED					__OFF	// 어택 스피드 서버에 종속화
#	define __ONIBLA							__OFF	// 오니블라 개발
#	define __FRIEND_ADDITION				__OFF	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__OFF	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__OFF	// 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__OFF	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__OFF   // 다덴마켓 끄기
#	define __CREDIT							__OFF	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__OFF	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__OFF	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03
#	define __IMI_SENDGLASS					__OFF	// IMI용 모래시계
#	define __PET_MASHIMARO					__OFF	// 마시마로
#	define __TREASURE_BOX					__OFF	// 보물상자

#endif	// __DESIGNED_CHINA



//////////////////////////////////////////////////////////////////////////
// Japan

#ifdef __DESIGNED_JAPAN
#define __DESIGN_NAME "Designed Japan"

#	define __ADVANCEMENT_CHARACTER			__ON
#	define __LANGUAGE_CHANGE				__ON

//#	define __POWER_JJANG_ID					__OFF
#	define __USER_GRADE						__OFF
#	define __SMS_SERVICES					__OFF
#	define __RARE_SKILL						__ON
#	define __INTERNATIONAL_PREMIUM_SYSTEM	__OFF

#	define __NEW_SYSTEM_AVAILABILITIES		__OFF
#	define __ENCHANT_REQUIRE_LEVE			__OFF
#	define __ID_PASSWORD					__OFF

#	define __CHAR_DELETE					__ON
#	define __INTERNATIONAL_UI				__OFF
#	define __ENGLISH_STRFILTER				__OFF
#	define __CRYPT							__OFF
#	define __CONECT_LIMITER					__OFF
#	define __PERSNALSHOP_NAMING				__ON

#	define __HERO_SYSTEM					__ON
#	define __CONTRIBUTE_SYSTEM				__ON
#	define __LEVEL_WAR_RENEWAL				__ON
#	define __SERVER_SELECT_REVEWAL			__ON
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__ON
#	define __2008_FIRST_CHARGE_ITEM			__ON	//2008 1차 유료 아이템
#	define __DSG_EFFECT						__ON
#	define __GEAR_SWAP_CHANGE				__ON	//이중 장착 창
#	define __2008_WHITEDAY_LOVECANDY		__ON	//2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__ON	//2008 계급경험의 모래시계
#	define __PAYZONE_PASS_TICKET			__ON
#	define __QUEST_RENEWAL					__OFF	//퀘스트 리뉴얼
#	define __EFFECT_RENEWAL					__ON
#	define __SECOND_RARE_SKILL				__ON
#	define __080405_FIREST_UI_UPDATE		__ON
#	define __LIMITED_ITEM_UNISEX			__ON
#	define __FAST_TRANSFORTER				__ON
#	define __CLOVER_EVENT					__ON
#	define __QUEST_RENEWAL2					__ON
#	define __GORE_BLOOD_BIBLE				__ON	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__ON	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정

#	define __TIPOJYU_CASTLE					__ON
#	define __2008_JUNE_EVENT_EX_STONE		__ON	// 경험의 돌 이벤트(13일의 금요일)
#	define __NEW_ADVANCEMENT_ITEM			__ON	// 새로운 승직아이템
#	define __TUNING_ITEM					__ON	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__ON	// 일본 아이템 역개발
#	define __CHINESE_UI						__OFF	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__ON	// 일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__ON	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND_TRANSFORTER				__ON	// 신규이동수단 2차
#	define __ADD_MAGICPROTECTION			__ON	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)

#	define __LOCALIZING_LONGNAME			__ON	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__ON	// OX이벤트
#	define __ITEM_SHOW						__ON	// 아이템 보기
#	define __BLITZ_COUPON_EVENT				__ON	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__ON	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__ON	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF
#	define __PREMIUM_GIVE_ITEM_UI			__ON	// 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트
#	define __2008_OCTOBER_MAKETITEM			__ON	// 10월 유료 아이템
#	define __PREMIUM_CARD					__ON	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__ON	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__ON	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__ON	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__ON	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__ON	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__ON	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__OFF	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__ON	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__ON	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__ON	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__ON	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__ON	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__ON	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__ON	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__ON	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.

#	define __HERO_SYSTEM_ITEM				__ON	
#	define __CONTRIBUTE_SYSTEM_ITEM			__ON	
#	define __LEVEL_WAR_RENEWAL_ITEM			__ON	
#	define __MENEGROTH_ITEM					__ON
#	define __LC_SELECT_WORLD_ERROR			__ON	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__ON	// 기능성 펫푸드
#	define __PCITEMINFO						__ON	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흐텨져 있던 녀석들을 통합
#	define __REPAIRSYSTEM_RENEWAL			__ON	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__ON	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__ON	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__ON	// 스마트 푸더
#	define __VISION_AMPLE					__ON	// 비전 앰플
#	define __THIRD_ENCHANT_2				__ON	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__ON	// NProtect 넣고 빼기
#	define __PREMIUM_CARD_ADAY				__ON	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__ON	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__ON	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__OFF	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__OFF	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__OFF	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__OFF   // PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자
#	define __ATTACK_SPEED					__OFF	// 어택 스피드 서버에 종속화
#	define __ONIBLA							__OFF	// 오니블라 개발
#	define __FRIEND_ADDITION				__OFF	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__OFF	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__OFF	// 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__OFF	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__ON   // 다덴마켓 끄기
#	define __CREDIT							__ON	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__OFF	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__OFF	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03
#	define __IMI_SENDGLASS					__OFF	// IMI용 모래시계
#	define __PET_MASHIMARO					__OFF	// 마시마로
#	define __TREASURE_BOX					__OFF	// 보물상자

#endif	// __DESIGNED_JAPAN



#ifdef __DESIGNED_ITEMMANIA
#define __DESIGN_NAME "Designed IMI"

#	define __ADVANCEMENT_CHARACTER			__ON
#	define __LANGUAGE_CHANGE				__ON

//#	define __POWER_JJANG_ID					__OFF
#	define __USER_GRADE						__OFF
#	define __SMS_SERVICES					__OFF
#	define __RARE_SKILL						__ON

#	define __INTERNATIONAL_PREMIUM_SYSTEM	__OFF
#	define __NEW_SYSTEM_AVAILABILITIES		__OFF
#	define __ENCHANT_REQUIRE_LEVE			__OFF
#	define __ID_PASSWORD					__OFF

#	define __CHAR_DELETE					__ON
#	define __INTERNATIONAL_UI				__OFF
#	define __ENGLISH_STRFILTER				__OFF
#	define __CRYPT							__OFF
#	define __CONECT_LIMITER					__OFF
#	define __PERSNALSHOP_NAMING				__ON
#	define __HERO_SYSTEM					__ON
#	define __CONTRIBUTE_SYSTEM				__ON
#	define __LEVEL_WAR_RENEWAL				__ON
#	define __SERVER_SELECT_REVEWAL			__ON
#	define __MENEGROTH						__ON
#	define __GPS_ADD						__OFF
#	define __2008_NEW_YEAR_EVENT			__ON
#	define __2008_FIRST_CHARGE_ITEM			__ON	// 2008 1차 유료 아이템
#	define __DSG_EFFECT						__ON
#	define __GEAR_SWAP_CHANGE				__ON	//								본섭,	테섭,	개발섭
#	define __2008_WHITEDAY_LOVECANDY		__ON	// 2008 화이트데이아이템
#	define __080327_ORDINIS_VAPOR			__ON	// 2008 계급경험의 모래시계				테섭,	개발섭
#	define __PAYZONE_PASS_TICKET			__ON	// 유료존 입장권
#	define __QUEST_RENEWAL					__OFF	//										테섭,	개발섭
#	define __EFFECT_RENEWAL					__ON	//								본섭,	테섭,	개발섭
#	define __SECOND_RARE_SKILL				__ON	//										테섭,	개발섭
#	define __080405_FIREST_UI_UPDATE		__ON	//										테섭,	개발섭
#	define __LIMITED_ITEM_UNISEX			__ON	
#	define __FAST_TRANSFORTER				__ON	
#	define __CLOVER_EVENT					__ON
#	define __QUEST_RENEWAL2					__ON	// __QUEST_RENEWAL은 미리 패치 되었고 __QUEST_RENEWAL2은 수정 중에 있는 내용이다.
#	define __GORE_BLOOD_BIBLE				__ON	// 피의성서 등급에 따른 아이템이미지
#	define __AN_ADVANCEMENT_GUN_REFLAME		__ON	// 비승직슬에이어 총구화염이펙트(기본,제빙베인,블릿오브라이트)좌표 수정

#	define __TIPOJYU_CASTLE					__ON
#	define __2008_JUNE_EVENT_EX_STONE		__ON	// 경험의 돌 이벤트(13일의 금요일)
#	define __NEW_ADVANCEMENT_ITEM			__ON	// 새로운 승직아이템
#	define __TUNING_ITEM					__ON	// 튜닝아이템
#	define __JAPAN_REVERSE_DEVELOP			__OFF	// 일본 아이템 역개발
#	define __CHINESE_UI						__OFF	// 중국 폰트 키우기와 UI변경내용 적용
#	define __JAPAN_UI						__OFF	// 일본 UI
#	define __NEW_PET_FIRE_ELEMENTAL			__ON	// 신규마켓 아이템(파이어 엘레멘탈)
#	define __SECOND_TRANSFORTER				__ON	// 신규이동수단 2차
#	define __ADD_MAGICPROTECTION			__ON	// 마법방어력 추가 - 액세서리류 아이템에 추가 (코어잽류와 승직아이템 제외)
#	define __AUTO_ITEM_ROOTING				__OFF	// 아이템 자동 루팅 코드(테스트 코드의 가까움)
#	define __LOCALIZING_LONGNAME			__OFF	// 일본 로컬라이징시 몬스터 이름을 출력하는데 긴 이름을 출력하기 위한 배려
#	define __OX_EVENT						__ON	// OX이벤트
#	define __ITEM_SHOW						__ON	// 아이템 보기

#	define __BLITZ_COUPON_EVENT				__ON	// 블리츠서버 쿠폰 이벤트
#	define __TITLE_UI_RENWEAL				__ON	// 타이틀 UI 리뉴얼(2008. 08 블리츠)
#	define __LOGIN_PACKET					__OFF	// 스피드 핵을 막기 위해 클라이언트를 다운 받을 수 있도록 패킷으로 막았던 부분(2008.09.08)
#	define __PET_FOOD_ITEM_ADD				__ON	// 맥스푸더 추가(2008.09.08)
#	define __DYNAMIC_UI						__OFF	// 
#	define __PREMIUM_GIVE_ITEM_UI			__ON	// 프리미엄 지급 아이템 UI
#	define __2008_AUTUMN_LEAVES_EVENT		__ON	// 가을 낙엽 이벤트			2008.10.1 패치
#	define __2008_OCTOBER_MAKETITEM			__ON	// 10월 유료 아이템
#	define __PREMIUM_CARD					__ON	// 프리미엄 카드(__INTERNATIONAL_PREMIUM_SYSTEM에서 분리)
#	define __EXTREME_FORCE_SCROLL			__ON	// 프리미엄 지급아이템 "익스트림 포스 스크롤"
#	define __PREMIUM_LOTTER					__ON	// 프리미엄 지급아이템 "프리미엄 복권"
#	define __TIPOJYU_CASTLE_ITEM			__ON	// 티포쥬 컨텐츠는 둔채로 아이템 인덱스만 추가해야 할 경우가 생겨 생긴 필터 차후에 유사한 필터가 많이 생길지도...
#	define __HALLOWEEN						__ON	// 일본 할로윈 이벤트 아이템 바로 본섭도 넣어야 될텐데.
#	define __SECOND_TRANSFORTER_ITEM		__ON	// 일본 두번째빠른 이동수단 아이템만 컨텐츠로 분리
#	define __GLOBAL_NPC						__ON	// npc소환
#	define __MODIFY_FORCE_SCROLL			__ON	// 오딘 포스스크롤과 티투오스 포스스크롤 중첩가능하게 수정
#	define __MOVE_ZONE						__ON	// 개발자 & 운영자의 편의를 위한 맵 이동하는 기능추가
#	define __PREMIUM_CARD_D					__ON	// Premium Card D 추가
#	define __NEW_PET_INCUBUS				__ON	// 인큐버스 펫 추가
#	define __EXPERT_PET_CHANGER				__ON	// 익스퍼트 펫 체인저 추가
#	define __PET_VISION_AMPLE				__ON	// 펫 아이템 비젼 엠플
#	define __CHECKTIME_CONTROLL				__ON	// 스핵 관련하여 시간체크 패킷을 자주 보내도록 한다.
#	define __PREMIUM_CARD_C					__ON	// 프리미엄 카드 C기존에 아이템빈공간을 서버에서 미리 잡아 놓아 었기 때문에 
#	define __TIME_STAMP						__ON	// 소켓 출력에 추가된 코드(이 코드에 따라 로그인 접속이 되지 않을수 있다.) ID: shootkj, 날짜: 2008.12.18
#	define __CHRISTMAS_SOCKS_EVENT			__ON	// 크리스마스 양말 조각을 위시한 이벤트
#	define __EVENTTREE_PILE					__ON	// 이벤트 트리를 중첩 가능하냐 마냐를 온오프 하는 코드 블리츠이벤트가 ON되면 같이 ON 되야 한다.

#	define __HERO_SYSTEM_ITEM				__ON	
#	define __CONTRIBUTE_SYSTEM_ITEM			__ON	
#	define __LEVEL_WAR_RENEWAL_ITEM			__ON	
#	define __MENEGROTH_ITEM					__ON
#	define __LC_SELECT_WORLD_ERROR			__ON	// 서버선택에 오류가 생기면 오는 패킷 추가 국외 버젼 때문에 추가
#	define __FUNCTION_PETFOOD				__ON	// 기능성 펫푸드
#	define __PCITEMINFO						__ON	// PCItemInfo를 통해서 아이템 정보를 주고 받기 위해서 기존에 흩어져 있던 녀석들을 통합

#	define __REPAIRSYSTEM_RENEWAL			__ON	// 수리가격을 서버에 종속적으로 변경 한다.
#	define __DCGLOBAL_NPC					__ON	// 디스카운터 NPC호출 아이템
#	define __GAMEOPTION_ITEMBAY_MESSAGEBOX	__ON	// 아이템을 팔때 아이템을 "팔겠냐?"는 메세지 박스를 게임 옵션에서 설정
#	define __WORLDCHATING					__ON	// 월드간 채팅 시스템
#	define __FUNCTION_PETFOOD_SMART			__ON	// 스마트 푸더
#	define __VISION_AMPLE					__ON	// 비전 앰플
#	define __THIRD_ENCHANT_2				__ON	// 서드인첸트 2
#	define __XTRAP							__OFF	// XTrap 넣고 빼기 물론 lib파일 등을 이동 시켜야 하는건 당연 지사
#	define __NPROTECT						__ON	// NProtect 넣고 빼기
#	define __PREMIUM_CARD_ADAY				__ON	// 프리미엄 카드 A Day - 새로 추가된 날짜별 프리미엄 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_A				__ON	// 프리미엄 복권 A - 새로 추가된 복권 카드 - 2009.04.28
#	define __PREMIUM_LOTTER_B				__ON	// 프리미엄 복권 B - 새로 추가된 복권 카드 - 2009.04.29
#	define __ADVANCEMENT_NEW_UNIQUE_ITEM	__ON	// 승직유니크 아이템 - 2009.05.14
#	define __HIDDEN_BOSS_SYSTEM				__ON	// 히든 보스 나오는 시스템
#	define __CHANGE_PET_INFO_WINDOW_IMAGE	__ON	// 펫 정보창 이미지 변경 - 2009.05.21
#	define __PET_MIXINGFORGE				__ON	// 펫 조합기,조합 취소기
#	define __PCITEMINFO2					__ON	// PCItemInfo통합 2번째 빨리 해버리고 1과 통합해버리자
#	define __ATTACK_SPEED					__ON	// 어택 스피드 서버에 종속화
#	define __ONIBLA							__ON	// 오니블라 개발
#	define __FRIEND_ADDITION				__ON	// 친구 추가 컨텐츠 - 2009.06.12
#	define __ONIBLA_ITEM					__ON	// 오닐블라 컨텐츠 아이템 - 2009.06.29
#	define __AUTO_ATTACT					__ON	// 자동공격 몬스터에만 사용 가능하도록 한다.
#	define __IMI_INTERFACE					__ON	// IMI 인터페이스
#	define __DARKEDEN_MARKET				__OFF   // 다덴마켓 끄기
#	define __CREDIT							__OFF	// 크레디트 끄기
#	define __15_PVP_INHIBITION				__OFF	// 만15세이하 pvp서버 접속 금지(12세이용가 심의용)
#	define __SUMMER_VACTATION_COOL_EVENT	__ON	// 여름방학 썸머 스페셜 - 쿨 썸머 이벤트 - 2009.07.27
#	define __IMI_LOW_LEVEL_ITEM				__OFF	// IMI용 저급 아이템 - 2009.08.03



//////////////////////////////////////////////////////////////////////////
// Taiwan



//#	define __POWER_JJANG_ID					__OFF






#	define __IMI_SENDGLASS					__ON	// IMI용 모래시계
#	define __PET_MASHIMARO					__ON	// 마시마로
#	define __TREASURE_BOX					__ON	// 보물상자

#endif	// __DESIGNED_TAIWAN




//////////////////////////////////////////////////////////////////////////
// Macro

// 매크로가 define 안되어 있다면 __ON으로 친다.
#define __CONTENTS(f)	((f) == __ON)


#ifdef __DESIGN_NAME
#	pragma message ("Darkeden project \"" __DESIGN_NAME "\"")
#else
#	pragma message ("Darkeden design macro undefined")
#endif#undef __DESIGN_NAME