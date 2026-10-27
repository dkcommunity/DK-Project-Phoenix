//////////////////////////////////////////////////////////////////////////////
// Filename    : LethalClaw.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2CHAOSSPLITTER.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "NewRankGem.h"
//////////////////////////////////////////////////////////////////////////////
// »ýŒºÀÚ
// ž¶œºÅ©žŠ ÃÊ±âÈ­ÇÑŽÙ.
//////////////////////////////////////////////////////////////////////////////
ADV2CHAOSSPLITTER::ADV2CHAOSSPLITTER()
	throw()
{
	__BEGIN_TRY

	m_pLethalClawMask[0].set(0,  0);
	m_pLethalClawMask[1].set(-1, -1);
	m_pLethalClawMask[2].set(0, -1);
	m_pLethalClawMask[3].set(1, -1);
	m_pLethalClawMask[4].set(-1,  0);
	m_pLethalClawMask[5].set(1,  0);
	m_pLethalClawMask[6].set(-1,  1);
	m_pLethalClawMask[7].set(0,  1);
	m_pLethalClawMask[8].set(1,  1);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2CHAOSSPLITTER::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY


	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pVampire, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2CHAOSSPLITTER::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY



	SkillInput input(pVampire);
	SkillOutput output;
	computeOutput(input, output);

	if (pVampire->hasRankGem(NewRankGem::SKILL_ADV2_CHAOS_SPLITTER_DAM ) )
	{
		NewRankGem* pRankGemCHAOSD = pVampire->getRankGem( NewRankGem::SKILL_ADV2_CHAOS_SPLITTER_DAM );
		Assert( pRankGemCHAOSD != NULL );

		output.Damage += pRankGemCHAOSD->getGrade();
	}

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = false;
	param.bMagicDamage  = false;
	if (pVampire->hasRankGem(NewRankGem::SKILL_ADV2_CHAOS_SPLITTER_CRI ) )
	{
		NewRankGem* pRankGemCHAOSCRI = pVampire->getRankGem( NewRankGem::SKILL_ADV2_CHAOS_SPLITTER_CRI );
		Assert( pRankGemCHAOSCRI != NULL );

		param.bAdd          = true;
	}
	else
	{
	param.bAdd          = false;
	}

	SIMPLE_SKILL_OUTPUT result;

	for (int i=0; i<9; i++)
	{
		param.addMask(m_pLethalClawMask[i].x, m_pLethalClawMask[i].y, 100);
	}

	return g_SimpleTileMissileSkill.execute(pVampire, X, Y, pVampireSkillSlot, param, result, CEffectID);

	__END_CATCH
}

ADV2CHAOSSPLITTER g_ADV2CHAOSSPLITTER;
