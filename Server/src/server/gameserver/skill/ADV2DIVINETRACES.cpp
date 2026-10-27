#include "ADV2DIVINETRACES.h"
#include "SimpleCureSkill.h"
#include "Gpackets/GCAddEffect.h"
#include "EffectADV2DIVINETRACES.h"
#include "ZoneUtil.h"

const uint LightBloodDrainLevel = 10000;

SkillResultType ADV2DIVINETRACES::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	__BEGIN_TRY
	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);
	Zone* pZone = pSlayer->getZone();

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
	Assert(pTargetCreature != NULL);

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	input.TargetType = SkillInput::TARGET_OTHER;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.Level         = LightBloodDrainLevel;

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleCureSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);
	
	int healnum = 500;

	if( result.bSuccess)
	{
			EffectADV2DIVINETRACES* pEffect = new EffectADV2DIVINETRACES(pTargetCreature);
			pEffect->setDeadline(60);
			pEffect->setHealPoint(healnum);
			pEffect->setNextTime(10);
			pEffect->setTick(6);
			pTargetCreature->addEffect(pEffect);
			pTargetCreature->setFlag(Effect::EFFECT_CLASS_ADV2DIVINETRACES);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pTargetCreature->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2DIVINETRACES);
			gcAddEffect.setDuration(60);
			pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
	}

	__END_CATCH

  	return skillResult;
}

SkillResultType ADV2DIVINETRACES::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	SkillResultType skillResult;
	__BEGIN_TRY
	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);
	Zone* pZone = pSlayer->getZone();

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	input.TargetType = SkillInput::TARGET_SELF;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.Level         = LightBloodDrainLevel;

	SIMPLE_SKILL_OUTPUT result;

	skillResult = g_SimpleCureSkill.execute(pSlayer, pSkillSlot, param, result);

	int healnum = 500;


	if( result.bSuccess)
	{
			EffectADV2DIVINETRACES* pEffect = new EffectADV2DIVINETRACES(pSlayer);
			pEffect->setDeadline(60);
			pEffect->setHealPoint(healnum);
			pEffect->setNextTime(10);
			pEffect->setTick(6);
			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_ADV2DIVINETRACES);

			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pSlayer->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2DIVINETRACES);
			gcAddEffect.setDuration(60);
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);
	}

	__END_CATCH

  	return skillResult;
}

ADV2DIVINETRACES g_ADV2DIVINETRACES;
