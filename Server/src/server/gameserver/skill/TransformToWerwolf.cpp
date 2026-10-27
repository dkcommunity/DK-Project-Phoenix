//////////////////////////////////////////////////////////////////////////////
// Filename    : TransformToWerwolf.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TransformToWerwolf.h"
#include "EffectTransformToWerwolf.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "RankBonus.h"
#include "PKZoneInfoManager.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToInventoryOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddWolf.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� �κ��丮 �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void TransformToWerwolf::execute(Vampire* pVampire, ObjectID_t InvenObjectID, CoordInven_t X, CoordInven_t Y, CoordInven_t TargetX, CoordInven_t TargetY, VampireSkillSlot* pSkillSlot)
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

		Item* pItem = pInventory->getItem(X, Y);
		Assert(pItem != NULL);

		// ����� �������� �ƴ϶�� �翬�� ������ �� ���.
		// PK�������� ������ �� ���.
		if (pItem->getItemClass() != Item::ITEM_CLASS_SKULL 
			|| pItem->getItemType() != 39
			|| pVampire->hasRelicItem()
			|| g_pPKZoneInfoManager->isPKZone( pZone->getZoneID() )
			|| pVampire->isFlag( Effect::EFFECT_CLASS_REFINIUM_TICKET )
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
		bool bEffected      = pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_STONE_SKIN );

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bMoveModeCheck && !bEffected)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToInventoryOK1);

			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);
			
			pSkillSlot->setRunTime(output.Delay);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectTransformToWerwolf* pEffectTTW = new EffectTransformToWerwolf(pVampire);
			pEffectTTW->setDeadline(output.Duration);
			pEffectTTW->setHPBonus(true); //20070918
			pVampire->addEffect(pEffectTTW);
			pVampire->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);

			// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord(prev);
			pVampire->initAllStatAndSend();
			pVampire->addModifyInfo(prev, _GCSkillToInventoryOK1);

			_GCSkillToInventoryOK1.setSkillType(SkillType);
			_GCSkillToInventoryOK1.setCEffectID(0);
			_GCSkillToInventoryOK1.setDuration(output.Duration); //by viva
		
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToInventoryOK1 );

			pPlayer->sendPacket(&_GCSkillToInventoryOK1);

			DWORD dummyFlag;
			Color_t color[PCVampireInfo::VAMPIRE_COLOR_MAX];
			pVampire->getShapeInfo( dummyFlag, color );

			// �����̾� ��� ���븦 ���϶�� �˷��ش�.
			GCAddWolf gcAddWerwolf;
			gcAddWerwolf.setObjectID(pVampire->getObjectID());
			gcAddWerwolf.setName(pVampire->getName());
			gcAddWerwolf.setXYDir(x, y, pVampire->getDir());
			gcAddWerwolf.setMainColor( color[PCVampireInfo::VAMPIRE_COLOR_COAT] );
			gcAddWerwolf.setItemType(pItem->getItemType());
			gcAddWerwolf.setCurrentHP(pVampire->getHP());
			gcAddWerwolf.setMaxHP(pVampire->getHP(ATTR_MAX));
			gcAddWerwolf.setGuildID(pVampire->getGuildID());
			pZone->broadcastPacket(x, y, &gcAddWerwolf, pVampire);

/*			GCAddVampire gcAddVampire;
			makeGCAddVampire( &gcAddVampire, pVampire );

			pZone->broadcastPacket(x, y, &gcAddVampire, pVampire);*/

			decreaseItemNum(pItem, pInventory, pVampire->getName(), STORAGE_INVENTORY, 0, X, Y);
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
void TransformToWerwolf::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);
		
		SkillType_t SkillType  = SKILL_TRANSFORM_TO_WERWOLF;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pMonster->getX();
		ZoneCoord_t y = pMonster->getY();

		bool bRangeCheck    = checkZoneLevelToUseSkill(pMonster);
		bool bHitRoll       = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
		bool bMoveModeCheck = pMonster->isWalking();
		bool bEffected      = pMonster->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);

		if (bRangeCheck && bHitRoll && bMoveModeCheck && !bEffected)
		{
			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectTransformToWerwolf* pEffectTTW = new EffectTransformToWerwolf(pMonster);
			pEffectTTW->setDeadline(999999999);
			pMonster->addEffect(pEffectTTW);
			pMonster->setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF);

			// �̷� ���ؼ� ���ϴ� �ɷ�ġ���� �����ش�.
			pMonster->initAllStat();

			// �����̾� ��� ���븦 ���϶�� �˷��ش�.
			GCAddWolf gcAddWerwolf;
			gcAddWerwolf.setObjectID(pMonster->getObjectID());
			gcAddWerwolf.setName(pMonster->getName());
			gcAddWerwolf.setXYDir(x, y, pMonster->getDir());
			gcAddWerwolf.setItemType(3);
			gcAddWerwolf.setCurrentHP(pMonster->getHP());
			gcAddWerwolf.setMaxHP(pMonster->getHP(ATTR_MAX));
			gcAddWerwolf.setGuildID(1);
			pZone->broadcastPacket(x, y, &gcAddWerwolf, pMonster);
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

TransformToWerwolf g_TransformToWerwolf;
