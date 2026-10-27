/*-----------------------------------------------------------------------------

	PhoenixHudLayout.cpp

	2026-09-03 plan F3 — per-resolution HUD konum defteri.
	Detay: PhoenixHudLayout.h

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"
#include "PhoenixHudLayout.h"
#include "crash_trace_tool.h"

#include <cstdio>
#include <map>
#include <string>
#include <utility>

namespace
{
	// Client CWD = GameData (OptionRenewal.set ile ayni pattern)
	const char* kHudLayoutPath = "UserSet\\PhoenixHudLayout.set";

	typedef std::map<std::string, std::pair<int, int> > LayoutMap;
	LayoutMap	g_Layout;
	bool		g_bLoaded = false;

	std::string MakeKey(int resW, int resH, int windowId)
	{
		char buf[64];
		sprintf(buf, "%dx%d.%d", resW, resH, windowId);
		return std::string(buf);
	}

	void LoadIfNeeded()
	{
		if (g_bLoaded)
			return;
		g_bLoaded = true;

		FILE* fp = fopen(kHudLayoutPath, "rt");
		if (fp == NULL)
		{
			// Ilk calistirma: dosya henuz yok — bos defter normal durum.
			CT_Done("HudLayout: '%s' yok — bos defterle basliyor", kHudLayoutPath);
			return;
		}

		char line[256];
		while (fgets(line, sizeof(line), fp) != NULL)
		{
			if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
				continue;

			int rw = 0, rh = 0, id = 0, px = 0, py = 0;
			if (sscanf(line, "%dx%d.%d=%d,%d", &rw, &rh, &id, &px, &py) == 5)
				g_Layout[MakeKey(rw, rh, id)] = std::make_pair(px, py);
			else
				CT_Done("HudLayout: parse edilemeyen satir atlandi: '%s'", line);
		}
		fclose(fp);
		CT_Done("HudLayout: %d kayit yuklendi (%s)", (int)g_Layout.size(), kHudLayoutPath);
	}

	void SaveAll()
	{
		FILE* fp = fopen(kHudLayoutPath, "wt");
		if (fp == NULL)
		{
			CT_Done("HudLayout: '%s' YAZILAMADI — kayit kaybolabilir", kHudLayoutPath);
			return;
		}
		fprintf(fp, "# Phoenix per-resolution HUD yerlesimi — format: WxH.windowId=x,y\n");
		fprintf(fp, "# windowId = C_VS_UI_WINDOW_MANAGER::WINDOW_ID enum degeri\n");
		for (LayoutMap::const_iterator it = g_Layout.begin(); it != g_Layout.end(); ++it)
			fprintf(fp, "%s=%d,%d\n", it->first.c_str(), it->second.first, it->second.second);
		fclose(fp);
	}
}

bool Phoenix_HudLayout_Get(int resW, int resH, int windowId, int* outX, int* outY)
{
	LoadIfNeeded();

	LayoutMap::const_iterator it = g_Layout.find(MakeKey(resW, resH, windowId));
	if (it == g_Layout.end())
		return false;

	if (outX) *outX = it->second.first;
	if (outY) *outY = it->second.second;
	return true;
}

void Phoenix_HudLayout_Set(int resW, int resH, int windowId, int posX, int posY)
{
	LoadIfNeeded();
	g_Layout[MakeKey(resW, resH, windowId)] = std::make_pair(posX, posY);
	SaveAll();
}
