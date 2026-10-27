#include "Client_PCH.h"
#include <fstream>
#include "vfstream.h"
#include "MGpsTable.h"

#if __CONTENTS(__GPS_ADD)

MGPS_INFO::MGPS_INFO() : 
m_dwTitleMonster_Info_Count(0),
m_dwZone_Info_Count(0),
m_dwNpc_Info_Count(0),
m_dwPotal_Info_Count(0),
m_dwListMonster_Info_Count(0),
m_dwQuest_Info_Count(0)
{
	m_SGps_TitleMonster_Info.clear();
	m_SGps_TitleMonster_Info.resize(0);

	m_SGps_Zone_Info.clear();
	m_SGps_Zone_Info.resize(0);
	
	m_SGps_Npc_Info.clear();
	m_SGps_Npc_Info.resize(0);
	
	m_SGps_Potal_Info.clear();
	m_SGps_Potal_Info.resize(0);
	
	m_SGps_ListMonster_Info.clear();
	m_SGps_ListMonster_Info.resize(0);

	m_SGps_Quest_Info.clear();
	m_SGps_Quest_Info.resize(0);

	GpsTitleMonsterInfoInit();
	GpsZoneInfoInit();
	GpsNPCInfoInit();
	GpsPotalInfoInit();
	GpsListMonsterInfoInit();
	GpsQuestInfoInit();
}

MGPS_INFO::~MGPS_INFO()
{
	Realease();
}

void	MGPS_INFO::GpsTitleMonsterInfoInit()
{
	SGPS_MONSTER_INFO	SGps_TitleMonster_Info;

	//에슬라니아
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_ESL_NE;
	SGps_TitleMonster_Info.strZoneName		= "에슬라니아 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 35;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_ESL_NW;
	SGps_TitleMonster_Info.strZoneName		= "에슬라니아 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 2;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 14;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_ESL_SE;
	SGps_TitleMonster_Info.strZoneName		= "에슬라니아 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 58;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_ESL_SW;
	SGps_TitleMonster_Info.strZoneName		= "에슬라니아 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 10;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 23;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	//Limbo
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LIMBO_NE;
	SGps_TitleMonster_Info.strZoneName		= "Limbo 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 33;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LIMBO_NW;
	SGps_TitleMonster_Info.strZoneName		= "Limbo 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 35;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 70;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LIMBO_SE;
	SGps_TitleMonster_Info.strZoneName		= "Limbo 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 5;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();
	
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LIMBO_SW;
	SGps_TitleMonster_Info.strZoneName		= "Limbo 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 15;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 63;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	//Drobeta
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_DROBETA_NE;
	SGps_TitleMonster_Info.strZoneName		= "Drobeta 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 70;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_DROBETA_NW;
	SGps_TitleMonster_Info.strZoneName		= "Drobeta 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 5;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 30;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_DROBETA_SE;
	SGps_TitleMonster_Info.strZoneName		= "Drobeta 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 60;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 99;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_DROBETA_SW;
	SGps_TitleMonster_Info.strZoneName		= "Drobeta 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 18;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 40;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	//Mt. Rodin
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LODIN_NE;
	SGps_TitleMonster_Info.strZoneName		= "Mt. Rodin 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 125;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 145;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LODIN_NW;
	SGps_TitleMonster_Info.strZoneName		= "Mt. Rodin 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 135;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 165;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LODIN_SE;
	SGps_TitleMonster_Info.strZoneName		= "Mt. Rodin 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 125;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 165;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_LODIN_SW;
	SGps_TitleMonster_Info.strZoneName		= "Mt. Rodin 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 125;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 165;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	//Timor
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_TIMOR_NE;
	SGps_TitleMonster_Info.strZoneName		= "Timor 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 70;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_TIMOR_NW;
	SGps_TitleMonster_Info.strZoneName		= "Timor 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 14;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 40;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_TIMOR_SE;
	SGps_TitleMonster_Info.strZoneName		= "Timor 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 99;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_TIMOR_SW;
	SGps_TitleMonster_Info.strZoneName		= "Timor 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 45;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 99;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	//Perona 국도
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_PERONA_NE;
	SGps_TitleMonster_Info.strZoneName		= "Perona국도 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 5;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_PERONA_NW;
	SGps_TitleMonster_Info.strZoneName		= "Perona국도 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 2;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_PERONA_SE;
	SGps_TitleMonster_Info.strZoneName		= "Perona국도 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 20;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();	

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_PERONA_SW;
	SGps_TitleMonster_Info.strZoneName		= "Perona국도 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 1;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 2;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	//Castalo
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_CASTALO_NE;
	SGps_TitleMonster_Info.strZoneName		= "Castalo 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 5;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 25;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	//Branco
	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_BRANCO_NE;
	SGps_TitleMonster_Info.strZoneName		= "Branco 북동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 86;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 115;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_BRANCO_NW;
	SGps_TitleMonster_Info.strZoneName		= "Branco 북서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 86;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 96;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_BRANCO_SE;
	SGps_TitleMonster_Info.strZoneName		= "Branco 남동";
	SGps_TitleMonster_Info.wMinMonsterLv	= 86;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 117;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();

	SGps_TitleMonster_Info.wZoneID			= WORLDMAP_ID_BRANCO_SW;
	SGps_TitleMonster_Info.strZoneName		= "Branco 남서";
	SGps_TitleMonster_Info.wMinMonsterLv	= 93;
	SGps_TitleMonster_Info.wMaxMonsterLv	= 115;
	m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
	SGps_TitleMonster_Info.Realease();
}



void	MGPS_INFO::GpsZoneInfoInit()
{
	SGPS_ZONE_INFO		SGps_Zone_Info;

	//Eslania
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Eslania NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_ESL_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Eslania NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_ESL_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Eslania SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_ESL_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Eslania SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_ESL_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Limbo
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Limbo NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LIMBO_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Limbo NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LIMBO_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Limbo SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LIMBO_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Limbo SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LIMBO_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Drobeta
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Drobeta NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_DROBETA_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Drobeta NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_DROBETA_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Drobeta SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_DROBETA_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Drobeta SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_DROBETA_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Mt. Rodin
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Mt. Rodin NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LODIN_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Mt. Rodin NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LODIN_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Mt. Rodin SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LODIN_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Mt. Rodin SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_LODIN_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Timor
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Timor NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_TIMOR_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Timor NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_TIMOR_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Timor SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_TIMOR_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Timor SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_TIMOR_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();


	//Timor
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Perona NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_PERONA_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Perona NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_PERONA_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Perona SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_PERONA_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Perona SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_PERONA_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Perona
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Perona SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_CASTALO_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	//Branco
	SGps_Zone_Info.strGpsTitle_ZoneName		= "Branco NE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_BRANCO_NE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Branco NW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_BRANCO_NW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Branco SE";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_BRANCO_SE;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();

	SGps_Zone_Info.strGpsTitle_ZoneName		= "Branco SW";
	SGps_Zone_Info.wZoneID					= WORLDMAP_ID_BRANCO_SW;
	m_SGps_Zone_Info.push_back(SGps_Zone_Info);
	SGps_Zone_Info.Realease();
}



