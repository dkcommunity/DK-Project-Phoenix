
#include "EventItemUtil.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "VariableManager.h"

#include "ItemFactoryManager.h"

#include <list>

NewYear2005Item getNewYear2005ItemKind( PlayerCreature* pPC, Monster* pMonster )
{
//20090120 wlzzi - 신년이벤트 보상 기준 추가 -- 레벨차이에 따른 제한
	//	비승직일 경우 : 레벨차 +- 30
	//	승직일 경우 : 몬스터 레벨이 130 이상일 때
	if (!pPC) return NO_ITEM;
	
	Level_t userLevel;
	Level_t MonsterLevel = pMonster->getLevel();
	if( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );
		userLevel = pSlayer->getLevel();
	}
	else if( pPC->isVampire() )
	{
		Vampire* pVampire= dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );
		userLevel = pVampire->getLevel();
	}
	else if( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );
		userLevel = pOusters->getLevel();
	}
	
	if (pPC->isAdvanced()) //승직 캐릭터 일 때,
	{
		if (MonsterLevel < 140)	//몬스터가 140미만이면 불가!
		{
			return NO_ITEM;
		}
	}
	else 					// 비승직 캐릭터 일 때,
	{
		if ((userLevel + 20 < MonsterLevel) || (userLevel - 20 > MonsterLevel))	//몬스와의 레벨차가 20보다 크면 불가!
		{
			return NO_ITEM;
		}
	}
//20090114 wlzzi - 09년 신년이벤트 //기존 2005년 신년이벤트 코드를 재활용하여, 매년 신년 이벤트 시~ 사용하도록 한다.
//			- 보상아이템 목록등이 매년 변경될 수 있으므로, 기존 2005년신년 이벤트를 위해 AttrInfo에 추가한 설정정보 중, 이벤트 ON/OFF를 위한 것만 재활용한다.
//			- AttrInfo :: 128 (새해 맞이 이벤트 2005) 만 재활용. 그외, 129, 130, 131, 132, 133, 134 은 실질적으로 폐기처분.
// -- 2009년도 신년이벤트 보상아이템 --
	static NewYear2005Item GiveItems[12] = {EXPERIENCE_STONE, CRISTAL, BLUEGREEN_CORAL, GREEN_CORAL, RED_CORAL, BLUE_CORAL, EVENT_GREEN_RICE_CAKE_SOUP,
			EVENT_RED_RICE_CAKE_SOUP, EVENT_BLUE_RICE_CAKE_SOUP, BOKJORY, ELIXIR_SCROLL, RESURRECTION_SCROLL };
	static string EventItemNames[12] = {"Experience Stone", "Cristal", "Bluegreen Coral", "Green Coral", "Red Coral", "Blue Coral", "Green RiceCakeSoup",
			"Red RiceCakeSoup", "Blue RiceCakeSoup", "Bokjory", "Elixir Scroll", "Resurrection Scroll" };

	static int GiveItemRatios[12] = {102400, 51200, 25600, 10000, 9200, 6000, 2000, 1600, 1200, 800, 600, 400};
//	static int GiveItemRatios[12] = {51200, 25600, 12800, 5000, 4600, 3000, 1000, 800, 600, 400, 300, 200};
//	int GiveItemRatios[12] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
	
	for (int i=0; i<12; i++)
	{
		if (rand() % GiveItemRatios[i] == 0)
		{
			filelog("2009NewyearEventLog.txt", "%s 캐릭터(Level=%d)가  [MonsterType = %d, MonsterLevel = %d]로부터, %s아이템을 획득함 ", pPC->getName().c_str(), userLevel, pMonster->getMonsterType(), MonsterLevel, EventItemNames[i].c_str());
			return GiveItems[i];
		}
	}
