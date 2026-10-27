//////////////////////////////////////////////////////////////////////////////
// Filename    : Tanning.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Tanning.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "OptionInfo.h"
#include "Effect.h"
#include "Inventory.h"
#include "VariableManager.h"
#include "ItemInfoManager.h"
#include "ItemFactoryManager.h"
#include "VampireCorpse.h"
#include "AlignmentManager.h"
#include "PCVampireInfo3.h"
#include "MonsterCorpse.h"
#include "ItemUtil.h"
#include "PacketUtil.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCRemoveCorpseHead.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCDeleteInventoryItem.h"
#include "Gpackets/GCSystemMessage.h"
#include "StringPool.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 타일 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType Tanning::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSlayerSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pSlayer != NULL );

	try
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pSlayer);
		Inventory* pInventory = pSlayer->getInventory();

		Assert( pPlayer != NULL );
		Assert( pZone != NULL );
		Assert( pPC != NULL );
		Assert( pInventory != NULL );

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		Item* pItem = NULL;
		//MonsterCorpse* pCorpse = NULL;

		bool bRangeCheck = verifyDistance(pSlayer, X, Y, 1);
		bool bNeedItem = false;
		_TPOINT pt;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.hasItem()) pItem = tile.getItem();
		}

		bool bTanningAbleCorpse = false;

		if ( pItem != NULL
		  && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE
		  && pItem->getItemType() == MONSTER_CORPSE )
		{
			MonsterCorpse* pCorpse = dynamic_cast<MonsterCorpse*>(pItem);
			Assert( pCorpse != NULL );

			// 준저터널 1,2 층이여야만 한다.
			// 노드코피라, 레이저 트라슬라, 푸스카 (몬스터 종류)
			if ( !pCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE)
			  && !pCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE) 
			  && ( pZone->getZoneID() == 1611 || pZone->getZoneID() == 1612 )
			  && (pCorpse->getMonsterType() == 771 || pCorpse->getMonsterType() == 775 || pCorpse->getMonsterType() == 772 || pCorpse->getMonsterType() == 774)
			  && pCorpse->gethasHead()
			  && !pCorpse->isShrine() )
			{
				bTanningAbleCorpse = true;
			}
		}

		// 스킬 사용시에 필요한 아이템을 체크 한다.
		if( pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1) )
		{
			bNeedItem = true;
		}

		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 4 );
		Assert( pItemInfo != NULL );

		Item* pGiveItem = NULL;

		GCSystemMessage	gcSystemMessage;

		if ( bRangeCheck && bTanningAbleCorpse && bNeedItem )
		{
			Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);
			Assert(pCorpse != NULL);

			Range_t Range = 1;	// 항상 1이다.

			// 스킬 사용에 필요한 아이템을 소모하여 준다.
			pInventory->decreaseNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1, pPlayer );

			int ratio = rand()%100000;
			if( ratio < 35000 )
			{
				ItemType_t giveItemType = 5;
				if( ratio < 10000 )
				{
					giveItemType = 4;
				}

				int needItemNum = 1;

				list<OptionType_t> options;
				pGiveItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), giveItemType, options );
				Assert( pGiveItem != NULL );

				pGiveItem->setNum(needItemNum);

				if( !pInventory->findAddStackItem( pGiveItem, pt ) )
				{
					// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
					if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
					{
						executeSkillFailException(pSlayer, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE) ); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}

					pZone->registerObject( pGiveItem );

					if( pInventory->addItem( pGiveItem, pt ) )
					{
						GCModifyInformation gcMI;

						pGiveItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

						remainTraceLog( pGiveItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
					}
					else
					{
						pInventory->deleteItem( pGiveItem->getObjectID() );
						executeSkillFailException(pSlayer, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_FAILED_CAST_SKILL)); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}
				}
				else
				{
					pGiveItem = pInventory->findItem( pGiveItem->getItemClass(), pGiveItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
					Assert(pGiveItem != NULL);

					GCDeleteInventoryItem gcDI;
					gcDI.setObjectID( pGiveItem->getObjectID() );
					pPlayer->sendPacket( &gcDI );

					pGiveItem->setNum( pGiveItem->getNum() + needItemNum );

					pGiveItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
				}

				GCCreateItem gcCreateItem;
				makeGCCreateItem(&gcCreateItem, pGiveItem, pt.x, pt.y);

				if( giveItemType == 4 )
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_CLEAN_SKIN) ); // 20070814
				}
				else 
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_HURT_SKIN) ); // 20070814
				}
				pPlayer->sendPacket(&gcSystemMessage);

				pPlayer->sendPacket( &gcCreateItem );
			}
			else
			{
				gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_GET_SKIN) ); // 20070814
				pPlayer->sendPacket(&gcSystemMessage);
			}

			ZoneCoord_t myX = pSlayer->getX();
			ZoneCoord_t myY = pSlayer->getY();

			_GCSkillToTileOK1.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(0);
			_GCSkillToTileOK1.setRange(Range);

			_GCSkillToTileOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK3.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(0);
			_GCSkillToTileOK4.setRange(Range);
			
			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK5.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(0);
			_GCSkillToTileOK5.setRange(Range);

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pSlayer, &_GCSkillToTileOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToTileOK1);

			list<Creature*> cList;
			cList.push_back(pSlayer);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			// 모든 정리작업이 끝이 났으면 시체를 삭제한다.
			// 사라지는 패킷을 날린다.
			GCDeleteObject gcDO;
			gcDO.setObjectID(pCorpse->getObjectID());
			pZone->broadcastPacket(X, Y,  &gcDO);
			// 존에서 지운다.
			pZone->deleteItem(pCorpse , X , Y);
			// 존안에서 실제적으로 포인터를 없애지는 않으므로 포인터를 삭제 시켜 줘야 한다.
			SAFE_DELETE(pCorpse);
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailException(pSlayer, getSkillType());
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;
	__END_CATCH
	
}

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 타일 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType Tanning::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	try
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pVampire);
		Inventory* pInventory = pVampire->getInventory();

		Assert( pPlayer != NULL );
		Assert( pZone != NULL );
		Assert( pPC != NULL );
		Assert( pInventory != NULL );

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		Item* pItem = NULL;
		//MonsterCorpse* pCorpse = NULL;

		bool bRangeCheck = verifyDistance(pVampire, X, Y, 1);
		bool bNeedItem = false;
		_TPOINT pt;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.hasItem()) pItem = tile.getItem();
		}

		bool bTanningAbleCorpse = false;

		if ( pItem != NULL
		  && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE
		  && pItem->getItemType() == MONSTER_CORPSE )
		{
			MonsterCorpse* pCorpse = dynamic_cast<MonsterCorpse*>(pItem);
			Assert( pCorpse != NULL );

			// 준저터널 1,2 층이여야만 한다.
			// 노드코피라, 레이저 트라슬라, 푸스카 (몬스터 종류)
			if ( !pCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE)
			  && !pCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE) 
			  && ( pZone->getZoneID() == 1611 || pZone->getZoneID() == 1612 )
			  && (pCorpse->getMonsterType() == 771 || pCorpse->getMonsterType() == 775 || pCorpse->getMonsterType() == 772 || pCorpse->getMonsterType() == 774)
			  && pCorpse->gethasHead()
			  && !pCorpse->isShrine() )
			{
				bTanningAbleCorpse = true;
			}
		}

		// 스킬 사용시에 필요한 아이템을 체크 한다.
		if( pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1) )
		{
			bNeedItem = true;
		}

		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 4 );
		Assert( pItemInfo != NULL );

		Item* pGiveItem = NULL;

		GCSystemMessage	gcSystemMessage;

		if ( bRangeCheck && bTanningAbleCorpse && bNeedItem )
		{
			Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);
			Assert(pCorpse != NULL);

			Range_t Range = 1;	// 항상 1이다.

			// 스킬 사용에 필요한 아이템을 소모하여 준다.
			pInventory->decreaseNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1, pPlayer );

			int ratio = rand()%100000;
			if( ratio < 35000 )
			{
				ItemType_t giveItemType = 5;
				if( ratio < 10000 )
				{
					giveItemType = 4;
				}

				int needItemNum = 1;

				list<OptionType_t> options;
				pGiveItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), giveItemType, options );
				Assert( pGiveItem != NULL );

				pGiveItem->setNum(needItemNum);

				if( !pInventory->findAddStackItem( pGiveItem, pt ) )
				{
					// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
					if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
					{
						executeSkillFailException(pVampire, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE) ); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}

					pZone->registerObject( pGiveItem );

					if( pInventory->addItem( pGiveItem, pt ) )
					{
						GCModifyInformation gcMI;

						pGiveItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

						remainTraceLog( pGiveItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
					}
					else
					{
						pInventory->deleteItem( pGiveItem->getObjectID() );
						executeSkillFailException(pVampire, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_FAILD_SKILL_GET_SKIN) ); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}

				}
				else
				{
					pGiveItem = pInventory->findItem( pGiveItem->getItemClass(), pGiveItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
					Assert(pGiveItem != NULL);

					GCDeleteInventoryItem gcDI;
					gcDI.setObjectID( pGiveItem->getObjectID() );
					pPlayer->sendPacket( &gcDI );

					pGiveItem->setNum( pGiveItem->getNum() + needItemNum );

					pGiveItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
				}
				GCCreateItem gcCreateItem;
				makeGCCreateItem(&gcCreateItem, pGiveItem, pt.x, pt.y);

				pPlayer->sendPacket( &gcCreateItem );

				if( giveItemType == 4 )
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_CLEAN_SKIN) ); // 20070814
				}
				else 
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_HURT_SKIN) ); // 20070814
				}
				pPlayer->sendPacket(&gcSystemMessage);
			}
			else
			{
				gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_GET_SKIN) ); // 20070814
				pPlayer->sendPacket(&gcSystemMessage);
			}

			ZoneCoord_t myX = pVampire->getX();
			ZoneCoord_t myY = pVampire->getY();

			_GCSkillToTileOK1.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(0);
			_GCSkillToTileOK1.setRange(Range);

			_GCSkillToTileOK3.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK3.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(0);
			_GCSkillToTileOK4.setRange(Range);
			
			_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK5.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(0);
			_GCSkillToTileOK5.setRange(Range);

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pVampire, &_GCSkillToTileOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToTileOK1);

			list<Creature*> cList;
			cList.push_back(pVampire);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			// 모든 정리작업이 끝이 났으면 시체를 삭제한다.
			// 사라지는 패킷을 날린다.
			GCDeleteObject gcDO;
			gcDO.setObjectID(pCorpse->getObjectID());
			pZone->broadcastPacket(X, Y,  &gcDO);
			// 존에서 지운다.
			pZone->deleteItem(pCorpse , X , Y);
			// 존안에서 실제적으로 포인터를 없애지는 않으므로 포인터를 삭제 시켜 줘야 한다.
			SAFE_DELETE(pCorpse);
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailException(pVampire, getSkillType());
			gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_FAILD_SKILL_GET_SKIN) ); // 20070814
			pPlayer->sendPacket(&gcSystemMessage);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}
	
	return SKILL_RESULT_FAIL_UNKNOWN;
	__END_CATCH
}

