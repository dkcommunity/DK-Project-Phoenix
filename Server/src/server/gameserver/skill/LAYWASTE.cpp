#include "LAYWASTE.h"
#include "SimpleMissileSkill.h"

void LAYWASTE::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType     = getSkillType();
	param.SkillDamage   = output.Damage;
	param.Delay         = output.Delay;
	param.ItemClass     = Item::ITEM_CLASS_MAX;
	param.STRMultiplier = 0;
	param.DEXMultiplier = 0;
	param.INTMultiplier = 0;
	param.bMagicHitRoll = true;
	param.bMagicDamage  = true;
	param.bAdd          = false;

	SIMPLE_SKILL_OUTPUT result;

	if (0)
	{
		int x = pMonster->getX();
		int y = pMonster->getY();

		int Splash = 3 + rand()%5; 
		int range = 2;
		list<Creature*> creatureList;
		getSplashVictims(pMonster->getZone(), x, y, Creature::CREATURE_CLASS_MAX, creatureList, Splash, range);

		list<Creature*>::iterator itr = creatureList.begin();
		int i=0;
		for (; itr != creatureList.end(); itr++)
		{
			Creature* pTargetCreature = (*itr);
			Assert(pTargetCreature != NULL);
		
			if (pMonster!=pTargetCreature)
			{
				cout << "Master's LAYWASTE: " << i << endl;
				i++;
				g_SimpleMissileSkill.execute(pMonster, pTargetCreature, param, result);
			}
		}
	}
	else
	{
		g_SimpleMissileSkill.execute(pMonster, pEnemy, param, result);
	}

	__END_CATCH
}

LAYWASTE g_LAYWASTE;
