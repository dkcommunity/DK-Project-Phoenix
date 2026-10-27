//////////////////////////////////////////////////////////////////////////////
// Filename    : VICIOUSGUIDANCE.cpp
// Written by  : excel96
// Description :DivineGuidance
//////////////////////////////////////////////////////////////////////////////

#include "VICIOUSGUIDANCE.h"
#include "EffectVICIOUSGUIDANCE.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"
#include "NewRankGem.h"
//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType VICIOUSGUIDANCE::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuchÁŠ°Å. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !canAttack( pSlayer, pTargetCreature )
		)
		{
			executeSkillFailException(pSlayer, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		// ÀÌÆåÆ®ÀÇ È¿°ú¿Í ÁöŒÓœÃ°£À» °è»êÇÑŽÙ.
		SkillInput input(pSlayer, pSkillSlot);
		SkillOutput output;
		if ( pTargetCreature->isPC() )
		{
			input.TargetType = SkillInput::TARGET_PC;
		}
		else
		{
			input.TargetType = SkillInput::TARGET_MONSTER;
		}
		computeOutput(input, output);

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll    = (rand()%100 < output.ToHit);//HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
#ifdef VERSION_SKILL_1
		bHitRoll	 &= HitRoll::isSuccessMagic(pSlayer, pTargetCreature);
#endif
		bool bEffected2   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_VICIOUS_GUIDANCE);
		bool bEffected   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_DIVINE_GUIDANCE);
		bool bPK         = verifyPK(pSlayer, pTargetCreature);

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected && !bEffected2 && bPK)
		{
			// ž¶³ªžŠ ÁÙÀÎŽÙ.
                        Damage_t	skillDamage = output.Damage;
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

			if (!pTargetCreature->isSlayer())
			{
				// °æÇèÄ¡žŠ ¿Ã·ÁÁØŽÙ.
				//SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel(pSlayer->getSkillDomainLevel(DomainType));
				//Exp_t      ExpUp = 10* (Grade + 1);
				//shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToObjectOK1);
				//
				if ( bIncreaseDomainExp )
				{
					increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1, pTargetCreature->getLevel());
				}

				increaseSkillExp(pSlayer, DomainType,  pSkillSlot, pSkillInfo, _GCSkillToObjectOK1);
			}
		
			Duration_t GemDur = output.Duration;
			if (pSlayer->hasRankGem(NewRankGem::SKILL_VICIOUS_GUIDANCE_DAM ) )
			{
				NewRankGem* pRankGemVICIOUSD = pSlayer->getRankGem( NewRankGem::SKILL_VICIOUS_GUIDANCE_DAM );
				Assert( pRankGemVICIOUSD != NULL );

				skillDamage += pRankGemVICIOUSD->getGrade();
			}

			if (pSlayer->hasRankGem(NewRankGem::SKILL_VICIOUS_GUIDANCE_MCRI ) )
			{
				NewRankGem* pRankGemVICIOUSDMCRI = pSlayer->getRankGem( NewRankGem::SKILL_VICIOUS_GUIDANCE_MCRI );
				Assert( pRankGemVICIOUSDMCRI != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pSlayer, 200);
				if(isCri)
				skillDamage += skillDamage*0.5;
			}

			if (pSlayer->hasRankGem(NewRankGem::SKILL_VICIOUS_GUIDANCE_DUR ) )
			{
				NewRankGem* pRankGemTHORNSDUR = pSlayer->getRankGem( NewRankGem::SKILL_VICIOUS_GUIDANCE_DUR );
				Assert( pRankGemTHORNSDUR != NULL );

				GemDur += pRankGemTHORNSDUR->getGrade();
			}

			EffectVICIOUSGUIDANCE* pEffect = new EffectVICIOUSGUIDANCE(pTargetCreature);
			Assert(pEffect != NULL);
			pEffect->setDeadline(GemDur);
			pEffect->setTick(output.Tick);
			pEffect->setPoint(skillDamage); 
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setNextTime(0);
			pTargetCreature->addEffect(pEffect);
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_VICIOUS_GUIDANCE);

			// ÆÐÅ¶À» ÁØºñÇØŒ­ ºž³œŽÙ.
			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(GemDur);

			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration (GemDur);

			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setDuration (GemDur);

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			if (pSlayer != pTargetCreature && pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			}
			else if ( pTargetCreature->isMonster() )
			{
				Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
				if ( pMonster != NULL ) pMonster->addEnemy(pSlayer);
			}

			list<Creature*> cList;
			cList.push_back(pSlayer);
			cList.push_back(pTargetCreature);
		
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToObjectOK5 , cList);

//			// ÀÌÆåÆ®°¡ ºÙŸúŽÙ°í ŸË·ÁÁØŽÙ.
//			GCAddEffect gcAddEffect;
//			gcAddEffect.setObjectID(pTargetCreature->getObjectID());
//			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_VICIOUS_GUIDANCE);
//			gcAddEffect.setDuration(output.Duration);
//			pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect, pTargetCreature);
//
			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), pTargetCreature);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

VICIOUSGUIDANCE g_VICIOUSGUIDANCE;
