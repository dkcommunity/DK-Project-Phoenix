//////////////////////////////////////////////////////////////////////////////
// Filename    : TransformToWolf.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TransformToWolf.h"
#include "EffectTransformToWolf.h"
#include "ItemUtil.h"
#include "RankBonus.h"
#include "PKZoneInfoManager.h"

#include "CreatureUtil.h"
#include "GamePlayer.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToInventoryOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddWolf.h"
#include "GDRLairManager.h"

#include "item/SubInventory.h"
#include "TradeManager.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� �κ��丮 �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void TransformToWolf::execute(Vampire* pVampire, ObjectID_t InvenObjectID, ObjectID_t InventoryItemObjectID, CoordInven_t X, CoordInven_t Y, CoordInven_t TargetX, CoordInven_t TargetY, VampireSkillSlot* pSkillSlot)
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
		Assert(pZone != NULL);
		Assert(pInventory!= NULL);

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
		Assert(pItem != NULL);

		// ����� �������� �ƴ϶�� �翬�� ������ �� ���.
		// PK�������� ������ �� ���.
		if (pItem->getItemClass() != Item::ITEM_CLASS_VAMPIRE_ETC 
			|| pItem->getItemType() != 0
			|| pVampire->hasRelicItem()
			|| g_pPKZoneInfoManager->isPKZone( pZone->getZoneID() )
			|| pVampire->isFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET )
			|| GDRLairManager::Instance().isGDRLairZone(pZone->getZoneID())
			|| pZone->getZoneID() == 8001
		)
		{
			executeSkillFailException(pVampire, getSkillType());
			//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
			return;
		}
		
		GCSkillToInventoryOK1 _GCSkillToInventoryOK1;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pVampire->getX();
		ZoneCoord_t y = pVampire->getY();

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
		bool bEffected      = pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_STONE_SKIN);

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bMoveModeCheck && !bEffected)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToInventoryOK1);

			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectTransformToWolf* pEffectTTW = new EffectTransformToWolf(pVampire);
			pEffectTTW->setDeadline(999999999);
			pVampire->addEffect(pEffectTTW);
			pVampire->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);

			// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord(prev);
			pVampire->initAllStat();
			pVampire->addModifyInfo(prev, _GCSkillToInventoryOK1);

			_GCSkillToInventoryOK1.setSkillType(SkillType);
			_GCSkillToInventoryOK1.setCEffectID(0);
			_GCSkillToInventoryOK1.setDuration(0);//by viva
		
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToInventoryOK1 );

			pPlayer->sendPacket(&_GCSkillToInventoryOK1);

			// �����̾� ��� ���븦 ���϶�� �˷��ش�.
			GCAddWolf gcAddWolf;
			gcAddWolf.setObjectID(pVampire->getObjectID());
			gcAddWolf.setName(pVampire->getName());
			gcAddWolf.setXYDir(x, y, pVampire->getDir());
			gcAddWolf.setItemType(pItem->getItemType());
			gcAddWolf.setCurrentHP(pVampire->getHP());
			gcAddWolf.setMaxHP(pVampire->getHP(ATTR_MAX));
			gcAddWolf.setGuildID(pVampire->getGuildID());
			pZone->broadcastPacket(x, y, &gcAddWolf, pVampire);

			decreaseItemNum(pItem, pInventory, pVampire->getName(), STORAGE_INVENTORY, invenID, X, Y);

			if ( pVampire->getPetInfo() != NULL )
			{
				pVampire->setPetInfo(NULL);
				sendPetInfo( dynamic_cast<GamePlayer*>(pVampire->getPlayer()), true );
			}

			pSkillSlot->setRunTime(output.Delay);
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
void TransformToWolf::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);
		
		SkillType_t SkillType  = SKILL_TRANSFORM_TO_WOLF;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pMonster->getX();
		ZoneCoord_t y = pMonster->getY();

		bool bRangeCheck    = checkZoneLevelToUseSkill(pMonster);
		bool bHitRoll       = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
		bool bMoveModeCheck = pMonster->isWalking();
		bool bEffected      = pMonster->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);

		if (bRangeCheck && bHitRoll && bMoveModeCheck && !bEffected)
		{
			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectTransformToWolf* pEffectTTW = new EffectTransformToWolf(pMonster);
			pEffectTTW->setDeadline(999999999);
			pMonster->addEffect(pEffectTTW);
			pMonster->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);

			// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
			pMonster->initAllStat();

			// �����̾� ��� ���븦 ���϶�� �˷��ش�.
			GCAddWolf gcAddWolf;
			gcAddWolf.setObjectID(pMonster->getObjectID());
			gcAddWolf.setName(pMonster->getName());
			gcAddWolf.setXYDir(x, y, pMonster->getDir());
			gcAddWolf.setItemType(0);
			gcAddWolf.setCurrentHP(pMonster->getHP());
			gcAddWolf.setMaxHP(pMonster->getHP(ATTR_MAX));
			gcAddWolf.setGuildID(1);
			pZone->broadcastPacket(x, y, &gcAddWolf, pMonster);
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

TransformToWolf g_TransformToWolf;
