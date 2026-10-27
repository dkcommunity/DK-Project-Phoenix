/////////////////////////////////////////////////////////////////////////////
// DynamicZoneGateOfAlter.cpp
/////////////////////////////////////////////////////////////////////////////

#include "DynamicZoneGateOfAlter.h"
#include "DynamicZoneInfo.h"
#include "PlayerCreature.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "PCManager.h"
#include "EffectManager.h"
#include "EffectDeleteTile.h"
#include "EffectTilePortal.h"
#include "GQuestManager.h"
#include "DB.h"
#include "Assert.h"

#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCDeleteEffectFromTile.h"

#include <stdio.h>
#include <stdlib.h>
#include <utility>

const BYTE EffectMask[32][32] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

enum GATE_OF_ALTER_STATUS
{
	ADD_TILE_EFFECT_STATUS,
	WAIT_FOR_PC,
	DELETE_TILE_EFFECT_STATUS,
	OPEN_GATE_TO_ALTER,
	WAIT_FOR_CLEAR,
	REMOVE_TILE_EFFECT,
	WAIT_FOR_NO_EFFECT,
	GATE_OF_ALTER_STATUS_END,

	GATE_OF_ALTER_STATUS_MAX
};

DynamicZoneGateOfAlter::DynamicZoneGateOfAlter()
{
	m_GOAStatus = ADD_TILE_EFFECT_STATUS;
	m_RevealActive = false;
	m_InitialHiddenCount = 0;
	m_RevealStart.tv_sec = 0;
	m_RevealStart.tv_usec = 0;
}

DynamicZoneGateOfAlter::~DynamicZoneGateOfAlter()
{
}

void DynamicZoneGateOfAlter::init()
{
	m_GOAStatus = ADD_TILE_EFFECT_STATUS;
	m_RevealActive = false;
	m_InitialHiddenCount = 0;
	m_PendingClientReveal.clear();
	getCurrentTime( m_Deadline );
	m_Deadline.tv_sec += 60;
}

void DynamicZoneGateOfAlter::heartbeat()
{
	switch ( m_GOAStatus )
	{
	case ADD_TILE_EFFECT_STATUS:
		if ( addTileEffect() )
		{
			m_GOAStatus = WAIT_FOR_PC;
			cout << "WAIT_FOR_PC" << endl;
		}
		break;

	case WAIT_FOR_PC:
		if ( checkPC() )
		{
			processEntering();
			addFakeEffect();
			startTileReveal();
			m_GOAStatus = DELETE_TILE_EFFECT_STATUS;
			cout << "DELETE_TILE_EFFECT_STATUS" << endl;
		}
		break;

	case DELETE_TILE_EFFECT_STATUS:
		heartbeatTileReveal();
		if ( checkNoDeleteTileEffect() )
		{
			if ( !m_PendingClientReveal.empty() )
				openHiddenTiles( (int)m_PendingClientReveal.size() );
			m_GOAStatus = OPEN_GATE_TO_ALTER;
			cout << "OPEN_GATE_TO_ALTER" << endl;
		}
		break;

	case OPEN_GATE_TO_ALTER:
		if ( openGateToAlter() )
		{
			m_GOAStatus = WAIT_FOR_CLEAR;
			cout << "WAIT_FOR_CLEAR" << endl;
		}
		break;

	case WAIT_FOR_CLEAR:
		if ( !checkPC() )
		{
			m_GOAStatus = REMOVE_TILE_EFFECT;
			cout << "REMOVE_TILE_EFFECT" << endl;
		}
		break;

	case REMOVE_TILE_EFFECT:
		if ( removeTileEffect() )
		{
			m_GOAStatus = WAIT_FOR_NO_EFFECT;
			cout << "WAIT_FOR_NO_EFFECT" << endl;
		}
		break;

	case WAIT_FOR_NO_EFFECT:
		if ( checkNoEffect() )
		{
			m_GOAStatus = GATE_OF_ALTER_STATUS_END;
			m_Status = DYNAMIC_ZONE_STATUS_READY;
			cout << "READY" << endl;
		}
		break;
	}

	if ( !checkPC() )
	{
		if ( m_GOAStatus == WAIT_FOR_PC || m_GOAStatus == ADD_TILE_EFFECT_STATUS )
		{
			Timeval current;
			getCurrentTime( current );

			if ( current > m_Deadline )
			{
				cout << "----------------------------Wait Time Out" << endl;
				m_GOAStatus = WAIT_FOR_CLEAR;
			}
		}
		else if ( m_GOAStatus == DELETE_TILE_EFFECT_STATUS  )
		{
			// Phoenix 2026-09-07: Çember tamamsa PC kısa süre yok olsa bile OPEN_GATE atlama.
			// Eski yol REMOVE_TILE'a gidip TILE_PORTAL/clearDynamicZone'u tamamen kaçırıyordu.
			if ( checkNoDeleteTileEffect() )
			{
				if ( !m_PendingClientReveal.empty() )
					openHiddenTiles( (int)m_PendingClientReveal.size() );
				m_GOAStatus = OPEN_GATE_TO_ALTER;
				cout << "OPEN_GATE_TO_ALTER (circle done, pc empty)" << endl;
			}
			else
			{
				m_GOAStatus = REMOVE_TILE_EFFECT;
				cout << "REMOVE_TILE_EFFECT (pc empty, circle incomplete)" << endl;
			}
		}
	}
}

