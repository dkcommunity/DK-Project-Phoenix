//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSatelliteBombFire2.cpp
// Written by  : bezz
//////////////////////////////////////////////////////////////////////////////

#include "EffectSatelliteBombFire2.h"
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
EffectSatelliteBombFire2::EffectSatelliteBombFire2(Creature* pCreature, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y)
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
void EffectSatelliteBombFire2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSatelliteBombFire2::unaffect(Creature* pCastCreature)
	throw(Error)
{
	__BEGIN_TRY
	
	Assert(pCastCreature != NULL);

	//å® ·¹±°¡ ŸøŽžé ŸúŽ°³ª Ž ¹®· transport ö Ÿ°ŽŽ° ¹Ž.
	if ( !pCastCreature->isFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE_2 ) )
	return;
	
	// Effectž ŸøŸ°í Ÿž°Ž.
	pCastCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE_2 );
	
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCastCreature->getObjectID() );
	gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE_2 );
	
	m_pZone->broadcastPacket( pCastCreature->getX(), pCastCreature->getY(), &gcRemoveEffect );
	
	VSRect rect( 0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1 );	// -1 °¡ by sigi. 2003.1.10

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

			// žŸ¿¡ žçŽ ¿ºê§®ž °¡®¿Ž.
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

					// º žö ŸŽŽ. ¹«ûµµ Ÿ žŽŽ. ·¹ŸîŽ žö ŸŽŽ.
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

						SlayerSkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB_2 );
						if ( pSkillSlot != NULL )
							bonus = pSkillSlot->getExpLevel();
					}

					bool bHitRoll           = HitRoll::isSuccess( pCastCreature, pCreature, bonus );

					if ( bPK && bZoneLevelCheck && bHitRoll )
					{
						// ¿ø·¡ µ¥¹ö¿ º³ µ¥¹ö ºž³ºž Žõ Ÿ µ¥¹öž ±žŽ.
						Damage_t FinalDamage = 0;
						FinalDamage = computeDamage( pCastCreature, pCreature );
						FinalDamage += Damage;

						if ( pCreature->isPC() && pCreature->getCreatureClass() != pCastCreature->getCreatureClass() )
						{
//							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

							GCModifyInformation gcMI;
							::setDamage( pCreature, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB_2, &gcMI, NULL, true, false ); // ::°¡ by Sequoia

							pCreature->getPlayer()->sendPacket( &gcMI );

							// žŽ µ¿» ºž¿©Ž.
							gcSkillToObjectOK2.setObjectID( 1 );    // ¹ ŸøŽ.
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

							::setDamage( pMonster, FinalDamage, pCastCreature, SKILL_SATELLITE_BOMB_2, NULL, NULL, true, false );	// ::°¡ by Sequoia

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
			SlayerSkillSlot* pSkillSlot   = pSlayer->hasSkill( SKILL_SATELLITE_BOMB_2 );
			SkillInfo* pSkillInfo   = g_pSkillInfoManager->getSkillInfo( SKILL_SATELLITE_BOMB_2 );
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
void EffectSatelliteBombFire2::unaffect()
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
		// ¶ µµ Ÿú°³ª µ¿ŽŽ ¹¹· å®ž ŸøŸ°í ºê·µå³ºŽ.
		// Effectž ŸøŸ°í Ÿž°Ž.
		Zone* pZone = pCreature->getZone();
		Assert( pZone != NULL );

		pCreature->removeFlag( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE_2 );

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID( pCreature->getObjectID() );
		gcRemoveEffect.addEffectList( Effect::EFFECT_CLASS_SATELLITE_BOMB_FIRE_2 );

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcRemoveEffect );
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSatelliteBombFire2::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectSatelliteBombFire2("
		<< "Zone:" << g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() )->getFullName()
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ")";
	return msg.toString();

	__END_CATCH
}

