//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectDummyDrakeBomb.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectDummyDrakeBomb.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectDummyDrakeBomb::EffectDummyDrakeBomb(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrakeBomb::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectDummyDrakeBomb " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectDummyDrakeBomb " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrakeBomb::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 스킬 사용자를 가져온다.
	// !! 이미 존을 나갔을 수 있으므로 NULL이 될 수 있다.
	// by bezz. 2003.1.4
	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

	if(pCastCreature == NULL)
	{
		setDeadline(0);
		return;
	}

	Damage_t DummyDrakeBombDamage = m_Damage;

	ZoneCoord_t	X = pCreature->getX();
	ZoneCoord_t Y = pCreature->getY();

	VSRect	rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

	for( int x = -1; x <= 1; ++x )
	{
		for( int y = -1; y <= 1; ++y )
		{
			ZoneCoord_t targetX = X+x;
			ZoneCoord_t targetY = Y+y;

			if( rect.ptInRect( targetX, targetY) )
			{
				Tile& tile = pZone->getTile( targetX, targetY );
				Creature* pTargetCreature = NULL;
				
				if( tile.hasCreature(Creature::MOVE_MODE_WALKING) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				if( pTargetCreature == NULL 
#ifdef VERSION_SKILL_1
				|| HitRoll::isSuccessMagic( pCastCreature, pTargetCreature )
#endif
				)
					continue;

#ifndef	VERSION_SKILL_1
				DummyDrakeBombDamage = computeMagicDamage( pTargetCreature, m_Damage, SKILL_DUMMY_DRAKE );
#else
				DummyDrakeBombDamage = m_Damage;
#endif
				GCModifyInformation	gcMI;
				
				if( pTargetCreature->isSlayer() )
				{
					Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
					::setDamage( pSlayer, DummyDrakeBombDamage, pCastCreature, SKILL_DUMMY_DRAKE, &gcMI, NULL, true, false );
					pTargetCreature->getPlayer()->sendPacket(&gcMI);
				}
				else if( pTargetCreature->isVampire() )
				{
					Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
					::setDamage( pVampire, DummyDrakeBombDamage, pCastCreature, SKILL_DUMMY_DRAKE, &gcMI, NULL, true, false );
					pTargetCreature->getPlayer()->sendPacket(&gcMI);
				}
				else if( pTargetCreature->isMonster() )
				{
					Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
					::setDamage( pMonster, DummyDrakeBombDamage, pCastCreature, SKILL_DUMMY_DRAKE, &gcMI, NULL, true, false );
				}

				
				if( pTargetCreature->isPC() )
				{
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					gcSkillToObjectOK2.setObjectID( 1 );
					gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
					gcSkillToObjectOK2.setDuration(0);

					pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
				}

				if( pCastCreature != NULL )
				{
					if( pTargetCreature->isDead() && pCastCreature->isOusters() )
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCastCreature);
						Assert( pOusters != NULL );

						int exp = computeCreatureExp(pTargetCreature, 70, pOusters);
						shareOustersExp(pOusters, exp, gcMI);
					}
				}
			}
		}
	}
	setDeadline(0);
	//cout << "EffectDummyDrakeBomb " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrakeBomb::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_DUMMY_DRAKE_BOMB);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_DUMMY_DRAKE_BOMB);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDummyDrakeBomb::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDummyDrakeBomb::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectDummyDrakeBomb("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