SkillResultType Tanning::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY
	Assert(pOusters != NULL);

	try
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pOusters);
		Inventory* pInventory = pOusters->getInventory();

		Assert( pPlayer != NULL );
		Assert( pZone != NULL );
		Assert( pPC != NULL );
		Assert( pInventory != NULL );

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		Item* pItem = NULL;
		//MonsterCorpse* pCorpse = NULL;

		bool bRangeCheck = verifyDistance(pOusters, X, Y, 1);
		bool bNeedItem = false;
		_TPOINT pt;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.hasItem()) pItem = tile.getItem();
		}

		bool bTanningAbleCorpse = false;

		if ( pItem != NULL
		  && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE
		  && pItem->getItemType() == MONSTER_CORPSE )
		{
			MonsterCorpse* pCorpse = dynamic_cast<MonsterCorpse*>(pItem);
			Assert( pCorpse != NULL );

			// 준저터널 1,2 층이여야만 한다.
			// 노드코피라, 레이저 트라슬라, 푸스카 (몬스터 종류)
			if ( !pCorpse->isFlag(Effect::EFFECT_CLASS_SLAYER_RELIC_TABLE)
			  && !pCorpse->isFlag(Effect::EFFECT_CLASS_VAMPIRE_RELIC_TABLE) 
			  && ( pZone->getZoneID() == 1611 || pZone->getZoneID() == 1612 )
			  && (pCorpse->getMonsterType() == 771 || pCorpse->getMonsterType() == 775 || pCorpse->getMonsterType() == 772 || pCorpse->getMonsterType() == 774)
			  && pCorpse->gethasHead()
			  && !pCorpse->isShrine() )
			{
				bTanningAbleCorpse = true;
			}
		}

		// 스킬 사용시에 필요한 아이템을 체크 한다.
		if( pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1) )
		{
			bNeedItem = true;
		}

		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 4 );
		Assert( pItemInfo != NULL );

		Item* pGiveItem = NULL;

		GCSystemMessage	gcSystemMessage;

		if ( bRangeCheck && bTanningAbleCorpse && bNeedItem )
		{
			Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);
			Assert(pCorpse != NULL);

			Range_t Range = 1;	// 항상 1이다.

			// 스킬 사용에 필요한 아이템을 소모하여 준다.
			pInventory->decreaseNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 3, 1, pPlayer );

			int ratio = rand()%100000;
			if( ratio < 35000 )
			{
				ItemType_t giveItemType = 5;
				if( ratio < 10000 )
				{
					giveItemType = 4;
				}

				int needItemNum = 1;

				list<OptionType_t> options;
				pGiveItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), giveItemType, options );
				Assert( pGiveItem != NULL );

				pGiveItem->setNum(needItemNum);

				if( !pInventory->findAddStackItem( pGiveItem, pt ) )
				{
					// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
					if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
					{
						executeSkillFailException(pOusters, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE) ); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}

					pZone->registerObject( pGiveItem );

					if( pInventory->addItem( pGiveItem, pt ) )
					{
						GCModifyInformation gcMI;

						pGiveItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

						remainTraceLog( pGiveItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
					}
					else
					{
						pInventory->deleteItem( pGiveItem->getObjectID() );
						executeSkillFailException(pOusters, getSkillType());
						gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_FAILD_SKILL_GET_SKIN) ); // 20070814
						pPlayer->sendPacket(&gcSystemMessage);
						
						return SKILL_RESULT_FAIL_NOT_ENOUGH_INVENTORY_SPACE;
					}
				}
				else
				{
					pGiveItem = pInventory->findItem( pGiveItem->getItemClass(), pGiveItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
					Assert(pGiveItem != NULL);

					GCDeleteInventoryItem gcDI;
					gcDI.setObjectID( pGiveItem->getObjectID() );
					pPlayer->sendPacket( &gcDI );

					pGiveItem->setNum( pGiveItem->getNum() + needItemNum );

					pGiveItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
				}
				GCCreateItem gcCreateItem;
				makeGCCreateItem(&gcCreateItem, pGiveItem, pt.x, pt.y);

				if( giveItemType == 4 )
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_CLEAN_SKIN) ); // 20070814
				}
				else
				{
					gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GET_HURT_SKIN) ); // 20070814
				}

				pPlayer->sendPacket( &gcCreateItem );
				pPlayer->sendPacket(&gcSystemMessage);
			}
			else
			{
				gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_NOT_GET_SKIN) ); // 20070814
				pPlayer->sendPacket(&gcSystemMessage);
			}

			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

