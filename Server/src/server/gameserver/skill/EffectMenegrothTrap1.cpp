//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothTrap1.cpp
// Written by  : Sequoia
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__ 

#include "EffectMenegrothTrap1.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "ZoneUtil.h"
#include "SkillHandler.h"
#include "SkillHandlerManager.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

EffectMenegrothTrap1::EffectMenegrothTrap1(Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;

	setDeadline(99999999);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap1::affect()
	throw(Error)
{
	__BEGIN_TRY
	affect(m_pZone, m_X, m_Y);
	__END_CATCH
}

void EffectMenegrothTrap1::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap1::affectCreature(Creature *pCreature)
{
	if ( pCreature->isMonster() )
		return;

	if ( pCreature->isPC() )
	{
		//cout << "EffectMenegrothTrap1 Creature Name on TrapRegion : " << pCreature->getName() << endl;

		GCModifyInformation gcMI;

		int iDamage;

		iDamage = m_iMinDamage + (rand() % ( m_iMaxDamage - m_iMinDamage));

		::setDamage( pCreature, iDamage, NULL, SKILL_ATTACK_MELEE, &gcMI, NULL, true );
		pCreature->getPlayer()->sendPacket( &gcMI );

		GCSkillToObjectOK2 gcSkillToObjectOK2;
		GCSkillToObjectOK4 gcSkillToObjectOK4;

		// user한테는 맞는 모습을 보여준다.
		gcSkillToObjectOK2.setObjectID( 1 );	// 의미 없다.
		gcSkillToObjectOK2.setSkillType( SKILL_ATTACK_MELEE );
		gcSkillToObjectOK2.setDuration(0);
		pCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);

		gcSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
		gcSkillToObjectOK4.setSkillType( SKILL_ATTACK_MELEE );
		gcSkillToObjectOK4.setDuration(0);

		m_pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcSkillToObjectOK4, pCreature);
				

		/*
		SkillHandler *pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_MENEGROTH_TRAP_1);
		pSkillHandler->execute(pCreature, 24, 30);
		*/
	}
}

void EffectMenegrothTrap1::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectMenegrothTrap1::affect()" << endl;;

	int iTileX;
	int iTileY;

	for(iTileY = y - 2; iTileY <= y + 2 ; ++iTileY)
	{
		for(iTileX = x - 1; iTileX <= x + 1 ; ++iTileX)
		{

			if ( isValidZoneCoord(pZone, iTileX, iTileY) )
			{

				Tile& tile = pZone->getTile(iTileX, iTileY);

				const list<Object*>& oList = tile.getObjectList();
				list<Object*>::const_iterator itr = oList.begin();
			   	for (; itr != oList.end(); itr++) 
				{
					Object *pObject = (*itr);

					if ( pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE)
					{
						Creature* pCreature = dynamic_cast<Creature*>(pObject);

						affectCreature(pCreature);
	
					}
				}
			}
		}
	}

	setNextTime(10);
	setDeadline(99999999);	// 무한대 지속

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap1::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap1::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	cout << "EffectMenegrothTrap1" << "unaffect BEGIN" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap1::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectMenegrothTrap1::setDamage(int iMinDamage, int iMaxDamage)
{
	m_iMinDamage = iMinDamage;
	m_iMaxDamage = iMaxDamage;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectMenegrothTrap1::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectMenegrothTrap1("
		<< ")";
	return msg.toString();

	__END_CATCH

}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */

