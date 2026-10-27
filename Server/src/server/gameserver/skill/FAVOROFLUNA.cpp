//////////////////////////////////////////////////////////////////////////////
// Filename    : FAVOROFLUNA.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "FAVOROFLUNA.h"
#include "EffectFAVOROFLUNA.h"
#include "PacketUtil.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCOtherModifyInfo.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType FAVOROFLUNA::execute(Ousters* pOusters, ObjectID_t TargetObjectID,  OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pSkillSlot->getSkillType());

			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL)	
		{
			executeSkillFailException(pOusters, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
	
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pSkillSlot->getExpLevel()/2;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, pTargetCreature, pSkillInfo->getRange());
		bool bHitRoll	 = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pSkillSlot);
		bool bEffected   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_FAVOROFLUNA);
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		if ( !pTargetCreature->isOusters() && !canAttack( pOusters, pTargetCreature ) )
		{
			bHitRoll = false;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToObjectOK1);

			// ±âŒúÀÇ È¿°úÄ¡ ¹× ÁöŒÓœÃ°£À» °è»êÇÑŽÙ.
			SkillInput input(pOusters, pSkillSlot);
			if( pTargetCreature->getCreatureClass() == Creature::CREATURE_CLASS_OUSTERS )
				input.TargetType = SkillInput::TARGET_SELF;
			else
				input.TargetType = SkillInput::TARGET_OTHER;
			SkillOutput output;
			computeOutput(input, output);

			pSkillSlot->setRunTime(output.Delay);

			int hpbonus = min( 300, (int)( 100 + ( input.INTE * 0.5 )) );
			int defensebonus = min( 120, (int)(20 + ( input.INTE * 0.1 )));
                        int resbonus = min( 20, (int)(10 + ( input.INTE * 0.01) ));

			EffectFAVOROFLUNA* pEffect = new EffectFAVOROFLUNA(pTargetCreature);
			pEffect->setRESBonus(resbonus);
			pEffect->setHPBonus( hpbonus );
			pEffect->setDefenseBonus( defensebonus );
			pEffect->setDeadline(output.Duration);
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_FAVOROFLUNA);
			pTargetCreature->addEffect(pEffect);


			if ( pTargetCreature->isPC() )
			{
				PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pTargetCreature);
				pTargetPC->initAllStatAndSend();
			}

			// °æÇèÄ¡žŠ ¿Ã·ÁÁØŽÙ.
			
			Coord_t targetX = pTargetCreature->getX();
			Coord_t targetY = pTargetCreature->getY();
			Coord_t myX     = pOusters->getX();
			Coord_t myY     = pOusters->getY();

			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(0);
		
			_GCSkillToObjectOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(0);
		
			_GCSkillToObjectOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK3.setSkillType(SkillType);
			_GCSkillToObjectOK3.setTargetXY (targetX, targetY);
			
			_GCSkillToObjectOK4.setSkillType(SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK4.setDuration(0);
			
			_GCSkillToObjectOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setDuration(0);

			list<Creature *> cList;
			cList.push_back(pTargetCreature);
			cList.push_back(pOusters);
			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);
			pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3, cList);
			pZone->broadcastPacket(targetX, targetY, &_GCSkillToObjectOK4, cList);

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToObjectOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToObjectOK1);
		
			if (pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);

				_GCSkillToObjectOK2.setObjectID(pOusters->getObjectID());

				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			}
			else
			{
			}
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(TargetObjectID);
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_FAVOROFLUNA);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(targetX, targetY, &gcAddEffect);

			
			return SKILL_RESULT_SUCCESS;
		}
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), pTargetCreature);
		}

	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}


	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType FAVOROFLUNA::execute(Ousters* pOusters, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pSkillSlot->getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

	        SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t myX = pOusters->getX();
		ZoneCoord_t myY = pOusters->getY();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pSkillSlot->getExpLevel()/2;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pSkillSlot);
		bool bEffected   = pOusters->isFlag(Effect::EFFECT_CLASS_FAVOROFLUNA);
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );


		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pOusters, pSkillSlot);
			input.TargetType = SkillInput::TARGET_SELF;
			SkillOutput output;
			computeOutput(input, output);


			int hpbonus = min( 300, (int)( 100 + ( input.INTE * 0.5 )) );
			int defensebonus = min( 120, (int)(20 + ( input.INTE * 0.1 )));
                        int resbonus = min( 20, (int)(10 + ( input.INTE * 0.01) ));

			EffectFAVOROFLUNA* pEffect = new EffectFAVOROFLUNA(pOusters);
			pEffect->setRESBonus(resbonus);
			pEffect->setHPBonus( hpbonus );
			pEffect->setDefenseBonus( defensebonus );
			pEffect->setDeadline(output.Duration);
			pOusters->setFlag(Effect::EFFECT_CLASS_FAVOROFLUNA);
			pOusters->addEffect(pEffect);


			OUSTERS_RECORD prev;
			pOusters->getOustersRecord(prev);
			pOusters->initAllStat();
			pOusters->addModifyInfo(prev, _GCSkillToSelfOK1);

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
	
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2, pOusters);
		
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pOusters->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_FAVOROFLUNA);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(myX, myY, &gcAddEffect);

			GCOtherModifyInfo gcOtherModifyInfo;
			makeGCOtherModifyInfo(&gcOtherModifyInfo, pOusters, &prev);
			pZone->broadcastPacket(myX, myY, &gcOtherModifyInfo, pOusters);
			
			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}



	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

FAVOROFLUNA g_FAVOROFLUNA;
