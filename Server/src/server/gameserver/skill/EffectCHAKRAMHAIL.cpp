//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSharpHail.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "Effect.h"
#include "EffectCHAKRAMHAIL.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "HitRoll.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectCHAKRAMHAIL::EffectCHAKRAMHAIL(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_UserObjectID = 0;
	m_bForce = false;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectCHAKRAMHAIL::affect()
	throw(Error)
{
	__BEGIN_TRY

	
	Assert(m_pZone != NULL);

	Creature * pCastCreature = m_pZone->getCreature( m_UserObjectID );

	if ( pCastCreature == NULL && !isForce() )
	{
		setNextTime(m_Tick);

		return;
	}

	Ousters* ThisOusters = dynamic_cast<Ousters*>(pCastCreature);
	Assert(ThisOusters != NULL);
	int myhitbonus =0;
	int mycribonus =0;

	if (ThisOusters->hasRankGem(NewRankGem::SKILL_CHAKRAM_HAIL_TOHIT ) )
	{
		NewRankGem* pRankGemCHAKRAMTOHIT = ThisOusters->getRankGem( NewRankGem::SKILL_CHAKRAM_HAIL_TOHIT );
		Assert( pRankGemCHAKRAMTOHIT != NULL );

		myhitbonus += pRankGemCHAKRAMTOHIT->getGrade();
	}

	if (ThisOusters->hasRankGem(NewRankGem::SKILL_CHAKRAM_HAIL_CRI ) )
	{
		NewRankGem* pRankGemCHAKRAMCRI = ThisOusters->getRankGem( NewRankGem::SKILL_CHAKRAM_HAIL_CRI );
		Assert( pRankGemCHAKRAMCRI != NULL );

		mycribonus += pRankGemCHAKRAMCRI->getGrade();
	}

    Tile& tile = m_pZone->getTile(m_X, m_Y);

    const list<Object*>& oList = tile.getObjectList();
	list<Object*>::const_iterator itr = oList.begin();
    for (; itr != oList.end(); itr++) 
	{
		Assert(*itr != NULL);

		Object* pObject = *itr;
		Assert(pObject != NULL);

    	if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
		{
			Creature* pCreature = dynamic_cast<Creature*>(pObject);
			Assert(pCreature != NULL);

			if ( pCastCreature != NULL &&
				( !canAttack( pCastCreature, pCreature )
				|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA) 
				|| !canHit( pCastCreature, pCreature, SKILL_CHAKRAM_HAIL, getLevel() ))
			)
			{
				continue;
			}

			if( !checkZoneLevelToHitTarget( pCreature ) ) continue;

			bool bHitRoll = HitRoll::isSuccess( pCastCreature, pCreature,myhitbonus );
			bool bCriticalHit = false;
			if ( pCastCreature != NULL )
			{
				if(bHitRoll)
				{
					m_Damage += computeDamage(pCastCreature, pCreature, mycribonus, bCriticalHit);

					if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING)
					{
						GCModifyInformation gcAttackerMI;
						GCModifyInformation gcDefenderMI;

						if (pCreature->isSlayer()) 
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

							::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_CHAKRAM_HAIL, &gcDefenderMI, &gcAttackerMI, true, true );

							Player* pPlayer = pSlayer->getPlayer();
							Assert(pPlayer != NULL);
							pPlayer->sendPacket(&gcDefenderMI);
						} 
						else if (pCreature->isVampire())
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

							::setDamage( pVampire, m_Damage, pCastCreature, SKILL_CHAKRAM_HAIL, &gcDefenderMI, &gcAttackerMI, true, true );

							Player* pPlayer = pVampire->getPlayer();
							Assert(pPlayer != NULL);
							pPlayer->sendPacket(&gcDefenderMI);
						}
						else if (pCreature->isMonster())
						{
							Monster* pMonster = dynamic_cast<Monster*>(pCreature);
				
							::setDamage( pMonster, m_Damage, pCastCreature, SKILL_CHAKRAM_HAIL, NULL, &gcAttackerMI, true, true );

							pMonster->addEnemy(pCastCreature);
						}
						else if (pCreature->isOusters() && isForce() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

							::setDamage( pOusters, m_Damage, pCastCreature, SKILL_CHAKRAM_HAIL, &gcDefenderMI, &gcAttackerMI, true, true );

							Player* pPlayer = pOusters->getPlayer();
							Assert(pPlayer != NULL);
							pPlayer->sendPacket(&gcDefenderMI);
						}
						else continue; // 아우스터즈나 NPC 상대로... -_-

						// 죽었으면 경험치준다. 음.....
						if ( pCastCreature != NULL )
						{
							if (pCreature->isDead() && pCastCreature->isOusters())
							{
								Ousters* pCastOusters = dynamic_cast<Ousters*>( pCastCreature );
								Assert( pCastOusters != NULL );

								int exp = computeCreatureExp(pCreature, 100, pCastOusters);
								shareOustersExp(pCastOusters, exp, gcAttackerMI);
							}
						}

						if ( gcAttackerMI.getShortCount() != 0 || gcAttackerMI.getLongCount() != 0 ) pCastCreature->getPlayer()->sendPacket(&gcAttackerMI);
					}
				}
			}
		}
	}
	
	setNextTime(m_Tick);

	//cout << "EffectSharpHail" << "affect END" << endl;

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectCHAKRAMHAIL::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectSharpHail" << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectSharpHail" << "unaffect END" << endl;

	__END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectCHAKRAMHAIL::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectSharpHail("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

