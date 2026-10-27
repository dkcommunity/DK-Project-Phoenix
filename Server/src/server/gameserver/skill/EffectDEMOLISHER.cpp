#include "DB.h"
#include "EffectDEMOLISHER.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "ZoneUtil.h"
#include "Zone.h"

#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK6.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

EffectDEMOLISHER::EffectDEMOLISHER(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_pTarget = pCreature;
	m_UserOID = 0;
	m_Damage = 0;
	m_TargetClass = Creature::CREATURE_CLASS_MAX;
	m_TargetSpriteType = 0;

	__END_CATCH
}

void EffectDEMOLISHER::setTargetType( Creature* pCreature )
{
	if ( pCreature == NULL ) return;
	m_TargetClass = pCreature->getCreatureClass();
	m_TargetCreature = pCreature;

	if ( m_TargetClass == Creature::CREATURE_CLASS_MONSTER )
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);
		m_TargetSpriteType = pMonster->getSpriteType();
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMOLISHER::affect()
	throw(Error)
{
	__BEGIN_TRY

	setDeadline(0);
	Creature* pCaster = dynamic_cast<Creature*>(m_pTarget);

	int cx = m_TargetCreature->getX();
	int cy = m_TargetCreature->getY();

	Zone* pZone = pCaster->getZone();

	int targetNum = 0;

	int tx = cx;
	int ty = cy;
	int sx = 1;
	int sy = 0;
	int maxCount = 1;
	int count = 1;
	int checkCount = 49;

	VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );

	do
	{
		if ( rect.ptInRect( tx, ty ) )
		{
			Tile& tile = pZone->getTile( tx, ty );

			list<Object*>& olist = tile.getObjectList();
			list<Object*>::iterator itr = olist.begin();

			for ( ; itr != olist.end() ; ++itr )
			{
				Object* pObject = *itr;
				if ( pObject == NULL || pObject->getObjectClass() != Object::OBJECT_CLASS_CREATURE ) continue;

				Creature* pCreature = dynamic_cast<Creature*>(pObject);
				if ( pCreature == NULL ) continue;

				bool isTarget = false;
				if ( pCreature->getCreatureClass() == m_TargetClass )
				{
					isTarget = true;
					if ( m_TargetClass == Creature::CREATURE_CLASS_MONSTER )
					{
						Monster* pTargetMonster = dynamic_cast<Monster*>(pCreature);
						if ( pTargetMonster->getSpriteType() != m_TargetSpriteType ) isTarget = false;
					}
				}

				if ( isTarget ) 
				{
					affect(pCreature);
					++targetNum;

					if( targetNum >= 10 )
						return;
				}
			}
		}

		tx += sx;
		ty += sy;

		if ( --count == 0 )
		{
			if ( sx == 0 ) maxCount++;

			int temp = sx;
			sx = -sy;
			sy = temp;
			
			count = maxCount;
		}
	} while ( --checkCount );

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMOLISHER::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	if ( pCreature == NULL ) return;

	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	Ousters* pOusters = dynamic_cast<Ousters*>(m_pTarget);
	if ( pOusters == NULL ) return;

	GCModifyInformation gcMI, gcAttackerMI;

	if ( canAttack( pOusters, pCreature )
	&& !(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE) 
#ifdef VERSION_SKILL_1
	&& HitRoll::isSuccessMagic( pOusters, pCreature )
#endif
	   )
	{
#ifndef VERSION_SKILL_1
		Damage_t damage = computeMagicDamage( pCreature, computeOustersMagicDamage( pOusters, pCreature, getDamage(), SKILL_DEMOLISHER ), SKILL_DEMOLISHER );
#else
		Damage_t damage = getDamage();
#endif
		//damage /= 2;
		if ( pCreature->isPC() )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

			::setDamage( pPC, damage, pOusters, SKILL_DEMOLISHER, &gcMI, &gcAttackerMI, true, false );
			pPC->getPlayer()->sendPacket( &gcMI );
		}
		else if ( pCreature->isMonster() )
		{
			::setDamage( pCreature, damage, pOusters, SKILL_DEMOLISHER, NULL, &gcAttackerMI, true, false );
		}

		computeAlignmentChange(pCreature, damage, pOusters, &gcMI, &gcAttackerMI);
		increaseAlignment(pOusters, pCreature, gcAttackerMI);

		if (pCreature->isDead())
		{
			int exp = computeCreatureExp(pCreature, 100, pOusters);
			shareOustersExp(pOusters, exp, gcAttackerMI);
		}

		pOusters->getPlayer()->sendPacket( &gcAttackerMI );

		GCSkillToObjectOK4 gcOK;
		gcOK.setTargetObjectID( pCreature->getObjectID() );
		gcOK.setSkillType( SKILL_DEMOLISHER );

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcOK, pCreature );

		if ( pCreature->isPC() )
		{
			GCSkillToObjectOK6 gcOK6;
			gcOK6.setXY( pCreature->getX(), pCreature->getY() );
			gcOK6.setSkillType( SKILL_DEMOLISHER );

			pCreature->getPlayer()->sendPacket( &gcOK6 );
		}

	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMOLISHER::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMOLISHER::unaffect()
	throw(Error)
{
	__BEGIN_TRY


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDEMOLISHER::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectDEMOLISHER("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