void	MGPS_INFO::GpsNPCInfoInit()
{
	SGPS_NPC_INFO		SGps_Npc_Info;

	SGps_Npc_Info.wPos_X								= 159;
	SGps_Npc_Info.wPos_Y								= 167;
	SGps_Npc_Info.strNpcName							= "Jack";
	SGps_Npc_Info.strNpcExplanation						= "Motorcycle Sales and Recalls";
	SGps_Npc_Info.wSpkIndex								= GPS_MOVE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 137;
	SGps_Npc_Info.wPos_Y								= 145;
	SGps_Npc_Info.strNpcName							= "Matthew";
	SGps_Npc_Info.strNpcExplanation						= "Exchanging Contribution Reward Items and Traveling to Other Regions";
	SGps_Npc_Info.wSpkIndex								= GPS_SPECIAL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 222;
	SGps_Npc_Info.wPos_Y								= 95;
	SGps_Npc_Info.strNpcName							= "Kaiser";
	SGps_Npc_Info.strNpcExplanation						= "Sword and Dagger Training and Quests";
	SGps_Npc_Info.wSpkIndex								= GPS_SKILL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 217;
	SGps_Npc_Info.wPos_Y								= 95;
	SGps_Npc_Info.strNpcName							= "Jacques";
	SGps_Npc_Info.strNpcExplanation						= "Technical Training and Arms Distribution";
	SGps_Npc_Info.wSpkIndex								= GPS_SKILL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 207;
	SGps_Npc_Info.wPos_Y								= 97;
	SGps_Npc_Info.strNpcName							= "Tanya";
	SGps_Npc_Info.strNpcExplanation						= "Enchant and Heal Skills Training and Virtual Training System";
	SGps_Npc_Info.wSpkIndex								= GPS_SKILL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 211;
	SGps_Npc_Info.wPos_Y								= 116;
	SGps_Npc_Info.strNpcName							= "Terry";
	SGps_Npc_Info.strNpcExplanation						= "Selling Beginner Items and the Blood Contract";
	SGps_Npc_Info.wSpkIndex								= GPS_SALE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 220;
	SGps_Npc_Info.wPos_Y								= 113;
	SGps_Npc_Info.strNpcName							= "Nancy";
	SGps_Npc_Info.strNpcExplanation						= "General and Special Item Sales";
	SGps_Npc_Info.wSpkIndex								= GPS_SALE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 47;
	SGps_Npc_Info.wPos_Y								= 181;
	SGps_Npc_Info.strNpcName							= "Argonauts";
	SGps_Npc_Info.strNpcExplanation						= "Travel to the Menegros Dungeon";
	SGps_Npc_Info.wSpkIndex								= GPS_MOVE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 216;
	SGps_Npc_Info.wPos_Y								= 230;
	SGps_Npc_Info.strNpcName							= "Belegost";
	SGps_Npc_Info.strNpcExplanation						= "Travel to the Menegros Dungeon";
	SGps_Npc_Info.wSpkIndex								= GPS_MOVE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_LIMBO_NW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 129;
	SGps_Npc_Info.wPos_Y								= 86;
	SGps_Npc_Info.strNpcName							= "Larva";
	SGps_Npc_Info.strNpcExplanation						= "Exchange Promotion Items";
	SGps_Npc_Info.wSpkIndex								= GPS_SPECIAL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_LIMBO_SE;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 35;
	SGps_Npc_Info.wPos_Y								= 190;
	SGps_Npc_Info.strNpcName							= "Raven";
	SGps_Npc_Info.strNpcExplanation						= "Buying and Selling General and Special Items (Coats)";
	SGps_Npc_Info.wSpkIndex								= GPS_SALE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_DROBETA_SW;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 115;
	SGps_Npc_Info.wPos_Y								= 203;
	SGps_Npc_Info.strNpcName							= "Griffith";
	SGps_Npc_Info.strNpcExplanation						= "Purchase storage boxes, store items, and buy and sell general and special items (serums, seals); repair items";
	SGps_Npc_Info.wSpkIndex								= GPS_SAFE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_NE;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 98;
	SGps_Npc_Info.wPos_Y								= 217;
	SGps_Npc_Info.strNpcName							= "Murphy";
	SGps_Npc_Info.strNpcExplanation						= "Buying and selling general and special items, and traveling to other regions";
	SGps_Npc_Info.wSpkIndex								= GPS_SALE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_NE;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 32;
	SGps_Npc_Info.wPos_Y								= 22;
	SGps_Npc_Info.strNpcName							= "Dior";
	SGps_Npc_Info.strNpcExplanation						= "Buying Storage Boxes, Storage, and General and Special Item Sales, The Blood Bible";
	SGps_Npc_Info.wSpkIndex								= GPS_SAFE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_NW;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 26;
	SGps_Npc_Info.wPos_Y								= 52;
	SGps_Npc_Info.strNpcName							= "Manescu";
	SGps_Npc_Info.strNpcExplanation						= "Exchanging Promotion Items and Buying/Selling Items";
	SGps_Npc_Info.wSpkIndex								= GPS_SPECIAL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_NW;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 17;
	SGps_Npc_Info.wPos_Y								= 222;
	SGps_Npc_Info.strNpcName							= "Robin";
	SGps_Npc_Info.strNpcExplanation						= "Purchasing storage boxes, storing and repairing items, and buying and selling";
	SGps_Npc_Info.wSpkIndex								= GPS_SAFE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_SW;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 36;
	SGps_Npc_Info.wPos_Y								= 228;
	SGps_Npc_Info.strNpcName							= "Carmen";
	SGps_Npc_Info.strNpcExplanation						= "Healing and item trading, traveling to other regions, and setting a respawn location";
	SGps_Npc_Info.wSpkIndex								= GPS_HEEL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_SW;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 23;
	SGps_Npc_Info.wPos_Y								= 235;
	SGps_Npc_Info.strNpcName							= "Frederick";
	SGps_Npc_Info.strNpcExplanation						= "Exchanging Promotion Items and Entering Hell Garden";
	SGps_Npc_Info.wSpkIndex								= GPS_SPECIAL_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_PERONA_SW;
	SGps_Npc_Info.wMapWidth								= 128;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 190;
	SGps_Npc_Info.wPos_Y								= 111;
	SGps_Npc_Info.strNpcName							= "EA";
	SGps_Npc_Info.strNpcExplanation						= "Travel to the Menegros Dungeon";
	SGps_Npc_Info.wSpkIndex								= GPS_MOVE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_CASTALO_NE;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();

	SGps_Npc_Info.wPos_X								= 196;
	SGps_Npc_Info.wPos_Y								= 136;
	SGps_Npc_Info.strNpcName							= "Karl Koch";
	SGps_Npc_Info.strNpcExplanation						= "Selling Quest Items";
	SGps_Npc_Info.wSpkIndex								= GPS_MOVE_NPC_ICON;
	SGps_Npc_Info.wZoneID								= WORLDMAP_ID_BRANCO_NE;
	SGps_Npc_Info.wMapWidth								= 256;
	SGps_Npc_Info.wMapHeight							= 256;
	m_SGps_Npc_Info.push_back(SGps_Npc_Info);
	SGps_Npc_Info.Realease();
}



