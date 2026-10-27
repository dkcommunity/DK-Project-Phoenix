//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMenegrothTrap2.cpp
// Written by  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__ 

#include "EffectMenegrothTrap2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "SkillHandler.h"
#include "SkillHandlerManager.h"

#include "Gpackets/GCAddEffectToTile.h" 
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

EffectMenegrothTrap2::EffectMenegrothTrap2(Zone *pZone, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pZone != NULL);

	m_pZone = pZone;
	m_X = X;
	m_Y = Y;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap2::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectMenegrothTrap2" << "affect BEGIN" << endl;
	affect(m_pZone, m_X, m_Y);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap2::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void EffectMenegrothTrap2::affectCreature(Creature *pCreature)
{
    if ( pCreature->isMonster() )
		return;

	if ( pCreature->isPC() )
	{
		//cout << "EffectMenegrothTrap2 Creature Name on TrapRegion : " << pCreature->getName() << endl;

		int iDamage;

		iDamage = m_iMinDamage + (rand() % ( m_iMaxDamage - m_iMinDamage));

		GCModifyInformation gcMI;

		::setDamage( pCreature, iDamage, NULL, SKILL_ATTACK_MELEE, &gcMI, NULL, true );
		pCreature->getPlayer()->sendPacket( &gcMI );

		GCSkillToObjectOK2 gcSkillToObjectOK2;
        GCSkillToObjectOK4 gcSkillToObjectOK4;

        // user한테는 맞는 모습을 보여준다.
        gcSkillToObjectOK2.setObjectID( 1 );    // 의미 없다.
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

void EffectMenegrothTrap2::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y)
	throw(Error)
{
	__BEGIN_TRY

	int iTileX;
	int iTileY;

	for(iTileY = y - 1; iTileY <= y + 1 ; ++iTileY)
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

				// 이텍트를 X 자 모양으로 표현
				int iOffsetX;
				int iOffsetY;

				iOffsetX = iTileX - (x-1);
				iOffsetY = iTileY - (y-1);

				if ( iOffsetX == iOffsetY || (2-iOffsetX) == iOffsetY )
				{
					GCAddEffectToTile gcAE;
					gcAE.setEffectID(this->getEffectClass());
					gcAE.setObjectID(this->getObjectID());
					gcAE.setXY(iTileX, iTileY);
					gcAE.setDuration(20);

					m_pZone->broadcastPacket(iTileX, iTileY, &gcAE);
				}

			}
		}
	}

	setNextTime(10);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap2::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectMenegrothTrap2" << "unaffect BEGIN" << endl;

	//cout << "EffectMenegrothTrap2" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap2::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMenegrothTrap2::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void EffectMenegrothTrap2::setDamage(int iMinDamage, int iMaxDamage)
{
	m_iMinDamage = iMinDamage;
	m_iMaxDamage = iMaxDamage;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectMenegrothTrap2::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectMenegrothTrap2("
		<< ")";
	return msg.toString();

	__END_CATCH

}

#endif /* __MENEGROTH_DOUNGEON_SYSTEM__ */