bool DynamicZoneGateOfAlter::addTileEffect()
{
	Assert( m_pZone != NULL );

	m_PendingClientReveal.clear();

	for ( ZoneCoord_t x = 0; x < m_pZone->getWidth(); ++x )
	//for ( ZoneCoord_t x = 15; x <= 17; ++x )
	{
		for ( ZoneCoord_t y = 0; y < m_pZone->getHeight(); ++y )
		//for ( ZoneCoord_t y = 15; y <= 17; ++y )
		{
			if ( EffectMask[y][x] != 0 )
			{
				// ????? ??????? ??????? ?????? ?? ????? ???
				Tile& tile = m_pZone->getTile( x, y );

				if ( !tile.canAddEffect() || tile.hasEffect() )
				{
					if ( !tile.hasPortal() )
						m_PendingClientReveal.push_back( std::make_pair(x, y) );
					continue;
				}

				// ????? ??????? ????
				EffectDeleteTile* pEffect = new EffectDeleteTile( m_pZone, x, y );
				pEffect->setDeadline( 999999 );
				
				// ???? OID ???
				m_pZone->registerObject( pEffect );
				m_pZone->addEffect( pEffect );

				// ??? ?? ?????
				tile.addEffect( pEffect );
			}
		}
	}

	return true;
}

bool DynamicZoneGateOfAlter::checkPC()
{
	Assert( m_pZone != NULL );

	// PC ?? ??
	uint size = m_pZone->getPCManager()->getSize();

	return size != 0;
}

bool DynamicZoneGateOfAlter::checkNoEffect()
{
	Assert( m_pZone != NULL );

	// ????? ?? ??
	uint size = m_pZone->getEffectManager()->getSize();

//		char msg[8];
//		sprintf( msg, "%u", size );
//		GCSystemMessage gcmsg;
//		gcmsg.setMessage( msg );
//		m_pZone->broadcastPacket( &gcmsg );

	return size == 0;
}

bool DynamicZoneGateOfAlter::checkNoDeleteTileEffect()
{
	Assert( m_pZone != NULL );
	return countLiveDeleteTiles() == 0;
}

void DynamicZoneGateOfAlter::startTileReveal()
{
	Assert( m_pZone != NULL );

	getCurrentTime( m_RevealStart );
	m_InitialHiddenCount = countRemainingHiddenTiles();
	m_RevealActive = true;

	cout << "GOA tile reveal start hidden=" << m_InitialHiddenCount << endl;

	GCSystemMessage gcmsg;
	gcmsg.setMessage( "The remaining hidden tiles will appear within 5 minutes. Keep hunting." );
	m_pZone->broadcastPacket( &gcmsg );
}

