/////////////////////////////////////////////////////////////////////////////
// DynamicZoneGateOfAlter.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_ZONE_GATE_OF_ALTER_H__
#define __DYNAMIC_ZONE_GATE_OF_ALTER_H__

#include "Types.h"
#include "Timeval.h"
#include "DynamicZone.h"
#include <vector>

///////////////////////////////////////////////////////////
// class DynamicZoneGateOfAlter
///////////////////////////////////////////////////////////
class DynamicZoneGateOfAlter : public DynamicZone
{
public:
	DynamicZoneGateOfAlter();
	virtual ~DynamicZoneGateOfAlter();

public:
	void init();		// 다이나믹 존 초기화
	void heartbeat();	// 다이나믹 존 하트비트

public:
	bool checkPC();
	bool addTileEffect();
	bool checkNoEffect();
	bool checkNoDeleteTileEffect();
	bool openGateToAlter();
	bool removeTileEffect();

	void processEntering();
	void removeEffect( ZoneCoord_t x, ZoneCoord_t y );
	void addFakeEffect();

private:
	void startTileReveal();
	void heartbeatTileReveal();
	int countLiveDeleteTiles();
	int countRemainingHiddenTiles();
	int openHiddenTiles(int maxOpen);

	enum { GOA_REVEAL_SECONDS = 300 };

	int m_GOAStatus;
	bool m_RevealActive;
	Timeval m_RevealStart;
	int m_InitialHiddenCount;
	std::vector<std::pair<ZoneCoord_t, ZoneCoord_t> > m_PendingClientReveal;
};


DECLARE_DYNAMIC_ZONE_FACTORY( DynamicZoneGateOfAlter )

#endif

