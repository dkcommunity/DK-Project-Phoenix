//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSharpHail.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "Effect.h"
#include "EffectADV3FROZENGROUND1.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "Gpackets/GCAddEffect.h"
#include "EffectIceFieldToCreature.h"
#include "EffectSimplePassive.h"

//#include "HitRoll.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFROZENGROUND1::EffectFROZENGROUND1(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
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
void EffectFROZENGROUND1::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectSharpHail" << "affect BEGIN" << endl;
	
	Assert(m_pZone != NULL);

	// å® »ç¿ëž °¡®¿Ž.
	// ž¿¡ Ÿø» ö ž¹· NULL  µ ö Ž.
	Creature * pCastCreature = m_pZone->getCreature( m_UserObjectID );

	if ( pCastCreature == NULL && !isForce() )
	{
		setNextTime(m_Tick);

		return;
	}

	// öç å®°¡ ºŸîŽ ž» ¹Ÿ¿Ž.
    Tile& tile = m_pZone->getTile(m_X, m_Y);

	// ž Ÿ¿¡ žçŽ ¿ºê§®µé» °»öŽ.
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

			// ¹«û»ó ©. by sigi. 2002.9.5
			// »ê žé¿ª. by sigi. 2002.9.13
			if ( pCastCreature != NULL &&
				( !canAttack( pCastCreature, pCreature )
				|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA) 
				|| !canHit( pCastCreature, pCreature, SKILL_FROZEN_GROUND_1, getLevel() ))
			)
			{
				continue;
			}

			// 2003.1.10 by Sequoia
			// ŸüöŽë ©
			if( !checkZoneLevelToHitTarget( pCreature ) ) continue;
			//if ( pCastCreature != NULL && !HitRoll::isSuccess( pCastCreature, pCreature ) ) continue;

			if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING)
			{
				GCModifyInformation gcAttackerMI;
				GCModifyInformation gcDefenderMI;

				if (pCreature->isSlayer()) 
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

					::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_FROZEN_GROUND_1, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pSlayer->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);
				} 
				else if (pCreature->isVampire())
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

					::setDamage( pVampire, m_Damage, pCastCreature, SKILL_FROZEN_GROUND_1, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pVampire->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);

				}
				else if (pCreature->isMonster())
				{
					Monster* pMonster = dynamic_cast<Monster*>(pCreature);
				
					::setDamage( pMonster, m_Damage, pCastCreature, SKILL_FROZEN_GROUND_1, NULL, &gcAttackerMI, true, true );

					pMonster->addEnemy(pCastCreature);

				}
				else if (pCreature->isOusters() && isForce() )
				{
					Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

					::setDamage( pOusters, m_Damage, pCastCreature, SKILL_FROZEN_GROUND_1, &gcDefenderMI, &gcAttackerMI, true, true );

					Player* pPlayer = pOusters->getPlayer();
					Assert(pPlayer != NULL);
					pPlayer->sendPacket(&gcDefenderMI);
				}
                                if (pCastCreature != NULL && !pCreature->isDead() &&
								!pCreature->isFlag( Effect::EFFECT_CLASS_COMA ) && !pCreature->isOusters())

                                {
                                EffectIceFieldToCreature* pEffect = new EffectIceFieldToCreature(pCreature);
				pEffect->setDeadline(20);
				pCreature->addEffect(pEffect);
				pCreature->setFlag(Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE);

				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(pCreature->getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ICE_FIELD_TO_CREATURE);
				gcAddEffect.setDuration(20);

				m_pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect );

				EffectSimplePassive* pEffect2 = new EffectSimplePassive( pCreature );
				pEffect2->setEffectClass( Effect::EFFECT_CLASS_FROZENGROUND1 );
				pEffect2->setDeadline(20);
				pCreature->setFlag( Effect::EFFECT_CLASS_FROZENGROUND1 );
				pCreature->addEffect( pEffect2 );

				GCAddEffect gcAddEffectSimple;
				gcAddEffectSimple.setObjectID( pCreature->getObjectID() );
				gcAddEffectSimple.setEffectID( pEffect2->getEffectClass() );
				gcAddEffectSimple.setDuration( 20 );
				pCreature->getZone()->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffectSimple );
                                  }
				else continue; // Ÿ¿ìºî³ª NPC »óŽë·... -_-

				// Ÿúžžé °æè¡Ž. .....
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
void EffectFROZENGROUND1::unaffect()
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
string EffectFROZENGROUND1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectFROZENGROUND1("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

