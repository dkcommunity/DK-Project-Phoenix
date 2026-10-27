//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectIceHorizon.cpp
// Written by  :
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "EffectDEMONICRESURRECTIO.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "SkillUtil.h"
#include "ZoneUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectDEMONICRESURRECTIO::EffectDEMONICRESURRECTIO(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY) 
	throw(Error)
{
	__BEGIN_TRY

	m_pZone = pZone;
	m_X = zoneX;
	m_Y = zoneY;
	m_CasterName ="";
	m_CasterID = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMONICRESURRECTIO::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectIceHorizon" << "affect BEGIN" << endl;
	
	Assert(m_pZone != NULL);

	// å®ž »ç¿ë ©ž®ž °¡®¿Ž.
	// !! ¹ ž» ³ª°¬» öµµ ž¹· NULL µ ö Ž.
	// by bezz. 2003.1.4
	Creature* pCastCreature = m_pZone->getCreature( m_CasterID );

	// öç å®°¡ ºŸîŽ ž» ¹Ÿ¿Ž.
    Tile& tile = m_pZone->getTile(m_X, m_Y);

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

			if ( pCreature->isFlag(Effect::EFFECT_CLASS_COMA) )
			{
				continue;
			}
			
			if ( !pCreature->isVampire() )
			{
				continue;
			}

			if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING)
			{
				Vampire* pTargetVampire = dynamic_cast<Vampire*>(pCreature);
				Assert( pTargetVampire != NULL );

				GCModifyInformation gcTargetMI;
				GCStatusCurrentHP gcTargetHP;
				
				HP_t final = min( (HP_t)pTargetVampire->getHP(ATTR_MAX), (HP_t)(pTargetVampire->getHP() + m_IncreaseAmount ));
				if ( final > pTargetVampire->getHP(ATTR_MAX) - pTargetVampire->getSilverDamage() )
				{
					pTargetVampire->setSilverDamage( pTargetVampire->getHP(ATTR_MAX) - final );
					gcTargetMI.addShortData(MODIFY_SILVER_DAMAGE, pTargetVampire->getSilverDamage());
				}

				if ( pTargetVampire->getHP() != final )
				{
					pTargetVampire->setHP( final );
					gcTargetMI.addShortData(MODIFY_CURRENT_HP, final);
				}
				gcTargetHP.setObjectID( pTargetVampire->getObjectID() );
				gcTargetHP.setCurrentHP( final );

				pTargetVampire->getPlayer()->sendPacket( &gcTargetMI );
				m_pZone->broadcastPacket( pTargetVampire->getX(), pTargetVampire->getY(), &gcTargetHP, pTargetVampire );

				GCAddEffect gcAE;
				gcAE.setObjectID( pTargetVampire->getObjectID() );
				gcAE.setEffectID( Effect::EFFECT_CLASS_DEMONICRESURRECTIO );
				gcAE.setDuration( 0 );
				m_pZone->broadcastPacket( pTargetVampire->getX(), pTargetVampire->getY(), &gcAE );
			}
		}
	}
	
	setNextTime(m_Tick);

	//cout << "EffectIceHorizon" << "affect END" << endl;

	__END_CATCH 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMONICRESURRECTIO::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMONICRESURRECTIO::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectDEMONICRESURRECTIO::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectIceHorizon" << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
	tile.deleteEffect(m_ObjectID);

	//cout << "EffectIceHorizon" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectDEMONICRESURRECTIO::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectDEMONICRESURRECTIO("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

