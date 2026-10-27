//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSharpHail.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "Effect.h"
#include "EffectBLIZZARD2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
//#include "HitRoll.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBLIZZARD2::EffectBLIZZARD2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
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
void EffectBLIZZARD2::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectSharpHail" << "affect BEGIN" << endl;
	
	Assert(m_pZone != NULL);

	// æ£é£ ¤ä¾©ç£ å»ä¿.
	// ç²ä¿ ç»  ä¹éª¨è NULL    ä¹ä¿.
	Creature * pCastCreature = m_pZone->getCreature( m_UserObjectID );

	if ( pCastCreature == NULL && !isForce() )
	{
		setNextTime(m_Tick);

		return;
	}

	// æ³ æ£é£ ¿ç»¢ä¹ç» é¸¥è ç½ä¿.
    Tile& tile = m_pZone->getTile(m_X, m_Y);

	// é¸¥è ä¿ ç²çªç» ·åé£¸é «ç¥¸ä¿.
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

			// ¬å©æ . by sigi. 2002.9.5
			// é­ å¼. by sigi. 2002.9.13
			if ( pCastCreature != NULL &&
				( !canAttack( pCastCreature, pCreature )
				|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA) 
				|| !canHit( pCastCreature, pCreature, SKILL_BLIZZARD2, getLevel() ))
			)
			{
				continue;
			}

			// 2003.1.10 by Sequoia
			// ¤æ 
			if( !checkZoneLevelToHitTarget( pCreature ) ) continue;
			//if ( pCastCreature != NULL && !HitRoll::isSuccess( pCastCreature, pCreature ) ) continue;

			if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING)
			{
				GCModifyInformation gcAttackerMI;
				GCModifyInformation gcDefenderMI;

				if (pCreature->isSlayer()) 
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

					::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_BLIZZARD2, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pSlayer->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);
				} 
				else if (pCreature->isVampire())
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

					::setDamage( pVampire, m_Damage, pCastCreature, SKILL_BLIZZARD2, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pVampire->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);
				}
				else if (pCreature->isMonster())
				{
					Monster* pMonster = dynamic_cast<Monster*>(pCreature);
				
					::setDamage( pMonster, m_Damage, pCastCreature, SKILL_BLIZZARD2, NULL, &gcAttackerMI, true, true );

					pMonster->addEnemy(pCastCreature);
				}
				else if (pCreature->isOusters() && isForce() )
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

					::setDamage( pOusters, m_Damage, pCastCreature, SKILL_BLIZZARD2, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pOusters->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);
				}
				else continue; // å¿«è¶çä»¤å NPC ªè... -_-

				// ç£·è æ°°æ¹éä¿. æ¾.....
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
	
	setNextTime(m_Tick);

	//cout << "EffectSharpHail" << "affect END" << endl;

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLIZZARD2::unaffect()
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
string EffectBLIZZARD2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBLIZZARD2("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