//			_GCSkillToTileOK1.addShortData(MODIFY_CURRENT_HP , RemainMP);

			_GCSkillToTileOK1.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(0);
			_GCSkillToTileOK1.setRange(Range);

			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(0);
			_GCSkillToTileOK4.setRange(Range);
			
			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SKILL_TANNING);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(0);
			_GCSkillToTileOK5.setRange(Range);

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pOusters, &_GCSkillToTileOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToTileOK1);

			executeSkillFailException(pOusters, getSkillType());

			list<Creature*> cList;
			cList.push_back(pOusters);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			// 모든 정리작업이 끝이 났으면 시체를 삭제한다.
			// 사라지는 패킷을 날린다.
			GCDeleteObject gcDO;
			gcDO.setObjectID(pCorpse->getObjectID());
			pZone->broadcastPacket(X, Y,  &gcDO);
			// 존에서 지운다.
			pZone->deleteItem(pCorpse , X , Y);
			// 존안에서 실제적으로 포인터를 없애지는 않으므로 포인터를 삭제 시켜 줘야 한다.
			SAFE_DELETE(pCorpse);
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailException(pOusters, getSkillType());
			gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_FAILD_SKILL_GET_SKIN) ); // 20070814
			pPlayer->sendPacket(&gcSystemMessage);

		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}
	return SKILL_RESULT_FAIL_UNKNOWN;
	__END_CATCH
	
}

Tanning g_Tanning;