/*	
	if ( rand() % 51200 == 0 )	//경험의 돌
	{
		return EXPERIENCE_STONE;
	}
	else if ( rand() % 25600 == 0 )	//크리스탈
	{
		return CRISTAL;
	}
	else if ( rand() % 12800 == 0 )	//블루그린 코랄
	{
		return BLUEGREEN_CORAL;
	}
	else if ( rand() % 5000 == 0 )	//그린 코랄
	{
		return GREEN_CORAL;
	}
	else if ( rand() % 4600 == 0 )	//레드 코랄
	{
		return RED_CORAL;
	}
	else if ( rand() % 3000 == 0 )	//블루 코랄
	{
		return BLUE_CORAL;
	}
	else if ( rand() % 1000 == 0 )	//녹색 떡국
	{
		return EVENT_GREEN_RICE_CAKE_SOUP;
	}
	else if ( rand() % 800 == 0 )	//빨간 떡국
	{
		return EVENT_RED_RICE_CAKE_SOUP;
	}
	else if ( rand() % 600 == 0 )	//파란 떡국
	{
		return EVENT_BLUE_RICE_CAKE_SOUP;
	}
	else if ( rand() % 400 == 0 )	//복조리
	{
		return BOKJORY;
	}
	else if ( rand() % 300 == 0 )	//엘릭서 스크롤
	{
		return ELIXIR_SCROLL;
	}
	else if ( rand() % 200 == 0 )	//부활 스크롤
	{
		return RESURRECTION_SCROLL;
	}
*/
	
// -- 2005년도 신년이벤트 보상아이템 --
/*
	if ( rand() % g_pVariableManager->getVariable( BOKJORY_RATIO ) == 0 )
	{
		return BOKJORY;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_BLACK_RICE_CAKE_SOUP_RATIO ) == 0 )
	{
		return EVENT_BLACK_RICE_CAKE_SOUP;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_MUGWORT_RICE_CAKE_SOUP_RATIO ) == 0 )
	{
		return EVENT_MUGWORT_RICE_CAKE_SOUP;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_RED_RICE_CAKE_SOUP_RATIO ) == 0 )
	{
		return EVENT_RED_RICE_CAKE_SOUP;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_GREEN_RICE_CAKE_SOUP_RATIO ) == 0 )
	{
		return EVENT_GREEN_RICE_CAKE_SOUP;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_BLUE_RICE_CAKE_SOUP_RATIO ) == 0 )
	{
		return EVENT_BLUE_RICE_CAKE_SOUP;
	}
*/
	return NO_ITEM;
}

Item* getNewYear2005Item( NewYear2005Item kind )
{
	Item* ret = NULL;

	switch ( kind )
	{
		case NO_ITEM:
			break;

		case BOKJORY:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_MIXING_ITEM, 18, list<OptionType_t>() );
			break;

		case EVENT_RED_RICE_CAKE_SOUP:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 17, list<OptionType_t>() );
			break;

		case EVENT_GREEN_RICE_CAKE_SOUP:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 18, list<OptionType_t>() );
			break;

		case EVENT_BLUE_RICE_CAKE_SOUP:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 19, list<OptionType_t>() );
			break;

		case EVENT_BLACK_RICE_CAKE_SOUP:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 20, list<OptionType_t>() );
			break;

		case EVENT_MUGWORT_RICE_CAKE_SOUP:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 21, list<OptionType_t>() );
			break;
//20090114 wlzzi - 2009년 신년이벤트에 보상아이템으로 추가된 녀석들
		case EXPERIENCE_STONE:				//경험의 돌
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EFFECT_ITEM, 47, list<OptionType_t>() );
			break;
		case CRISTAL:						//크리스탈
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 58, list<OptionType_t>() );
			break;
		case BLUEGREEN_CORAL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 53, list<OptionType_t>() );
			break;
		case GREEN_CORAL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 50, list<OptionType_t>() );
			break;
		case RED_CORAL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 49, list<OptionType_t>() );
			break;
		case BLUE_CORAL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 51, list<OptionType_t>() );
			break;
		case RESURRECTION_SCROLL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 0, list<OptionType_t>() );
			break;
		case ELIXIR_SCROLL:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_RESURRECT_ITEM, 1, list<OptionType_t>() );
			break;
//-			
		default:
			break;
	}

	return ret;
}

