//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectHeterChakram.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectHeterChakram.h"
#include "EffectHeterChakramBomb.h"
#include "SimpleSkill.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK5.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectHeterChakram::EffectHeterChakram(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;
	m_Step = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakram::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectHeterChakram " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectHeterChakram " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakram::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

	if( pCastCreature == NULL )
	{
		setDeadline(0);
		return;
	}

	m_ObjectList.push_back(pCreature->getObjectID());

	Damage_t damage = 0;
	GCAddEffect gcAddEffect;
	GCModifyInformation gcMI;
	GCSkillToObjectOK1	_GCSkillToObjectOK1;
	GCSkillToObjectOK2	_GCSkillToObjectOK2;
	GCSkillToObjectOK5	_GCSkillToObjectOK5;

	if( m_Step == 0 || m_Step == 1)
	{
		damage = Damage_t(getDamage() * 0.9) ;
		
		Creature* pTargetCreature = serchTargetCreature( pZone, pCreature->getX(), pCreature->getY() );

		m_Step += 1;

		if( pTargetCreature == NULL )
		{
			setDeadline(0);
			return;
		}

		bool bCritical;
		damage += computeDamage( pCastCreature, pTargetCreature, 0, bCritical );

		if( pTargetCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pTargetCreature);

			::setDamage( pSlayer, damage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI);

			pSlayer->getPlayer()->sendPacket(&gcMI);
		}
		else if( pTargetCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);

			::setDamage( pVampire, damage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI);

			pVampire->getPlayer()->sendPacket(&gcMI);
		}
		else if( pTargetCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);

			::setDamage( pOusters, damage, pCastCreature, SKILL_HETER_CHAKRAM, &gcMI);

			pOusters->getPlayer()->sendPacket(&gcMI);
		}
		else if( pTargetCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			::setDamage( pMonster, damage, pCastCreature, SKILL_HETER_CHAKRAM);
		}

		list<Creature*> cList;
		cList.push_back(pCreature);
		cList.push_back(pTargetCreature);

		_GCSkillToObjectOK1.setSkillType(SKILL_HETER_CHAKRAM_CHAIN);
		_GCSkillToObjectOK1.setCEffectID(NULL);
		_GCSkillToObjectOK1.setTargetObjectID(pTargetCreature->getObjectID());
		_GCSkillToObjectOK1.setDuration(10);

		_GCSkillToObjectOK2.setObjectID(pCreature->getObjectID());
		_GCSkillToObjectOK2.setSkillType(SKILL_HETER_CHAKRAM_CHAIN);
		_GCSkillToObjectOK2.setDuration(10);

		_GCSkillToObjectOK5.setObjectID(pCreature->getObjectID());
		_GCSkillToObjectOK5.setSkillType(SKILL_HETER_CHAKRAM_CHAIN);
		_GCSkillToObjectOK5.setTargetObjectID(pTargetCreature->getObjectID());
		_GCSkillToObjectOK5.setDuration(10);

		cList = pZone->broadcastSkillPacket(pCreature->getX(), pCreature->getY(),
				pTargetCreature->getX(), pTargetCreature->getY(), &_GCSkillToObjectOK5, cList);

		if( pTargetCreature->isPC() )
		{
			Player* pTargetPlayer = pTargetCreature->getPlayer();
			Assert( pTargetPlayer != NULL );
			pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
		}
		else
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			pMonster->addEnemy(pCastCreature);
		}

		if( pCreature->isPC() )
		{
			pCreature->getPlayer()->sendPacket(&_GCSkillToObjectOK1);
		}

		EffectHeterChakram* pEffect = new EffectHeterChakram(pTargetCreature);
		pEffect->setDamage(getDamage());
		pEffect->setUserObjectID( pCastCreature->getObjectID() );
		pEffect->setStep(m_Step);
		pEffect->setNextTime(5);
		pEffect->setTargetCreature(pTargetCreature->getCreatureClass());
		pEffect->setAddEffectCreatureList(m_ObjectList);
		if( pTargetCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
			pEffect->setTargetSpriteType(pMonster->getSpriteType());
		}

		pTargetCreature->setFlag( Effect::EFFECT_CLASS_HETER_CHAKRAM );
		pTargetCreature->addEffect(pEffect);

		gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HETER_CHAKRAM );
		gcAddEffect.setDuration(10);

		pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect);
		setDeadline(0);
		return;
	}
	else if( m_Step == 2 )
	{
		damage = Damage_t(getDamage() * 0.8);

		bool bCritical;
		damage += computeDamage( pCastCreature, pCreature, 0, bCritical);

		EffectHeterChakramBomb* pEffect = new EffectHeterChakramBomb(pCreature);
		pEffect->setDamage(damage);
		pEffect->setUserObjectID( m_UserObjectID );
		pEffect->setTargetCreature( m_TargetClass );
		if( pCreature->isMonster() )
		{
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);
			pEffect->setTargetSpriteType(pMonster->getSpriteType());
		}

		pEffect->setNextTime(10);

		pCreature->setFlag( Effect::EFFECT_CLASS_HETER_CHAKRAM_BOMB );
		pCreature->addEffect( pEffect );

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID( pCreature->getObjectID() );
		gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HETER_CHAKRAM_BOMB );

		pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);
	}

	setDeadline(0);

	//cout << "EffectHeterChakram " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakram::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_HETER_CHAKRAM);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_HETER_CHAKRAM);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectHeterChakram::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectHeterChakram::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectHeterChakram("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

