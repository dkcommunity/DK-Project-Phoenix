//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeterChakramBomb.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectHeterChakramBomb.h"
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHeterChakramBomb::EffectHeterChakramBomb(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;
	m_SpriteType = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakramBomb::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectHeterChakramBomb " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectHeterChakramBomb " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakramBomb::affect(Creature* pCreature)
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

	Damage_t HeterChakramBombDamage = m_Damage;

	ZoneCoord_t	X = pCreature->getX();
	ZoneCoord_t Y = pCreature->getY();

	VSRect	rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

	for( int x = -2; x <= 2; ++x )
	{
		for( int y = -2; y <= 2; ++y )
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

				if( pTargetCreature == NULL )
					continue;

				GCModifyInformation	gcMI;
				
				if( pTargetCreature->getCreatureClass() == m_TargetClass )
				{
					if( pTargetCreature->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
						::setDamage( pSlayer, HeterChakramBombDamage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI, NULL, true, false );
						pTargetCreature->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isVampire() )
					{
						Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
						::setDamage( pVampire, HeterChakramBombDamage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI, NULL, true, false );
						pTargetCreature->getPlayer()->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
						if( pMonster->getSpriteType() != m_SpriteType )
							continue;
						::setDamage( pMonster, HeterChakramBombDamage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI, NULL, true, false);
					}
				}
				else
					continue;

				
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
	//cout << "EffectHeterChakramBomb " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakramBomb::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_HETER_CHAKRAM_BOMB);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_HETER_CHAKRAM_BOMB);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakramBomb::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHeterChakramBomb::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectHeterChakramBomb("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

