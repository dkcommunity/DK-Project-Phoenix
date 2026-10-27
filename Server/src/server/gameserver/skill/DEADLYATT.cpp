//////////////////////////////////////////////////////////////////////////////
// Filename    : DEADLYATT.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DEADLYATT.h"
#include "SimpleTileMissileSkill.h"
#include "RankBonus.h"
#include "Vampire.h"
#include "GamePlayer.h"
//////////////////////////////////////////////////////////////////////////////
// 积己磊
// 付胶农甫 檬扁拳茄促.
//////////////////////////////////////////////////////////////////////////////
DEADLYATT::DEADLYATT()
	throw()
{
	__BEGIN_TRY

	m_pGoreGlandFireMask[0].set(0,  1);
	m_pGoreGlandFireMask[1].set(-1, -1);
	m_pGoreGlandFireMask[2].set(0, -1);
	m_pGoreGlandFireMask[3].set(1, -1);
	m_pGoreGlandFireMask[4].set(-1,  0);
	m_pGoreGlandFireMask[5].set(1,  0);
	m_pGoreGlandFireMask[6].set(-1,  1);
	m_pGoreGlandFireMask[7].set(0,  1);
	m_pGoreGlandFireMask[8].set(1,  1);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 阁胶磐 鸥老 勤甸矾
//////////////////////////////////////////////////////////////////////////////
void DEADLYATT::execute(Monster* pMonster, Creature *pEnemy)
	throw(Error)
{
	__BEGIN_TRY
        Zone* pZone = pMonster->getZone();
	Assert( pZone != NULL );
	
	ZoneCoord_t targetX, targetY;
	
	targetX = pMonster->getX();
	targetY = pMonster->getY();

	ObjectID_t m_UserObjectID = pMonster->getOwnerObjectID();

	int offset = 3;

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);


	if(m_UserObjectID != 0)
	{
		Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

		if(pCastCreature !=NULL)
		{
			Vampire* ThisVampire = dynamic_cast<Vampire*>(pCastCreature);
			Assert(ThisVampire != NULL);

			if (ThisVampire->hasRankGem(NewRankGem::SKILL_ADV2_DEADLY_GROUND_DAM ) )
			{
				NewRankGem* pRankGemDEADLYD = ThisVampire->getRankGem( NewRankGem::SKILL_ADV2_DEADLY_GROUND_DAM );
				Assert( pRankGemDEADLYD != NULL );

				output.Damage += pRankGemDEADLYD->getGrade();
			}


			if (ThisVampire->hasRankGem(NewRankGem::SKILL_ADV2_DEADLY_GROUND_RANGE ) )
			{
				NewRankGem* pRankGemDEADLYRANGE = ThisVampire->getRankGem( NewRankGem::SKILL_ADV2_DEADLY_GROUND_RANGE );
				Assert( pRankGemDEADLYRANGE != NULL );

				offset=4;
			}
		}
	}

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



	for ( int i=-offset; i<=offset; ++i )
	{
		for ( int j=-offset; j<=offset; ++j )
		{
			param.addMask( i, j, 100 );
		}
	}
	g_SimpleTileMissileSkill.execute(pMonster, targetX,targetY, param, result);

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
}

DEADLYATT g_DEADLYATT;

