#include "ADV2INFINITYBLAZE.h"
#include "SimpleMissileSkill.h"
#include "RankBonus.h"
#include "Gpackets/GCAddEffect.h"
#include "EffectADV2INFINITYBLAZE.h"

SkillResultType ADV2INFINITYBLAZE::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	SkillResultType skillResult;
	Zone* pZone = pOusters->getZone();
	Assert(pZone != NULL);

	Creature* pTargetCreature = pZone->getCreature(TargetObjectID);

	if ( pTargetCreature == NULL )
	{
		executeSkillFailException(pOusters, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

	SkillInput input(pOusters, pOustersSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	int bufftime = 50;
	int ratio = max( 40, min( 80, 100 - ( 20 + pTargetCreature->getLevel() - pOusters->getLevel() ) ) );

	/*if(pOusters->isFlag(Effect::EFFECT_CLASS_taozhuang221_set_5) || pOusters->isFlag(Effect::EFFECT_CLASS_taozhuang221_set_6))
	{
		bufftime = 10;
		ratio = 100;
		output.Damage = output.Damage*1.2;
	}*/

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_OUSTERS_WRISTLET;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	skillResult =  g_SimpleMissileSkill.execute(pOusters, TargetObjectID, pOustersSkillSlot, param, result, CEffectID);
	if ( result.bSuccess )
	{

			if ( rand() % 100 <= ratio )
			{

				EffectADV2INFINITYBLAZE* pEffect = new EffectADV2INFINITYBLAZE(pTargetCreature);
				pEffect->setDeadline(bufftime);
				pTargetCreature->setFlag(Effect::EFFECT_CLASS_ADV2INFINITYBLAZE);
				pTargetCreature->addEffect(pEffect);

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(pTargetCreature->getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ADV2INFINITYBLAZE);
				gcAddEffect.setDuration(bufftime);
				pTargetCreature->getZone()->broadcastPacket( pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect );

				if ( pTargetCreature->isPC() )
				{
					PlayerCreature* pTargetPC = dynamic_cast<PlayerCreature*>(pTargetCreature);
					pTargetPC->initAllStatAndSend();
				}
			}
	}

	return skillResult;

	__END_CATCH
}

ADV2INFINITYBLAZE g_ADV2INFINITYBLAZE;

