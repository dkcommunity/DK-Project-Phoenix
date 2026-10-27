//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombFire.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectSatelliteBombFire.h"
#include "EffectBlind.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "SkillInfo.h"
#include "SkillUtil.h"
#include "HitRoll.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

/*
int GSGDamageModify[5][5] =
{
    { 50, 50,  50, 50, 50 },
    { 50, 75,  75, 75, 50 },
    { 50, 75, 100, 75, 50 },
    { 50, 75,  75, 75, 50 },
    { 50, 50,  50, 50, 50 }
};
*/
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSatelliteBombFire::EffectSatelliteBombFire(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);
	m_pZone = pZone;
	m_X = x;
	m_Y = y;
	m_SkillLevel = 0;
	m_STR = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombFire::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombFire::unaffect(Creature* pCastCreature)
	throw(Error)
{
	__BEGIN_TRY
	
	Assert(pCastCreature != NULL);

	//이펙트 플레그가 없다면 죽었다거나 하는 문제로 transport 하지 않겠다는걸 의미한다.
	if ( !pCastCreature->isFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE ) )
	return;
	
	// Effect를 없애고 알린다.
	pCastCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );
	
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCastCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );
	
	m_pZone->broadcastPacket( pCastCreature->getX(), pCastCreature->getY(), &gcRemoveEffect );
	
	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );	// -1 추가 by sigi. 2003.1.10

	bool bHit = false;
	Damage_t maxDamage=0;

	GCSkillToObjectOK2 gcSkillToObjectOK2;
	GCSkillToObjectOK4 gcSkillToObjectOK4;

	Level_t maxEnemyLevel = 0;
	uint EnemyNum = 0;

	for ( int x=-2; x<=2; x++ )
	{
		for ( int y=-2; y<=2; y++ )
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if ( !rect.ptInRect( X, Y ) ) continue;

			// 타일안에 존재하는 오브젝트를 가져온다.
			Tile& tile = m_pZone->getTile( X, Y );
			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator itr = oList.begin();

//			int DamageModifier = GSGDamageModify[x+2][y+2];
			Damage_t Damage = 0;

			if ( x == 0 && y == 0 )
			{
				Damage = min(240, 50 + (m_STR/2 + STR/2 + m_SkillLevel*3));
			}
			else if( x == -1 || x == 1 || y == -1 || y == 1 )
			{
				Damage = min(190, 30 + (m_STR/2 + STR/3 + m_SkillLevel*3));
			}
			else
			{
				Damage = min(170, 15 + (m_STR/2 + STR/4 + m_SkillLevel*3));
			}

			for ( ; itr != oList.end(); itr++ )
			{
				Object* pObject = *itr;
				Assert( pObject != NULL );

				if ( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
				{
					Creature* pCreature = dynamic_cast<Creature*>(pObject);
					Assert( pCreature != NULL );

					// 자신은 맞지 않는다. 무적도 안 맞는다. 슬레이어는 맞지 않는다.
					if ( pCreature == m_pTarget
					  || !canAttack( pCastCreature, pCreature )
					  || pCreature->isFlag( Effect::EFFECT_CLASS_COMA ) )
					{
						continue;
					}

					bool bPK                = verifyPK( pCastCreature, pCreature );
					bool bZoneLevelCheck    = checkZoneLevelToHitTarget( pCreature );
					
					int bonus = 100;
					if ( pCastCreature->isSlayer() )
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
						Assert( pSlayer != NULL );

						SlayerSkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB );
						if ( pSkillSlot != NULL )
							bonus = pSkillSlot->getExpLevel();
					}

					bool bHitRoll           = HitRoll::isSuccess( pCastCreature, pCreature, bonus );

					if ( bPK && bZoneLevelCheck && bHitRoll )
					{
						// 원래 데미지와 스킬 데미지 보너스를 더한 최종 데미지를 구한다.
						Damage_t FinalDamage = 0;
						FinalDamage = computeDamage( pCastCreature, pCreature );
						FinalDamage += Damage;

						if ( pCreature->isPC() && pCreature->getCreatureClass() != pCastCreature->getCreatureClass() )
						{
//							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

							GCModifyInformation gcMI;
							::setDamage( pCreature, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB, &gcMI, NULL, true, false ); // ::추가 by Sequoia

							pCreature->getPlayer()->sendPacket( &gcMI );

							// 맞는 동작을 보여준다.
							gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
							gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
							gcSkillToObjectOK2.setDuration(0);
							pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

							if( rand()%100 <= 25 )
							{
								EffectBlind* pEffect = new EffectBlind(pCreature);
								pEffect->setDeadline(50);
								pCreature->setFlag(Effect::EFFECT_CLASS_BLIND);
								pCreature->addEffect(pEffect);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pCreature->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_BLIND);
								gcAddEffect.setDuration(50);
								pCreature->getPlayer()->sendPacket( &gcAddEffect );
								//m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);
							}


							bHit = true;
						}
						else if ( pCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pCreature);

							::setDamage( pMonster, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB, NULL, NULL, true, false );	// ::추가 by Sequoia

							pMonster->addEnemy( pCastCreature );
							bHit = true;
						}
						else continue;

						gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
						gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
						gcSkillToObjectOK4.setDuration( 0 );
						m_pZone->broadcastPacket( X, Y, &gcSkillToObjectOK4, pCreature );

						if ( maxEnemyLevel < pCreature->getLevel() ) maxEnemyLevel = pCreature->getLevel();
						EnemyNum++;

						if ( FinalDamage > maxDamage ) maxDamage = FinalDamage;

					}
				}
			}
		}
	}

	if ( bHit && pCastCreature->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);
		Assert( pSlayer != NULL );

		bool bIncreaseExp = pSlayer->isRealWearingEx( Slayer::WEAR_RIGHTHAND );
		if ( bIncreaseExp )
		{
			SlayerSkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB );
			SkillInfo* pSkillInfo   = g_pSkillInfoManager->getSkillInfo( SKILL_SATELLITE_BOMB );
			SkillDomainType_t DomainType = pSkillInfo->getDomainType();
			SkillLevel_t SkillLevel = pSkillSlot->getExpLevel();

			GCModifyInformation gcMI;
			shareAttrExp( pSlayer, maxDamage, 1, 8, 1, gcMI );
			increaseDomainExp( pSlayer, DomainType, pSkillInfo->getPoint(), gcMI, maxEnemyLevel, EnemyNum );
			increaseSkillExp( pSlayer, DomainType, pSkillSlot, pSkillInfo, gcMI );

			pSlayer->getPlayer()->sendPacket( &gcMI );
		}
	}

	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombFire::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);	// by Sequoia

	Assert( pCreature != NULL );

	if ( m_pZone != NULL && m_pZone == pCreature->getZone() )
	{
		unaffect(pCreature);
	}
	else
	{
		// 조준 도중 죽었거나 이동했다는 의미이므로 이펙트를 없애고 브로드캐스팅한다.
		// Effect를 없애고 알린다.
		Zone* pZone = pCreature->getZone();
		Assert( pZone != NULL );

		pCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID( pCreature->getObjectID() );
		gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect );
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSatelliteBombFire::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSatelliteBombFire("
		<< "Zone:" << g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() )->getFullName()
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ")";
	return msg.toString();

	__END_CATCH
}