Creature* EffectHeterChakram::serchTargetCreature(Zone* pZone, Coord_t X, Coord_t Y)
	throw()
{
	Assert(pZone != NULL);
/*
	list<TILE_MASK> MaskList;

	MaskList.push_back( TILE_MASK(  1,  0,  0 ) );
	MaskList.push_back( TILE_MASK(  1,  1,  0 ) );
	MaskList.push_back( TILE_MASK(  0,  1,  0 ) );
	MaskList.push_back( TILE_MASK( -1,  1,  0 ) );
	MaskList.push_back( TILE_MASK( -1,  0,  0 ) );
	MaskList.push_back( TILE_MASK( -1, -1,  0 ) );
	MaskList.push_back( TILE_MASK(  0, -1,  0 ) );
	MaskList.push_back( TILE_MASK(  1, -1,  0 ) );

	MaskList.push_back( TILE_MASK(  2, -1,  0 ) );
	MaskList.push_back( TILE_MASK(  2,  0,  0 ) );
	MaskList.push_back( TILE_MASK(  2,  1,  0 ) );
	MaskList.push_back( TILE_MASK(  2,  2,  0 ) );
	MaskList.push_back( TILE_MASK(  1,  2,  0 ) );
	MaskList.push_back( TILE_MASK(  0,  2,  0 ) );
	MaskList.push_back( TILE_MASK( -1,  2,  0 ) );
	MaskList.push_back( TILE_MASK( -2,  2,  0 ) );
	MaskList.push_back( TILE_MASK( -2,  1,  0 ) );
	MaskList.push_back( TILE_MASK( -2,  0,  0 ) );
	MaskList.push_back( TILE_MASK( -2, -1,  0 ) );
	MaskList.push_back( TILE_MASK( -2, -2,  0 ) );
	MaskList.push_back( TILE_MASK( -1, -2,  0 ) );
	MaskList.push_back( TILE_MASK(  0, -2,  0 ) );
	MaskList.push_back( TILE_MASK(  1, -2,  0 ) );
	MaskList.push_back( TILE_MASK(  2, -2,  0 ) );

	MaskList.push_back( TILE_MASK(	3, -2,  0 ) );
	MaskList.push_back( TILE_MASK(	3, -1,  0 ) );
	MaskList.push_back( TILE_MASK(	3,  0,  0 ) );
	MaskList.push_back( TILE_MASK(	3,  1,  0 ) );
	MaskList.push_back( TILE_MASK(	3,  2,  0 ) );
	MaskList.push_back( TILE_MASK(	3,  3,  0 ) );
	MaskList.push_back( TILE_MASK(	1,  3,  0 ) );
	MaskList.push_back( TILE_MASK(	0,  3,  0 ) );
	MaskList.push_back( TILE_MASK( -1,  3,  0 ) );
	MaskList.push_back( TILE_MASK( -2,  3,  0 ) );
	MaskList.push_back( TILE_MASK( -3,  3,  0 ) );
	MaskList.push_back( TILE_MASK( -3,  2,  0 ) );
	MaskList.push_back( TILE_MASK( -3,  1,  0 ) );
	MaskList.push_back( TILE_MASK( -3,  0,  0 ) );
	MaskList.push_back( TILE_MASK( -3, -1,  0 ) );
	MaskList.push_back( TILE_MASK( -3, -2,  0 ) );
	MaskList.push_back( TILE_MASK( -3, -3,  0 ) );
	MaskList.push_back( TILE_MASK( -2, -3,  0 ) );
	MaskList.push_back( TILE_MASK( -1, -3,  0 ) );
	MaskList.push_back( TILE_MASK(	0, -3,  0 ) );
	MaskList.push_back( TILE_MASK(	1, -3,  0 ) );
	MaskList.push_back( TILE_MASK(	2, -3,  0 ) );
	MaskList.push_back( TILE_MASK(	3, -3,  0 ) );

	list<TILE_MASK>::const_iterator itr = MaskList.begin();
	list<TILE_MASK>::const_iterator endItr = MaskList.end();
*/
	VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

//	for( ; itr != endItr; ++itr )
	int targetNum = 0;
	int tx = X;
	int ty = Y;
	int sx = 1;
	int sy = 0;
	int maxCount = 1;
	int count = 1;
	int checkCount = 49;

	do
	{
		if( tx > 0 && ty > 0 && tx < pZone->getWidth() && ty < pZone->getHeight() )
		{
			if( rect.ptInRect( tx, ty) )
			{
				Tile& tile = pZone->getTile( tx, ty);

				const list<Object*>& oList = tile.getObjectList();
				list<Object*>::const_iterator oitr = oList.begin();
				list<Object*>::const_iterator oendItr = oList.end();

				for(; oitr != oendItr; ++oitr)
				{
					Object* pObject = *oitr;
					Assert( pObject != NULL );
					bool bCreatureCheck = true;

					if( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
					{
						Creature* pCreature = dynamic_cast<Creature*>(pObject);
						Assert( pCreature != NULL );

						if( pCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pCreature);
							Assert(pMonster != NULL);

							if( pMonster->getSpriteType() != m_TargetSpriteType )
							{
								bCreatureCheck = false;
							}
						}

						list<ObjectID_t>::const_iterator obeginlist = m_ObjectList.begin();
						list<ObjectID_t>::const_iterator oendlist = m_ObjectList.end();

						for( ; obeginlist != oendlist; ++obeginlist)
						{
							ObjectID_t objectID = *obeginlist;
							if( objectID == pObject->getObjectID() ||
									objectID == m_UserObjectID)
								bCreatureCheck = false;
						}

						if( pCreature->getObjectID() == m_UserObjectID
								|| pCreature->isFlag( Effect::EFFECT_CLASS_COMA )
								|| !checkZoneLevelToHitTarget( pCreature )
								|| pCreature->isNPC()
								|| pCreature->isDead()
								|| !bCreatureCheck
								|| pCreature->getCreatureClass() != m_TargetClass
						  )
						{
							continue;
						}

						if( bCreatureCheck )
						{
							return pCreature;
						}
					}
				}
			}
		}
		tx += sx;
		ty += sy;
		if( --count == 0 )
		{
			if( sx == 0 ) maxCount++;

			int temp = sx;
			sx = -sy;
			sy = temp;

			count = maxCount;
		}
	} while ( --checkCount );
	return NULL;
}

void EffectHeterChakram::setAddEffectCreatureList(list<ObjectID_t> objectList)
{
	list<ObjectID_t>::const_iterator obeginlist = objectList.begin();
	list<ObjectID_t>::const_iterator oendlist = objectList.end();

	for(; obeginlist != oendlist; ++obeginlist)
	{
		m_ObjectList.push_back(*obeginlist);
	}
}
