//////////////////////////////////////////////////////////////////////////////
// Filename    : GREATHEAL2.cpp
// Written by  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GREATHEAL2.h"
#include "SimpleCureSkill.h"
#include "NewRankGem.h"

const uint LightBloodDrainLevel = 10000;

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType GREATHEAL2::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	input.TargetType = SkillInput::TARGET_OTHER;
	computeOutput(input, output);

	if (pSlayer->hasRankGem(NewRankGem::SKILL_GREAT_HEAL_2_DAM ) )
	{
		NewRankGem* pRankGemGREATHEAL2 = pSlayer->getRankGem( NewRankGem::SKILL_GREAT_HEAL_2_DAM );
		Assert( pRankGemGREATHEAL2 != NULL );

		output.Damage += pRankGemGREATHEAL2->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.Level         = LightBloodDrainLevel;

	SIMPLE_SKILL_OUTPUT result;

	return g_SimpleCureSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType GREATHEAL2::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	input.TargetType = SkillInput::TARGET_SELF;
	computeOutput(input, output);

	if (pSlayer->hasRankGem(NewRankGem::SKILL_GREAT_HEAL_2_DAM ) )
	{
		NewRankGem* pRankGemGREATHEAL2 = pSlayer->getRankGem( NewRankGem::SKILL_GREAT_HEAL_2_DAM );
		Assert( pRankGemGREATHEAL2 != NULL );

		output.Damage += pRankGemGREATHEAL2->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.STRMultiplier = 1;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 8;
	param.Level         = LightBloodDrainLevel;

	SIMPLE_SKILL_OUTPUT result;

	return g_SimpleCureSkill.execute(pSlayer, pSkillSlot, param, result);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
}

GREATHEAL2 g_GREATHEAL2;
