//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyScarifyBomb.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodyScarifyBomb.h"
#include "GamePlayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Tile.h"
#include "SkillUtil.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBloodyScarifyBomb::EffectBloodyScarifyBomb( Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y )
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;

	__END_CATCH
}

void EffectBloodyScarifyBomb::affect()
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = m_pZone;
	Assert( pZone != NULL );

	Creature* pCreature = m_pZone->getCreature( m_UserObjectID );

	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );


	for( int x = -1; x <= 1; ++x )
	{
		for( int y = -1; y <= 1; ++y )
		{
			ZoneCoord_t X = m_X + x;
			ZoneCoord_t Y = m_Y + y;

			if( rect.ptInRect( X, Y ) )
			{
				Tile& tile = pZone->getTile(X, Y);
				Creature* pTargetCreature = NULL;
				GCStatusCurrentHP gcHP;

				if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				int currentHP, finalHP = 0;
				GCModifyInformation	gcMI;

				if( pTargetCreature != NULL
//					&& !(pTargetCreature->isGOD() || pTargetCreature->isDM())
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
					&& !pTargetCreature->isDead()
					&& !pTargetCreature->isNPC()
				  )
				{

					Damage_t damage = m_Damage;

					if( pTargetCreature->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

						::setDamage( pSlayer, damage, pCreature, SKILL_BLOODY_SCARIFY, &gcMI);

						Player* pPlayer = pSlayer->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isOusters() ) 
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

						::setDamage( pOusters, damage, pCreature, SKILL_BLOODY_SCARIFY, &gcMI);
						
						Player* pPlayer = pOusters->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);
					}
					else if( pTargetCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

						::setDamage( pMonster, damage, pCreature, SKILL_BLOODY_SCARIFY );

						if( pCreature != NULL && pCreature->isPC() )
						{
							pMonster->addEnemy( pCreature );
						}
					}
					else
						continue;

					if( pTargetCreature->isPC() )
					{
						GCSkillToObjectOK2 gcSkillToObjectOK2;
						gcSkillToObjectOK2.setObjectID(1);
						gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
						gcSkillToObjectOK2.setDuration(0);
						pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
					}

					// 공격 대상이 죽었을 경우 경험치를 올려준다
					if( pTargetCreature->isDead() )
					{
						if( pCreature != NULL && pCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
							Assert( pVampire != NULL );


							GCModifyInformation gcAttackerMI;
							int exp = computeCreatureExp(pTargetCreature, KILL_EXP);
							shareVampExp(pVampire, exp, gcAttackerMI);

							pVampire->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}
				}
			}
		}
	}
	// 한번만 발동 되기 때문에 데드라인 0으로 마무리!
	setDeadline(0);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyScarifyBomb::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile( m_X, m_Y );
	tile.deleteEffect( m_ObjectID );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBloodyScarifyBomb::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectBloodyScarifyBomb("
		<< ")";
	return msg.toString();

	__END_CATCH
}