void	MGPS_INFO::GpsPotalInfoInit()
{
	SGPS_POTAL_INFO		SGps_Potal_Info;

//ID 11
	SGps_Potal_Info.wPos_X							= 102;
	SGps_Potal_Info.wPos_Y							= 52;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 102;
	SGps_Potal_Info.wPos_Y							= 52;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 226;
	SGps_Potal_Info.wPos_Y							= 10;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 245;
	SGps_Potal_Info.wPos_Y							= 114;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 197;
	SGps_Potal_Info.wPos_Y							= 154;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 141;
	SGps_Potal_Info.wPos_Y							= 149;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 99;
	SGps_Potal_Info.wPos_Y							= 144;
	SGps_Potal_Info.strPotalName					= "Eslania Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
	
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Eslania SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 12
	SGps_Potal_Info.wPos_X							= 74;
	SGps_Potal_Info.wPos_Y							= 155;
	SGps_Potal_Info.strPotalName					= "무사 길드 F1";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 117;
	SGps_Potal_Info.wPos_Y							= 198;
	SGps_Potal_Info.strPotalName					= "군인 길드 F1";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 118;
	SGps_Potal_Info.wPos_Y							= 113;
	SGps_Potal_Info.strPotalName					= "성직자 길드 F1";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 216;
	SGps_Potal_Info.wPos_Y							= 103;
	SGps_Potal_Info.strPotalName					= "가상 훈련장";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Eslania SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 13
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Eslania NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Drobeta NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 14
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Eslania NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Drobeta NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 80;
	SGps_Potal_Info.wPos_Y							= 96;
	SGps_Potal_Info.strPotalName					= "칼리고 Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 130;
	SGps_Potal_Info.wPos_Y							= 31;
	SGps_Potal_Info.strPotalName					= "리피니움 탄광";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 196;
	SGps_Potal_Info.wPos_Y							= 240;
	SGps_Potal_Info.strPotalName					= "테메리에 성지";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_ESL_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 21
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Limbo SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 20;
	SGps_Potal_Info.wPos_Y							= 187;
	SGps_Potal_Info.strPotalName					= "리피니움 탄광";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 22
	SGps_Potal_Info.wPos_X							= 234;
	SGps_Potal_Info.wPos_Y							= 211;
	SGps_Potal_Info.strPotalName					= "Lost Taiyan B1";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 230;
	SGps_Potal_Info.wPos_Y							= 231;
	SGps_Potal_Info.strPotalName					= "Limbo 내부";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 219;
	SGps_Potal_Info.wPos_Y							= 148;
	SGps_Potal_Info.strPotalName					= "힐라놈 성지";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Limbo SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 23
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Limbo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Castalo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 151;
	SGps_Potal_Info.wPos_Y							= 191;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 163;
	SGps_Potal_Info.wPos_Y							= 149;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 159;
	SGps_Potal_Info.wPos_Y							= 74;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 65;
	SGps_Potal_Info.wPos_Y							= 109;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 57;
	SGps_Potal_Info.wPos_Y							= 163;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 89;
	SGps_Potal_Info.wPos_Y							= 193;
	SGps_Potal_Info.strPotalName					= "Limbo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 16;
	SGps_Potal_Info.wPos_Y							= 14;
	SGps_Potal_Info.strPotalName					= "Limbo 내부";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 24
	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Limbo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Limbo SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 234;
	SGps_Potal_Info.wPos_Y							= 23;
	SGps_Potal_Info.strPotalName					= "Limbo 내부";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LIMBO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 31
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Eslania SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Branco NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Drobeta SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 217;
	SGps_Potal_Info.wPos_Y							= 181;
	SGps_Potal_Info.strPotalName					= "Raohm Dungeon B1F";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 110;
	SGps_Potal_Info.wPos_Y							= 163;
	SGps_Potal_Info.strPotalName					= "Garbel Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();


//ID 32
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Castalo NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Eslania SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Drobeta SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 151;
	SGps_Potal_Info.wPos_Y							= 129;
	SGps_Potal_Info.strPotalName					= "Eisen Dungeon B1F";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 33
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Drobeta NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Branco SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 150;
	SGps_Potal_Info.wPos_Y							= 23;
	SGps_Potal_Info.strPotalName					= "Garbel Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 230;
	SGps_Potal_Info.wPos_Y							= 27;
	SGps_Potal_Info.strPotalName					= "Raohm Dungeon B1F";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
	
//ID 34
	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Drobeta NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 192;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Perona NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Perona NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_DROBETA_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 41
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
	
	SGps_Potal_Info.wPos_X							= 223;
	SGps_Potal_Info.wPos_Y							= 43;
	SGps_Potal_Info.strPotalName					= "Lost Taiyan B1";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 235;
	SGps_Potal_Info.wPos_Y							= 29;
	SGps_Potal_Info.strPotalName					= "Inside Rasen Castle";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 42
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 182;
	SGps_Potal_Info.wPos_Y							= 240;
	SGps_Potal_Info.strPotalName					= "Clone Bathory Lair";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 43
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Branco NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 192;
	SGps_Potal_Info.wPos_Y							= 58;
	SGps_Potal_Info.strPotalName					= "Kali Caves";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 44
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Eslania SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_LODIN_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 51
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Lake Timor NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Lake Timor SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();


//ID 52
	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Perona SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 192;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Perona SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
	
	SGps_Potal_Info.wPos_X							= 256;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Timor NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 98;
	SGps_Potal_Info.wPos_Y							= 83;
	SGps_Potal_Info.strPotalName					= "Clone Tepez Lair";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 53
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Lake Timor SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Lake Timor NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 190;
	SGps_Potal_Info.wPos_Y							= 48;
	SGps_Potal_Info.strPotalName					= "Adam's Holy Land E";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 161;
	SGps_Potal_Info.wPos_Y							= 167;
	SGps_Potal_Info.strPotalName					= "Adam's Holy Land C";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 243;
	SGps_Potal_Info.wPos_Y							= 226;
	SGps_Potal_Info.strPotalName					= "Rust Tower 1F";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 35;
	SGps_Potal_Info.wPos_Y							= 162;
	SGps_Potal_Info.strPotalName					= "Adam's Holy Land W";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID  54
	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Lake Timor NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Lake Timor SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 61
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Perona NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Drobeta SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Perona SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 34;
	SGps_Potal_Info.wPos_Y							= 115;
	SGps_Potal_Info.strPotalName					= "Asylion Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 103;
	SGps_Potal_Info.wPos_Y							= 215;
	SGps_Potal_Info.strPotalName					= "Lost Taiyan B1";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 62
	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Drobeta SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Perona NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Perona SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_NW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 63
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Perona SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Perona NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Perona NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 86;
	SGps_Potal_Info.wPos_Y							= 31;
	SGps_Potal_Info.strPotalName					= "Asylion Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SE;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 64
	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Perona NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Perona SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 118;
	SGps_Potal_Info.wPos_Y							= 186;
	SGps_Potal_Info.strPotalName					= "Asylion Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 64;
	SGps_Potal_Info.wPos_Y							= 256;
	SGps_Potal_Info.strPotalName					= "Lake Timor NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_PERONA_SW;
	SGps_Potal_Info.wMapWidth						= 128;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 81
	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Limbo SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_CASTALO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_CASTALO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 27;
	SGps_Potal_Info.wPos_Y							= 160;
	SGps_Potal_Info.strPotalName					= "Ripinium Coal Mine";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_CASTALO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 125;
	SGps_Potal_Info.wPos_Y							= 76;
	SGps_Potal_Info.strPotalName					= "Caligo Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_CASTALO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 173;
	SGps_Potal_Info.wPos_Y							= 78;
	SGps_Potal_Info.strPotalName					= "The Village of Ouster";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_CASTALO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 91
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Branco NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Mt. Rodin SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Branco SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 196;
	SGps_Potal_Info.wPos_Y							= 140;
	SGps_Potal_Info.strPotalName					= "준저터널 1층";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 92
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Branco NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 255;
	SGps_Potal_Info.strPotalName					= "Branco SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_DWON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 106;
	SGps_Potal_Info.wPos_Y							= 54;
	SGps_Potal_Info.strPotalName					= "Garbel Dungeon";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 32;
	SGps_Potal_Info.wPos_Y							= 115;
	SGps_Potal_Info.strPotalName					= "Raohm Dungeon B1F";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_NW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 93
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Branco SW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Branco NE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

//ID 94
	SGps_Potal_Info.wPos_X							= 1;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Drobeta SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_LEFT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 128;
	SGps_Potal_Info.wPos_Y							= 1;
	SGps_Potal_Info.strPotalName					= "Branco NW";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_UP;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 255;
	SGps_Potal_Info.wPos_Y							= 128;
	SGps_Potal_Info.strPotalName					= "Vranko SE";
	SGps_Potal_Info.wSpkIndex						= GPS_FIELDMOVE_RIGHT;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 209;
	SGps_Potal_Info.wPos_Y							= 199;
	SGps_Potal_Info.strPotalName					= "IK Office";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();

	SGps_Potal_Info.wPos_X							= 236;
	SGps_Potal_Info.wPos_Y							= 195;
	SGps_Potal_Info.strPotalName					= "IK Office";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_BRANCO_SW;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
	
	SGps_Potal_Info.wPos_X							= 227;
	SGps_Potal_Info.wPos_Y							= 20;
	SGps_Potal_Info.strPotalName					= "Château de Tiffauges";
	SGps_Potal_Info.wSpkIndex						= GPS_DUNGEON_ICON;
	SGps_Potal_Info.wZoneID							= WORLDMAP_ID_TIMOR_NE;
	SGps_Potal_Info.wMapWidth						= 256;
	SGps_Potal_Info.wMapHeight						= 256;
	m_SGps_Potal_Info.push_back(SGps_Potal_Info);
	SGps_Potal_Info.Realease();
}


