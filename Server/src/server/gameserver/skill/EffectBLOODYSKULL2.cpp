//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBLOODYSKULL2.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectBLOODYSKULL2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBLOODYSKULL2::EffectBLOODYSKULL2(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_Damage = 0;
	m_UserObjectID = 0;
	m_bPlayer = bPlayer;

	m_SplashRatio[0] = 100;
	m_SplashRatio[1] = 100;
	m_SplashRatio[2] = 100;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLOODYSKULL2::affect()
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

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	Vampire* ThisVampire = dynamic_cast<Vampire*>(pCastCreature);
	
	Assert(ThisVampire != NULL);

	int diff =2;

	if (ThisVampire->hasRankGem(NewRankGem::SKILL_BLOODY_SKULL_RANGE ) )
	{
		NewRankGem* pRankGemVENOMRANGE = ThisVampire->getRankGem( NewRankGem::SKILL_BLOODY_SKULL_RANGE );
		Assert( pRankGemVENOMRANGE != NULL );

		diff=3;
	}

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

			// °¡¿îµ¥Ž 100%
//			if ( m_bPlayer )
//			{
/*				if ( splash == 0 )
				{
					Damage = m_Damage;
				}
				else if ( splash == 1 )
				{
					Damage = getPercentValue( m_Damage, 85 );
				}
				else
				{
					Damage = getPercentValue( m_Damage, 70 );
				}*/

				if ( splash >= 3 ) splash = 2;
				Damage = getPercentValue( m_Damage, m_SplashRatio[splash] );
				
//			}
//			else
//			{
//				// §¿¡Ž 50% damage
//				if ( splash != 0 ) Damage = m_Damage >> splash;
//				else Damage = m_Damage;
//			}

			// ž Ÿ¿¡ žçŽ ¿ºê§®µé» °»öŽ.
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
//		ú°ø û¿ë...
#ifndef VERSION_SKILL_1
					Damage = computeMagicDamage(pCreature, Damage, SKILL_BLOODY_SKULL_2);
#endif
					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

					//GCModifyInformation gcMI;
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;

					if (pCreature->isSlayer()) 
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, Damage, pCastCreature, SKILL_BLOODY_SKULL_2, &gcSkillToObjectOK2, &gcAttackerMI);

/*						Player* pPlayer = pSlayer->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);*/

					} 
					else if (pCreature->isVampire())
					{
						// ¹ìŸî°¡ »ç¿ë» °æ¿ì ¹ìŸîŽ  ž» ¿°íŽ žö ŸŽŽ.
						if ( m_bPlayer )// && splash != 0 )
							continue;

						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, Damage, pCastCreature, SKILL_BLOODY_SKULL_2, &gcSkillToObjectOK2, &gcAttackerMI );

/*						Player* pPlayer = pVampire->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);*/
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, Damage, pCastCreature, SKILL_BLOODY_SKULL_2, &gcSkillToObjectOK2, &gcAttackerMI );

/*						Player* pPlayer = pOusters->getPlayer();
						Assert(pPlayer != NULL);
						pPlayer->sendPacket(&gcMI);*/
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, Damage, pCastCreature, SKILL_BLOODY_SKULL_2, NULL, &gcAttackerMI );

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );
					}

					// »óŽë°¡ ŸúŽžé °æè¡ž ¿·Ž.
					if ( pCreature->isDead() )
					{
						if ( pCastCreature != NULL && pCastCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pCastCreature);
							Assert( pVampire != NULL );

							int exp = computeCreatureExp(pCreature, KILL_EXP);
							shareVampExp(pVampire, exp, gcAttackerMI);
							computeAlignmentChange( pCreature, Damage, pCastCreature, &gcSkillToObjectOK2, &gcAttackerMI );

							pVampire->getPlayer()->sendPacket( &gcAttackerMI );
						}
					}

					// userŽ žŽ žð» ºž¿©Ž.
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
void EffectBLOODYSKULL2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLOODYSKULL2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBLOODYSKULL2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectBLOODYSKULL2" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBLOODYSKULL2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBLOODYSKULL2("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectBLOODYSKULL2Loader* g_pEffectBLOODYSKULL2Loader = NULL;
