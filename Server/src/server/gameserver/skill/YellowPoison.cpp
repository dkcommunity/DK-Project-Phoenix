/////////////////////////////////////////////////////////////////////////////
// Filename    : YellowPoison.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "YellowPoison.h"
#include "EffectYellowPoison.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillResultType YellowPoison::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;


	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		if (pTargetCreature == NULL
			|| !canAttack( pVampire, pTargetCreature )
			)
		{
			executeSkillFailException(pVampire, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////
//
// YellowPoison::execute()
//
//////////////////////////////////////////////////////////////////////
SkillResultType YellowPoison::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
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

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		// Knowledge of Poison �� �ִٸ� hit bonus 10
		int HitBonus = 0;
/*		if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_POISON ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_POISON );
			Assert( pRankBonus != NULL );

			HitBonus = pRankBonus->getPoint();
		}*/

		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, X, Y, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);

		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
			if ( tile.getEffect(Effect::EFFECT_CLASS_HEAVEN_GROUND) != NULL ) bTileCheck=false;
			if ( tile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) bTileCheck=false;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToTileOK1);
		
			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 1;

			// ���� ����Ʈ�� ������ �����.
			Effect* pOldEffect = NULL;
			if ((pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_YELLOW_POISON)))
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);// fix me
			}

			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ ������Ʈ�� ���Ѵ�.
			EffectYellowPoison *pEffect = new EffectYellowPoison(pZone, X, Y);
			pEffect->setVampire();
			pEffect->setDeadline(output.Duration);
			pEffect->setDuration(output.Duration);
			pEffect->setLevel(pSkillInfo->getLevel());
			pEffect->setPoisonLevel(pVampire->getINT());

			// Ÿ�Ͽ� ���̴� Effect�� OID�� ��Ϲ޾ƾ� �Ѵ�.
			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			// �� �� Ÿ�Ͽ��ٰ� ����Ʈ�� �߰��Ѵ�.
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			// tile���� creature�� �ִٸ� �ٷ� ������ �ֵ��� �Ѵ�.
			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pVampire->getX();
			ZoneCoord_t myY = pVampire->getY();

			if (pTargetCreature != NULL && ( pTargetCreature->isSlayer()||pTargetCreature->isOusters() ))
			{
				if (pEffect->affectCreature(pTargetCreature, false) == true)
				{
					bEffected = true;

					// ���κп��� �����̾��ΰ��� �˻��ϱ� ������,
					// �� �κ��� if�� �׻� ���� �� ���� �и��ϴ�.
					// ������ ���߿� YellowPoison�� �����̾ ���Ϳ��Ե�
					// �ɸ��� ����ȴٸ�, ���κ��� isSlayer �˻� �׸��� ������ 
					// �� ���̹Ƿ�, �̸� �� �κп��� Monster�� ��� ������ ���� �ʰ�
					// isPC() �˻��ϴ� �κ��� ���־� �д�.
					if (pTargetCreature->isPC())
					{
						Player* pTargetPlayer = pTargetCreature->getPlayer();
						bool bCanSee = canSee(pTargetCreature, pVampire);

						if (bCanSee)
						{
							_GCSkillToTileOK2.setObjectID(pVampire->getObjectID());
							_GCSkillToTileOK2.setSkillType(SkillType);
							_GCSkillToTileOK2.setX(X);
							_GCSkillToTileOK2.setY(Y);
							_GCSkillToTileOK2.setDuration(output.Duration);
							_GCSkillToTileOK2.setRange(Range);
							pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
						} 
						else 
						{
							_GCSkillToTileOK6.setOrgXY(myX, myY);
							_GCSkillToTileOK6.setSkillType(SkillType);
							_GCSkillToTileOK6.setX(X);
							_GCSkillToTileOK6.setY(Y);
							_GCSkillToTileOK6.setDuration(output.Duration);
							_GCSkillToTileOK6.setRange(Range);
							_GCSkillToTileOK6.addCListElement(pTargetCreature->getObjectID());
							pTargetPlayer->sendPacket(&_GCSkillToTileOK6);
						}
					}
				}
			}
				
			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			_GCSkillToTileOK3.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);

			if (bEffected) 
			{
				_GCSkillToTileOK1.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
			}
			
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pVampire);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pVampireSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void YellowPoison::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pMonster != NULL);
	Assert(pEnemy != NULL);

	execute(pMonster, pEnemy->getX(), pEnemy->getY());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� Ÿ�� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void YellowPoison::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
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
			//cout << "Monster cannot use skill while hiding." << endl;
			//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(monster)" << endl;
			return;
		}
		if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
		{
			Effect* pInvi = pMonster->findEffect( Effect::EFFECT_CLASS_INVISIBILITY );
			if ( pInvi ) pInvi->setDeadline(0);
			//addVisibleCreature(pZone, pMonster, true);
		}

		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = SKILL_YELLOW_POISON;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		bool bRangeCheck = verifyDistance(pMonster, X, Y, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccessMagic(pMonster, pSkillInfo);

		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
			if ( tile.getEffect(Effect::EFFECT_CLASS_HEAVEN_GROUND) != NULL ) bTileCheck=false;
			if ( tile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) bTileCheck=false;
		}

		if (bRangeCheck && bHitRoll && bTileCheck)
		{
			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 1;

			// ���� ����Ʈ�� �̹� �����Ѵٸ� �����Ѵ�.
			Effect* pOldEffect = NULL;
			if ((pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_YELLOW_POISON)))
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);// fix me
			}

			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			// ����Ʈ ������Ʈ�� ���Ѵ�.
			EffectYellowPoison *pEffect = new EffectYellowPoison(pZone, X, Y);
			pEffect->setDeadline(output.Duration);
			pEffect->setDuration(output.Duration);
			pEffect->setLevel(pSkillInfo->getLevel()/2);
			pEffect->setPoisonLevel(pMonster->getINT());

			// Ÿ�Ͽ� ���̴� Effect�� ObjectID�� ��Ϲ޾ƾ� �Ѵ�.
			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			// �� �� Ÿ�Ͽ��ٰ� ����Ʈ�� �߰��Ѵ�.
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			// tile���� creature�� �ִٸ� �ٷ� ������ �ֵ��� �Ѵ�.
			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pMonster->getX();
			ZoneCoord_t myY = pMonster->getY();

			if (pTargetCreature != NULL && ( pTargetCreature->isSlayer()||pTargetCreature->isOusters() ))
			{
				if (pEffect->affectCreature(pTargetCreature, false) == true)
				{
					bEffected = true;

					// ���κп��� �����̾��ΰ��� �˻��ϱ� ������,
					// �� �κ��� if�� �׻� ���� �� ���� �и��ϴ�.
					// ������ ���߿� YellowPoison�� �����̾ ���Ϳ��Ե�
					// �ɸ��� ����ȴٸ�, ���κ��� isSlayer �˻� �׸��� ������ 
					// �� ���̹Ƿ�, �̸� �� �κп��� Monster�� ��� ������ ���� �ʰ�
					// isPC() �˻��ϴ� �κ��� ���־� �д�.
					if (pTargetCreature->isPC())
					{
						Player* pTargetPlayer = pTargetCreature->getPlayer();
						bool bCanSee = canSee(pTargetCreature, pMonster);

						if (bCanSee)
						{
							_GCSkillToTileOK2.setObjectID(pMonster->getObjectID());
							_GCSkillToTileOK2.setSkillType(SkillType);
							_GCSkillToTileOK2.setX(X);
							_GCSkillToTileOK2.setY(Y);
							_GCSkillToTileOK2.setDuration(output.Duration);
							_GCSkillToTileOK2.setRange(Range);
							pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
						} 
						else 
						{
							_GCSkillToTileOK6.setOrgXY(myX, myY);
							_GCSkillToTileOK6.setSkillType(SkillType);
							_GCSkillToTileOK6.setX(X);
							_GCSkillToTileOK6.setY(Y);
							_GCSkillToTileOK6.setDuration(output.Duration);
							_GCSkillToTileOK6.setRange(Range);
							_GCSkillToTileOK6.addCListElement(pTargetCreature->getObjectID());
							pTargetPlayer->sendPacket(&_GCSkillToTileOK6);
						}
					}
				}
			}
				
			_GCSkillToTileOK3.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);

			if (bEffected) 
			{
				_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
			}
		
			list<Creature*> cList;
			cList.push_back(pMonster);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);
		} 
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}

YellowPoison g_YellowPoison;
