//////////////////////////////////////////////////////////////////////////////
// Filename    : NymphRecovery.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "NymphRecovery.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "EffectPoison.h"
#include "EffectYellowPoisonToCreature.h"
#include "EffectDarkBluePoison.h"
#include "EffectGreenStalker.h"
#include "EffectFlare.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType NymphRecovery::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	BYTE Grade = 0;
	if ( pOustersSkillSlot->getExpLevel() < 15 ) Grade = 0;
	else if ( pOustersSkillSlot->getExpLevel() < 30 ) Grade = 1;
	else Grade = 2;

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType(), Grade);

			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !pTargetCreature->isOusters())
		{
			executeSkillFailException(pOusters, getSkillType(), Grade);

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;

		SkillType_t       SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Level_t           SkillLevel = pOustersSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, pTargetCreature, pSkillInfo->getRange()) && canHit( pOusters, pTargetCreature, SkillType, pOustersSkillSlot->getExpLevel() );
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);
		bool bSatisfyRequire	= pOusters->satisfySkillRequire( pSkillInfo );
		bool bHPCheck	= pTargetOusters->getHP(ATTR_MAX) > pTargetOusters->getHP() || pTargetOusters->getMP(ATTR_MAX) > pTargetOusters->getMP();

		// 독 계열 해소
		bool bGreenPoison    = false; // GreenPoison을 치료할까의 여부
		bool bYellowPoison   = false; // YellowPoison을 치료할까의 여부
		bool bDarkBluePoison = false; // DarkBluePoison을 치료할까의 여부
		bool bGreenStalker   = false; // GreenStalker를 치료할까의 여부
		bool bFlare          = false; // Flare 를 치료할까의 여부

		bool bEffected = false;

		EffectPoison*                 pEffectPoison                 = NULL;
		EffectYellowPoisonToCreature* pEffectYellowPoisonToCreature = NULL;
		EffectDarkBluePoison*         pEffectDBP                    = NULL;
		EffectGreenStalker*           pEffectGreenStalker           = NULL;
		EffectFlare*                  pEffectFlare                  = NULL;

		if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_POISON)) 
		{
			pEffectPoison = (EffectPoison*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_POISON);
			Assert(pEffectPoison != NULL);

			bGreenPoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectPoison->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE) && SkillLevel > 15 )
		{
			pEffectYellowPoisonToCreature = (EffectYellowPoisonToCreature*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
			Assert(pEffectYellowPoisonToCreature != NULL);

			bYellowPoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectYellowPoisonToCreature->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_DARKBLUE_POISON) && SkillLevel > 15 )
		{
			pEffectDBP = (EffectDarkBluePoison*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_DARKBLUE_POISON);
			Assert(pEffectDBP != NULL);

			bDarkBluePoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectDBP->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_GREEN_STALKER)) 
		{
			pEffectGreenStalker = (EffectGreenStalker*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_GREEN_STALKER);
			Assert(pEffectGreenStalker != NULL);

			bGreenStalker = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectGreenStalker->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pTargetCreature->isEffect(Effect::EFFECT_CLASS_FLARE)) 
		{
			pEffectFlare = (EffectFlare*)pTargetCreature->findEffect(Effect::EFFECT_CLASS_FLARE);
			Assert(pEffectFlare != NULL);

			bFlare = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectFlare->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bSatisfyRequire && bHPCheck)
		{
			// 마나를 줄인다.
			decreaseMana(pOusters, RequiredMP, _GCSkillToObjectOK1);

			// 이펙트의 효과와 지속시간을 계산한다.
			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			input.TargetType = SkillInput::TARGET_OTHER;
			computeOutput(input, output);

			// Holy Cross 이펙트가 걸려있으면
			if ( pOusters->isFlag( Effect::EFFECT_CLASS_WATER_FORCE ) )
			{
				// Nymph Recovery 힐량 두배
				output.Damage = (Damage_t)(output.Damage * 1.5);
			}

			// 각각의 저주를 삭제하고,
			// 패킷에다 이펙트 삭제하라고 더한다.
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pTargetCreature->getObjectID());

			// 각각의 독마다 치료를 하고,
			// 패킷에다 이펙트 삭제하라고 더한다.
			if (bGreenPoison)
			{
				pEffectPoison->setDeadline(0);
				pTargetCreature->removeFlag(Effect::EFFECT_CLASS_POISON);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_POISON);
			}
			if (bYellowPoison)
			{
				pEffectYellowPoisonToCreature->setDeadline(0);
				pTargetCreature->removeFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
			}
			if (bDarkBluePoison)
			{
				pEffectDBP->setDeadline(0);
				pTargetCreature->removeFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DARKBLUE_POISON);
			}
			if (bGreenStalker)
			{
				pEffectGreenStalker->setDeadline(0);
				pTargetCreature->removeFlag(Effect::EFFECT_CLASS_GREEN_STALKER);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GREEN_STALKER);
			}
			if (bFlare)
			{
				pEffectFlare->setDeadline(0);
				//pTargetCreature->removeFlag(Effect::EFFECT_CLASS_FLARE);
				//gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_FLARE);
			}

			if( bEffected ) {
				pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcRemoveEffect);
			}

			HP_t final = min( (HP_t)pTargetOusters->getHP(ATTR_MAX),(HP_t)( pTargetOusters->getHP() + output.Damage ));
			if ( final > pTargetOusters->getHP(ATTR_MAX) - pTargetOusters->getSilverDamage() )
			{
				pTargetOusters->setSilverDamage( pTargetOusters->getHP(ATTR_MAX) - final );
				_GCSkillToObjectOK2.addShortData(MODIFY_SILVER_DAMAGE, pTargetOusters->getSilverDamage());
			}

			if ( pTargetOusters->getHP() != final )
			{
				pTargetOusters->setHP( final );
				_GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_HP, final);
			}

			GCStatusCurrentHP gcHP;
			gcHP.setObjectID( pTargetOusters->getObjectID() );
			gcHP.setCurrentHP( final );

			if ( pTargetOusters->getMP(ATTR_MAX) > pTargetOusters->getMP() )
			{
				final = min( (MP_t)pTargetOusters->getMP(ATTR_MAX), (MP_t)(pTargetOusters->getMP() + output.Damage ));
				if ( pTargetOusters->getMP() != final )
				{
					pTargetOusters->setMP( final );
					_GCSkillToObjectOK2.addShortData(MODIFY_CURRENT_MP, final);
				}
			}

			// 패킷을 준비해서 보낸다.
			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setGrade(Grade);

			_GCSkillToObjectOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setGrade(Grade);

			_GCSkillToObjectOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setGrade(Grade);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToObjectOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			if (pOusters != pTargetCreature && pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			}

			list<Creature*> cList;
			cList.push_back(pOusters);
			cList.push_back(pTargetCreature);
		
			pZone->broadcastPacket(pOusters->getX(), pOusters->getY(),  &_GCSkillToObjectOK5 , cList);
			pZone->broadcastPacket(pTargetOusters->getX(), pTargetOusters->getY(),  &gcHP, pTargetOusters);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), pTargetCreature, Grade);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), Grade);
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType NymphRecovery::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	BYTE Grade = 0;
	if ( pOustersSkillSlot->getExpLevel() < 15 ) Grade = 0;
	else if ( pOustersSkillSlot->getExpLevel() < 30 ) Grade = 1;
	else Grade = 2;

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType(), Grade);
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		Level_t           SkillLevel = pOustersSkillSlot->getExpLevel();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pOusters);
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);
		bool bSatisfyRequire	= pOusters->satisfySkillRequire( pSkillInfo );
		bool bHPCheck	= pOusters->getHP(ATTR_MAX) > pOusters->getHP() || pOusters->getMP(ATTR_MAX) > pOusters->getMP();

		// 독 계열 해소
		bool bGreenPoison    = false; // GreenPoison을 치료할까의 여부
		bool bYellowPoison   = false; // YellowPoison을 치료할까의 여부
		bool bDarkBluePoison = false; // DarkBluePoison을 치료할까의 여부
		bool bGreenStalker   = false; // GreenStalker를 치료할까의 여부
		bool bFlare          = false; // Flare 를 치료할까의 여부

		bool bEffected = false;

		EffectPoison*                 pEffectPoison                 = NULL;
		EffectYellowPoisonToCreature* pEffectYellowPoisonToCreature = NULL;
		EffectDarkBluePoison*         pEffectDBP                    = NULL;
		EffectGreenStalker*           pEffectGreenStalker           = NULL;
		EffectFlare*                  pEffectFlare                  = NULL;

		if (pOusters->isEffect(Effect::EFFECT_CLASS_POISON)) 
		{
			pEffectPoison = (EffectPoison*)pOusters->findEffect(Effect::EFFECT_CLASS_POISON);
			Assert(pEffectPoison != NULL);

			bGreenPoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectPoison->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pOusters->isEffect(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE) && SkillLevel > 15 )
		{
			pEffectYellowPoisonToCreature = (EffectYellowPoisonToCreature*)pOusters->findEffect(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
			Assert(pEffectYellowPoisonToCreature != NULL);

			bYellowPoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectYellowPoisonToCreature->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pOusters->isEffect(Effect::EFFECT_CLASS_DARKBLUE_POISON)) 
		{
			pEffectDBP = (EffectDarkBluePoison*)pOusters->findEffect(Effect::EFFECT_CLASS_DARKBLUE_POISON);
			Assert(pEffectDBP != NULL);

			bDarkBluePoison = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectDBP->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pOusters->isEffect(Effect::EFFECT_CLASS_GREEN_STALKER)) 
		{
			pEffectGreenStalker = (EffectGreenStalker*)pOusters->findEffect(Effect::EFFECT_CLASS_GREEN_STALKER);
			Assert(pEffectGreenStalker != NULL);

			bGreenStalker = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectGreenStalker->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (pOusters->isEffect(Effect::EFFECT_CLASS_FLARE) && SkillLevel > 15 )
		{
			pEffectFlare = (EffectFlare*)pOusters->findEffect(Effect::EFFECT_CLASS_FLARE);
			Assert(pEffectFlare != NULL);

			bFlare = HitRoll::isSuccessNewCurePoisonForOusters( pOusters->getINT(), SkillLevel, pEffectFlare->getPoisonLevel() );
			bEffected = true;
			bHPCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bSatisfyRequire && bHPCheck)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToSelfOK1);

			// 이펙트의 효과와 지속시간을 계산한다.
			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			input.TargetType = SkillInput::TARGET_SELF;
			computeOutput(input, output);

			// Holy Cross 이펙트가 걸려있으면
			if ( pOusters->isFlag( Effect::EFFECT_CLASS_WATER_FORCE ) )
			{
				// Nymph Recovery 힐량 두배
				output.Damage = (Damage_t)(output.Damage * 1.5);
			}

			// 각각의 저주를 삭제하고,
			// 패킷에다 이펙트 삭제하라고 더한다.
			GCRemoveEffect gcRemoveEffect;
			gcRemoveEffect.setObjectID(pOusters->getObjectID());

			// 각각의 독마다 치료를 하고,
			// 패킷에다 이펙트 삭제하라고 더한다.
			if (bGreenPoison)
			{
				pEffectPoison->setDeadline(0);
				pOusters->removeFlag(Effect::EFFECT_CLASS_POISON);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_POISON);
			}
			if (bYellowPoison)
			{
				pEffectYellowPoisonToCreature->setDeadline(0);
				pOusters->removeFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
			}
			if (bDarkBluePoison)
			{
				pEffectDBP->setDeadline(0);
				pOusters->removeFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DARKBLUE_POISON);
			}
			if (bGreenStalker)
			{
				pEffectGreenStalker->setDeadline(0);
				pOusters->removeFlag(Effect::EFFECT_CLASS_GREEN_STALKER);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_GREEN_STALKER);
			}
			if (bFlare)
			{
				pEffectFlare->setDeadline(0);
				pOusters->removeFlag(Effect::EFFECT_CLASS_FLARE);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_FLARE);
			}

			if( bEffected ) {
				pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &gcRemoveEffect);
			}

			HP_t final = min( (HP_t)pOusters->getHP(ATTR_MAX),(HP_t)( pOusters->getHP() + output.Damage ));
			if ( final > pOusters->getHP(ATTR_MAX) - pOusters->getSilverDamage() )
			{
				pOusters->setSilverDamage( pOusters->getHP(ATTR_MAX) - final );
				_GCSkillToSelfOK1.addShortData(MODIFY_SILVER_DAMAGE, pOusters->getSilverDamage());
			}

			if ( pOusters->getHP() != final )
			{
				pOusters->setHP( final );
				_GCSkillToSelfOK1.addShortData(MODIFY_CURRENT_HP, final);
			}

			GCStatusCurrentHP gcHP;
			gcHP.setObjectID( pOusters->getObjectID() );
			gcHP.setCurrentHP( final );

			if ( pOusters->getMP(ATTR_MAX) > pOusters->getMP() )
			{
				final = min( (MP_t)pOusters->getMP(ATTR_MAX), (MP_t)(pOusters->getMP() + output.Damage ));
				if ( pOusters->getMP() != final )
				{
					pOusters->setMP( final );
					_GCSkillToSelfOK1.addShortData(MODIFY_CURRENT_MP, final);
				}
			}

			// 패킷을 준비해서 보낸다.
			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setGrade(Grade);
			
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setGrade(Grade);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2, pOusters);
			pZone->broadcastPacket(myX, myY, &gcHP, pOusters);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL, Grade);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), Grade);
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

NymphRecovery g_NymphRecovery;
