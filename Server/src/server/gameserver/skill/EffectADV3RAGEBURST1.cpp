//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRAGEBURST1.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectADV3RAGEBURST1.h"
//#include "EffectOUTRAGEBOOSTER2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCAddEffect.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRAGEBURST1::EffectRAGEBURST1(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_Damage = 0;
        m_EffectDamage = 0;
	m_UserObjectID = 0;
	m_bPlayer = bPlayer;

	m_SplashRatio[0] = 100;
	m_SplashRatio[1] = 100;
	m_SplashRatio[2] = 100;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEBURST1::affect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	// å®ž ° ©ž®ž °¡®¿Ž.
	// !! ž» ¶°³µ» öµµ ž¹· NULL  µ ö Ž.
	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );
	if ( m_bPlayer )
	{
//		pCastCreature = m_pZone->getCreature( m_UserObjectID );
		if ( pCastCreature == NULL )
		{
//			Tile& tile = m_pZone->getTile(m_X, m_Y);
//			tile.deleteEffect(m_ObjectID);
			setDeadline(0);
			return;
		}
	}

	Ousters* ThisOusters = dynamic_cast<Ousters*>(pCastCreature);

	Assert(ThisOusters != NULL);

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	int diff =2;

	for (int x=-diff; x<=diff; x++)
	{
		for (int y=-diff; y<=diff; y++)
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			int Damage = 0;
			int splash = max(abs(x), abs(y));


				if ( splash >= 3 ) splash = 2;
				Damage = getPercentValue( m_Damage, m_SplashRatio[splash] );
				
			const list<Object*>& oList = tile.getObjectList();
			list<Object*>::const_iterator itr = oList.begin();
			for (; itr != oList.end(); itr++) 
			{
				Assert(*itr != NULL);

				Object* pObject = *itr;
				Assert(pObject != NULL);

				if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
				{
					Creature* pCreature = dynamic_cast<Creature*>(pObject);
					Assert(pCreature != NULL);

					// º žö ŸŽŽ
					// ¹«û»ó ©. by sigi. 2002.9.5
					if (pCreature->getObjectID()==m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| !checkZoneLevelToHitTarget( pCreature )
//#ifdef VERSION_SKILL_1
//						|| !HitRoll::isSuccessMagic( pCastCreature, pCreature )
//#endif
					)
					{
						continue;
					}

					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;
                                        int success = 0;
					if (pCreature->isSlayer()) 
					{

						success = max( 0, 200 );


					} 
					else if (pCreature->isVampire())
					{
							


						success = max( 0, 200 );

					}
					else if (pCreature->isOusters())
					{
                                                if ( m_bPlayer )
                                                continue;

						success = max( 0, 0 );

					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						success = max( 0, 200 );

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}
				
					if (pCreature->isPC())
					{
						gcSkillToObjectOK2.setObjectID( 1 );	// ¹ ŸøŽ.
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

	setDeadline(0);

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEBURST1::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEBURST1::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRAGEBURST1::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectRAGEBURST1" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectRAGEBURST1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectRAGEBURST1("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectRAGEBURST1Loader* g_pEffectRAGEBURST1Loader = NULL;
