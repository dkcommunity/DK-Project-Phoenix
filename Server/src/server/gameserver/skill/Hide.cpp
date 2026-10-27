//////////////////////////////////////////////////////////////////////////////
// Filename    : Hide.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Hide.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "ZoneUtil.h"
#include "RankBonus.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ���� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType Hide::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK3 _GCSkillToSelfOK3;

		SkillType_t SkillType  = pVampireSkillSlot->getSkillType();
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

		if( pZone->getZoneID() == 8001 || pZone->isMasterLair() || pZone->getZoneID() == 6050 || pZone->getZoneID() == 6051 || pZone->getZoneID() == 1412 || pZone->getZoneID() == 1413 || pZone->getZoneID() == 6100 || pZone->getZoneID() == 7001)
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		Tile& rTile = pZone->getTile( pVampire->getX(), pVampire->getY() );
		int  RequiredMP     = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck     = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck     = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck    = checkZoneLevelToUseSkill(pVampire);
		bool bHitRoll       = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);
		bool bTileCheck     = canBurrow(pZone, x, y);
		bool bMoveModeCheck = pVampire->isWalking();
		bool bEffected      = pVampire->isFlag(Effect::EFFECT_CLASS_HIDE)
								|| pVampire->hasRelicItem()
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG)
								|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)
								|| rTile.getEffect(Effect::EFFECT_CLASS_TRYING_POSITION) != NULL;
		bool bZone			= pZone->isHellGardenTower();

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck && bMoveModeCheck && !bEffected && !bZone)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);
			
			// �����̾ �� ������ �����ϱ� ���� ��� ��Ŷ���� ������.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK3.setXY(x, y);
			_GCSkillToSelfOK3.setSkillType(SkillType);
			_GCSkillToSelfOK3.setDuration(0);
		
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK3, pVampire);

			// �� ���� �����ִ� �����̾� �����϶�� �˸���.
			GCDeleteObject gcDO;
			gcDO.setObjectID(pVampire->getObjectID());
			pZone->broadcastPacket(x, y, &gcDO, pVampire);

			// �� �ӿ��ٰ� �����̾ �߰��Ѵ�.
			addBurrowingCreature(pZone, pVampire, x, y);
			
			pVampireSkillSlot->setRunTime();
			
			return SKILL_RESULT_SUCCESS;
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

	return SKILL_RESULT_FAIL_UNKNOWN;

}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void Hide::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE))
		{
			//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
			return;
		}
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) pInvi->setDeadline(0);
			//addVisibleCreature(pZone, pMonster, true);
		}

		GCSkillToSelfOK3 _GCSkillToSelfOK3;

		SkillType_t SkillType  = SKILL_HIDE;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t x = pMonster->getX();
		ZoneCoord_t y = pMonster->getY();

		bool bRangeCheck    = checkZoneLevelToUseSkill(pMonster);
		bool bHitRoll       = HitRoll::isSuccessMagic(pMonster, pSkillInfo);
		bool bTileCheck     = canBurrow(pZone, x, y);
		bool bMoveModeCheck = pMonster->isWalking();
		bool bEffected      = pMonster->isFlag(Effect::EFFECT_CLASS_HIDE);

		if (bRangeCheck && bHitRoll && bTileCheck && bMoveModeCheck && !bEffected)
		{
			// ���͸� �� ������ �����ϱ� ���� ��� ��Ŷ���� ������.
			_GCSkillToSelfOK3.setXY(x, y);
			_GCSkillToSelfOK3.setDuration(0);
			_GCSkillToSelfOK3.setSkillType(SkillType);
		
			pZone->broadcastPacket(x, y, &_GCSkillToSelfOK3);

			// �� ���� �����ִ� ���� �����϶�� �˸���.
			GCDeleteObject gcDO;
			gcDO.setObjectID(pMonster->getObjectID());
			pZone->broadcastPacket(x, y, &gcDO);

			// �� �ӿ��ٰ� ���͸� �߰��Ѵ�.
			addBurrowingCreature(pZone, pMonster, x, y);
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

Hide g_Hide;
