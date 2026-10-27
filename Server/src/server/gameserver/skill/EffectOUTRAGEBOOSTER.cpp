//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectOUTRAGEBOOSTER.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectOUTRAGEBOOSTER.h"
#include "EffectOUTRAGEBOOSTER2.h"
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
EffectOUTRAGEBOOSTER::EffectOUTRAGEBOOSTER(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY, bool bPlayer) 
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
void EffectOUTRAGEBOOSTER::affect()
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
	if (ThisOusters->hasRankGem(NewRankGem::SKILL_OUTRAGE_BOOSTER_DAM ) )
	{
		NewRankGem* pRankGemOUTRAGEDAM = ThisOusters->getRankGem( NewRankGem::SKILL_OUTRAGE_BOOSTER_DAM );
		Assert( pRankGemOUTRAGEDAM != NULL );

		diff =3;
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
//#ifndef VERSION_SKILL_1
				//	Damage = computeMagicDamage(pCreature, Damage, SKILL_OUTRAGE_BOOSTER);
//#endif
					if ( pCastCreature != NULL && pCastCreature->isMonster() )
					{
						Monster* pMonster = dynamic_cast<Monster*>(pCastCreature);
						if ( pMonster != NULL && !pMonster->isEnemyToAttack( pCreature ) ) continue;
					}

					//GCModifyInformation gcMI;
					GCModifyInformation gcAttackerMI;
					GCSkillToObjectOK2 gcSkillToObjectOK2;
                                        int success = 0;
					if (pCreature->isSlayer()) 
					{

						success = max( 0, 200 );


					} 
					else if (pCreature->isVampire())
					{
						// ¹ìŸî°¡ »ç¿ë» °æ¿ì ¹ìŸîŽ  ž» ¿°íŽ žö ŸŽŽ.
						// && splash != 0 )
							


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
							success = min( 100, success );
							int ratio = rand()%100+1;

							if( success >= ratio &&
								!pCreature->isDead() &&
								!pCreature->isFlag( Effect::EFFECT_CLASS_COMA ) 

								)
							{
								//::setDamage( pCreature, output.Damage, pOusters, SKILL_OUTRAGE_BOOSTER );
                                                              int EffectDamage = m_EffectDamage;
								EffectOUTRAGEBOOSTER2* pEffectPoison = new EffectOUTRAGEBOOSTER2(pCreature);

								pEffectPoison->setDeadline(200);
								pEffectPoison->setSTRPenalty(EffectDamage);
								pEffectPoison->setINTPenalty(EffectDamage);
 	                                                        pEffectPoison->setDEXPenalty(EffectDamage);
				      				pEffectPoison->setUserObjectID(pCreature->getObjectID());
								pEffectPoison->affect(pCreature);

								pCreature->addEffect( pEffectPoison );
								pCreature->setFlag( Effect::EFFECT_CLASS_OUTRAGEBOOSTER2 );
                                                            //    cout << "Succeed11111!" << endl;
                                                                if ( pCreature->isPC() )
	                                                         {
		                                                   PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		                                                   pPC->initAllStatAndSend();
                                                               // cout << "Succeed222222!" << endl;
	                                                        }

								GCAddEffect gcAddEffect;
								gcAddEffect.setObjectID(pCreature->getObjectID());
								gcAddEffect.setEffectID( Effect::EFFECT_CLASS_OUTRAGEBOOSTER2);
								gcAddEffect.setDuration(200);
								m_pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &gcAddEffect);
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
void EffectOUTRAGEBOOSTER::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectOUTRAGEBOOSTER::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectOUTRAGEBOOSTER::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectOUTRAGEBOOSTER" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectOUTRAGEBOOSTER::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectOUTRAGEBOOSTER("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

EffectOUTRAGEBOOSTERLoader* g_pEffectOUTRAGEBOOSTERLoader = NULL;
