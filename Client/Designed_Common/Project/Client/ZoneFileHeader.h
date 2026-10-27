//----------------------------------------------------------------------
// ZoneFileHeader.h
//----------------------------------------------------------------------

#ifndef	__ZONEFILEHEADER_H__
#define	__ZONEFILEHEADER_H__

#include <Windows.h>
#include "MString.h"
#include "MZoneDef.h"
std::ifstream;
std::ofstream;

//----------------------------------------------------------------------
// FILEINFO_ZONE_HEADER
//----------------------------------------------------------------------
class FILEINFO_ZONE_HEADER {
public:
	enum ZONE_TYPE
	{
		NORMAL,
		SLAYER_GUILD,
		RESERVED_SLAYER_GUILD,
		NPC_VAMPIRE_LAIR,
		PC_VAMPIRE_LAIR,
		NPC_HOME,
		NPC_SHOP,
		RANDOMAP
	};

public :
	MString		ZoneVersion;	// Last updated
	WORD		ZoneID;			//	Zone ID
	WORD		ZoneGroupID;	//	Zone Group ID
	MString		ZoneName;		// Zone Name


	BYTE		ZoneType;		//	Zone Type
	BYTE		ZoneLevel;		//	The level of the monsters that appear in the zone. Between 1 and 10.
	MString		Description;	//	Description of the Zone

public :
	FILEINFO_ZONE_HEADER()
	{
		ZoneVersion = MAP_VERSION_2000_05_10;
	}

	void		SaveToFile(std::ofstream& file);
	void		LoadFromFile(ivfstream& file);

};

#endif

