
#ifndef	__MGPSTABLE_H__
#define	__MGPSTABLE_H__

//#include <Windows.h>
//#include <list>
//#include <map>
//#include <string>

#if __CONTENTS(__GPS_ADD)
#include <vector>
#include "vfstream.h"
#include "MString.h"
#include "GpsDef.h"

struct SGPS_MONSTER_INFO
{
	SGPS_MONSTER_INFO() : wZoneID(0), wMinMonsterLv(0), wMaxMonsterLv(0), wExplanationCount(0){}

	WORD	wZoneID;
	MString	strZoneName;
	MString strMonsterLv;
	MString strExplanation[20];
	WORD	wMinMonsterLv;
	WORD	wMaxMonsterLv;
	WORD	wExplanationCount;

	void	Realease()
	{
			wZoneID	= 0;
			strZoneName.Release();
			strMonsterLv.Release();

			for(int i = 0; i < 20; i++)
			{
				strExplanation[i].Release();
			}

			wMinMonsterLv		= 0;
			wMaxMonsterLv		= 0;
			wExplanationCount	= 0;
	}
};

struct SGPS_ZONE_INFO
{
	SGPS_ZONE_INFO() : wZoneID(0) {}

	MString strGpsTitle_ZoneName;
	WORD	wZoneID;
	
	void	Realease()
	{
		strGpsTitle_ZoneName.Release();
		wZoneID = 0;
	}
};

struct SGPS_NPC_INFO
{
	SGPS_NPC_INFO() : wPos_X(0), wPos_Y(0), wSpkIndex(0), wZoneID(0), wMapWidth(0), wMapHeight(0){}

	WORD	wPos_X;
	WORD	wPos_Y;
	MString strNpcName;
	MString strNpcExplanation;	//NPC 설명
	WORD	wSpkIndex;
	WORD	wZoneID;
	WORD	wMapWidth;
	WORD	wMapHeight;

	void	Realease()
	{
			wPos_X				= 0;
			wPos_Y				= 0;

			strNpcName.Release();
			strNpcExplanation.Release();	//NPC 설명

			wSpkIndex				= 0;
			wZoneID					= 0;
			wMapWidth				= 0;
			wMapHeight				= 0;
	}
};

struct SGPS_POTAL_INFO
{
	SGPS_POTAL_INFO() : wPos_X(0), wPos_Y(0), wSpkIndex(0), wZoneID(0), wMapWidth(0), wMapHeight(0){}

	WORD	wPos_X;
	WORD	wPos_Y;
	MString strPotalName;
	WORD	wSpkIndex;
	WORD	wZoneID;
	WORD	wMapWidth;
	WORD	wMapHeight;	

	void	Realease()
	{
			wPos_X				= 0;
			wPos_Y				= 0;
			strPotalName.Release();
			wSpkIndex			= 0;
			wZoneID				= 0;
			wMapWidth			= 0;
			wMapHeight			= 0;
	}
};

struct SGPS_QUEST_INFO
{
	SGPS_QUEST_INFO() : wPos_X(0), wPos_Y(0), wSpkIndex(0), wZoneID(0), wMapWidth(0),  wMapHeight(0){}

	WORD	wPos_X;
	WORD	wPos_Y;
	WORD	wSpkIndex;
	WORD	wZoneID;
	WORD	wMapWidth;
	WORD	wMapHeight;	

	MString strQuestName;
	MString strQuestExplanation;

	void	Realease()
	{
			wPos_X				= 0;
			wPos_Y				= 0;
			wSpkIndex			= 0;
			wZoneID				= 0;
			wMapWidth			= 0;
			wMapHeight			= 0;

			strQuestName.Release();
			strQuestExplanation.Release();
	}
};


class MGPS_INFO
{
public:
	MGPS_INFO();
	~MGPS_INFO();

	void				GpsTitleMonsterInfoInit();
	void				GpsZoneInfoInit();
	void				GpsNPCInfoInit();
	void				GpsPotalInfoInit();
	void				GpsListMonsterInfoInit();
	void				GpsQuestInfoInit();

	void				SaveToFile(std::ofstream& file);
	void				LoadFromFile(ivfstream& file);

private:
	void				SaveToFileTitleMonsterInfo(std::ofstream& file);
	void				SaveToFileZoneInfo(std::ofstream& file);
	void				SaveToFileNPCInfo(std::ofstream& file);
	void				SaveToFilePotalInfoInfo(std::ofstream& file);
	void				SaveToFileListMonsterInfo(std::ofstream& file);
	void				SaveToFileQuestInfo(std::ofstream& file);

	void				LoadFromFileTitleMonsterInfo(ivfstream& file);
	void				LoadFromFileZoneInfo(ivfstream& file);
	void				LoadFromFileNPCInfoInit(ivfstream& file);
	void				LoadFromFilePotalInfoInit(ivfstream& file);
	void				LoadFromFileMonsterInfoInit(ivfstream& file);
	void				LoadFromFileQuestInfoInit(ivfstream& file);
public:
	std::vector<SGPS_MONSTER_INFO>	GetTitleMonsterInfo();
	std::vector<SGPS_ZONE_INFO>		GetZoneInfo();
	std::vector<SGPS_NPC_INFO>		GetNpcInfo();
	std::vector<SGPS_POTAL_INFO>	GetPotalInfo();
	std::vector<SGPS_MONSTER_INFO>	GetListMonsterInfo();
	std::vector<SGPS_QUEST_INFO>	GetQuestInfo();

	DWORD				GetTitleMonsterInfoSize();
	DWORD				GetZoneInfoSize();
	DWORD				GetNpcInfoSize();
	DWORD				GetPotalInfoSize();
	DWORD				GetListMonsterInfoSize();
	DWORD				GetQuestInfoSize();

	void				Realease();

private:
	std::vector<SGPS_MONSTER_INFO>	m_SGps_TitleMonster_Info;
	std::vector<SGPS_ZONE_INFO>		m_SGps_Zone_Info;
	std::vector<SGPS_NPC_INFO>		m_SGps_Npc_Info;
	std::vector<SGPS_POTAL_INFO>	m_SGps_Potal_Info;
	std::vector<SGPS_MONSTER_INFO>	m_SGps_ListMonster_Info;
	std::vector<SGPS_QUEST_INFO>	m_SGps_Quest_Info;

	DWORD				m_dwTitleMonster_Info_Count;
	DWORD				m_dwZone_Info_Count;
	DWORD				m_dwNpc_Info_Count;
	DWORD				m_dwPotal_Info_Count;
	DWORD				m_dwListMonster_Info_Count;
	DWORD				m_dwQuest_Info_Count;
};

extern MGPS_INFO		*g_pGpsInfoTable;
#endif //__GPS_ADD

#endif	//__MGPSTABLE_H__
