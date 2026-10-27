//////////////////////////////////////////////////////////////////////////////
// Filename    : TransformToBat.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TransformToBat.h"
#include "EffectTransformToBat.h"
#include "ZoneUtil.h"
#include "ItemUtil.h"
#include "RankBonus.h"
#include "PKZoneInfoManager.h"

#include "CreatureUtil.h"
#include "GamePlayer.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToInventoryOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddBat.h"
#include "GDRLairManager.h"
#include "SiegeManager.h"
#include "GQuestManager.h"
#include "DynamicZone.h"

#include "item/SubInventory.h"
#include "item/VampireWingItem.h"
#include "TradeManager.h"
#include "ItemInfoManager.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� �κ��丮 �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void TransformToBat::execute(Vampire* pVampire, ObjectID_t InvenObjectID, ObjectID_t InventoryItemObjectID, CoordInven_t X, CoordInven_t Y, CoordInven_t TargetX, CoordInven_t TargetY, VampireSkillSlot* pSkillSlot)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Inventory* pInventory = pVampire->getInventory();

		Assert(pPlayer != NULL);
		Assert(pInventory != NULL);
		Assert(pZone != NULL);

		SubInventory* pInventoryItem = NULL;
		int invenID = 0;

		if ( InventoryItemObjectID != 0 )
		{
			cout << "���� �κ��丮���� ��� : " << InventoryItemObjectID << endl;
			CoordInven_t X, Y;
			pInventoryItem = dynamic_cast<SubInventory*>(pInventory->findItemOID( InventoryItemObjectID, X, Y ));

			TradeManager* pTradeManager = pZone->getTradeManager();
			Assert(pTradeManager != NULL);

			if ( pInventoryItem == NULL || pTradeManager->hasTradeInfo(pVampire->getName()) )
			{
				cout << "�ٵ� ���� �κ��丮�� ���." <<endl;
				executeSkillFailException(pVampire, getSkillType());
				return;
			}

			pInventory = pInventoryItem->getInventory();
			invenID = pInventoryItem->getItemID();
		}

		Item* pItem = pInventory->getItem(X, Y);
		// ����� �������� �ƴ϶�� �翬�� ������ �� ���.
		// PK���̶�� ������ �� ���.
		if (pItem == NULL 
			|| !(
					( pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_ETC && pItem->getItemType() == 1 ) 
					|| pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_WING_ITEM 
				)
			|| pVampire->hasRelicItem()
		 	|| g_pPKZoneInfoManager->isPKZone( pZone->getZoneID() )
			|| pVampire->isFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET )
			|| GDRLairManager::Instance().isGDRLairZone(pZone->getZoneID())
			|| SiegeManager::Instance().isSiegeZone(pZone->getZoneID())
			|| (pZone->isDynamicZone() && pZone->getDynamicZone()->getTemplateZoneID() == 4002 )
			//|| ( pZone->isNoPortalZone() && !pZone->getZoneID() == 1125 && !pZone->getZoneID() == 1121 && !pZone->getZoneID() == 1128 && !pZone->getZoneID() == 1129 )
			|| ( pZone->isNoPortalZone() && pZone->getZoneID() != 1125 && pZone->getZoneID() != 1121 && pZone->getZoneID() != 1128 && pZone->getZoneID() != 1129 )
			|| pZone->getZoneID() == 8001
			|| pZone->isHellGardenTower() 
		)
		{
			executeSkillFailException(pVampire, getSkillType());
			//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
			return;
		}

		GCSkillToInventoryOK1 _GCSkillToInventoryOK1;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x    = pVampire->getX();
		ZoneCoord_t y    = pVampire->getY();
		Tile&       tile = pZone->getTile(x, y);

		// Knowledge of Innate �� �ִٸ� hit bonus 10
		int HitBonus = 0;
