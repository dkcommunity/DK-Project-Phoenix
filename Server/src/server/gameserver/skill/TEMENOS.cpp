//////////////////////////////////////////////////////////////////////////////
// Filename    : TEMENOS.cpp
// Written by  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TEMENOS.h"
#include "EffectTEMENOS.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "SimpleMissileSkill.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType TEMENOS::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert( pSlayer != NULL );
	Assert( pSkillSlot != NULL );

	try {
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert( pPlayer != NULL );
		Assert( pZone != NULL );

		Creature* pTargetCreature = pZone->getCreature( TargetObjectID );

		// NPC žŠ °ø°ÝÇÒ Œö ŸøŽÙ
		// žé¿ªÀÌ°Å³ª.. by sigi. 2002.9.13
		// NoSuchÁŠ°Å. by sigi. 2002.5.5
		if ( pTargetCreature==NULL
			|| pTargetCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_PARALYZE)
			|| !canAttack( pSlayer, pTargetCreature )
			|| pTargetCreature->isNPC() )
		{
			executeSkillFailException( pSlayer, getSkillType() );

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		SkillType_t SkillType = pSkillSlot->getSkillType();
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SkillType );

		// œºÅ³ »ç¿ëÀÌ °¡ŽÉÇÑÁö ÃŒÅ©ÇÑŽÙ
		int	RequiredMP = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck = hasEnoughMana( pSlayer, RequiredMP );
		bool bTimeCheck = verifyRunTime( pSkillSlot );
		bool bRangeCheck = verifyDistance( pSlayer, pTargetCreature, pSkillInfo->getRange() );
		bool bHitRoll = HitRoll::isSuccessMagic( pSlayer, pSkillInfo, pSkillSlot );
#ifdef VERSION_SKILL_1
		bHitRoll &= HitRoll::isSuccessMagic( pSlayer, pTargetCreature );