void	MGPS_INFO::GpsListMonsterInfoInit()
{
	SGPS_MONSTER_INFO	SGps_ListMonster_Info;

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_ListMonster_Info.strZoneName							= "가상훈련장1 (Eslania 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "슬레이어 초보 Dungeon";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 0;
	SGps_ListMonster_Info.wMaxMonsterLv							= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_ListMonster_Info.strZoneName							= "가상훈련장2 (Eslania 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "슬레이어 초보 Dungeon";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 0;
	SGps_ListMonster_Info.wMaxMonsterLv							= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LIMBO_SE;
	SGps_ListMonster_Info.strZoneName							= "Limbo Dungeon(Limbo 남동 )";
	SGps_ListMonster_Info.strMonsterLv							= "뱀파이어 초보 Dungeon";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 0;
	SGps_ListMonster_Info.wMaxMonsterLv							= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_CASTALO_NE;
	SGps_ListMonster_Info.strZoneName							= "하니알Dungeon (Castalo 북동)";
	SGps_ListMonster_Info.strMonsterLv							= "아우스터즈 초보 Dungeon";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 0;
	SGps_ListMonster_Info.wMaxMonsterLv							= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_SW;
	SGps_ListMonster_Info.strZoneName							= "테메리에 성지 (Eslania 남서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  10 ~ 25";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 10;
	SGps_ListMonster_Info.wMaxMonsterLv							= 25;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_ListMonster_Info.strZoneName							= "페이악 터널 (Eslania 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  14 ~ 33";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 14;
	SGps_ListMonster_Info.wMaxMonsterLv							= 33;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LIMBO_SE;
	SGps_ListMonster_Info.strZoneName							= "페이악 터널 (Limbo 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  14 ~ 33";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 14;
	SGps_ListMonster_Info.wMaxMonsterLv							= 33;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LIMBO_SE;
	SGps_ListMonster_Info.strZoneName							= "고르고바 터널 (Limbo 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  14 ~ 33";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 14;
	SGps_ListMonster_Info.wMaxMonsterLv							= 33;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LIMBO_SE;
	SGps_ListMonster_Info.strZoneName							= "칼리석굴 (Mt. Rodin 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  20 ~ 60";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 20;
	SGps_ListMonster_Info.wMaxMonsterLv							= 60;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_NE;
	SGps_ListMonster_Info.strZoneName							= "Eslania Dungeon (Eslania 북동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  22 ~ 55";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 22;
	SGps_ListMonster_Info.wMaxMonsterLv							= 55;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LODIN_SE;
	SGps_ListMonster_Info.strZoneName							= "라센 수련장 (Mt. Rodin 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  30 ~ 50";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 30;
	SGps_ListMonster_Info.wMaxMonsterLv							= 50;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LODIN_SE;
	SGps_ListMonster_Info.strZoneName							= "라센 외성 (Mt. Rodin 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  30 ~ 58";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 30;
	SGps_ListMonster_Info.wMaxMonsterLv							= 58;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_DROBETA_NE;
	SGps_ListMonster_Info.strZoneName							= "Garbel Dungeon (Drobeta 북동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_DROBETA_SE;
	SGps_ListMonster_Info.strZoneName							= "Garbel Dungeon (Drobeta 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_BRANCO_NW;
	SGps_ListMonster_Info.strZoneName							= "Garbel Dungeon (Branco 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_PERONA_NE;
	SGps_ListMonster_Info.strZoneName							= "Asylion Dungeon (Perona국도 북동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_PERONA_SE;
	SGps_ListMonster_Info.strZoneName							= "아실리온Dungeon (Perona국도 남동)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_PERONA_SW;
	SGps_ListMonster_Info.strZoneName							= "아실리온Dungeon (Perona국도 남서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  79 ~ 93";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 79;
	SGps_ListMonster_Info.wMaxMonsterLv							= 93;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_BRANCO_SW;
	SGps_ListMonster_Info.strZoneName							= "IK Office (Branco 남서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  102 ~ 124";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 102;
	SGps_ListMonster_Info.wMaxMonsterLv							= 124;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_BRANCO_SW;
	SGps_ListMonster_Info.strZoneName							= "IK Lap (Branco 남서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  115 ~ 224";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 115;
	SGps_ListMonster_Info.wMaxMonsterLv							= 224;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_ESL_NW;
	SGps_ListMonster_Info.strZoneName							= "지하수로 (Eslania 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  125 ~ 145";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 125;
	SGps_ListMonster_Info.wMaxMonsterLv							= 145;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID								= WORLDMAP_ID_LIMBO_NW;
	SGps_ListMonster_Info.strZoneName							= "힐라놈 성지 (Limbo 북서)";
	SGps_ListMonster_Info.strMonsterLv							= "몬스터 레벨대:  132 ~ 148";
	SGps_ListMonster_Info.strExplanation;
	SGps_ListMonster_Info.wMinMonsterLv							= 132;
	SGps_ListMonster_Info.wMaxMonsterLv							= 148;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LIMBO_SW;
	SGps_ListMonster_Info.strZoneName												= "바토리Dungeon 지하 (Limbo 남서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "바토리 Dungeon 지하1층  몬스터 레벨대:  1 ~ 2";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "바토리 Dungeon 지하2층  몬스터 레벨대:  1 ~ 2";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "바토리 Dungeon 지하3층  몬스터 레벨대:  25 ~ 40";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "바토리 Dungeon 지하4층  몬스터 레벨대:  38 ~ 60";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LIMBO_NW;
	SGps_ListMonster_Info.strZoneName												= "Lost Taiyan 지하 (Limbo 북서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하1층  몬스터 레벨대:  20 ~ 58";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하2층  몬스터 레벨대:  30 ~ 58";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_PERONA_NE;
	SGps_ListMonster_Info.strZoneName												= "Lost Taiyan 지하 (Perona국도 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하1층  몬스터 레벨대:  20 ~ 58";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하2층  몬스터 레벨대:  30 ~ 58";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LODIN_NE;
	SGps_ListMonster_Info.strZoneName												= "Lost Taiyan 지하 (Mt. Rodin 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하1층  몬스터 레벨대:  20 ~ 58";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Lost Taiyan 지하2층  몬스터 레벨대:  30 ~ 58";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_ESL_SW;
	SGps_ListMonster_Info.strZoneName												= "칼리고 Dungeon (Eslania 남서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 1층  몬스터 레벨대:  20 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 2층  몬스터 레벨대:  55 ~ 70";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 3층  몬스터 레벨대:  70 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 4층  몬스터 레벨대:  99 ~ 110";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LIMBO_SE;
	SGps_ListMonster_Info.strZoneName												= "칼리고 Dungeon (Limbo 남동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 1층  몬스터 레벨대:  20 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 2층  몬스터 레벨대:  55 ~ 70";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 3층  몬스터 레벨대:  70 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 4층  몬스터 레벨대:  99 ~ 110";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_CASTALO_NE;
	SGps_ListMonster_Info.strZoneName												= "칼리고 Dungeon (Castalo 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 1층  몬스터 레벨대:  20 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 2층  몬스터 레벨대:  50 ~ 65";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 3층  몬스터 레벨대:  70 ~ 85";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "칼리고 Dungeon 4층  몬스터 레벨대:  99 ~ 110";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_ESL_NW;
	SGps_ListMonster_Info.strZoneName												= "메네그로스 Dungeon (Eslania 북서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 1층  몬스터 레벨대:  30 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 2층  몬스터 레벨대:  55 ~ 70";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 3층  몬스터 레벨대:  70 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 4층  몬스터 레벨대:  90 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 5층  몬스터 레벨대:  110 ~ 125";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 6층  몬스터 레벨대:  130 ~ 155";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LIMBO_NW;
	SGps_ListMonster_Info.strZoneName												= "메네그로스 Dungeon (Limbo 북서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 1층  몬스터 레벨대:  30 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 2층  몬스터 레벨대:  55 ~ 70";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 3층  몬스터 레벨대:  70 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 4층  몬스터 레벨대:  90 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 5층  몬스터 레벨대:  110 ~ 125";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 6층  몬스터 레벨대:  130 ~ 155";
	SGps_ListMonster_Info.wMinMonsterLv							= 0;
	SGps_ListMonster_Info.wMaxMonsterLv							= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_CASTALO_NE;
	SGps_ListMonster_Info.strZoneName												= "메네그로스 Dungeon (Castalo 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 1층  몬스터 레벨대:  30 ~ 45";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 2층  몬스터 레벨대:  55 ~ 70";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 3층  몬스터 레벨대:  70 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 4층  몬스터 레벨대:  90 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 5층  몬스터 레벨대:  110 ~ 125";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "메네그로스 Dungeon 6층  몬스터 레벨대:  130 ~ 155";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_DROBETA_NW;
	SGps_ListMonster_Info.strZoneName												= "Eisen Dungeon (Northwest of Drobeta)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Eisen Dungeon, Floor 1  Monster Level Range:  70–99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Eisen Dungeon, 2nd Floor  Monster Level Range:  99–105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Eisen Dungeon, Floor 3  Monster Level Range:  100–140";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_LODIN_SE;
	SGps_ListMonster_Info.strZoneName												= "라센 내성 (Mt. Rodin 남동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "라센 내성 1층  몬스터 레벨대:  73 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "라센 내성 2층  몬스터 레벨대:  99 ~ 100";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_DROBETA_NE;
	SGps_ListMonster_Info.strZoneName												= "Raohm Dungeon (Drobeta 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 1층  몬스터 레벨대:  73 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 2층  몬스터 레벨대:  99 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 3층  몬스터 레벨대:  104 ~ 112";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 4층  몬스터 레벨대:  104 ~ 135";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 5층  몬스터 레벨대:  135 ~ 146";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_DROBETA_SE;
	SGps_ListMonster_Info.strZoneName												= "Raohm Dungeon (Drobeta 남동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 1층  몬스터 레벨대:  73 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 2층  몬스터 레벨대:  99 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 3층  몬스터 레벨대:  104 ~ 112";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 4층  몬스터 레벨대:  104 ~ 135";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 5층  몬스터 레벨대:  135 ~ 146";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();

	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_BRANCO_NW;
	SGps_ListMonster_Info.strZoneName												= "Raohm Dungeon (Branco 북서)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 1층  몬스터 레벨대:  73 ~ 99";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 2층  몬스터 레벨대:  99 ~ 105";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 3층  몬스터 레벨대:  104 ~ 112";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 4층  몬스터 레벨대:  104 ~ 135";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Raohm Dungeon 5층  몬스터 레벨대:  135 ~ 146";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_TIMOR_SE;
	SGps_ListMonster_Info.strZoneName												= "Rust Tower (Timor 남동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Rust Tower 1층  몬스터 레벨대:  100 ~ 135";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "Rust Tower 2층  몬스터 레벨대:  103 ~ 140";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
	
	SGps_ListMonster_Info.wZoneID													= WORLDMAP_ID_BRANCO_NE;
	SGps_ListMonster_Info.strZoneName												= "준저 터널 (Branco 북동)";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "준저 터널 1층  몬스터 레벨대:  165 ~ 177";
	SGps_ListMonster_Info.strExplanation[SGps_ListMonster_Info.wExplanationCount++]	= "준저 터널 2층  몬스터 레벨대:  185 ~ 194";
	SGps_ListMonster_Info.wMinMonsterLv												= 0;
	SGps_ListMonster_Info.wMaxMonsterLv												= 0;
	m_SGps_ListMonster_Info.push_back(SGps_ListMonster_Info);
	SGps_ListMonster_Info.Realease();
}

