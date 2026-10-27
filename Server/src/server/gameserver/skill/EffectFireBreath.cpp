//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFireBreath.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectFireBreath.h"
#include "SimpleSkill.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "EffectBleeding.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFireBreath::EffectFireBreath(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_UserObjectID = 0;

	m_bFirst = true;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFireBreath::affect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	// 이펙트를 건 크리쳐를 가져온다.
	// !! 존을 떠났을 수도 있으므로 NULL 이 될 수 있다.
	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	list<TILE_MASK> MaskList;
	MaskList.push_back( TILE_MASK(  0, -2, 0 ) );
	MaskList.push_back( TILE_MASK( -1, -1, 0 ) );
	MaskList.push_back( TILE_MASK(  0, -1, 0 ) );
	MaskList.push_back( TILE_MASK(  1, -1, 0 ) );
	MaskList.push_back( TILE_MASK( -2,  0, 0 ) );
	MaskList.push_back( TILE_MASK( -1,  0, 0 ) );
	MaskList.push_back( TILE_MASK(  0,  0, 0 ) );
	MaskList.push_back( TILE_MASK(  1,  0, 0 ) );
	MaskList.push_back( TILE_MASK(  2,  0, 0 ) );
	MaskList.push_back( TILE_MASK( -1,  1, 0 ) );
	MaskList.push_back( TILE_MASK(  0,  1, 0 ) );
	MaskList.push_back( TILE_MASK(  1,  1, 0 ) );
	MaskList.push_back( TILE_MASK(  0,  2, 0 ) );

	list<TILE_MASK>::const_iterator itr = MaskList.begin();
	list<TILE_MASK>::const_iterator endItr = MaskList.end();

	for ( ; itr != endItr; ++itr )
	{
		TILE_MASK mask = (*itr);
		ZoneCoord_t X = m_X + mask.x;
		ZoneCoord_t Y = m_Y + mask.y;

		if ( rect.ptInRect( X, Y ) )
		{
			Tile& tile = m_pZone->getTile( X, Y );

			// 타일 안에 존재하는 오브젝트들을 검색한다.
			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator oitr = oList.begin();
			list<Object*>::const_iterator oendItr = oList.end();
			for ( ; oitr != oendItr; ++oitr )
			{
				Object* pObject = *oitr;
				Assert( pObject != NULL );

				if ( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
				{
					Creature* pCreature = dynamic_cast<Creature*>(pObject);
					Assert( pCreature != NULL );

					bool bCheck = true;

					if ( pCreature->getObjectID() == m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag( Effect::EFFECT_CLASS_COMA )
						|| !checkZoneLevelToHitTarget( pCreature )
						|| pCreature->isNPC()
						|| pCreature->isDead()
					)
					{
						bCheck = false;
					}

					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) )
						{
							bCheck = false;
						}
					}

					if ( bCheck )
					{
						GCSkillToObjectOK2 gcSkillToObjectOK2;

						if ( pCreature->isSlayer() )
						{
							Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
							Assert( pSlayer != NULL );

							::setDamage( pSlayer, ( m_bFirst ? 200 : pSlayer->getHP(ATTR_MAX) / 20 ), pCastCreature, SKILL_FIRE_WAVE, &gcSkillToObjectOK2, NULL );
						}
						else if ( pCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
							Assert( pVampire != NULL );

							::setDamage( pVampire, ( m_bFirst ? 200 : pVampire->getHP(ATTR_MAX) / 20 ), pCastCreature, SKILL_FIRE_WAVE, &gcSkillToObjectOK2, NULL );
						}
						else if ( pCreature->isOusters() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
							Assert( pOusters != NULL );

							::setDamage( pOusters, ( m_bFirst ? 200 : pOusters->getHP(ATTR_MAX) / 20 ), pCastCreature, SKILL_FIRE_WAVE, &gcSkillToObjectOK2, NULL );
						}
						else if ( pCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pCreature);
							Assert( pMonster != NULL );

							::setDamage( pMonster, ( m_bFirst ? 200 : pMonster->getHP(ATTR_MAX) / 20 ), pCastCreature, SKILL_FIRE_WAVE, NULL, NULL );

							if ( pCastCreature != NULL )
								pMonster->addEnemy( pCastCreature );
						}

						if (pCreature->isPC())
						{
							gcSkillToObjectOK2.setObjectID( 1 );	// 의미 없다.
							gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
							gcSkillToObjectOK2.setDuration(0);
							pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
						}

						GCSkillToObjectOK4 gcSkillToObjectOK4;
						gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
						gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
						gcSkillToObjectOK4.setDuration(0);

						m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillToObjectOK4, pCreature);
					}
				}
			}
		}
	}

	if ( m_bFirst )
		m_bFirst = false;

	setNextTime( m_Tick );

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFireBreath::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectFireBreath::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectFireBreath("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectFireBreathLoader* g_pEffectFireBreathLoader = NULL;
