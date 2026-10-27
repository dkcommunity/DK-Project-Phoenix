//////////////////////////////////////////////////////////////////////////////
// Filename    : CurseOfBlood1.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CurseOfBlood1.h"
#include "EffectCurseOfBlood.h"
#include "SkillUtil.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// �����̾� ���� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType CurseOfBlood1::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		int hitBonus = 0;
/*		if( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_BLOOD );
			Assert ( pRankBonus != NULL );
			hitBonus = pRankBonus->getPoint();
		}*/

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
//		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, hitBonus);
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD);
		// 20070906
		bool bBlockZone = isBlockTheSkillToZone(pZone->getZoneID());

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected && !bBlockZone)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

			VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );

			Duration_t duration = ( pVampire->getINT() / pVampire->getLevel() ) * 15;

			// ���� �ð��� ����Ѵ�.
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			for( int x = -2; x <= 2; ++x )
			{
				for( int y = -2; y <= 2; ++y )
				{
					ZoneCoord_t X = pVampire->getX() + x;
					ZoneCoord_t Y = pVampire->getY() + y;

					if( rect.ptInRect(X, Y) )
					{
						Tile& tile = pZone->getTile(X, Y);
						Creature* pTargetCreature = NULL;

						if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
						{
							pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
						}
						
						if( pTargetCreature != NULL 
							&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
							&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_FREEZE)
							&& !pTargetCreature->isDead()
							&& !pTargetCreature->isNPC()
							&& pTargetCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

							if( pVampire != NULL )
							{
								pMonster->addEnemy( pVampire );
							}

							int success = 70;
							//success = max( 0, ( 110 - ( pMonster->getLevel() - pVampire->getLevel() ) * 5 ) );
							//success = min( 100, success );
							int ratio = rand()%100+1;

							if( success >= ratio &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_CURSE_OF_BLOOD ) &&
								!pTargetCreature->isDead() &&
								!pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA )
#ifdef VERSION_SKILL_1
								&& HitRoll::isSuccessMagic( pVampire, pTargetCreature )
#endif
								)
							{
								::setDamage( pMonster, output.Damage, pVampire, SKILL_CURSE_OF_BLOOD1 );

								EffectCurseOfBlood* pEffectCursOfBlood = new EffectCurseOfBlood(pMonster);

								pEffectCursOfBlood->setDeadline(duration);

								pMonster->addEffect( pEffectCursOfBlood );
								pMonster->setFlag( Effect::EFFECT_CLASS_CURSE_OF_BLOOD );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pMonster->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_CURSE_OF_BLOOD );
								gcAddEffect.setDuration(duration);
								pZone->broadcastPacket( pMonster->getX(), pMonster->getY(), &gcAddEffect);
							}
						}

					}
				}
			}
			// ����Ʈ Ŭ������ ����� ���δ�.
/*			EffectCurseOfBlood1* pEffect = new EffectCurseOfBlood1(pVampire, pVampire->getX(), pVampire->getY());
			pEffect->setDeadline(999999);
			pEffect->setDamage(output.Damage);
			pVampire->addEffect(pEffect);
			pVampire->setFlag(Effect::EFFECT_CLASS_CURSE_OF_BLOOD1);
			pEffect->affect(); */

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(duration);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(duration);
		
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);

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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

CurseOfBlood1 g_CurseOfBlood1;
