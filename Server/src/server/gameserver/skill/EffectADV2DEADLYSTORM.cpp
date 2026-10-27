#include "EffectADV2DEADLYSTORM.h"
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
#include "EffectSeduction.h"
#include "EffectPoison.h"
#include "EffectParalyze.h"
#include "EffectDarkBluePoison.h"
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2DEADLYSTORM::EffectADV2DEADLYSTORM(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_Damage = 0;
	m_UserObjectID = 0;
	m_bPlayer = bPlayer;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2DEADLYSTORM::affect()
	throw(Error)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	Creature* pCastCreature = m_pZone->getCreature( m_UserObjectID );
	if ( m_bPlayer )
	{
		if ( pCastCreature == NULL )
		{
			setDeadline(0);
			return;
		}
	}

	Vampire* ThisVampire = dynamic_cast<Vampire*>(pCastCreature);
	Assert( ThisVampire != NULL );

	int mabitime = 30;

	VSRect rect(0, 0, m_pZone->getWidth()-1, m_pZone->getHeight()-1);

	for (int x=-2; x<=2; x++)
	{
		for (int y=-2; y<=2; y++)
		{
			int X = m_X + x;
			int Y = m_Y + y;

			if (!rect.ptInRect(X, Y)) continue;
			Tile& tile = m_pZone->getTile(X, Y);

			int Damage = m_Damage;

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

					if (pCreature->getObjectID()==m_UserObjectID
						|| !canAttack( pCastCreature, pCreature )
						|| pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
						|| !checkZoneLevelToHitTarget( pCreature )
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

					if (pCreature->isSlayer()) 
					{
						Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

						::setDamage( pSlayer, Damage, pCastCreature, SKILL_ADV2_DEADLY_STORM, &gcSkillToObjectOK2, &gcAttackerMI);

						srand( (unsigned)time( NULL ) );
						bool bEffected1 = pSlayer->isFlag(Effect::EFFECT_CLASS_SEDUCTION);
						bool bEffected2 = pSlayer->isFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);
						bool bEffected3 = pSlayer->isFlag(Effect::EFFECT_CLASS_PARALYZE);
						bool bEffected4 = pSlayer->isFlag(Effect::EFFECT_CLASS_POISON);

						if (!bEffected1 && !bEffected2 && !bEffected3 && !bEffected4)
						 {
							int ratio = rand()%4;
							if (ratio == 1)
							   {
								EffectSeduction* pEffect = new EffectSeduction(pSlayer);
								pEffect->setDeadline(30);
								pEffect->setLevel(100);
								pEffect->setToHitPenalty(30);
								pEffect->setDamagePenalty(30);
								pSlayer->setFlag(Effect::EFFECT_CLASS_SEDUCTION);
								pSlayer->addEffect(pEffect);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pSlayer->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SEDUCTION);
								gcAddEffect.setDuration(30);
								pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect );
								pSlayer->initAllStatAndSend();
							   }
							if (ratio == 2)
							   {
								EffectPoison* pEffectPoison = new EffectPoison(pSlayer);

								pEffectPoison->setDeadline(30);
								pEffectPoison->setPoint(200);
								pEffectPoison->setTick(10); 
								pEffectPoison->setUserObjectID(m_UserObjectID);
								pEffectPoison->affect(pSlayer);

								pSlayer->addEffect( pEffectPoison );
								pSlayer->setFlag( Effect::EFFECT_CLASS_POISON );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pSlayer->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_POISON);
								gcAddEffect.setDuration(30);
								pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect);
							   }
							if (ratio == 3)
							   {
								EffectParalyze* pEffectParalyze = new EffectParalyze(pSlayer);
								pEffectParalyze->setLevel(100);
								pEffectParalyze->setDamageReduceRatio(30);
								pEffectParalyze->setDeadline(mabitime);
								pSlayer->addEffect(pEffectParalyze);
								pSlayer->setFlag(Effect::EFFECT_CLASS_PARALYZE);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pSlayer->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_PARALYZE);
								gcAddEffect.setDuration(mabitime);
								pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect );
							   }
							if (ratio == 0)
							   {
								EffectDarkBluePoison* pEffectDarkBluePoison = new EffectDarkBluePoison(pSlayer);
								pEffectDarkBluePoison->setDamage(80);
								pEffectDarkBluePoison->setLevel(30);
								pEffectDarkBluePoison->setPoisonLevel(30);
								pEffectDarkBluePoison->setDeadline(30);
								pEffectDarkBluePoison->setTick(3);
								pEffectDarkBluePoison->setNextTime(0);
								pSlayer->addEffect(pEffectDarkBluePoison);
								pSlayer->setFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pSlayer->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_DARKBLUE_POISON);
								gcAddEffect.setDuration(30);
								pSlayer->getZone()->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect );
							   }
						 }
					} 
					else if (pCreature->isVampire())
					{
						if ( m_bPlayer )
							continue;

						Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

						::setDamage( pVampire, Damage, pCastCreature, SKILL_ADV2_DEADLY_STORM, &gcSkillToObjectOK2, &gcAttackerMI);
					}
					else if (pCreature->isOusters())
					{
						Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

						::setDamage( pOusters, Damage, pCastCreature, SKILL_ADV2_DEADLY_STORM, &gcSkillToObjectOK2, &gcAttackerMI);

						srand( (unsigned)time( NULL ) );
						bool bEffected1 = pOusters->isFlag(Effect::EFFECT_CLASS_SEDUCTION);
						bool bEffected2 = pOusters->isFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);
						bool bEffected3 = pOusters->isFlag(Effect::EFFECT_CLASS_PARALYZE);
						bool bEffected4 = pOusters->isFlag(Effect::EFFECT_CLASS_POISON);

						if (!bEffected1 && !bEffected2 && !bEffected3 && !bEffected4)
						 {
							int ratio = rand()%4;
							if (ratio == 1)
							   {
								EffectSeduction* pEffect = new EffectSeduction(pOusters);
								pEffect->setDeadline(30);
								pEffect->setLevel(100);
								pEffect->setToHitPenalty(30);
								pEffect->setDamagePenalty(30);
								pOusters->setFlag(Effect::EFFECT_CLASS_SEDUCTION);
								pOusters->addEffect(pEffect);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pOusters->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SEDUCTION);
								gcAddEffect.setDuration(30);
								pOusters->getZone()->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect );
								pOusters->initAllStatAndSend();
							   }
							if (ratio == 2)
							   {
								EffectPoison* pEffectPoison = new EffectPoison(pOusters);

								pEffectPoison->setDeadline(30);
								pEffectPoison->setPoint(200);
								pEffectPoison->setTick(10); 
								pEffectPoison->setUserObjectID(m_UserObjectID);
								pEffectPoison->affect(pOusters);

								pOusters->addEffect( pEffectPoison );
								pOusters->setFlag( Effect::EFFECT_CLASS_POISON );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pOusters->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_POISON);
								gcAddEffect.setDuration(30);
								pOusters->getZone()->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect);
							   }
							if (ratio == 3)
							   {
								EffectParalyze* pEffectParalyze = new EffectParalyze(pOusters);
								pEffectParalyze->setLevel(100);
								pEffectParalyze->setDamageReduceRatio(30);
								pEffectParalyze->setDeadline(mabitime);
								pOusters->addEffect(pEffectParalyze);
								pOusters->setFlag(Effect::EFFECT_CLASS_PARALYZE);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pOusters->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_PARALYZE);
								gcAddEffect.setDuration(mabitime);
								pOusters->getZone()->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect );
							   }
							if (ratio == 0)
							   {
								EffectDarkBluePoison* pEffectDarkBluePoison = new EffectDarkBluePoison(pOusters);
								pEffectDarkBluePoison->setDamage(80);
								pEffectDarkBluePoison->setLevel(30);
								pEffectDarkBluePoison->setPoisonLevel(30);
								pEffectDarkBluePoison->setDeadline(30);
								pEffectDarkBluePoison->setTick(3);
								pEffectDarkBluePoison->setNextTime(0);
								pOusters->addEffect(pEffectDarkBluePoison);
								pOusters->setFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pOusters->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_DARKBLUE_POISON);
								gcAddEffect.setDuration(30);
								pOusters->getZone()->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect );
							   }
						 }
					}
					else if (pCreature->isMonster())
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCreature);

						::setDamage( pMonster, Damage*3, pCastCreature, SKILL_ADV2_DEADLY_STORM, NULL, &gcAttackerMI);

						if ( pCastCreature != NULL ) pMonster->addEnemy( pCastCreature );

						srand( (unsigned)time( NULL ) );
						bool bEffected1 = pMonster->isFlag(Effect::EFFECT_CLASS_SEDUCTION);
						bool bEffected2 = pMonster->isFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);
						bool bEffected3 = pMonster->isFlag(Effect::EFFECT_CLASS_PARALYZE);
						bool bEffected4 = pMonster->isFlag(Effect::EFFECT_CLASS_POISON);

						if (!bEffected1 && !bEffected2 && !bEffected3 && !bEffected4)
						 {
							int ratio = rand()%4;
							if (ratio == 1)
							   {
								EffectSeduction* pEffect = new EffectSeduction(pMonster);
								pEffect->setDeadline(30);
								pEffect->setLevel(100);
								pEffect->setToHitPenalty(30);
								pEffect->setDamagePenalty(30);
								pMonster->setFlag(Effect::EFFECT_CLASS_SEDUCTION);
								pMonster->addEffect(pEffect);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pMonster->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SEDUCTION);
								gcAddEffect.setDuration(30);
								pMonster->getZone()->broadcastPacket( pMonster->getX(), pMonster->getY(), &gcAddEffect );
							   }
							if (ratio == 2)
							   {
								EffectPoison* pEffectPoison = new EffectPoison(pMonster);

								pEffectPoison->setDeadline(30);
								pEffectPoison->setPoint(200);
								pEffectPoison->setTick(10); 
								pEffectPoison->setUserObjectID(m_UserObjectID);
								pEffectPoison->affect(pMonster);

								pMonster->addEffect( pEffectPoison );
								pMonster->setFlag( Effect::EFFECT_CLASS_POISON );

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pMonster->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_POISON);
								gcAddEffect.setDuration(30);
								pMonster->getZone()->broadcastPacket( pMonster->getX(), pMonster->getY(), &gcAddEffect);
							   }
							if (ratio == 3)
							   {
								EffectParalyze* pEffectParalyze = new EffectParalyze(pMonster);
								pEffectParalyze->setLevel(100);
								pEffectParalyze->setDamageReduceRatio(30);
								pEffectParalyze->setDeadline(mabitime);
								pMonster->addEffect(pEffectParalyze);
								pMonster->setFlag(Effect::EFFECT_CLASS_PARALYZE);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pMonster->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_PARALYZE);
								gcAddEffect.setDuration(mabitime);
								pMonster->getZone()->broadcastPacket( pMonster->getX(), pMonster->getY(), &gcAddEffect );
							   }
							if (ratio == 0)
							   {
								EffectDarkBluePoison* pEffectDarkBluePoison = new EffectDarkBluePoison(pMonster);
								pEffectDarkBluePoison->setDamage(80);
								pEffectDarkBluePoison->setLevel(30);
								pEffectDarkBluePoison->setPoisonLevel(30);
								pEffectDarkBluePoison->setDeadline(30);
								pEffectDarkBluePoison->setTick(3);
								pEffectDarkBluePoison->setNextTime(0);
								pMonster->addEffect(pEffectDarkBluePoison);
								pMonster->setFlag(Effect::EFFECT_CLASS_DARKBLUE_POISON);

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pMonster->getObjectID());
								gcAddEffect.setEffectID(Effect::EFFECT_CLASS_DARKBLUE_POISON);
								gcAddEffect.setDuration(30);
								pMonster->getZone()->broadcastPacket( pMonster->getX(), pMonster->getY(), &gcAddEffect );
							   }
						 }
					}

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

					if (pCreature->isPC())
					{
						gcSkillToObjectOK2.setObjectID( 1 );	
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

	setNextTime(m_Tick);

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2DEADLYSTORM::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2DEADLYSTORM::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2DEADLYSTORM::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2DEADLYSTORM::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectADV2DEADLYSTORM("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectADV2DEADLYSTORMLoader* g_pEffectADV2DEADLYSTORMLoader = NULL;
