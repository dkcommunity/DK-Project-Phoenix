//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFreezeRing2.cpp
// Written by  : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectFreezeRing2.h"
#include "EffectFreeze.h"
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
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Zone.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFreezeRing2::EffectFreezeRing2( Creature* pCreature, ZoneCoord_t X, ZoneCoord_t Y )
	throw(Error)
{
	__BEGIN_TRY

	m_X = X;
	m_Y = Y;
	m_pCreature = pCreature;
	m_pZone = m_pCreature->getZone();

	__END_CATCH
}

void EffectFreezeRing2::affect()
	throw(Error)
{
	__BEGIN_TRY

	Zone* pZone = m_pZone;
	Assert( pZone != NULL );

	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );

#ifdef VERSION_SKILL_1
	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );
#endif

	for( int x = -3; x <= 3; ++x )
	{
		for( int y = -3; y <= 3; ++y )
		{
			ZoneCoord_t X = m_X + x;
			ZoneCoord_t Y = m_Y + y;

			if( rect.ptInRect( X, Y ) )
			{
				Tile& tile = pZone->getTile(X, Y);
				Creature* pTargetCreature = NULL;

				if( tile.hasCreature( Creature::MOVE_MODE_WALKING ) )
				{
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				}

				if( pTargetCreature != NULL
					&& !pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA)
                                        && !pTargetCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_FREEZE)
					&& !pTargetCreature->isDead()
					&& !pTargetCreature->isNPC()
					&& pTargetCreature->isMonster()
#ifdef VERSION_SKILL_1
					&& HitRoll::isSuccessMagic(pCastCreature, pTargetCreature)
#endif
				  )
				{
					Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

					int success = 0;
					success = max( 0, ( 110 - ( pMonster->getLevel() - m_pCreature->getLevel() ) * 5 ) );
					success = min( 100, success );
					int ratio = rand()%100+1;

					if( m_pCreature != NULL && m_pCreature->isPC() )
					{
						pMonster->addEnemy( m_pCreature );
					}

					if( success >= ratio &&
						!pTargetCreature->isFlag(Effect::EFFECT_CLASS_FREEZE) &&
                                                !pTargetCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_FREEZE) &&
						!pTargetCreature->isDead() &&
						!pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA) )
					{
						::setDamage( pMonster, getDamage(), m_pCreature, SKILL_FREEZE_RING2, NULL, NULL, true, false );

						EffectFreeze* pEffectFreeze = new EffectFreeze(pMonster);

						// 20071025
						pEffectFreeze->setDeadline(getDuration());

						pMonster->addEffect(pEffectFreeze);
						pMonster->setFlag(Effect::EFFECT_CLASS_FREEZE);

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID(pMonster->getObjectID());
						gcAddEffect.setEffectID(Effect::EFFECT_CLASS_FREEZE);
						gcAddEffect.setDuration(getDuration());
						pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcAddEffect);
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
void EffectFreezeRing2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pCreature != NULL);
	Assert(m_pCreature->isSlayer());

	//플래그를 끈다.
	m_pCreature->removeFlag(Effect::EFFECT_CLASS_FREEZE_RING2);

	Zone* pZone = m_pCreature->getZone();
	Assert(pZone != NULL);

	// 이펙트를 삭제하라고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(m_pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_FREEZE_RING2);
	pZone->broadcastPacket(m_pCreature->getX(), m_pCreature->getY(), &gcRemoveEffect);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectFreezeRing2::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectFreezeRing2("
		<< ")";
	return msg.toString();

	__END_CATCH
}

