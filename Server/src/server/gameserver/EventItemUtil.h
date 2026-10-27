#ifndef __EVENT_ITEM_UTIL_H__
#define __EVENT_ITEM_UTIL_H__

#include "Types.h"
#include "Exception.h"

class Item;
class PlayerCreature;
class Monster;

enum MoonCard
{
	NO_CARD,
	FULL_MOON,
	OLD_MOON,
	HALF_MOON,
	NEW_MOON
};

MoonCard getCardKind( PlayerCreature* pPC, Monster* pMonster );
Item* getCardItem( MoonCard card );

enum LuckyBag
{
	NO_LUCKY_BAG,
	GREEN_LUCKY_BAG,
	BLUE_LUCKY_BAG,
	GOLD_LUCKY_BAG,
	RED_LUCKY_BAG
};

LuckyBag getLuckyBagKind( PlayerCreature* pPC, Monster* pMonster );
Item* getLuckyBagItem( LuckyBag luckybag );

enum GiftBox
{
	NO_GIFT_BOX,
	RED_GIFT_BOX,
	BLUE_GIFT_BOX,
	GREEN_GIFT_BOX,
	YELLOW_GIFT_BOX
};

GiftBox getGiftBoxKind( PlayerCreature* pPC, Monster* pMonster );
Item* getGiftBoxItem( GiftBox giftbox );

int getBlackGiftBoxType( int t1, int t2);
bool canGiveEventItem( PlayerCreature* pPC, Monster* pMonster );

enum NewYear2005Item
{
	NO_ITEM,
	BOKJORY,
	EVENT_RED_RICE_CAKE_SOUP,
	EVENT_GREEN_RICE_CAKE_SOUP,
	EVENT_BLUE_RICE_CAKE_SOUP,
	EVENT_BLACK_RICE_CAKE_SOUP,
	EVENT_MUGWORT_RICE_CAKE_SOUP,
	
	//20090114 wlzzi - 09년 신년이벤트 보상아이템으로 추가된 녀석들
	EXPERIENCE_STONE,
	CRISTAL,
	BLUEGREEN_CORAL,
	GREEN_CORAL,
	RED_CORAL,
	BLUE_CORAL,
	RESURRECTION_SCROLL,
	ELIXIR_SCROLL,
};

NewYear2005Item getNewYear2005ItemKind( PlayerCreature* pPC, Monster* pMonster );
Item* getNewYear2005Item( NewYear2005Item kind );

enum Fall2005Item
{
	RICE_CAKE_SP_NO_ITEM,
	RICE_CAKE_SP_DEFENSE,
	RICE_CAKE_SP_PROTECTION,
	RICE_CAKE_SP_ATTACK_SPEED,
	RICE_CAKE_SP_DAMAGE,
	RICE_CAKE_SP_LUCK,
	RICE_CAKE_SP_VISION,
};

Fall2005Item getFall2005ItemKind( PlayerCreature* pPC, Monster* pMonster );
Item* getFall2005Item( Fall2005Item kind );


void logEventItemCount(Item* pEventItem) throw(Error);

#endif// __EVENT_ITEM_UTIL_H__