/*		if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE );
			Assert( pRankBonus != NULL );

			HitBonus = pRankBonus->getPoint();
		}*/

		int  RequiredMP     = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck     = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck     = verifyRunTime(pSkillSlot);
		bool bRangeCheck    = checkZoneLevelToUseSkill(pVampire);
		bool bHitRoll       = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pSkillSlot, HitBonus);
		bool bMoveModeCheck = pVampire->isWalking();
		bool bTileCheck     = (canAddCreature(pZone, x, y, Creature::MOVE_MODE_FLYING) && tile.hasFlyingCreature() == false);
		bool bEffected      = pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bMoveModeCheck && bTileCheck && !bEffected)
		{
			TPOINT pt = findSuitablePosition(pZone, x, y, Creature::MOVE_MODE_FLYING);

			if (pt.x!=-1)	// �� ��ǥ üũ. by sigi. 2002.5.2
			{
				decreaseMana(pVampire, RequiredMP, _GCSkillToInventoryOK1);

				SkillInput input(pVampire);
				SkillOutput output;
				computeOutput(input, output);

				// ����Ʈ Ŭ������ ����� ���δ�.
				EffectTransformToBat* pEffectTTW = new EffectTransformToBat(pVampire);
				pEffectTTW->setDeadline(99999999);
				pVampire->addEffect(pEffectTTW);
				pVampire->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);

				switch( pItem->getItemClass())
				{
					case Item::ITEM_CLASS_VAMPIRE_ETC :
						pVampire->SetBatType(Vampire::BAT_TYPE_LEGACY);
						break;
					case Item::ITEM_CLASS_VAMPIRE_WING_ITEM :
						{
                                                 /*  by svi
						   VampireWingItemInfo*  pItemInfo = dynamic_cast<VampireWingItemInfo*>(g_pItemInfoManager->getItemInfo( pItem->getItemClass(), pItem->getItemType() ));                         */
                                                        VampireWingItem*  pItemObject = dynamic_cast<VampireWingItem*>(pItem);

							pVampire->SetBatType(Vampire::BAT_TYPE_WING);
							pVampire->SetWingItemType( pItemObject->getItemType());
							pVampire->SetWingBodyColor( pItemObject->getBodyColor());
							pVampire->SetWingEffectColor( pItemObject->getEffectColor());

						}
						break;
					default :
						pVampire->SetBatType(Vampire::BAT_TYPE_NONE);
						break;
				}

				// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
				VAMPIRE_RECORD prev;
				pVampire->getVampireRecord(prev);
				pVampire->initAllStat();
				pVampire->addModifyInfo(prev, _GCSkillToInventoryOK1);

				_GCSkillToInventoryOK1.setItemClass(pItem->getItemClass());
				_GCSkillToInventoryOK1.setItemType(pItem->getItemType());

				_GCSkillToInventoryOK1.setSkillType(SkillType);
				_GCSkillToInventoryOK1.setCEffectID(0);
				_GCSkillToInventoryOK1.setDuration(0);
			
				// ��ų �� ó��
				postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToInventoryOK1 );

				pPlayer->sendPacket(&_GCSkillToInventoryOK1);

				// �����̾� ��� ���㸦 ���϶�� �˷��ش�.
				GCAddBat gcAddBat;
				gcAddBat.setObjectID(pVampire->getObjectID());
				gcAddBat.setName(pVampire->getName());
				gcAddBat.setXYDir(x, y, pVampire->getDir());
				gcAddBat.setItemType(pItem->getItemType());
				gcAddBat.setCurrentHP(pVampire->getHP());
				gcAddBat.setMaxHP(pVampire->getHP(ATTR_MAX));
				gcAddBat.setGuildID(pVampire->getGuildID());

				gcAddBat.setBatType(pVampire->GetBatType());

				switch(pVampire->GetBatType())
				{
					case Vampire::BAT_TYPE_LEGACY :
						gcAddBat.setColor(pVampire->getBatColor());
						// ���� ������ ���� ���� ����Ʈ�� �߰��ؼ� �־��ִ°� ������ �Ѵ��...
						// 20070731
						gcAddBat.setAdvanceColor(pVampire->getAdvanceBatColor());
						break;
					case Vampire::BAT_TYPE_WING :
						gcAddBat.setWingBodyColor(pVampire->GetWingBodyColor());
						gcAddBat.setWingEffectColor(pVampire->GetWingEffectColor());
						break;
					default :
						break;
				}

				pZone->broadcastPacket(x, y, &gcAddBat, pVampire);
				pPlayer->sendPacket(&gcAddBat);

				// Ÿ�Ͽ��� �����, �ٽ� �������μ� �����带 �ٲ� �� �մ�.
				tile.deleteCreature(pVampire->getObjectID());

				Tile& newtile = pZone->getTile(pt.x, pt.y);

				pVampire->setMoveMode(Creature::MOVE_MODE_FLYING);
				newtile.addCreature(pVampire);
				pVampire->setXYDir(pt.x, pt.y, pVampire->getDir() );

				switch(pItem->getItemClass())
				{
					case Item::ITEM_CLASS_VAMPIRE_ETC :
						decreaseItemNum(pItem, pInventory, pVampire->getName(), STORAGE_INVENTORY, invenID, X, Y);
						break;
					case Item::ITEM_CLASS_VAMPIRE_WING_ITEM :
						// ������ ������
					default :
						break;
				}

				pSkillSlot->setRunTime(output.Delay);

				if ( pVampire->getPetInfo() != NULL )
				{
					pVampire->setPetInfo(NULL);
					sendPetInfo( dynamic_cast<GamePlayer*>(pVampire->getPlayer()), true );
				}

				pVampire->getGQuestManager()->rideMotorcycle();
			}
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void TransformToBat::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		SkillType_t SkillType  = SKILL_TRANSFORM_TO_BAT;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pMonster->getX();
		ZoneCoord_t y = pMonster->getY();
		Tile&       tile = pZone->getTile(x, y);

		bool bRangeCheck    = checkZoneLevelToUseSkill(pMonster);
		bool bHitRoll       = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
		bool bMoveModeCheck = pMonster->isWalking();
		bool bTileCheck     = (canAddCreature(pZone, x, y, Creature::MOVE_MODE_FLYING) && tile.hasFlyingCreature() == false);
		bool bEffected      = pMonster->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);

		if (bRangeCheck && bHitRoll && bMoveModeCheck && bTileCheck && !bEffected)
		{
			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectTransformToBat* pEffectTTW = new EffectTransformToBat(pMonster);
			pEffectTTW->setDeadline(99999999);
			pMonster->addEffect(pEffectTTW);
			pMonster->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);

			// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
			pMonster->initAllStat();

			// �����̾� ��� ���㸦 ���϶�� �˷��ش�.
			GCAddBat gcAddBat;
			gcAddBat.setObjectID(pMonster->getObjectID());
			gcAddBat.setName(pMonster->getName());
			gcAddBat.setXYDir(x, y, pMonster->getDir());
			gcAddBat.setItemType(1);
			gcAddBat.setCurrentHP(pMonster->getHP());
			gcAddBat.setMaxHP(pMonster->getHP(ATTR_MAX));
			gcAddBat.setGuildID(1);
			gcAddBat.setColor(0);
			pZone->broadcastPacket(x, y, &gcAddBat, pMonster);



			// Ÿ�Ͽ��� �����, �ٽ� �������μ� �����带 �ٲ� �� �մ�.
			Tile& tile = pZone->getTile(x, y);
			tile.deleteCreature(pMonster->getObjectID());

			TPOINT pt = findSuitablePosition(pZone, x, y, Creature::MOVE_MODE_FLYING);
			pMonster->setMoveMode(Creature::MOVE_MODE_FLYING);
			Tile& newtile = pZone->getTile(pt.x, pt.y);

			newtile.addCreature(pMonster);
			pMonster->setXYDir(pt.x, pt.y, pMonster->getDir() );

		} 
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}


TransformToBat g_TransformToBat;
