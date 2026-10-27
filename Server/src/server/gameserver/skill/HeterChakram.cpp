//////////////////////////////////////////////////////////////////////////////
// Filename    : HeterChakram.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "HeterChakram.h"
#include "ZoneUtil.h"
#include "EffectHeterChakram.h"
#include "SimpleMissileSkill.h"

#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 아우스터즈 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType HeterChakram::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
	SkillResultType skillResult;
	
    __BEGIN_TRY

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);
	Zone* pZone = pOusters->getZone();

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
	Assert(pTargetCreature != NULL);
	
	Inventory *pInventory = pOusters->getInventory();

	SkillInput		input(pOusters, pOustersSkillSlot);
	SkillOutput		output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT	param;
	param.SkillType		=	getSkillType();
	param.SkillDamage	=	output.Damage;
	param.Delay			=	output.Delay;
	param.ItemClass		=	Item::ITEM_CLASS_OUSTERS_CHAKRAM;
	param.STRMultiplier =	8;
	param.DEXMultiplier =	1;
	param.INTMultiplier =	1;
	param.bMagicHitRoll =	false;
	param.bMagicDamage	=	false;
	param.bAdd			=	true;
	param.bExpForTotalDamage = false;

	SIMPLE_SKILL_OUTPUT	result;

	skillResult = g_SimpleMissileSkill.execute( pOusters, TargetObjectID, pOustersSkillSlot, param, result );
	
	//bool bSummonItem = pInventory->hasEnoughNumItem( Item::ITEM_CLASS_PUPA, 5, 3);

	if( result.bSuccess)// && bSummonItem )
	{
		//pInventory->decreaseNumItem( Item::ITEM_CLASS_PUPA, 5, 3, pOusters->getPlayer() );

		EffectHeterChakram* pEffect = new EffectHeterChakram(pTargetCreature);
		pEffect->setDamage(output.Damage);
		pEffect->setUserObjectID( pOusters->getObjectID() );
		pEffect->setNextTime(5);
		pEffect->setTargetCreature(pTargetCreature->getCreatureClass());

		if( pTargetCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			pEffect->setTargetSpriteType(pMonster->getSpriteType());
		}

		pTargetCreature->setFlag(Effect::EFFECT_CLASS_HETER_CHAKRAM);
		pTargetCreature->addEffect(pEffect);

		GCAddEffect	gcAddEffect;
		gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HETER_CHAKRAM );
		gcAddEffect.setDuration(0);

		pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
	}

    __END_CATCH
    
    return skillResult;
}
void HeterChakram::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = pMonster->getZone();

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	bool damageUp = pMonster->isFlag(Effect::EFFECT_CLASS_BOSS_ANGER);

	int damage = 50+(input.INTE*2);
	if(damageUp)
	{
	   damage = damage*3;
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;

	SIMPLE_SKILL_OUTPUT result;

	g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);


	if( result.bSuccess)
	{

		EffectHeterChakram* pEffect = new EffectHeterChakram(pEnemy);
		pEffect->setDamage(damage);
		pEffect->setUserObjectID( pMonster->getObjectID() );
		pEffect->setNextTime(5);
		pEffect->setTargetCreature(pEnemy->getCreatureClass());


		pEnemy->setFlag(Effect::EFFECT_CLASS_HETER_CHAKRAM);
		pEnemy->addEffect(pEffect);

		GCAddEffect	gcAddEffect;
		gcAddEffect.setObjectID( pEnemy->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HETER_CHAKRAM );
		gcAddEffect.setDuration(0);

		pZone->broadcastPacket(pEnemy->getX(), pEnemy->getY(), &gcAddEffect);
	}
	__END_CATCH
}
HeterChakram g_HeterChakram;
