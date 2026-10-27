//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2SUDDENBLAST.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2SUDDENBLAST.h"
#include "RankBonus.h"
#include "Vampire.h"
#include "SkillUtil.h"
#include "Zone.h"
#include "HitRoll.h"
#include "CrossCounter.h"
#include "EffectSetAfire.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2SUDDENBLAST::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert( pOusters != NULL );
	Assert( pOustersSkillSlot != NULL );

	try
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL
			|| !canAttack( pOusters, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Range_t			  Range		 = 2 + pOusters->getSTR()/80 + pOusters->getDEX()/120 + pOusters->getINT()/240;
		Range = min( (Range_t)7, Range );

		int myhitbonus =0;
		int mycribonus =0;
		if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_SUDDEN_BLAST_TOHIT ) )
		{
			NewRankGem* pRankGemSUDDENTOHIT = pOusters->getRankGem( NewRankGem::SKILL_ADV2_SUDDEN_BLAST_TOHIT );
			Assert( pRankGemSUDDENTOHIT != NULL );

			myhitbonus += pRankGemSUDDENTOHIT->getGrade();
		}
		if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_SUDDEN_BLAST_CRI ) )
		{
			NewRankGem* pRankGemSUDDENCRI = pOusters->getRankGem( NewRankGem::SKILL_ADV2_SUDDEN_BLAST_CRI );
			Assert( pRankGemSUDDENCRI != NULL );

			mycribonus += pRankGemSUDDENCRI->getGrade();
		}
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck         = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck        = verifyDistance(pOusters, pTargetCreature, Range);
		bool bHitRoll           = HitRoll::isSuccess( pOusters, pTargetCreature,myhitbonus);
		bool bCanHit            = canHit(pOusters, pTargetCreature, getSkillType());
		bool bPK                = verifyPK(pOusters, pTargetCreature);
		bool bFastMove			= false;

		if ( getDistance( pOusters->getX(), pOusters->getY(), pTargetCreature->getX(), pTargetCreature->getY() ) > 1 )
		{
			bFastMove = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && bPK &&
				( !bFastMove || pZone->moveFastPC(pOusters, pOusters->getX(), pOusters->getY(), pTargetCreature->getX(), pTargetCreature->getY(), getSkillType()) )
				)
		{
			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			computeOutput(input, output);
			Damage_t Damage = output.Damage;

        		if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_SUDDEN_BLAST_DAM ) )
			{
				NewRankGem* pRankGemSUDDENDAM = pOusters->getRankGem( NewRankGem::SKILL_ADV2_SUDDEN_BLAST_DAM );
				Assert( pRankGemSUDDENDAM != NULL );

				Damage += pRankGemSUDDENDAM->getGrade();
			}

			bool bCriticalHit = false;
			Damage += computeDamage(pOusters, pTargetCreature, mycribonus, bCriticalHit);

			CheckCrossCounter(pOusters, pTargetCreature, Damage);

			decreaseMana(pOusters, RequiredMP, _GCSkillToObjectOK1);
	

                        EffectSetAfire* pEffect = new EffectSetAfire(pTargetCreature);
			pEffect->setDamage(Damage);
			pEffect->setTick(3);
			pEffect->setNextTime(3);
			pEffect->setTimes(2);
			pEffect->setCasterOID( pOusters->getObjectID() );
			pTargetCreature->addEffect( pEffect );

			increaseAlignment(pOusters, pTargetCreature, _GCSkillToObjectOK1);

			_GCSkillToObjectOK1.setSkillType(getSkillType());
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);

			_GCSkillToObjectOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK2.setSkillType(getSkillType());
			_GCSkillToObjectOK2.setDuration(0);

			_GCSkillToObjectOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK5.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK5.setSkillType(getSkillType());

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			Player* pTargetPlayer = NULL;

			if (pTargetCreature->isPC()) 
			{
				pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			} 
			else 
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				pMonster->addEnemy(pOusters);
			}

			list<Creature*> cList;
			cList.push_back(pOusters);
			cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(pOusters->getX(), pOusters->getY(),
				pTargetCreature->getX(), pTargetCreature->getY(), &_GCSkillToObjectOK5, cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else
		{
			executeSkillFailNormal(pOusters, getSkillType(), pTargetCreature );
		}
	}
	catch(Throwable & t)
	{
		executeSkillFailException( pOusters, getSkillType() );
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2SUDDENBLAST g_ADV2SUDDENBLAST;
