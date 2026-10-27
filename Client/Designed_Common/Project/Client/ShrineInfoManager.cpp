
#include "Client_PCH.h"
#include "ShrineInfoManager.h"
#include "RarFile.h"
#include "crash_trace_tool.h"
#ifdef __GAME_CLIENT__
#include "Properties.h"
#include "Client.h"
#else
#include "VS_UI_filepath.h"
#endif

RegenTowerInfoManager *g_pRegenTowerInfoManager = NULL;

void RegenTowerInfo::LoadFromLine(char *szLine)
{
	if( szLine == NULL )
		return;

	sscanf( szLine,"%d %d %d %d",&num, &zoneID,&x,&y);
	owner = -1;
}

RegenTowerInfoManager::RegenTowerInfoManager()
{
}

bool RegenTowerInfoManager::LoadRegenTowerInfo()
{
	CT_Done("RegenTower: fn-enter");

#ifdef __GAME_CLIENT__
	std::string _rti_path = g_pFileDef->getProperty("FILE_INFO_CLIENTCONFIG").empty() ? std::string("RegenTowerPosition.inf") : g_pFileDef->getProperty("FILE_REGEN_TOWER_INFO");
	if( _rti_path.find('\\') == std::string::npos && _rti_path.find('/') == std::string::npos )
		_rti_path = std::string("Data\\Info\\") + _rti_path;
	CT_Done("RegenTower: Open path-resolved '%s'", _rti_path.c_str());
	ivfstream infoFile;
	bool _rti_rc = FileOpenBinary( _rti_path.c_str(), infoFile );
	CT_Done("RegenTower: Open RETURNED rc=%d", _rti_rc ? 1 : 0);
#else
	CRarFile rarfile;
	rarfile.SetRAR( "data\\info\\infodata.rpk", RPK_PASSWORD );
	rarfile.Open( "RegenTowerPosition.inf" );
#endif

#ifdef __GAME_CLIENT__
	CT_Done("RegenTower: IsSet check = %d", infoFile.is_open() ? 1 : 0);
	if( !infoFile.is_open() )
		return false;
#else
	CT_Done("RegenTower: IsSet check = %d", rarfile.IsSet() ? 1 : 0);
	if( !rarfile.IsSet() )
		return false;
#endif

	char szLine[512];
	bool bInit = false;
	
	int lineCount = 0;
	CT_Done("RegenTower: entering GetString loop");
#ifdef __GAME_CLIENT__
	while( infoFile.getline( szLine, 512 ) )
#else
	while( rarfile.GetString( szLine, 512 ) )
#endif
	{
		lineCount++;
		if (lineCount == 1) {
		}

		if( szLine[0] == ';' )
			continue;

		if( szLine[0] == '*' && bInit == false )
		{
			int n;
			sscanf(szLine+1,"%d",&n);

			CT_Done("RegenTower: Init(%d) PRE", n);
			Init( n );
			CT_Done("RegenTower: Init POST GetSize=%d", GetSize());
			bInit = true;
			continue;
		}
		
		if( strlen(szLine) <= 0 )
			continue;
		
		int num;
		sscanf(szLine,"%d",&num);

		if( num >= 0 && num < GetSize() )
		{
			CT_Done("RegenTower: LoadFromLine[%d]", num);
			m_pTypeInfo[num].LoadFromLine( szLine );
		}
	}
	CT_Done("RegenTower: Release PRE (lineCount=%d)", lineCount);
#ifdef __GAME_CLIENT__
	infoFile.close();
#else
	rarfile.Release();
#endif
	CT_Done("RegenTower: fn-exit OK");
	return true;
}