#endif
		bool bCanHit = canHit( pSlayer, pTargetCreature, SkillType );
		bool bEffected = pTargetCreature->isFlag( Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS ) || pTargetCreature->isFlag( Effect::EFFECT_CLASS_TEMENOS );
		bool bPK = verifyPK( pSlayer, pTargetCreature );

		if ( bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bCanHit && !bEffected && bPK )
		{
			// packet
			GCSkillToObjectOK1 _GCSkillToObjectOK1;
			GCSkillToObjectOK2 _GCSkillToObjectOK2;
			GCSkillToObjectOK3 _GCSkillToObjectOK3;
			GCSkillToObjectOK4 _GCSkillToObjectOK4;
			GCSkillToObjectOK5 _GCSkillToObjectOK5;
			GCSkillToObjectOK6 _GCSkillToObjectOK6;

			ZoneCoord_t prayerX = pSlayer->getX();
			ZoneCoord_t prayerY = pSlayer->getY();
			ZoneCoord_t targetX = pTargetCreature->getX();
			ZoneCoord_t targetY = pTargetCreature->getY();

			// œºÅ³ÀÇ damage, delay, durationÀ» °è»ê
			SkillInput input( pSlayer, pSkillSlot );
			SkillOutput output;
			computeOutput( input, output );

			if (pSlayer->hasRankGem(NewRankGem::SKILL_TEMENOS_DAM ) )
			{
				NewRankGem* pRankGemTEMENOSD = pSlayer->getRankGem( NewRankGem::SKILL_TEMENOS_DAM );
				Assert( pRankGemTEMENOSD != NULL );

				output.Damage += pRankGemTEMENOSD->getGrade();
			}

			if (pSlayer->hasRankGem(NewRankGem::SKILL_TEMENOS_MCRI ) )
			{
				NewRankGem* pRankGemTEMENOSDMCRI = pSlayer->getRankGem( NewRankGem::SKILL_TEMENOS_MCRI );
				Assert( pRankGemTEMENOSDMCRI != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pSlayer, 200);
				if(isCri)
				output.Damage += output.Damage*0.5;
			}

			if (pSlayer->hasRankGem(NewRankGem::SKILL_TEMENOS_DUR ) )
			{
				NewRankGem* pRankGemTEMENOSDUR = pSlayer->getRankGem( NewRankGem::SKILL_TEMENOS_DUR );
				Assert( pRankGemTEMENOSDUR != NULL );

				output.Duration += pRankGemTEMENOSDUR->getGrade();
			}
			// ž¶³ªžŠ ÁÙÀÎŽÙ
			decreaseMana( pSlayer, RequiredMP, _GCSkillToObjectOK1 );

			// µ¥¹ÌÁöžŠ °¡ÇÏ°í, ³»±žµµžŠ ¶³Ÿî¶ßž°ŽÙ. °ø°ÝŽë»óÀÌ œœ·¹ÀÌŸî°¡ ŸÆŽÏ¹Ç·Î alignment, PK ÃŒÅ©ÇÒ ÇÊ¿ä°¡ ŸøŽÙ
			setDamage( pTargetCreature, output.Damage, pSlayer, SkillType, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1 );
			computeAlignmentChange( pTargetCreature, output.Damage, pSlayer, &_GCSkillToObjectOK2, &_GCSkillToObjectOK1 );
			decreaseDurability( pSlayer, pTargetCreature, NULL, &_GCSkillToObjectOK1, &_GCSkillToObjectOK2 );

			// Åž°ÙÀÌ œœ·¹ÀÌŸî°¡ ŸÆŽÑ °æ¿ì¿¡žž
			// °æÇèÄ¡žŠ ¿Ãž°ŽÙ
			// EffectžŠ ºÙÀÎŽÙ
			// EffectžŠ ºê·Îµå Ä³œºÆÃ ÇÑŽÙ
			if ( !pTargetCreature->isSlayer() )
			{
				//SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel( pSlayer->getSkillDomainLevel( DomainType ) );
				//Exp_t ExpUp = 10 * ( Grade + 1 );
				//shareAttrExp( pSlayer, ExpUp, 1, 1, 8, _GCSkillToObjectOK1 );
				//increaseDomainExp( pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToObjectOK1, pTargetCreature->getLevel() );
				//increaseSkillExp( pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToObjectOK1 );
				//increaseAlignment( pSlayer, pTargetCreature, _GCSkillToObjectOK1 );

				// EffectžŠ »ýŒºÇØŒ­ ºÙÀÎŽÙ
				EffectTEMENOS* pEffectTEMENOS = new EffectTEMENOS( pTargetCreature );
				pEffectTEMENOS->setDeadline( output.Duration );
				pEffectTEMENOS->setDamageReduceRatio( 50 - (pSkillSlot->getExpLevel() / 10 ) );
				pTargetCreature->addEffect( pEffectTEMENOS );
				pTargetCreature->setFlag( Effect::EFFECT_CLASS_TEMENOS );

				// Effect žŠ ºê·Îµå Ä³œºÆÃ ÇÑŽÙ
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
				gcAddEffect.setEffectID( Effect::EFFECT_CLASS_TEMENOS );
				gcAddEffect.setDuration( output.Duration );
				pZone->broadcastPacket( targetX, targetY, &gcAddEffect );
			}
			else // œœ·¹ÀÌŸî ÀÏ °æ¿ì
			{
				// œœ·¹ÀÌŸî ÀÏ °æ¿ì DurationÀ» 0Àž·Î ŒŒÆÃÇÑŽÙ. ž¶ºñŽÂ ÇÏÁö ŸÊ°í µ¥¹ÌÁöžž Àû¿ëÇÑŽÙ.
				output.Duration = 0;
			}

			bool bCanSeePrayer = canSee( pTargetCreature, pSlayer );

			// œºÅ³À» »ç¿ëÇÑ »ç¶÷¿¡°Ô
			_GCSkillToObjectOK1.setSkillType( SkillType );
			_GCSkillToObjectOK1.setCEffectID( CEffectID );
			_GCSkillToObjectOK1.setTargetObjectID( TargetObjectID );
			_GCSkillToObjectOK1.setDuration( output.Duration );

			// œºÅ³ÀÇ Žë»óÀÚ¿¡°Ô
			_GCSkillToObjectOK2.setObjectID( pSlayer->getObjectID() );
			_GCSkillToObjectOK2.setSkillType( SkillType );
			_GCSkillToObjectOK2.setDuration( output.Duration );

			// œºÅ³À» »ç¿ëÇÑ »ç¶÷žž ºŒ Œö ÀÖŽÂ »ç¶÷¿¡°Ô
			_GCSkillToObjectOK3.setObjectID( pSlayer->getObjectID() );
			_GCSkillToObjectOK3.setSkillType( SkillType );
			_GCSkillToObjectOK3.setTargetXY( targetX, targetY );

			// œºÅ³ÀÇ Žë»óÀÚžž ºÒ Œö ÀÖŽÂ »ç¶÷¿¡°Ô
			_GCSkillToObjectOK4.setSkillType( SkillType );
			_GCSkillToObjectOK4.setTargetObjectID( TargetObjectID );

			// œºÅ³ »ç¿ëÀÚ¿Í Žë»óÀÚ žðµÎ ºŒ Œö ÀÖŽÂ »ç¶÷¿¡°Ô
			_GCSkillToObjectOK5.setObjectID( pSlayer->getObjectID() );
			_GCSkillToObjectOK5.setTargetObjectID( TargetObjectID );
			_GCSkillToObjectOK5.setSkillType( SkillType );
			_GCSkillToObjectOK5.setDuration( output.Duration );

			// œºÅ³ Žë»óÀÚ°¡ œºÅ³ »ç¿ëÀÚžŠ ºŒ Œö ŸøÀ» ¶§
			_GCSkillToObjectOK6.setXY( prayerX, prayerY );
			_GCSkillToObjectOK6.setSkillType( SkillType );
			_GCSkillToObjectOK6.setDuration( output.Duration );

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket( &_GCSkillToObjectOK1 );

			if ( pTargetCreature->isPC() )
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert( pTargetPlayer != NULL );

				if ( bCanSeePrayer )
					pTargetPlayer->sendPacket( &_GCSkillToObjectOK2 );
				else
					pTargetPlayer->sendPacket( &_GCSkillToObjectOK6 );
			}
			else
			{
				Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
				Assert( pTargetMonster != NULL );

				pTargetMonster->addEnemy( pSlayer );
			}

			list<Creature*> cList;
			cList.push_back( pSlayer );
			cList.push_back( pTargetCreature );

			cList = pZone->broadcastSkillPacket( prayerX, prayerY, targetX, targetY, &_GCSkillToObjectOK5, cList );

			pZone->broadcastPacket( prayerX, prayerY, &_GCSkillToObjectOK3, cList );
			pZone->broadcastPacket( targetX, targetY, &_GCSkillToObjectOK4, cList );

			pSkillSlot->setRunTime( output.Delay );
			
			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailNormal( pSlayer, getSkillType(), pTargetCreature );
		}
	}
	catch ( Throwable& t )
	{
		executeSkillFailException( pSlayer, getSkillType() );
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

TEMENOS g_TEMENOS;

