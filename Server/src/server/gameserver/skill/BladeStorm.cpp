//////////////////////////////////////////////////////////////////////////////
// Filename    : BladeStorm.cpp
// Written by  : bigheart 
// Description : 20080403 
//////////////////////////////////////////////////////////////////////////////

#include "BladeStorm.h"
#include "SimpleLinearMeleeSkill.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BladeStorm::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Zone* pZone = pSlayer->getZone();
		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID);
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType BladeStorm::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);


	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_BLADE_STORM_DAM ) )
	{
		NewRankGem* pRankGem = pSlayer->getRankGem( NewRankGem::SKILL_BLADE_STORM_DAM );
		Assert( pRankGem != NULL );

		Gemdamage += pRankGem->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_BLADE;
	//param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
//	param.Level			= 2;	// 2로 해야 하나?
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	param.bAdd          = true;

	SIMPLE_SKILL_OUTPUT result;
	
	return g_SimpleLinearMeleeSkill.execute(pSlayer, X, Y, pSkillSlot, param, result);

	__END_CATCH
}

BladeStorm g_BladeStorm;




