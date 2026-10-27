//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombAim.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectSatelliteBombAim.h"
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
EffectSatelliteBombAim::EffectSatelliteBombAim(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)
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
void EffectSatelliteBombAim::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombAim::unaffect(Creature* pCastCreature)
	throw(Error)
{
	__BEGIN_TRY
	
	Assert(pCastCreature != NULL);

	// 이펙트 플레그가 없다면 죽었다거나 하는 문제로 transport 하지 않겠다는걸 의미한다.
	if ( !pCastCreature->isFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_AIM ) )
		return;

	// Effect를 없애고 알린다.
	pCastCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_AIM );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCastCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_AIM );

	m_pZone->broadcastPacket( pCastCreature->getX(), pCastCreature->getY(), &gcRemoveEffect );

	EffectSatelliteBombFire* pEffect = new EffectSatelliteBombFire(pCastCreature, m_pZone, m_X, m_Y);
	pEffect->setSTR(getSTR());
	pEffect->setSkillLevel(getSkillLevel());
	pEffect->setNextTime(10);
	pEffect->setDeadline(15);

	pCastCreature->addEffect( pEffect );
	pCastCreature->setFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );

	// 포격 이펙트를 보여주도록 한다.
	GCAddEffectToTile gcAddEffectToTile;
	gcAddEffectToTile.setObjectID( pCastCreature->getObjectID() );
	gcAddEffectToTile.setEffectID( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE );
	gcAddEffectToTile.setXY( m_X, m_Y );
	gcAddEffectToTile.setDuration( 10 ); // 별 의미없다. 걍 1초

	m_pZone->broadcastPacket( m_X, m_Y, &gcAddEffectToTile );

	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );

	GCSkillToObjectOK2 gcSkillToObjectOK2;
	GCSkillToObjectOK4 gcSkillToObjectOK4;

	if( rect.ptInRect( m_X, m_Y ) )
	{
		Tile& tile = m_pZone->getTile( m_X, m_Y );
		const list<Object*>& oList = tile.getObjectList();
		list<Object*>::const_iterator itr = oList.begin();

		Damage_t damage = 0;
		damage = min( 200, int(30 + (getSTR()/2 + getSTR()/3 + getSkillLevel()*3)) );

		for( ; itr != oList.end(); ++itr )
		{
			Object* pObject = *itr;
			Assert( pObject != NULL );

			if( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
			{
				Creature* pCreature = dynamic_cast<Creature*>(pObject);
				Assert( pCreature != NULL );

				if( pCreature == m_pTarget
				|| !canAttack(pCastCreature, pCreature)
				|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA) )
				{
					continue;
				}

				bool bPK				= verifyPK( pCastCreature, pCreature );
				bool bZoneLevelCheck	= checkZoneLevelToHitTarget(pCreature); 
				bool bHitRoll			= HitRoll::isSuccess( pCastCreature, pCreature, 100 );

				if( bPK && bZoneLevelCheck && bHitRoll )
				{
					if( pCreature->isPC() && pCreature->getCreatureClass() != pCastCreature->getCreatureClass() )
					{
						GCModifyInformation gcMI;
						::setDamage( pCreature, damage, pCastCreature, SKILL_SATELLITE_BOMB, &gcMI);

						pCreature->getPlayer()->sendPacket( &gcMI );

						gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
						gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
						gcSkillToObjectOK2.setDuration(0);
						pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
					}
					else if( pCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, damage, pCastCreature, SKILL_SATELLITE_BOMB );

						pMonster->addEnemy( pCastCreature );
					}
					else continue;

					gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
					gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
					gcSkillToObjectOK4.setDuration( 0 );
					m_pZone->broadcastPacket( m_X, m_Y, &gcSkillToObjectOK4, pCreature );
				}
			}
		}
	}
/*
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
				Damage = min(210, 20 + (m_STR/2 + STR/2 + m_SkillLevel*3));
			}
			else if( x == -1 || x == 1 || y == -1 || y == 1 )
			{
				Damage = min(170, 20 + (m_STR/2 + STR/3 + m_SkillLevel*3));
			}
			else
			{
				Damage = min(155, 20 + (m_STR/2 + STR/4 + m_SkillLevel*3));
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

						SkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB );
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
							::setDamage( pCreature, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB, &gcMI ); // ::추가 by Sequoia

							pCreature->getPlayer()->sendPacket( &gcMI );

							// 맞는 동작을 보여준다.
							gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
							gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
							gcSkillToObjectOK2.setDuration(0);
							pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

							if( rand()%10 <= 1 )
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

							::setDamage( pMonster, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB );	// ::추가 by Sequoia

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
			SkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB );
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
*/
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombAim::unaffect()
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

		pCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_AIM );

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID( pCreature->getObjectID() );
		gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_AIM );

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect );
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSatelliteBombAim::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSatelliteBombAim("
		<< "Zone:" << g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() )->getFullName()
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ")";
	return msg.toString();

	__END_CATCH
}

