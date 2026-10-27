//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeavenGround.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectGLORYGROUND.h"
#include "GamePlayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Zone.h"
#include "SkillUtil.h"
#include "Monster.h"
#include "Tile.h"
#include "Slayer.h"

#include "EffectTrapTriggered.h"
#include "Monster.h"

#include "Gpackets/GCDeleteEffectFromTile.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGLORYGROUND::EffectGLORYGROUND( Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y )
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;
	m_Damage = 10;
	m_Tick = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGLORYGROUND::affect()
	throw(Error)
{
	__BEGIN_TRY
	
	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );

	if( pCastCreature == NULL )
		return;

	Tile& tile = m_pZone->getTile( m_X, m_Y );
	Creature* pTargetCreature = NULL;

	if( tile.hasCreature( Creature::MOVE_MODE_WALKING) )
	{
		pTargetCreature = tile.getCreature( Creature::MOVE_MODE_WALKING );
	}

	if( pTargetCreature == NULL ||
		pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA) ||
		pTargetCreature->isDead()	||
		pTargetCreature->isNPC() ||
		!checkZoneLevelToHitTarget(pTargetCreature))
		return;

	GCModifyInformation gcMI;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//		ÀúÇ×°øœÄ Àû¿ë...
#ifndef VERSION_SKILL_1
	Damage_t damage = computeMagicDamage(pTargetCreature, m_Damage, SKILL_GLORY_GROUND);
#else
	Damage_t damage = m_Damage;
#endif

	if( pTargetCreature->isSlayer() )
	{					
		Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);
                           HP_t CurrentHP	= pSlayer->getHP( ATTR_CURRENT );
			   HP_t MaxHP		= pSlayer->getHP( ATTR_MAX	   );
						
			if ( CurrentHP < MaxHP )
			{
			HP_t RemainHP	= min((int)MaxHP, (int)CurrentHP + damage);
			pSlayer->setHP(RemainHP, ATTR_CURRENT);
                        }
		::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_GLORY_GROUND, &gcMI );
		pTargetCreature->getPlayer()->sendPacket(&gcMI);
		return;
	}
	else if( pTargetCreature->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
		if(pVampire->getHP()-damage <= 0)
			return;
		::setDamage( pVampire, damage, pCastCreature, SKILL_GLORY_GROUND, &gcMI, NULL, true, false );
		pTargetCreature->getPlayer()->sendPacket(&gcMI);
	}
	else if( pTargetCreature->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
		if(pOusters->getHP()-damage <= 0)
			return;
		::setDamage( pOusters, damage, pCastCreature, SKILL_GLORY_GROUND, &gcMI, NULL, true, false );
		pOusters->getPlayer()->sendPacket(&gcMI);
	}
	else if( pTargetCreature->isMonster() )
	{
		Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
		if(pMonster->getHP()-damage <= 0)
			return;
		::setDamage( pMonster, damage, pCastCreature, SKILL_GLORY_GROUND, NULL, NULL, true, false );
	}

	if( pTargetCreature->isPC() )
	{
		GCSkillToObjectOK2 gcSkillToObjectOK2;

		gcSkillToObjectOK2.setObjectID(1);
		gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
		gcSkillToObjectOK2.setDuration(0);

		pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
	}

	setNextTime(m_Tick);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGLORYGROUND::affect(Zone* pZone, ZoneCoord_t Cx, ZoneCoord_t Cy)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGLORYGROUND::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile( m_X, m_Y );
	tile.deleteEffect( m_ObjectID );

	GCDeleteEffectFromTile gcDeleteEffect;
	gcDeleteEffect.setXY( m_X, m_Y );
	gcDeleteEffect.setObjectID( getObjectID() );
	gcDeleteEffect.setEffectID( getSendEffectClass() );

	m_pZone->broadcastPacket( m_X, m_Y, &gcDeleteEffect );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGLORYGROUND::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectGLORYGROUND("
		<< ")";
	return msg.toString();

	__END_CATCH
}

