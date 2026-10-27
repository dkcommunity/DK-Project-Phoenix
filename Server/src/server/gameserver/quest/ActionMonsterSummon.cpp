////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionPcGetItem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionMonsterSummon.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "ItemFactoryManager.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "ZoneInfo.h"
#include "PacketUtil.h"
#include "Effect.h"
#include <stdio.h>
#include <cstdio>
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCOtherModifyInfo.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include "StringPool.h"
#include "ZoneGroup.h"
#include "Assert.h"
#include "MonsterCorpse.h"
#include "MonsterInfo.h"
#include "DB.h"
#include "Tile.h"
#include "Player.h"
#include "MonsterManager.h"
#include "Thread.h"
#include "CreatureUtil.h"
#include "MonsterNameManager.h"
#include "Properties.h"
#include "RelicUtil.h"
#include <fstream>
using namespace std;
#include "Viewable.h"
#include "Properties.h"
#include "Item.h"
#include "ItemInfoManager.h"
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////


void ActionMonsterSummon::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try 
	{
		m_Monster = propertyBuffer.getPropertyInt("Monster");
		m_Monster2 = propertyBuffer.getPropertyInt("DeleteMonsterr");
		m_X = propertyBuffer.getPropertyInt("X");
		m_Y = propertyBuffer.getPropertyInt("Y");
		m_Num = propertyBuffer.getPropertyInt("Num");
		m_Time = propertyBuffer.getPropertyInt("Time");
		m_ItemClass = propertyBuffer.getPropertyInt("NeedItemClass");
		m_ItemType = propertyBuffer.getPropertyInt("NeedItemType");
		m_ItemNum = propertyBuffer.getPropertyInt("NeedItemNum");
	} 
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
	
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionMonsterSummon::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY
	
	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );

	Player* pPlayer = pCreature2->getPlayer();
	Assert( pPlayer != NULL );

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature2->getPlayer());
	Assert( pGamePlayer != NULL );
	
	Assert( m_pZone != NULL );
	Zone* m_pZone = pCreature2->getZone();
	
	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
	Assert( pZoneInfo != NULL );
	
	SpriteType_t	SpriteType	= 0;
	MonsterType_t   MonsterType = 0;
	int 			MonsterNum	= 0;
			
	MonsterType = m_Monster;
	MonsterNum = m_Num;
	SUMMON_INFO summonInfo;
	

	if (pPC->getItemClassTypeNum( (Item::ItemClass)m_ItemClass, m_ItemType ) >= m_ItemNum)
		{
	if (pGamePlayer!=NULL)
	{
			const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(m_Monster);
			const MonsterInfo* pMonsterInfo2 = g_pMonsterInfoManager->getMonsterInfo(m_Monster2);
			
			Creature* pCreature1 = pGamePlayer->getCreature();
			Zone* pZone = pCreature1->getZone();
			MonsterManager* pMM = pZone->getMonsterManager();
			
			map< ObjectID_t, Creature* > & monsters = pMM->getCreatures();
			map< ObjectID_t, Creature* >::iterator itr = monsters.begin();
			
	
		

			/*	for (; itr==monsters.end(); itr++)
			{
									char str[80];	
		sprintf(str, "%s   맵에 없다 11 ",pMonsterInfo2->getHName().c_str());

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
			gcSystemMessage.setMessage(str);
					g_pZoneGroupManager->broadcast( &gcSystemMessage );
					//cout << "111111111: " << pMonster->getMonsterType() << endl;
					return;
			}
*/
			for (; itr!=monsters.end(); itr++)
			{
				Creature* pCreature1 = itr->second;
				Monster* pMonster = dynamic_cast<Monster*>(pCreature1);
				
			if ( pMonster->getMonsterType() == m_Monster )
				{
														char str[80];	
		sprintf(str, "%s   이미 있다  ",pMonsterInfo->getHName().c_str());

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
			gcSystemMessage.setMessage(str);
					g_pZoneGroupManager->broadcast( &gcSystemMessage );
					cout << "33333333333: " << pMonster->getMonsterType() << endl;
					return;
			}
				if ( pMonster->getMonsterType() == m_Monster2 )
				{				
										char str[80];	
		sprintf(str, "%s %s %s 소환 완료 ", pCreature2->getName().c_str(),pZoneInfo->getFullName().c_str(),pMonsterInfo->getHName().c_str());

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
			gcSystemMessage.setMessage(str);
					g_pZoneGroupManager->broadcast( &gcSystemMessage );
					cout << "addEnemy11111: " << pMonster->getMonsterType() << endl;
			//cout << "test2=" << (int)m_pZone->getZoneID() << endl;
				pPC->decreaseItemClassTypeNum( (Item::ItemClass)m_ItemClass, m_ItemType, m_ItemNum );
					
					if (pMonster != NULL && !pMonster->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
					{
						pMonster->setHP( 0 );
					}
						Zone* pZone = pCreature2->getZone();
						Coord_t ZoneX = m_X;
						Coord_t ZoneY = m_Y;
						SpriteType_t	SpriteType	= 0;

					if (!(pZone->getZoneLevel() & SAFE_ZONE))
					{
						if (SpriteType!=0)
						{
							g_pMonsterInfoManager->getMonsterTypeBySprite(SpriteType);
							addMonstersToZone(pZone, ZoneX, ZoneY, SpriteType, MonsterType, MonsterNum, summonInfo);
						}
						else if (MonsterType!=0)
						{
							g_pMonsterInfoManager->getMonsterInfo(MonsterType);
							addMonstersToZone(pZone, ZoneX, ZoneY, SpriteType, MonsterType, MonsterNum, summonInfo);
						}
						break;
					}
				}
						else
				{
					char str[80];	
		sprintf(str, "%s   맵에 없다 22 ",pMonsterInfo2->getHName().c_str());

			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
			gcSystemMessage.setMessage(str);
					g_pZoneGroupManager->broadcast( &gcSystemMessage );
					cout << "2222222222: " << pMonster->getMonsterType() << endl;
				return;
				}
			}
						
	}
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionMonsterSummon::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionSummonMonster("
	    << ")";

	return msg.toString();

	__END_CATCH
}