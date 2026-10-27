
#include "EventItemUtil.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "VariableManager.h"

#include "ItemFactoryManager.h"

#include <list>

Fall2005Item getFall2005ItemKind( PlayerCreature* pPC, Monster* pMonster )
{
	int UserLevel, MonsterLevel = pMonster->getLevel();

	if ( pPC->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
		Assert( pSlayer != NULL );

		UserLevel = pSlayer->getHighestSkillDomainLevel();
	}
	else if ( pPC->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
		Assert( pVampire != NULL );

		UserLevel = pVampire->getLevel();
	}
	else if ( pPC->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
		Assert( pOusters != NULL );

		UserLevel = pOusters->getLevel();
	}
	else return RICE_CAKE_SP_NO_ITEM;

	if ( UserLevel > 100 ) UserLevel = 100;
	if ( UserLevel > MonsterLevel + 20 ) return RICE_CAKE_SP_NO_ITEM;

	if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_DEFENSE_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_DEFENSE;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_PROTECTION_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_PROTECTION;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_ATTACK_SPEED_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_ATTACK_SPEED;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_DAMAGE_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_DAMAGE;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_LUCK_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_LUCK;
	}
	else if ( rand() % g_pVariableManager->getVariable( EVENT_FALL_2005_VISION_RATIO ) == 0 )
	{
		return RICE_CAKE_SP_VISION;
	}

	return RICE_CAKE_SP_NO_ITEM;
}

Item* getFall2005Item( Fall2005Item kind )
{
	Item* ret = NULL;

	switch ( kind )
	{
		case RICE_CAKE_SP_NO_ITEM:
			break;

		case RICE_CAKE_SP_DEFENSE:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 26, list<OptionType_t>() );
			break;

		case RICE_CAKE_SP_PROTECTION:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 27, list<OptionType_t>() );
			break;

		case RICE_CAKE_SP_ATTACK_SPEED:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 28, list<OptionType_t>() );
			break;

		case RICE_CAKE_SP_DAMAGE:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 29, list<OptionType_t>() );
			break;

		case RICE_CAKE_SP_LUCK:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 30, list<OptionType_t>() );
			break;

		case RICE_CAKE_SP_VISION:
			ret = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_STAR, 31, list<OptionType_t>() );
			break;

		default:
			break;
	}

	return ret;
}

