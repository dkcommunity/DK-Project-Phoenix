#include "EffectRingOfFlare2.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "SkillUtil.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCStatusCurrentHP.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRingOfFlare2::EffectRingOfFlare2(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	setTarget(pCreature);

	__END_CATCH
}

void EffectRingOfFlare2::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCastCreature = dynamic_cast<Creature*>(m_pTarget);

	if ( pCastCreature == NULL || !pCastCreature->isOusters() ) return;

	Ousters* pOusters = dynamic_cast<Ousters*>(pCastCreature);
	Assert( pOusters != NULL );

	Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
	if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
	{
		setDeadline(0);
		return;
	}

	Player* pPlayer = dynamic_cast<Player*>(pCastCreature->getPlayer());
	Assert( pPlayer != NULL );

	Zone* pZone = pCastCreature->getZone();
	Assert( pZone != NULL );

	VSRect rect( 0, 0, pZone->getWidth()-1, pZone->getHeight()-1 );

	ZoneCoord_t Cx = pCastCreature->getX();
	ZoneCoord_t Cy = pCastCreature->getY();

	for ( int x=-1; x<=1; x++ )
	{
		for ( int y=-1; y<=1; y++ )
		{
			if ( x == 0 && y == 0 ) continue;

			int X = Cx + x;
			int Y = Cy + y;

			if ( !rect.ptInRect( X, Y ) ) continue;

			Tile& tile = pZone->getTile( X, Y );

			if( tile.hasCreature(Creature::MOVE_MODE_WALKING) )
			{
				Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				Assert( pCreature != NULL );
				
				GCModifyInformation gcAttackerMI;

				if ( pCreature == m_pTarget
				  || !canAttack( pCastCreature, pCreature )
				  || pCreature->isFlag( Effect::EFFECT_CLASS_COMA )
				  || pCreature->isOusters()
				  || !checkZoneLevelToHitTarget(pCreature)
#ifdef VERSION_SKILL_1
				  || !HitRoll::isSuccessMagic( pCastCreature, pCreature )
#endif
				)
				{
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//		历亲傍侥 利侩...
#ifndef VERSION_SKILL_1
				Damage_t damage = computeMagicDamage(pCreature, m_Damage, SKILL_RING_OF_FLARE2);
#else
				Damage_t damage = m_Damage;
#endif 

				if ( pCreature->isPC() )
				{
					GCModifyInformation gcMI;
					::setDamage( pCreature, damage, pCastCreature, SKILL_RING_OF_FLARE2, &gcMI, &gcAttackerMI );

					pCreature->getPlayer()->sendPacket( &gcMI );

					// 嘎绰 悼累阑 焊咯霖促.
					GCSkillToObjectOK2 gcSkillToObjectOK2;
					gcSkillToObjectOK2.setObjectID( 1 );    // 狼固 绝促.
					gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
					gcSkillToObjectOK2.setDuration(0);
					pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

				}
				else if ( pCreature->isMonster() )
				{
					Monster* pMonster = dynamic_cast<Monster*>(pCreature);

					::setDamage( pMonster, damage, pCastCreature, SKILL_RING_OF_FLARE2, NULL, &gcAttackerMI );

					pMonster->addEnemy( pCastCreature );
				}

				GCSkillToObjectOK4 gcSkillToObjectOK4;
				gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
				gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
				gcSkillToObjectOK4.setDuration(0);

				pZone->broadcastPacket( X, Y, &gcSkillToObjectOK4, pCreature );

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

	setNextTime( 10 );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRingOfFlare2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pCreature != NULL);
	Assert(pCreature->isOusters());

	pCreature->removeFlag(Effect::EFFECT_CLASS_RING_OF_FLARE2);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
	Assert( pTargetOusters != NULL );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(getSendEffectClass());
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRingOfFlare2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectRingOfFlare2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectRingOfFlare2("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

