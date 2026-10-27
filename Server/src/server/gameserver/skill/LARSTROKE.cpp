//////////////////////////////////////////////////////////////////////////////
// Filename    : RainbowSlasher.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "LARSTROKE.h"
//#include "SimpleMissileSkill.h"
#include "SimpleMeleeSkill.h"
#include "RankBonus.h"
#include "SkillUtil.h"
#include "Item.h"
#include "NewRankGem.h"
//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType LARSTROKE::execute(Slayer * pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	Damage_t Gemdamage = output.Damage;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_LAR_STROKE_DAM ) )
	{
		NewRankGem* pRankGemCRUSHINGLARD = pSlayer->getRankGem( NewRankGem::SKILL_LAR_STROKE_DAM );
		Assert( pRankGemCRUSHINGLARD != NULL );

		Gemdamage += pRankGemCRUSHINGLARD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = Gemdamage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;

	if (pSlayer->hasRankGem(NewRankGem::SKILL_LAR_STROKE_CRI ) )
	{
		NewRankGem* pRankGemCRUSHINGLARCRI = pSlayer->getRankGem( NewRankGem::SKILL_LAR_STROKE_CRI );
		Assert( pRankGemCRUSHINGLARCRI != NULL );

		param.bAdd          = true;
	}
	else
	{	
		param.bAdd          = false;
	}

	SIMPLE_SKILL_OUTPUT result;

	//return g_SimpleMissileSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);
	
	return g_SimpleMeleeSkill.execute(pSlayer, TargetObjectID, pSkillSlot, param, result);

	__END_CATCH
}

LARSTROKE g_LARSTROKE;
