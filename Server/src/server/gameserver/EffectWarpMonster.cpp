//----------------------------------------------------------------------
//
// Filename    : EffectWarpMonster.cpp
// Written by  : Reiot
//
//----------------------------------------------------------------------

// include files
#include "Assert.h"
#include "EffectWarpMonster.h"
#include "Gpackets/GCDeleteObject.h"
#include "Tile.h"
#include "Zone.h"
#include "Item.h"
#include "ZoneUtil.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
EffectWarpMonster::EffectWarpMonster (Zone* pZone, ObjectID_t monsterObjectID, ZoneCoord_t x, ZoneCoord_t y, Dir_t dir) 
	throw (Error)
: m_MonsterObjectID(monsterObjectID), m_X(x), m_Y(y), m_Direction(dir)
{
	__BEGIN_TRY

	m_pZone = pZone;
	Assert( m_pZone != NULL );

	// 서버 전용 Effect이다. by sigi. 2002.11.14
	m_bBroadcastingEffect = false;

	__END_CATCH
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
EffectWarpMonster::~EffectWarpMonster () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
			

//----------------------------------------------------------------------
// affect
//----------------------------------------------------------------------
void EffectWarpMonster::affect ()
	throw (Error)
{
	__BEGIN_TRY
	__END_CATCH
}


//----------------------------------------------------------------------
// unaffect
//----------------------------------------------------------------------
void EffectWarpMonster::unaffect()
	throw (Error)
{
	__BEGIN_TRY

	Assert( m_pZone != NULL );

	Creature* pCreature = m_pZone->getCreature( m_MonsterObjectID );

	if ( pCreature != NULL )
	{
		m_pZone->deleteCreature( pCreature, pCreature->getX(), pCreature->getY() );
		m_pZone->addCreature( pCreature, m_X, m_Y, m_Direction );
	}

	__END_CATCH
}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string EffectWarpMonster::toString () const 
	throw ()
{
	StringStream msg;

	msg << "EffectWarpMonster("
			<< "MonsterObjectID:" << (int)m_MonsterObjectID
			<< ",X:"     << (int)getX()
			<< ",Y:"     << (int)getY();

	msg << ")";

	return msg.toString();
}