void	MGPS_INFO::GpsQuestInfoInit()
{
}


void	MGPS_INFO::SaveToFile(std::ofstream& file)
{
	SaveToFileTitleMonsterInfo(file);
	SaveToFileZoneInfo(file);
	SaveToFileNPCInfo(file);
	SaveToFilePotalInfoInfo(file);
	SaveToFileListMonsterInfo(file);
	SaveToFileQuestInfo(file);
}



void	MGPS_INFO::LoadFromFile(ivfstream& file)
{
	LoadFromFileTitleMonsterInfo(file);
	LoadFromFileZoneInfo(file);
	LoadFromFileNPCInfoInit(file);
	LoadFromFilePotalInfoInit(file);
	LoadFromFileMonsterInfoInit(file);	
	LoadFromFileQuestInfoInit(file);	
}




void	MGPS_INFO::SaveToFileTitleMonsterInfo(std::ofstream& file)
{
	m_dwTitleMonster_Info_Count	= m_SGps_TitleMonster_Info.size();
	file.write((const char*)&m_dwTitleMonster_Info_Count, sizeof(m_dwTitleMonster_Info_Count));

	for(DWORD i = 0; i < m_dwTitleMonster_Info_Count; i++)
	{

		file.write((const char*)&m_SGps_TitleMonster_Info[i].wZoneID,				sizeof(m_SGps_TitleMonster_Info[i].wZoneID));

		m_SGps_TitleMonster_Info[i].strZoneName.SaveToFile(file);

		file.write((const char*)&m_SGps_TitleMonster_Info[i].wMinMonsterLv,			sizeof(m_SGps_TitleMonster_Info[i].wMinMonsterLv));
		file.write((const char*)&m_SGps_TitleMonster_Info[i].wMaxMonsterLv,			sizeof(m_SGps_TitleMonster_Info[i].wMaxMonsterLv));
	}
}




