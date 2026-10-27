#include "ADV2CONTRADICTO.h"
#include "Effect.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCSkillToObjectOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "EffectADV2StealInt.h"

SkillResultType ADV2CONTRADICTO::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);
	
	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);


		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL
			|| pTargetCreature->isNPC() || pTargetCreature->isMonster() ||  pTargetCreature->isSlayer())
		{
			executeSkillFailException(pSlayer, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;
		GCSkillToObjectOK4 _GCSkillToObjectOK4;
		GCSkillToObjectOK5 _GCSkillToObjectOK5;
		GCSkillToObjectOK6 _GCSkillToObjectOK6;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		SkillInput input(pSlayer, pSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, pTargetCreature, pSkillInfo->getRange());

		ZoneCoord_t targetX = pTargetCreature->getX();
		ZoneCoord_t targetY = pTargetCreature->getY();
		ZoneCoord_t myX     = pSlayer->getX();
		ZoneCoord_t myY     = pSlayer->getY();

		int stealint = 0;
		int Fstealint = 0;

		if (bManaCheck && bTimeCheck && bRangeCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToObjectOK1);

        		bool bCanSeeCaster = canSee(pTargetCreature, pSlayer);

			static Effect::EffectClass targetVampireEffects[] =
			{
				Effect::EFFECT_CLASS_MEPHISTO,
				Effect::EFFECT_CLASS_EXTREME,
				Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD,
				Effect::EFFECT_CLASS_MAX
			};

			static Effect::EffectClass targetOusterEffects[] =
			{
				//Effect::EFFECT_CLASS_RING_OF_FLARE,
				Effect::EFFECT_CLASS_HANDS_OF_FIRE,//5
				Effect::EFFECT_CLASS_EVADE,//1
				Effect::EFFECT_CLASS_CROSS_GUARD,//2
				Effect::EFFECT_CLASS_DIVINE_SPIRITS,//7
				//Effect::EFFECT_CLASS_SHARP_CHAKRAM,//3
				Effect::EFFECT_CLASS_WATER_BARRIER,
				//Effect::EFFECT_CLASS_FROZEN_ARMOR,
				Effect::EFFECT_CLASS_GNOMES_WHISPER,//10
				Effect::EFFECT_CLASS_REACTIVE_ARMOR,//8
				Effect::EFFECT_CLASS_GROUND_BLESS,//9
				Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT,//11
				//Effect::EFFECT_CLASS_ALLYSTER_WIND,//4
				//Effect::EFFECT_CLASS_EVGENEIABELLUM,	
				//Effect::EFFECT_CLASS_SQUALLY_BARRIER1,
				//Effect::EFFECT_CLASS_SQUALLY_BARRIER2,
				Effect::EFFECT_CLASS_FLAME_SIGHT,//6
				Effect::EFFECT_CLASS_FIRE_ELEMENTAL,//12
				Effect::EFFECT_CLASS_WATER_ELEMENTAL,//13
				//Effect::EFFECT_CLASS_FAVOROFLUNA,
				//Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK,
				//Effect::EFFECT_CLASS_BREATHOFDRYAD,
				//Effect::EFFECT_CLASS_ADV2GRACEOFELQUINES,
				//Effect::EFFECT_CLASS_ADV2SUMMONDEFENSIVESPINEL,
				//Effect::EFFECT_CLASS_EVGENEIABELLUM,
				Effect::EFFECT_CLASS_ADV2GRACEOFELQUINES,
				//Effect::EFFECT_CLASS_REACTIVE_ARMOR2,
				//Effect::EFFECT_CLASS_WATER_BARRIER2,
				Effect::EFFECT_CLASS_MAX
			};
			if(pTargetCreature->isVampire())
			{
				for ( int i=0; targetVampireEffects[i] != Effect::EFFECT_CLASS_MAX; ++i )
				{
					if ( pTargetCreature->isFlag( targetVampireEffects[i] ) )
					{
						Effect* pEffect = pTargetCreature->findEffect( targetVampireEffects[i] );
						if ( pEffect != NULL ) pEffect->setDeadline(0);
					}
				}

				Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
				stealint = pVampire->getINT();
				if(stealint < 200)
				{
					Fstealint = 20;
				}
				else
				{
					Fstealint = stealint*0.1;	
				}
			}
			if(pTargetCreature->isOusters())
			{
				for ( int i=0; targetOusterEffects[i] != Effect::EFFECT_CLASS_MAX; ++i )
				{
					if ( pTargetCreature->isFlag( targetOusterEffects[i] ) )
					{
						Effect* pEffect = pTargetCreature->findEffect( targetOusterEffects[i] );
						if ( pEffect != NULL ) pEffect->setDeadline(0);
					}
				}
				Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
				stealint = pOusters->getINT();

				if(stealint < 200)
				{
					Fstealint = 20;
				}
				else
				{
					Fstealint = stealint*0.1;	
				}
			}
			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK1.setDuration(output.Duration);
		
			_GCSkillToObjectOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(output.Duration);
		
			_GCSkillToObjectOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK3.setSkillType(SkillType);
			_GCSkillToObjectOK3.setTargetXY (targetX, targetY);
			
			_GCSkillToObjectOK4.setSkillType(SkillType);
			_GCSkillToObjectOK4.setTargetObjectID(TargetObjectID);
			_GCSkillToObjectOK4.setDuration(output.Duration);
			
			_GCSkillToObjectOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToObjectOK5.setSkillType(SkillType);
			_GCSkillToObjectOK5.setTargetObjectID (TargetObjectID);
			_GCSkillToObjectOK5.setDuration(output.Duration);
			
			_GCSkillToObjectOK6.setXY(myX, myY);
			_GCSkillToObjectOK6.setSkillType(SkillType);
			_GCSkillToObjectOK6.setDuration(output.Duration);

			list<Creature *> cList;
			cList.push_back(pTargetCreature);
			cList.push_back(pSlayer);
			cList = pZone->broadcastSkillPacket(myX, myY, targetX, targetY, &_GCSkillToObjectOK5, cList);

			pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3, cList);
			pZone->broadcastPacket(targetX, targetY, &_GCSkillToObjectOK4, cList);

			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			if (pTargetCreature->isPC()) 
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				if (pTargetPlayer == NULL)
				{
					return SKILL_RESULT_FAIL_INVALID_TARGET;
				}

				if (bCanSeeCaster) pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
				else pTargetPlayer->sendPacket(&_GCSkillToObjectOK6);
			}
			
		/*	if(pSlayer->isFlag(Effect::EFFECT_CLASS_taozhuang221_set_5) || pSlayer->isFlag(Effect::EFFECT_CLASS_taozhuang221_set_6))
			{

				EffectADV2StealInt* pEffect = new EffectADV2StealInt(pSlayer);
				pEffect->setDeadline(600);
				pEffect->setReflection(Fstealint);
				pSlayer->setFlag(Effect::EFFECT_CLASS_ADV2StealInt);
				pSlayer->addEffect(pEffect);

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(pSlayer->getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2StealInt);
				gcAddEffect.setDuration(600);
				pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect );
				pSlayer->initAllStatAndSend();
			}*/

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

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2CONTRADICTO g_ADV2CONTRADICTO;