int DynamicZoneGateOfAlter::countLiveDeleteTiles()
{
	Assert( m_pZone != NULL );

	Timeval now;
	getCurrentTime( now );

	int count = 0;
	const int width = m_pZone->getWidth();
	const int height = m_pZone->getHeight();

	for ( int x = 0; x < width; ++x )
	{
		for ( int y = 0; y < height; ++y )
		{
			Tile& tile = m_pZone->getTile( x, y );
			Effect* pEffect = tile.getEffect( Effect::EFFECT_CLASS_DELETE_TILE );
			if ( pEffect != NULL && pEffect->getDeadline() > now )
				++count;
		}
	}

	return count;
}

int DynamicZoneGateOfAlter::countRemainingHiddenTiles()
{
	return countLiveDeleteTiles() + (int)m_PendingClientReveal.size();
}

int DynamicZoneGateOfAlter::openHiddenTiles(int maxOpen)
{
	Assert( m_pZone != NULL );

	if ( maxOpen <= 0 )
		return 0;

	Timeval now;
	getCurrentTime( now );

	int opened = 0;
	const int width = m_pZone->getWidth();
	const int height = m_pZone->getHeight();

	for ( int x = 0; x < width && opened < maxOpen; ++x )
	{
		for ( int y = 0; y < height && opened < maxOpen; ++y )
		{
			Tile& tile = m_pZone->getTile( x, y );
			Effect* pEffect = tile.getEffect( Effect::EFFECT_CLASS_DELETE_TILE );
			if ( pEffect != NULL && pEffect->getDeadline() > now )
			{
				pEffect->setDeadline( 0 );
				++opened;
			}
		}
	}

	while ( opened < maxOpen && !m_PendingClientReveal.empty() )
	{
		const ZoneCoord_t x = m_PendingClientReveal.back().first;
		const ZoneCoord_t y = m_PendingClientReveal.back().second;
		m_PendingClientReveal.pop_back();

		GCDeleteEffectFromTile gcDeleteEffectFromTile;
		gcDeleteEffectFromTile.setEffectID( Effect::EFFECT_CLASS_DELETE_TILE );
		gcDeleteEffectFromTile.setXY( x, y );
		m_pZone->broadcastPacket( &gcDeleteEffectFromTile );
		++opened;
	}

	return opened;
}

void DynamicZoneGateOfAlter::heartbeatTileReveal()
{
	if ( !m_RevealActive || m_InitialHiddenCount <= 0 )
		return;

	Timeval now;
	getCurrentTime( now );
	Timeval elapsed = timediff( now, m_RevealStart );

	int elapsedSec = (int)elapsed.tv_sec;
	if ( elapsedSec < 0 )
		elapsedSec = 0;

	int remaining = countRemainingHiddenTiles();
	int alreadyOpened = m_InitialHiddenCount - remaining;
	if ( alreadyOpened < 0 )
		alreadyOpened = 0;

	int targetOpened = 0;
	if ( elapsedSec >= GOA_REVEAL_SECONDS )
		targetOpened = m_InitialHiddenCount;
	else
		targetOpened = (m_InitialHiddenCount * elapsedSec) / GOA_REVEAL_SECONDS;

	int toOpen = targetOpened - alreadyOpened;
	if ( toOpen > 0 )
		openHiddenTiles( toOpen );

	if ( remaining <= 0 || elapsedSec >= GOA_REVEAL_SECONDS )
		m_RevealActive = false;
}

