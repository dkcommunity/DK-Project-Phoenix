//////////////////////////////////////////////////////////////////////////////
// Filename    : HydroConvergence.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "HydroConvergence.h"
#include "ZoneUtil.h"
#include "EffectHydroConvergence.h"
#include "SimpleMissileSkill.h"

#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 아우스터즈 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType HydroConvergence::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
	SkillResultType skillResult;
	
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);
	Zone* pZone = pOusters->getZone();

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
	Assert(pTargetCreature != NULL);
	
	Inventory *pInventory = pOusters->getInventory();

	ZoneCoord_t		X = pTargetCreature->getX();
	ZoneCoord_t		Y = pTargetCreature->getY();

	SkillInfo*		pSkillInfo = g_pSkillInfoManager->getSkillInfo(getSkillType());

	SkillInput		input(pOusters, pOustersSkillSlot);
	SkillOutput		output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT	param;
	param.SkillType		=	getSkillType();
	param.SkillDamage	=	output.Damage;
	param.Delay			=	output.Delay;
	param.ItemClass		=	Item::ITEM_CLASS_OUSTERS_WRISTLET;
	param.STRMultiplier =	1;
	param.DEXMultiplier =	1;
	param.INTMultiplier =	8;
	param.bMagicHitRoll =	true;
	param.bMagicDamage	=	true;
	param.bAdd			=	false;
	param.bExpForTotalDamage = false;

	SIMPLE_SKILL_OUTPUT	result;

	skillResult = g_SimpleMissileSkill.execute( pOusters, TargetObjectID, pOustersSkillSlot, param, result );
	
	//bool bSummonItem = pInventory->hasEnoughNumItem( Item::ITEM_CLASS_PUPA, 5, 3);

	if( result.bSuccess )//&& bSummonItem )
	{
		//pInventory->decreaseNumItem( Item::ITEM_CLASS_PUPA, 5, 3, pOusters->getPlayer());

		Damage_t dotDamage = 0;
		Damage_t effectDamage = 0;
		Level_t	advenceLevel = 0;

		if( input.Range > 10 )
		{
			advenceLevel = input.Range - 10;
		}

		dotDamage = Damage_t(input.INTE/15 + advenceLevel);
		effectDamage = Damage_t(20 + (input.INTE/4 + input.INTE/30 + advenceLevel*3));

		EffectHydroConvergence* pEffect = new EffectHydroConvergence(pTargetCreature);
		pEffect->setDamage(Damage_t(output.Damage));
		pEffect->setUserObjectID( pOusters->getObjectID() );
		pEffect->setDotDamage(dotDamage);
		pEffect->setEffectDamage(effectDamage);
		pEffect->setTick(20);
		pEffect->setNextTime(20);
		pEffect->setDeadline(Duration_t(output.Duration));

		pTargetCreature->setFlag(Effect::EFFECT_CLASS_HYDRO_CONVERGENCE);
		pTargetCreature->addEffect(pEffect);

		GCAddEffect	gcAddEffect;
		gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HYDRO_CONVERGENCE );
		gcAddEffect.setDuration(output.Duration);

		pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
	}

    __END_CATCH
    
    return skillResult;
}
HydroConvergence g_HydroConvergence;