void	MGPS_INFO::SaveToFileZoneInfo(std::ofstream& file)
{
	m_dwZone_Info_Count	= m_SGps_Zone_Info.size();
	file.write((const char*)&m_dwZone_Info_Count, sizeof(m_dwZone_Info_Count));

	for(DWORD i = 0; i < m_dwZone_Info_Count; i++)
	{

		m_SGps_Zone_Info[i].strGpsTitle_ZoneName.SaveToFile(file);

		file.write((const char*)&m_SGps_Zone_Info[i].wZoneID,							sizeof(m_SGps_Zone_Info[i].wZoneID));
	}
}




void	MGPS_INFO::SaveToFileNPCInfo(std::ofstream& file)
{
	m_dwNpc_Info_Count	= m_SGps_Npc_Info.size();
	file.write((const char*)&m_dwNpc_Info_Count, sizeof(m_dwNpc_Info_Count));

	for(DWORD i = 0; i < m_dwNpc_Info_Count; i++)
	{

		file.write((const char*)&m_SGps_Npc_Info[i].wPos_X,						sizeof(m_SGps_Npc_Info[i].wPos_X));
		file.write((const char*)&m_SGps_Npc_Info[i].wPos_Y,						sizeof(m_SGps_Npc_Info[i].wPos_Y));
		
		m_SGps_Npc_Info[i].strNpcName.SaveToFile(file);
		m_SGps_Npc_Info[i].strNpcExplanation.SaveToFile(file);

		file.write((const char*)&m_SGps_Npc_Info[i].wSpkIndex,					sizeof(m_SGps_Npc_Info[i].wSpkIndex));
		file.write((const char*)&m_SGps_Npc_Info[i].wZoneID,					sizeof(m_SGps_Npc_Info[i].wZoneID));
		file.write((const char*)&m_SGps_Npc_Info[i].wMapWidth,					sizeof(m_SGps_Npc_Info[i].wMapWidth));
		file.write((const char*)&m_SGps_Npc_Info[i].wMapHeight,					sizeof(m_SGps_Npc_Info[i].wMapHeight));
	}
}




void	MGPS_INFO::SaveToFilePotalInfoInfo(std::ofstream& file)
{
	m_dwPotal_Info_Count	= m_SGps_Potal_Info.size();
	file.write((const char*)&m_dwPotal_Info_Count, sizeof(m_dwPotal_Info_Count));

	for(DWORD i = 0; i < m_dwPotal_Info_Count; i++)
	{

		file.write((const char*)&m_SGps_Potal_Info[i].wPos_X,				sizeof(m_SGps_Potal_Info[i].wPos_X));
		file.write((const char*)&m_SGps_Potal_Info[i].wPos_Y,				sizeof(m_SGps_Potal_Info[i].wPos_Y));
		
		m_SGps_Potal_Info[i].strPotalName.SaveToFile(file);
		
		file.write((const char*)&m_SGps_Potal_Info[i].wSpkIndex,			sizeof(m_SGps_Potal_Info[i].wSpkIndex));
		file.write((const char*)&m_SGps_Potal_Info[i].wZoneID,				sizeof(m_SGps_Potal_Info[i].wZoneID));
		file.write((const char*)&m_SGps_Potal_Info[i].wMapWidth,			sizeof(m_SGps_Potal_Info[i].wMapWidth));
		file.write((const char*)&m_SGps_Potal_Info[i].wMapHeight,			sizeof(m_SGps_Potal_Info[i].wMapHeight));
	}
}