bool DynamicZoneGateOfAlter::openGateToAlter()
{
	Assert( m_pZone != NULL );

	// Quest ClearDynamicZone(4001) — PC varsa ilerlet.
	map<ObjectID_t, Creature*>::const_iterator itr = m_pZone->getPCManager()->getCreatures().begin();
	map<ObjectID_t, Creature*>::const_iterator endItr = m_pZone->getPCManager()->getCreatures().end();

	int clearedPC = 0;
	for ( ; itr != endItr; ++itr )
	{
		Assert( itr->second != NULL );
		
		if ( itr->second->isPC() )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(itr->second);
			Assert( pPC != NULL );

			pPC->getGQuestManager()->clearDynamicZone(m_TemplateZoneID);
			++clearedPC;
		}
	}

	// ZoneTriggers portal tile'ı (15-16) + client hardcode (16,16) ile uyumlu VFX.
	int tx = -1;
	int ty = -1;
	int portalTiles = 0;
	for (int x=0; x<m_pZone->getWidth(); x++)
	{
		for (int y=0; y<m_pZone->getHeight(); y++)
		{
			Tile& tile = m_pZone->getTile(x,y);

			if (tile.hasPortal())
			{
				tx = x;
				ty = y;
				++portalTiles;
			}
		}
	}

	// hasPortal bulunamazsa bile GOA merkez portal görseli (client 16,16 bekler).
	if ( tx == -1 )
	{
		tx = 16;
		ty = 16;
		cout << "GOA openGate: no hasPortal tile, fallback VFX at 16,16" << endl;
	}

	GCAddEffectToTile gcAddEffectToTile;
	gcAddEffectToTile.setEffectID( Effect::EFFECT_CLASS_TILE_PORTAL );
	gcAddEffectToTile.setDuration( 999999 );
	gcAddEffectToTile.setXY( tx, ty );
	m_pZone->broadcastPacket( &gcAddEffectToTile );

	cout << "GOA openGate: TILE_PORTAL at (" << tx << "," << ty
		 << ") portalTiles=" << portalTiles
		 << " clearedPC=" << clearedPC
		 << " templateZone=" << (int)m_TemplateZoneID << endl;

	return true;
}

bool DynamicZoneGateOfAlter::removeTileEffect()
{
	Assert( m_pZone != NULL );

	// ??? ??????? ?????.
	m_pZone->getEffectManager()->setTimeOutAllEffect();

	return true;
}

void DynamicZoneGateOfAlter::removeEffect( ZoneCoord_t x, ZoneCoord_t y )
{
	Assert( m_pZone != NULL );

	VSRect rect(0,0,m_pZone->getWidth()-1,m_pZone->getHeight()-1);
	for ( int i = (int)x-1; i<=(int)x+1; i++ )
	{
		for ( int j = (int)y-1; j<=(int)y+1; j++ )
		{
			if ( !rect.ptInRect(i,j) )
				continue;

			Tile& tile = m_pZone->getTile(i,j);

			Effect* pEffect = tile.getEffect( Effect::EFFECT_CLASS_DELETE_TILE );

			if ( pEffect != NULL )
			{
				pEffect->setDeadline(0);
			}
		}
	}
}

void DynamicZoneGateOfAlter::addFakeEffect()
{
	Assert( m_pZone != NULL );

	// ??? ???????? FakeEffect ?? ??????.
	for (int x=0; x<m_pZone->getWidth(); x++)
	{
		for (int y=0; y<m_pZone->getHeight(); y++)
		{
			Tile& tile = m_pZone->getTile(x,y);

			if (tile.hasPortal())
			{
				GCDeleteEffectFromTile gcDeleteEffectFromTile;
				gcDeleteEffectFromTile.setEffectID( Effect::EFFECT_CLASS_DELETE_TILE );
				gcDeleteEffectFromTile.setXY(x,y);
				m_pZone->broadcastPacket(&gcDeleteEffectFromTile);
			}
		}
	}
}

void DynamicZoneGateOfAlter::processEntering()
{
	// ????? ???? ???????? ?????.
	map<ObjectID_t, Creature*>::const_iterator itr = m_pZone->getPCManager()->getCreatures().begin();
	map<ObjectID_t, Creature*>::const_iterator endItr = m_pZone->getPCManager()->getCreatures().end();

	for ( ; itr != endItr; ++itr )
	{
		Assert( itr->second != NULL );
		
		if ( itr->second->isPC() )
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(itr->second);
			Assert( pPC != NULL );

			pPC->getGQuestManager()->enterDynamicZone(m_TemplateZoneID);
		}
	}
}


DEFINE_DYNAMIC_ZONE_FACTORY( DynamicZoneGateOfAlter, DYNAMIC_ZONE_GATE_OF_ALTER )