void	MGPS_INFO::SaveToFileListMonsterInfo(std::ofstream& file)
{
	m_dwListMonster_Info_Count	= m_SGps_ListMonster_Info.size();
	file.write((const char*)&m_dwListMonster_Info_Count, sizeof(m_dwListMonster_Info_Count));

	for(DWORD i = 0; i < m_dwListMonster_Info_Count; i++)
	{

		file.write((const char*)&m_SGps_ListMonster_Info[i].wZoneID,				sizeof(m_SGps_ListMonster_Info[i].wZoneID));

		m_SGps_ListMonster_Info[i].strZoneName.SaveToFile(file);
		m_SGps_ListMonster_Info[i].strMonsterLv.SaveToFile(file);

		file.write((const char*)&m_SGps_ListMonster_Info[i].wExplanationCount,		sizeof(m_SGps_ListMonster_Info[i].wExplanationCount));

		for(DWORD j = 0; j < m_SGps_ListMonster_Info[i].wExplanationCount; j++)
		{
			m_SGps_ListMonster_Info[i].strExplanation[j].SaveToFile(file);
		}

		file.write((const char*)&m_SGps_ListMonster_Info[i].wMinMonsterLv,		sizeof(m_SGps_ListMonster_Info[i].wMinMonsterLv));
		file.write((const char*)&m_SGps_ListMonster_Info[i].wMaxMonsterLv,		sizeof(m_SGps_ListMonster_Info[i].wMaxMonsterLv));
	}
}

void	MGPS_INFO::SaveToFileQuestInfo(std::ofstream& file)
{
}


void	MGPS_INFO::LoadFromFileTitleMonsterInfo(ivfstream& file)
{
	SGPS_MONSTER_INFO	SGps_TitleMonster_Info;
	SGps_TitleMonster_Info.Realease();

	m_SGps_TitleMonster_Info.clear();
	m_SGps_TitleMonster_Info.resize(0);

	m_dwTitleMonster_Info_Count	= 0;
	file.read((char*)&m_dwTitleMonster_Info_Count, sizeof(m_dwTitleMonster_Info_Count));

	for(DWORD i = 0; i < m_dwTitleMonster_Info_Count; i++)
	{

		file.read((char*)&SGps_TitleMonster_Info.wZoneID,				sizeof(SGps_TitleMonster_Info.wZoneID));
		
		SGps_TitleMonster_Info.strZoneName.LoadFromFile(file);		

		file.read((char*)&SGps_TitleMonster_Info.wMinMonsterLv,			sizeof(SGps_TitleMonster_Info.wMinMonsterLv));
		file.read((char*)&SGps_TitleMonster_Info.wMaxMonsterLv,			sizeof(SGps_TitleMonster_Info.wMaxMonsterLv));

		m_SGps_TitleMonster_Info.push_back(SGps_TitleMonster_Info);
		SGps_TitleMonster_Info.Realease();
	}

}

void	MGPS_INFO::LoadFromFileZoneInfo(ivfstream& file)
{
	SGPS_ZONE_INFO	SGps_Zone_Info;
	SGps_Zone_Info.Realease();
	
	m_SGps_Zone_Info.clear();
	m_SGps_Zone_Info.resize(0);

	m_dwZone_Info_Count	= 0;
	file.read((char*)&m_dwZone_Info_Count, sizeof(m_dwZone_Info_Count));

	for(DWORD i = 0; i < m_dwZone_Info_Count; i++)
	{

		SGps_Zone_Info.strGpsTitle_ZoneName.LoadFromFile(file);
		
		file.read((char*)&SGps_Zone_Info.wZoneID,						sizeof(SGps_Zone_Info.wZoneID));

		m_SGps_Zone_Info.push_back(SGps_Zone_Info);
		SGps_Zone_Info.Realease();
	}
}

void	MGPS_INFO::LoadFromFileNPCInfoInit(ivfstream& file)
{
	SGPS_NPC_INFO	SGps_Npc_Info;
	SGps_Npc_Info.Realease();

	m_SGps_Npc_Info.clear();
	m_SGps_Npc_Info.resize(0);

	m_dwNpc_Info_Count	= 0;
	file.read((char*)&m_dwNpc_Info_Count, sizeof(m_dwNpc_Info_Count));

	for(DWORD i = 0; i < m_dwNpc_Info_Count; i++)
	{

		file.read((char*)&SGps_Npc_Info.wPos_X,						sizeof(SGps_Npc_Info.wPos_X));
		file.read((char*)&SGps_Npc_Info.wPos_Y,						sizeof(SGps_Npc_Info.wPos_Y));

		SGps_Npc_Info.strNpcName.LoadFromFile(file);

		SGps_Npc_Info.strNpcExplanation.LoadFromFile(file);

		file.read((char*)&SGps_Npc_Info.wSpkIndex,					sizeof(SGps_Npc_Info.wSpkIndex));
		file.read((char*)&SGps_Npc_Info.wZoneID,					sizeof(SGps_Npc_Info.wZoneID));
		file.read((char*)&SGps_Npc_Info.wMapWidth,					sizeof(SGps_Npc_Info.wMapWidth));
		file.read((char*)&SGps_Npc_Info.wMapHeight,					sizeof(SGps_Npc_Info.wMapHeight));
		
		m_SGps_Npc_Info.push_back(SGps_Npc_Info);
		SGps_Npc_Info.Realease();
	}
}

void	MGPS_INFO::LoadFromFilePotalInfoInit(ivfstream& file)
{
	SGPS_POTAL_INFO SGps_Potal_Info;
	SGps_Potal_Info.Realease();	

	m_SGps_Potal_Info.clear();
	m_SGps_Potal_Info.resize(0);

	m_dwPotal_Info_Count	= 0;
	file.read((char*)&m_dwPotal_Info_Count, sizeof(m_dwPotal_Info_Count));

	for(DWORD i = 0; i < m_dwPotal_Info_Count; i++)
	{

		file.read((char*)&SGps_Potal_Info.wPos_X,				sizeof(SGps_Potal_Info.wPos_X));
		file.read((char*)&SGps_Potal_Info.wPos_Y,				sizeof(SGps_Potal_Info.wPos_Y));

		SGps_Potal_Info.strPotalName.LoadFromFile(file);

		file.read((char*)&SGps_Potal_Info.wSpkIndex,			sizeof(SGps_Potal_Info.wSpkIndex));
		file.read((char*)&SGps_Potal_Info.wZoneID,				sizeof(SGps_Potal_Info.wZoneID));
		file.read((char*)&SGps_Potal_Info.wMapWidth,			sizeof(SGps_Potal_Info.wMapWidth));
		file.read((char*)&SGps_Potal_Info.wMapHeight,			sizeof(SGps_Potal_Info.wMapHeight));

		m_SGps_Potal_Info.push_back(SGps_Potal_Info);
		SGps_Potal_Info.Realease();	
	}
}

void	MGPS_INFO::LoadFromFileMonsterInfoInit(ivfstream& file)
{
	SGPS_MONSTER_INFO SGps_ListMonster_Info;
	SGps_ListMonster_Info.Realease();

	m_SGps_ListMonster_Info.clear();
	m_SGps_ListMonster_Info.resize(0);
	
	m_dwListMonster_Info_Count	= 0;
	file.read((char*)&m_dwListMonster_Info_Count, sizeof(m_dwListMonster_Info_Count));
	
	m_SGps_ListMonster_Info.resize(m_dwListMonster_Info_Count, SGps_ListMonster_Info);
	std::vector<SGPS_MONSTER_INFO>::iterator ipos = m_SGps_ListMonster_Info.begin();
	std::vector<SGPS_MONSTER_INFO>::iterator iposEnd = m_SGps_ListMonster_Info.end();

	for(int i = 0; ipos != iposEnd; ipos++, ++i)
	{
		file.read((char*)&ipos->wZoneID,					sizeof(ipos->wZoneID));

		ipos->strZoneName.LoadFromFile(file);
		ipos->strMonsterLv.LoadFromFile(file);
		
		file.read((char*)&ipos->wExplanationCount,			sizeof(ipos->wExplanationCount));

		for(DWORD j = 0; j < ipos->wExplanationCount; j++)
		{
			ipos->strExplanation[j].LoadFromFile(file);
		}
		
		file.read((char*)&ipos->wMinMonsterLv,		sizeof(ipos->wMinMonsterLv));
		file.read((char*)&ipos->wMaxMonsterLv,		sizeof(ipos->wMaxMonsterLv));
	}
	
}

void	MGPS_INFO::LoadFromFileQuestInfoInit(ivfstream& file)
{
	
}




std::vector<SGPS_MONSTER_INFO>	MGPS_INFO::GetTitleMonsterInfo()
{
	return m_SGps_TitleMonster_Info;
}


std::vector<SGPS_ZONE_INFO>		MGPS_INFO::GetZoneInfo()
{
	return m_SGps_Zone_Info;
}


std::vector<SGPS_NPC_INFO>		MGPS_INFO::GetNpcInfo()
{
	return m_SGps_Npc_Info;
}


std::vector<SGPS_POTAL_INFO>	MGPS_INFO::GetPotalInfo()
{
	return m_SGps_Potal_Info;
}


std::vector<SGPS_MONSTER_INFO>	MGPS_INFO::GetListMonsterInfo()
{
	return m_SGps_ListMonster_Info;
}

std::vector<SGPS_QUEST_INFO>	MGPS_INFO::GetQuestInfo()
{
	return m_SGps_Quest_Info;
}





DWORD	MGPS_INFO::GetTitleMonsterInfoSize()
{
	return m_dwTitleMonster_Info_Count;
}

DWORD	MGPS_INFO::GetZoneInfoSize()
{
	return m_dwZone_Info_Count;
}

DWORD	MGPS_INFO::GetNpcInfoSize()
{
	return m_dwNpc_Info_Count;
}

DWORD	MGPS_INFO::GetPotalInfoSize()
{
	return m_dwPotal_Info_Count;
}

DWORD	MGPS_INFO::GetListMonsterInfoSize()
{
	return m_dwListMonster_Info_Count;
}

DWORD	MGPS_INFO::GetQuestInfoSize()
{
	return m_dwQuest_Info_Count;
}

void	MGPS_INFO::Realease()
{
	// m_SGps_TitleMonster_Info
	
	std::vector<SGPS_MONSTER_INFO>::iterator subitr1 = m_SGps_TitleMonster_Info.begin();

    while(subitr1 != m_SGps_TitleMonster_Info.end())
    {
		SGPS_MONSTER_INFO TempMission1 = (*subitr1);
		TempMission1.Realease();
		subitr1++;
	}

	m_SGps_TitleMonster_Info.clear();
	m_SGps_TitleMonster_Info.resize(0);

	// m_SGps_Zone_Info

	std::vector<SGPS_ZONE_INFO>::iterator subitr2 = m_SGps_Zone_Info.begin();

    while(subitr2 != m_SGps_Zone_Info.end())
    {
		SGPS_ZONE_INFO TempMission2 = (*subitr2);
		TempMission2.Realease();
		subitr2++;
	}

	m_SGps_Zone_Info.clear();
	m_SGps_Zone_Info.resize(0);
	
	// m_SGps_Npc_Info

	std::vector<SGPS_NPC_INFO>::iterator subitr3 = m_SGps_Npc_Info.begin();

    while(subitr3 != m_SGps_Npc_Info.end())
    {
		SGPS_NPC_INFO TempMission3 = (*subitr3);
		TempMission3.Realease();
		subitr3++;
	}

	m_SGps_Npc_Info.clear();
	m_SGps_Npc_Info.resize(0);
	
	// m_SGps_Potal_Info

	std::vector<SGPS_POTAL_INFO>::iterator subitr4 = m_SGps_Potal_Info.begin();

    while(subitr4 != m_SGps_Potal_Info.end())
    {
		SGPS_POTAL_INFO TempMission4 = (*subitr4);
		TempMission4.Realease();
		subitr4++;
	}

	m_SGps_Potal_Info.clear();
	m_SGps_Potal_Info.resize(0);
	
	// m_SGps_ListMonster_Info

	std::vector<SGPS_MONSTER_INFO>::iterator subitr5 = m_SGps_ListMonster_Info.begin();

    while(subitr5 != m_SGps_ListMonster_Info.end())
    {
		SGPS_MONSTER_INFO TempMission5 = (*subitr5);
		TempMission5.Realease();
		subitr5++;
	}

	m_SGps_ListMonster_Info.clear();
	m_SGps_ListMonster_Info.resize(0);

	// m_SGps_Quest_Info

	std::vector<SGPS_QUEST_INFO>::iterator subitr6 = m_SGps_Quest_Info.begin();

    while(subitr6 != m_SGps_Quest_Info.end())
    {
		SGPS_QUEST_INFO TempMission6 = (*subitr6);
		TempMission6.Realease();
		subitr6++;
	}

	m_SGps_Quest_Info.clear();
	m_SGps_Quest_Info.resize(0);
}


#endif //__GPS_ADD
