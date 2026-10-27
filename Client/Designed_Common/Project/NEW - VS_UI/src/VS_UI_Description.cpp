// VS_UI_Description.cpp

#include "client_PCH.h"

#include "VS_UI_Description.h"
//#include "VS_UI_GameCommon.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_GameCommon2.h"
#include "VS_UI_PetEnchantRules.h"

#include "VS_UI_Shop.h"
#include "VS_UI_mouse_pointer.h"

#include "MPriceManager.h"
#include "MItemOptionTable.h"
#include "UserOption.h"
#include "ClientConfig.H"

#include "MGameStringTable.H"
#include "MZoneTable.h"
#include "MTimeItemManager.h"
#include "SystemAvailabilities.h"
#include "VS_UI.h"
#include "VS_UI_GameCommon2.h"
#include "UserInformation.h"
#include "MPlayer.h"
#include "RaceType.h"

#ifdef _LIB
#include "ClientFunction.h"
#endif

// FAZ 4 (2026-05-04): ColorSet OOB güvenli erişim — many call sites use unbound
// indices (GetItemNameColorSet/GetUniqueItemColorset/etc.) which can return values
// >= MAX_COLORSET (795). Wrapper returns 0 if OOB instead of crashing.
static inline WORD SafeColorSet(int idx, int grad) {
	if (idx < 0 || idx >= 795 || grad < 0 || grad >= 30) return 0;
	return CIndexSprite::ColorSet[idx][grad];
}

static int PrintNameColorCycleStr(int x, int y, const char *sz_str, PrintInfo &pi, const MItem *p_item)
{
	if (sz_str == NULL || sz_str[0] == '\0' || p_item == NULL)
		return x;
	const int glyphCount = MItem::CountNameGlyphs(sz_str);
	int offset = 0;
	int glyphIndex = 0;
	int cx = x;
	char glyph[3];
	while (sz_str[offset] != '\0')
	{
		const int bytes = MItem::ReadNameGlyph(sz_str, offset, glyph);
		if (bytes <= 0)
			break;
		offset += bytes;
		const COLORREF color = p_item->GetNameColorCycleAtGlyph(glyphIndex, glyphCount);
		cx = g_PrintColorStr(cx, y, glyph, pi, color);
		glyphIndex++;
	}
	return cx;
}

// FAZ 4 (2026-05-06): defansif gameString format wrapper — audit phase 2.
// `sprintf(sz_buf, (*g_pGameStringTable)[ID].GetString(), val)` pattern'i
// 25+ site'da kullanıldı. game string ID OOB veya format bozuksa
// (örn. "%a" hex/float specifier) sprintf garbage hex dump üretir
// (kullanıcının "Requirements: 31. 0:1-a2f0a000000001p-534" gördüğü gibi).
// Bu helper format'ı runtime kontrol eder: "%d" specifier varsa kullan,
// yoksa hardcoded fallback. Bilgi kaybı kabul (label localization gider)
// ama crash/garbage yok.
static inline void DescSafeSprintf1d(char* buf, size_t buflen, int sid, int arg, const char* fallback_fmt) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL) {
		const char* pct = strchr(fmt, '%');
		if (pct != NULL && pct[1] == 'd') {
			snprintf(buf, buflen, fmt, arg);
			return;
		}
	}
	snprintf(buf, buflen, fallback_fmt, arg);
}

// İki string arg variant (örn. ELEMENTAL render: "%s %s" formatı).
static inline void DescSafeSprintf2s(char* buf, size_t buflen, int sid, const char* arg1, const char* arg2, const char* fallback_fmt) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL) {
		const char* pct1 = strchr(fmt, '%');
		const char* pct2 = (pct1 != NULL) ? strchr(pct1 + 1, '%') : NULL;
		if (pct1 != NULL && pct1[1] == 's' && pct2 != NULL && pct2[1] == 's') {
			snprintf(buf, buflen, fmt, arg1 ? arg1 : "", arg2 ? arg2 : "");
			return;
		}
	}
	snprintf(buf, buflen, fallback_fmt, arg1 ? arg1 : "", arg2 ? arg2 : "");
}

// İki int arg variant (örn. ZONEINFO_XY render: "(%d, %d)" formatı).
static inline void DescSafeSprintf2d(char* buf, size_t buflen, int sid, int arg1, int arg2, const char* fallback_fmt) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL) {
		const char* pct1 = strchr(fmt, '%');
		const char* pct2 = (pct1 != NULL) ? strchr(pct1 + 1, '%') : NULL;
		if (pct1 != NULL && pct1[1] == 'd' && pct2 != NULL && pct2[1] == 'd') {
			snprintf(buf, buflen, fmt, arg1, arg2);
			return;
		}
	}
	snprintf(buf, buflen, fallback_fmt, arg1, arg2);
}

// Sıfır arg variant (sadece string, format spec yok ya da hepsi literal).
static inline void DescSafeSprintf0(char* buf, size_t buflen, int sid, const char* fallback) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL && strchr(fmt, '%') == NULL) {
		// Format spec yok, güvenli kopya.
		snprintf(buf, buflen, "%s", fmt);
	} else {
		snprintf(buf, buflen, "%s", fallback);
	}
}

// Tek string arg variant (örn. SKILL_LEVEL_FOR_STRING: "%s" formatı).
static inline void DescSafeSprintf1s(char* buf, size_t buflen, int sid, const char* arg, const char* fallback_fmt) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL) {
		const char* pct = strchr(fmt, '%');
		if (pct != NULL && pct[1] == 's') {
			snprintf(buf, buflen, fmt, arg ? arg : "");
			return;
		}
	}
	snprintf(buf, buflen, fallback_fmt, arg ? arg : "");
}

// String + int arg variant (örn. ELEMENTAL_LEVEL: "%s %d" formatı).
static inline void DescSafeSprintf1s1d(char* buf, size_t buflen, int sid, const char* arg1, int arg2, const char* fallback_fmt) {
	const char* fmt = (g_pGameStringTable != NULL && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
	                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
	if (fmt != NULL) {
		const char* pct1 = strchr(fmt, '%');
		const char* pct2 = (pct1 != NULL) ? strchr(pct1 + 1, '%') : NULL;
		if (pct1 != NULL && pct1[1] == 's' && pct2 != NULL && pct2[1] == 'd') {
			snprintf(buf, buflen, fmt, arg1 ? arg1 : "", arg2);
			return;
		}
	}
	snprintf(buf, buflen, fallback_fmt, arg1 ? arg1 : "", arg2);
}

static const char* DescGetItemGradeSuffix(int grade)
{
	static const char* kFallbackGradeSuffix[11] = {
		"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
	};
	static char token[16];

	if (grade < 1 || grade > 10) return "";

	const char* table = (g_pGameStringTable != NULL &&
		UI_STRING_MESSAGE_ITEM_GRADE >= 0 &&
		UI_STRING_MESSAGE_ITEM_GRADE < (int)(*g_pGameStringTable).GetSize())
		? (*g_pGameStringTable)[UI_STRING_MESSAGE_ITEM_GRADE].GetString()
		: NULL;

	if (table != NULL && strchr(table, '|') != NULL) {
		const char* start = table;
		for (int i = 1; i < grade; ++i) {
			const char* next = strchr(start, '|');
			if (next == NULL) return kFallbackGradeSuffix[grade];
			start = next + 1;
		}

		const char* end = strchr(start, '|');
		if (end == NULL) end = start + strlen(start);

		size_t len = (size_t)(end - start);
		if (len == 0) return kFallbackGradeSuffix[grade];
		if (len >= sizeof(token)) len = sizeof(token) - 1;

		memcpy(token, start, len);
		token[len] = '\0';
		return token;
	}

	return kFallbackGradeSuffix[grade];
}

static inline void DescAppendItemGradeSuffix(char* dest, size_t destSize, int grade)
{
	if (dest == NULL || destSize == 0) return;

	const char* suffix = DescGetItemGradeSuffix(grade);
	if (suffix == NULL || suffix[0] == '\0') return;

	const size_t cur = strlen(dest);
	if (cur >= destSize - 1) return;

	strncat(dest, suffix, destSize - 1 - cur);
}

enum
{
	SMALL_FONT_Y_GAP = 16,
	DEFAULT_BOX_WIDTH = 220,
	SIDE_GAP = 14,
	SIDE_GAP_HALF = 7,
	NORMAL_FONT_Y_GAP = 18,
	NAME_STRING_LEN = 100, // 100 ���ϰ���?
	VALUE_GAP = 5,
};

static inline bool DescIsEscapedNewline(const char* text)
{
	return text != NULL && text[0] == '\\' && (text[1] == 'n' || text[1] == 'N');
}

static void DescReadFormattedDescriptionLine(const char*& text, char* out, size_t outSize)
{
	if (out == NULL || outSize == 0)
		return;

	size_t outLen = 0;
	out[0] = '\0';

	if (text == NULL)
		return;

	while (*text != '\0')
	{
		if (*text == '\r')
		{
			++text;
			if (*text == '\n')
				++text;
			break;
		}
		if (*text == '\n')
		{
			++text;
			break;
		}
		if (DescIsEscapedNewline(text))
		{
			text += 2;
			break;
		}

		if (outLen + 1 < outSize)
			out[outLen++] = *text;
		++text;
	}

	out[outLen] = '\0';
}

static int DescCountFormattedDescriptionLines(const char* text)
{
	if (text == NULL || text[0] == '\0')
		return 0;

	int lineCount = 1;
	for (const char* p = text; *p != '\0'; ++p)
	{
		if (*p == '\r')
		{
			if (p[1] == '\n')
				++p;
			++lineCount;
			continue;
		}
		if (*p == '\n')
		{
			++lineCount;
			continue;
		}
		if (DescIsEscapedNewline(p))
		{
			++lineCount;
			++p;
		}
	}

	return lineCount;
}

static int DescGetFormattedDescriptionMaxWidth(const char* text, HFONT hfont)
{
	if (text == NULL || text[0] == '\0')
		return 0;

	int maxWidth = 0;
	const char* scan = text;
	char line[1024];
	while (*scan != '\0')
	{
		DescReadFormattedDescriptionLine(scan, line, sizeof(line));
		if (line[0] != '\0')
			maxWidth = max(maxWidth, g_GetStringWidth(line, hfont));
	}

	return maxWidth;
}

static int DescPrintFormattedDescriptionLines(int x, int y, const char* text, PrintInfo& pi, COLORREF color)
{
	if (text == NULL || text[0] == '\0')
		return 0;

	int lineCount = 0;
	const char* scan = text;
	char line[1024];
	while (*scan != '\0')
	{
		DescReadFormattedDescriptionLine(scan, line, sizeof(line));
		if (line[0] != '\0')
			g_PrintColorStr(x, y + lineCount * SMALL_FONT_Y_GAP, line, pi, color);
		++lineCount;
	}

	return lineCount;
}

#define ITEM_NAME_NORMAL_COLOR			RGB_WHITE
#define ITEM_NAME_OPTION_COLOR			RGB_YELLOW
#define ITEM_ENAME_NORMAL_COLOR			RGB(200,200,200)
#define ITEM_ENAME_OPTION_COLOR			RGB(200,200,100)
#define ITEM_NAME_NORMAL_SHADE			RGB(100,100,100)
#define ITEM_NAME_OPTION_SHADE			RGB(100,100,0)
#define ITEM_ENAME_RARE_COLOR			RGB(15<<3,15<<3,31<<3)
#define ITEM_DESC_RGB					RGB(192, 192, 255)


#define MAX_EQUIP  50 //Ŀǰ���50��
#define MAX_EQUIP_DESCRIP 6 //6������

enum eEquipSet
{
	eEquipSet_Vampire,
	eEquipSet_Slayer,
	eEquipSet_Outer,
	eEquipSet_Vampire_221,
	eEquipSet_Slayer_221,
	eEquipSet_Outer_221,
	eEquipSet_Vampire_231,
	eEquipSet_Slayer_231,
	eEquipSet_Outer_231,
	eEquipSet_Vampire_241,
	eEquipSet_Slayer_241,
	eEquipSet_Outer_241,
	eEquipSet_Vampire_251,
	eEquipSet_Slayer_251,
	eEquipSet_Outer_251,

};

typedef struct  
{
	int itemClass;
	int itemType;
}stEquipUnit;


typedef std::vector<stEquipUnit> VEC_EquipSetList;
typedef VEC_EquipSetList::iterator VEC_EquipSetList_It;

typedef std::map<std::string, int> Map_EquipQuery;
typedef Map_EquipQuery::iterator Map_EquipQuery_It;

typedef std::map<int, std::string> Map_GemDesc;
typedef Map_GemDesc::iterator Map_GemDesc_It;


Map_GemDesc  g_mapGemDesc;  //��ʯ������������


VEC_EquipSetList g_vecEquipSetList[MAX_EQUIP];//��װ��������װ���ļ��ϳ�֮Ϊһ����װ��
Map_EquipQuery g_mapEquipQuery;//��װ��ѯ
std::string g_strEquipDescription[MAX_EQUIP][MAX_EQUIP_DESCRIP];//��װ����
std::string g_strEquipName[MAX_EQUIP];//��װ����

int GetEquipSetID(int itemClass, int itemType)
{
	char buff[125];
	sprintf(buff, "%d,%d", itemClass, itemType);
	Map_EquipQuery_It it = g_mapEquipQuery.find( buff );
	if ( it != g_mapEquipQuery.end() )
		return it->second;

	return -1;
}


void AddEquipToEquipSet(int itemClass, int itemType, eEquipSet eSet)
{
	char buff[125];
	sprintf(buff, "%d,%d", itemClass, itemType);
	std::string strIndex = buff;

	g_mapEquipQuery[strIndex.c_str()] = eSet;

	stEquipUnit eqUnit;
	eqUnit.itemClass = itemClass; eqUnit.itemType = itemType;
	g_vecEquipSetList[eSet].push_back(eqUnit);
}

static bool DescIsItemTableEntryValid(int itemClass, int itemType)
{
	if (g_pItemTable == NULL) return false;
	if (itemClass < 0 || itemClass >= (*g_pItemTable).GetSize()) return false;
	if (itemType < 0 || itemType >= (*g_pItemTable)[itemClass].GetSize()) return false;
	return true;
}

static const char* DescGetItemTableHName(int itemClass, int itemType)
{
	if (!DescIsItemTableEntryValid(itemClass, itemType)) return NULL;

	const char* name = (*g_pItemTable)[itemClass][itemType].HName.GetString();
	return (name != NULL) ? name : "";
}

static void DescFormatMissingEquipSetMember(char* buffer, size_t bufferSize, int itemClass, int itemType)
{
	if (buffer == NULL || bufferSize == 0) return;
	snprintf(buffer, bufferSize, "Missing item %d/%d", itemClass, itemType);
	buffer[bufferSize - 1] = '\0';
}

void InitEquipSetData()
{
	//for ( int i=0; i< 3; i++)
	//{
	//	g_strEquipDescription[eEquipSet_Vampire + i][0] = "2��������ֵ�ܺ�+5";		//set1
	//	g_strEquipDescription[eEquipSet_Vampire + i][1] = "3����������+2000";			  //set2
	//	g_strEquipDescription[eEquipSet_Vampire + i][2] = "4��������/ħ��ɱ����+10";					//set3
	//	g_strEquipDescription[eEquipSet_Vampire + i][3] = "5�����Թ�����ɵ��˺����5%";					  //set4
	//	g_strEquipDescription[eEquipSet_Vampire + i][4] = "6�����ܵ��������˺�����5%";						//set5
	//}

	//for ( int i=0; i< 3; i++)
	//{
	//	g_strEquipDescription[eEquipSet_Vampire_221 + i][0] = "2��������ֵ�ܺ�+10";		//set1
	//	g_strEquipDescription[eEquipSet_Vampire_221 + i][1] = "3����������+3000";			  //set2
	//	g_strEquipDescription[eEquipSet_Vampire_221 + i][2] = "4��������/ħ��ɱ����+20";					//set3
	//	g_strEquipDescription[eEquipSet_Vampire_221 + i][3] = "5�����Թ�����ɵ��˺����10%";					  //set4
	//	g_strEquipDescription[eEquipSet_Vampire_221 + i][4] = "6�����ܵ��������˺�����8%";						//set5
	//}
	//for ( int i=0; i< 3; i++)
	//{
	//	g_strEquipDescription[eEquipSet_Vampire_231 + i][0] = "2��������ֵ�ܺ�+15";		//set1
	//	g_strEquipDescription[eEquipSet_Vampire_231 + i][1] = "3����������+4000";			  //set2
	//	g_strEquipDescription[eEquipSet_Vampire_231 + i][2] = "4��������/ħ��ɱ����+30";					//set3
	//	g_strEquipDescription[eEquipSet_Vampire_231 + i][3] = "5�����Թ�����ɵ��˺����15%";					  //set4
	//	g_strEquipDescription[eEquipSet_Vampire_231 + i][4] = "6�����ܵ��������˺�����10%";						//set5
	//}
	//for ( int i=0; i< 3; i++)
	//{
	//	g_strEquipDescription[eEquipSet_Vampire_241 + i][0] = "2��������ֵ�ܺ�+20";		//set1
	//	g_strEquipDescription[eEquipSet_Vampire_241 + i][1] = "3����������+5000";			  //set2
	//	g_strEquipDescription[eEquipSet_Vampire_241 + i][2] = "4��������/ħ��ɱ����+50";					//set3
	//	g_strEquipDescription[eEquipSet_Vampire_241 + i][3] = "5�����Թ�����ɵ��˺����20%";					  //set4
	//	g_strEquipDescription[eEquipSet_Vampire_241 + i][4] = "6�����ܵ��������˺�����15%";						//set5
	//}
	for ( int i=0; i< 3; i++)
	{
		g_strEquipDescription[eEquipSet_Vampire + i][0] = "2 set: Total ability score +20";		//set1
		g_strEquipDescription[eEquipSet_Vampire + i][1] = "3 set: Health +5000";			  //set2
		g_strEquipDescription[eEquipSet_Vampire + i][2] = "4 set: Physical/Magical Damage +30";					//set3
		g_strEquipDescription[eEquipSet_Vampire + i][3] = "5 set: Increases damage dealt to monsters by 15%";					  //set4
		g_strEquipDescription[eEquipSet_Vampire + i][4] = "6 set: Reduces all damage taken by 10%";						//set5
	}

	for ( int i=0; i< 3; i++)
	{
		g_strEquipDescription[eEquipSet_Vampire_221 + i][0] = "2 set: Total ability score +30";		//set1
		g_strEquipDescription[eEquipSet_Vampire_221 + i][1] = "3 set: Health +6000";			  //set2
		g_strEquipDescription[eEquipSet_Vampire_221 + i][2] = "4 set: Physical/Magical Damage +40";					//set3
		g_strEquipDescription[eEquipSet_Vampire_221 + i][3] = "5 set: Increases damage dealt to monsters by 20%";					  //set4
		g_strEquipDescription[eEquipSet_Vampire_221 + i][4] = "6 set: Reduces all damage taken by 15%";						//set5
	}
	for ( int i=0; i< 3; i++)
	{
		g_strEquipDescription[eEquipSet_Vampire_231 + i][0] = "2 set: Total ability score +40";		//set1
		g_strEquipDescription[eEquipSet_Vampire_231 + i][1] = "3 set: Health +7000";			  //set2
		g_strEquipDescription[eEquipSet_Vampire_231 + i][2] = "4 set: Physical/Magical Damage +50";					//set3
		g_strEquipDescription[eEquipSet_Vampire_231 + i][3] = "5 set: Increases damage dealt to monsters by 25%";					  //set4
		g_strEquipDescription[eEquipSet_Vampire_231 + i][4] = "6 set: Reduces all damage taken by 20%";						//set5
	}
	for ( int i=0; i< 3; i++)
	{
		g_strEquipDescription[eEquipSet_Vampire_241 + i][0] = "2 set: Total ability score +50";		//set1
		g_strEquipDescription[eEquipSet_Vampire_241 + i][1] = "3 set: Health +8000";			  //set2
		g_strEquipDescription[eEquipSet_Vampire_241 + i][2] = "4 set: Physical/Magical Damage +60";					//set3
		g_strEquipDescription[eEquipSet_Vampire_241 + i][3] = "5 set: Increases damage dealt to monsters by 30%";					  //set4
		g_strEquipDescription[eEquipSet_Vampire_241 + i][4] = "6 set: Reduces all damage taken by 25%";						//set5
	}
	for ( int i=0; i< 3; i++)
	{
		g_strEquipDescription[eEquipSet_Vampire_251 + i][0] = "2 set: Total ability score +60";		//set1
		g_strEquipDescription[eEquipSet_Vampire_251 + i][1] = "3 set: Health +9000";			  //set2
		g_strEquipDescription[eEquipSet_Vampire_251 + i][2] = "4 set: Physical/Magical Damage +70";					//set3
		g_strEquipDescription[eEquipSet_Vampire_251 + i][3] = "5 set: Increases damage dealt to monsters by 35%";					  //set4
		g_strEquipDescription[eEquipSet_Vampire_251 + i][4] = "6 set: Reduces all damage taken by 30%";						//set5
	}
	//����
	AddEquipToEquipSet(30, 25, eEquipSet_Vampire);  //��ָ	A
	AddEquipToEquipSet(30, 32, eEquipSet_Vampire);  //��ָ	B
	AddEquipToEquipSet(30, 33, eEquipSet_Vampire);  //��ָ	C
	AddEquipToEquipSet(30, 34, eEquipSet_Vampire);  //��ָ	C
	AddEquipToEquipSet(86, 4,  eEquipSet_Vampire);  //רְ����	 1
	AddEquipToEquipSet(87, 4,  eEquipSet_Vampire);  //רְ���	 1  

	AddEquipToEquipSet(30, 26, eEquipSet_Vampire_221);  //��ָ	A
	AddEquipToEquipSet(30, 35, eEquipSet_Vampire_221);  //��ָ	B
	AddEquipToEquipSet(30, 36, eEquipSet_Vampire_221);  //��ָ	C
	AddEquipToEquipSet(30, 37, eEquipSet_Vampire_221);  //��ָ	D
	AddEquipToEquipSet(86, 5,  eEquipSet_Vampire_221);  //רְ����	 1
	AddEquipToEquipSet(87, 5,  eEquipSet_Vampire_221);  //רְ���	 1  

	AddEquipToEquipSet(30, 27, eEquipSet_Vampire_231);  //��ָ	A
	AddEquipToEquipSet(30, 28, eEquipSet_Vampire_231);  //��ָ	B
	AddEquipToEquipSet(30, 29, eEquipSet_Vampire_231);  //��ָ	C
	AddEquipToEquipSet(30, 30, eEquipSet_Vampire_231);  //��ָ	D
	AddEquipToEquipSet(86, 6,  eEquipSet_Vampire_231);  //רְ����	 1
	AddEquipToEquipSet(87, 6,  eEquipSet_Vampire_231);  //רְ���	 1 

	AddEquipToEquipSet(30, 31, eEquipSet_Vampire_241);  //��ָ	A
	AddEquipToEquipSet(30, 38, eEquipSet_Vampire_241);  //��ָ	B
	AddEquipToEquipSet(30, 39, eEquipSet_Vampire_241);  //��ָ	C
	AddEquipToEquipSet(30, 40, eEquipSet_Vampire_241);  //��ָ	D
	AddEquipToEquipSet(86, 7,  eEquipSet_Vampire_241);  //רְ����	 1
	AddEquipToEquipSet(87, 7,  eEquipSet_Vampire_241);  //רְ���	 1 

	AddEquipToEquipSet(30, 41, eEquipSet_Vampire_251);  //��ָ	A
	AddEquipToEquipSet(30, 42, eEquipSet_Vampire_251);  //��ָ	B
	AddEquipToEquipSet(30, 43, eEquipSet_Vampire_251);  //��ָ	C
	AddEquipToEquipSet(30, 44, eEquipSet_Vampire_251);  //��ָ	D
	AddEquipToEquipSet(86, 8,  eEquipSet_Vampire_251);  //רְ����	 1
	AddEquipToEquipSet(87, 8,  eEquipSet_Vampire_251);  //רְ���	 1 

	//����
	AddEquipToEquipSet(8,  25, eEquipSet_Slayer);  //��ָ	A
	AddEquipToEquipSet(8,  32, eEquipSet_Slayer);  //��ָ	B
	AddEquipToEquipSet(8,  33, eEquipSet_Slayer);  //��ָ	C
	AddEquipToEquipSet(8,  34, eEquipSet_Slayer);  //��ָ	D
	AddEquipToEquipSet(84, 4,  eEquipSet_Slayer);  //������	 1
	AddEquipToEquipSet(85, 4,  eEquipSet_Slayer);  //���	1

	AddEquipToEquipSet(8,  26, eEquipSet_Slayer_221);  //��ָ	A
	AddEquipToEquipSet(8,  35, eEquipSet_Slayer_221);  //��ָ	B
	AddEquipToEquipSet(8,  36, eEquipSet_Slayer_221);  //��ָ	C
	AddEquipToEquipSet(8,  37, eEquipSet_Slayer_221);  //��ָ	D
	AddEquipToEquipSet(84, 5,  eEquipSet_Slayer_221);  //������	 1
	AddEquipToEquipSet(85, 5,  eEquipSet_Slayer_221);  //���	1

	AddEquipToEquipSet(8,  27, eEquipSet_Slayer_231);  //��ָ	A
	AddEquipToEquipSet(8,  28, eEquipSet_Slayer_231);  //��ָ	B
	AddEquipToEquipSet(8,  29, eEquipSet_Slayer_231);  //��ָ	C
	AddEquipToEquipSet(8,  30, eEquipSet_Slayer_231);  //��ָ	D
	AddEquipToEquipSet(84, 6,  eEquipSet_Slayer_231);  //������	 1
	AddEquipToEquipSet(85, 6,  eEquipSet_Slayer_231);  //���	1

	AddEquipToEquipSet(8,  31, eEquipSet_Slayer_241);  //��ָ	A
	AddEquipToEquipSet(8,  38, eEquipSet_Slayer_241);  //��ָ	B
	AddEquipToEquipSet(8,  39, eEquipSet_Slayer_241);  //��ָ	C
	AddEquipToEquipSet(8,  40, eEquipSet_Slayer_241);  //��ָ	D
	AddEquipToEquipSet(84, 7,  eEquipSet_Slayer_241);  //������	 1
	AddEquipToEquipSet(85, 7,  eEquipSet_Slayer_241);  //���	1

	AddEquipToEquipSet(8,  41, eEquipSet_Slayer_251);  //��ָ	A
	AddEquipToEquipSet(8,  42, eEquipSet_Slayer_251);  //��ָ	B
	AddEquipToEquipSet(8,  43, eEquipSet_Slayer_251);  //��ָ	C
	AddEquipToEquipSet(8,  44, eEquipSet_Slayer_251);  //��ָ	D
	AddEquipToEquipSet(84, 8,  eEquipSet_Slayer_251);  //������	 1
	AddEquipToEquipSet(85, 8,  eEquipSet_Slayer_251);  //���	1

	//ħ��
	AddEquipToEquipSet(63, 25, eEquipSet_Outer);  //��ָA
	AddEquipToEquipSet(63, 30, eEquipSet_Outer);  //��ָB
	AddEquipToEquipSet(57, 22, eEquipSet_Outer);  //�ۻ�A
	AddEquipToEquipSet(57, 27, eEquipSet_Outer);  //�ۻ�B
	AddEquipToEquipSet(88, 4,  eEquipSet_Outer);  //����
	AddEquipToEquipSet(89, 4,  eEquipSet_Outer);  //����

	AddEquipToEquipSet(63, 26, eEquipSet_Outer_221);  //��ָA
	AddEquipToEquipSet(63, 31, eEquipSet_Outer_221);  //��ָB
	AddEquipToEquipSet(57, 23, eEquipSet_Outer_221);  //�ۻ�A
	AddEquipToEquipSet(57, 28, eEquipSet_Outer_221);  //�ۻ�B
	AddEquipToEquipSet(88, 5,  eEquipSet_Outer_221);  //����
	AddEquipToEquipSet(89, 5,  eEquipSet_Outer_221);  //����

	AddEquipToEquipSet(63, 27, eEquipSet_Outer_231);  //��ָA
	AddEquipToEquipSet(63, 28, eEquipSet_Outer_231);  //��ָB
	AddEquipToEquipSet(57, 24, eEquipSet_Outer_231);  //�ۻ�A
	AddEquipToEquipSet(57, 25, eEquipSet_Outer_231);  //�ۻ�B
	AddEquipToEquipSet(88, 6,  eEquipSet_Outer_231);  //����
	AddEquipToEquipSet(89, 6,  eEquipSet_Outer_231);  //����

	AddEquipToEquipSet(63, 29, eEquipSet_Outer_241);  //��ָA
	AddEquipToEquipSet(63, 32, eEquipSet_Outer_241);  //��ָB
	AddEquipToEquipSet(57, 26, eEquipSet_Outer_241);  //�ۻ�A
	AddEquipToEquipSet(57, 29, eEquipSet_Outer_241);  //�ۻ�B
	AddEquipToEquipSet(88, 7,  eEquipSet_Outer_241);  //����
	AddEquipToEquipSet(89, 7,  eEquipSet_Outer_241);  //����

	AddEquipToEquipSet(63, 33, eEquipSet_Outer_251);  //��ָA
	AddEquipToEquipSet(63, 34, eEquipSet_Outer_251);  //��ָB
	AddEquipToEquipSet(57, 30, eEquipSet_Outer_251);  //�ۻ�A
	AddEquipToEquipSet(57, 31, eEquipSet_Outer_251);  //�ۻ�B
	AddEquipToEquipSet(88, 8,  eEquipSet_Outer_251);  //����
	AddEquipToEquipSet(89, 8,  eEquipSet_Outer_251);  //����

	g_strEquipName[eEquipSet_Vampire] = "Commander Vlad III";
	g_strEquipName[eEquipSet_Slayer] = "Joan of Arc";
	g_strEquipName[eEquipSet_Outer] = "Kellandriel";

	g_strEquipName[eEquipSet_Vampire_221] = "Doom Set";
	g_strEquipName[eEquipSet_Slayer_221] = "Glamour Set";
	g_strEquipName[eEquipSet_Outer_221] = "The Power of Blossom";

	g_strEquipName[eEquipSet_Vampire_231] = "Doom II";
	g_strEquipName[eEquipSet_Slayer_231] = "Glory II";
	g_strEquipName[eEquipSet_Outer_231] = "Bloom II";

	g_strEquipName[eEquipSet_Vampire_241] = "The Bloody Set";
	g_strEquipName[eEquipSet_Slayer_241] = "Biohazard suit";
	g_strEquipName[eEquipSet_Outer_241] = "Divine Prophecy Set";

	g_strEquipName[eEquipSet_Vampire_251] = "Curse Set";
	g_strEquipName[eEquipSet_Slayer_251] = "Biohazard Suit II";
	g_strEquipName[eEquipSet_Outer_251] = "Elf Queen Set";

}

void InitGemDescData()
{	
	g_mapGemDesc[0] = "Strength";
	g_mapGemDesc[1] = "Dexterity";
	g_mapGemDesc[2] = "Intelligence";
	g_mapGemDesc[3] = "Health points";
	g_mapGemDesc[4] = "Mana Points";
	g_mapGemDesc[5] = "HP Drain";
	g_mapGemDesc[6] = "MP absorption";
	g_mapGemDesc[7] = "HP Regeneration";
	g_mapGemDesc[8] = "MP Regeneration";
	g_mapGemDesc[9] = "ToHit";
	g_mapGemDesc[10] = "Dodging";
	g_mapGemDesc[11] = "Critical Hit";
	g_mapGemDesc[12] = "Defence";
	g_mapGemDesc[18] = "Vision";
	g_mapGemDesc[19] = "Attack speed";
	g_mapGemDesc[20] = "Effective Combat";
	g_mapGemDesc[21] = "Luck";
	g_mapGemDesc[22] = "All ability";
	g_mapGemDesc[23] = "Total capacity";
	g_mapGemDesc[32] = "Magical damage";
	g_mapGemDesc[33] = "Physical damage";
	g_mapGemDesc[36] = "Magical Defence";
	g_mapGemDesc[37] = "Physical Defence";
	g_mapGemDesc[38] = "Magic damage reduction";
	g_mapGemDesc[39] = "Physical damage reduction";
	g_mapGemDesc[40] = "All damage reduction";
	
}

//-----------------------------------------------------------------------------
// _Item_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_Item_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	CT_ITEM("ItemDescShow: ENTER void_ptr=%p right=%ld", void_ptr, right);

	COLORREF ITEM_NAME_RARE_OPTION_SHADE = RGB(0,0,31<<3);
	MItem * p_item = (MItem *)void_ptr;
	// FIX 2026-05-12: gpC_mouse_pointer NULL guard (login/zone-change race'inde mouse pointer henüz init olmamış olabilir).
	MItem* pMouseItem = (gpC_mouse_pointer != NULL) ? gpC_mouse_pointer->GetPickUpItem() : NULL;
	if(p_item == NULL)
	{
		CT_ITEM("ItemDescShow: p_item NULL — return");
		return;
	}

	MItem* p_AddItem = (MItem*)right;
	CT_ITEM("ItemDescShow: pre GetItemClass/Type p_item=%p", p_item);
	int itemClass = p_item->GetItemClass();
	int itemType = p_item->GetItemType();
	CT_ITEM("ItemDescShow: post GetItemClass=%d Type=%d, pre GetEquipSetID", itemClass, itemType);
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
	
//	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
	gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);

//#endif

//	RECT dest_rect;
//	S_SURFACEINFO	dest_surface;

//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);

//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);

//			gpC_base->m_p_DDSurface_back->Unlock();
//		}

//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;
//	gpC_base->m_item_name_pi.text_color = ITEM_NAME_NORMAL_RGB;

	// name�� ���� �� �� �ֱ� ������ name�� �������� ���Ѵ�.
	char sz_name[NAME_STRING_LEN];
	char sz_ename[NAME_STRING_LEN];
	sz_name[0] = '\0'; // for strcat()
	sz_ename[0] = '\0'; // for strcat()
	
	COLORREF name_color = ITEM_NAME_NORMAL_COLOR;
	COLORREF ename_color = ITEM_ENAME_NORMAL_COLOR;
	COLORREF shadow_color = ITEM_NAME_NORMAL_SHADE;

	int nEquipSetID = GetEquipSetID(itemClass, itemType);
	CT_ITEM("ItemDescShow: post GetEquipSetID=%d, pre TimeItemManager", nEquipSetID);

	strcat(sz_ename, "(");
	TYPE_OBJECTID itemId_dbg = p_item->GetID();
	CT_ITEM("ItemDescShow: itemId=%d g_pTimeItemManager=%p g_pGameStringTable=%p",
	           (int)itemId_dbg, g_pTimeItemManager, g_pGameStringTable);
	bool bTimeItem_dbg = false;
	if (g_pTimeItemManager != NULL) {
		bTimeItem_dbg = g_pTimeItemManager->IsExist(itemId_dbg);
	}
	CT_ITEM("ItemDescShow: post IsExist bTimeItem=%d", (int)bTimeItem_dbg);
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	bool bTLS_dbg = p_item->IsTimeLimitStatic();
	CT_ITEM("ItemDescShow: post IsTimeLimitStatic bTLS=%d", (int)bTLS_dbg);
#endif
	if (bTimeItem_dbg
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
		|| bTLS_dbg
#endif //__PREMIUM_GIVE_ITEM_UI
		)
	{
		strcat( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_HAN].GetString() );
		strcat(sz_name, " "); // add 'space'
		strcat( sz_ename,(*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_ENG].GetString() );
		strcat(sz_ename, " "); // add 'space'
	}

	bool isIdent_dbg = p_item->IsIdentified();
	bool isUnique_dbg = p_item->IsUniqueItem();
	bool isQuest_dbg = p_item->IsQuestItem();
	int extColor_dbg = p_item->GetItemOptionExtColorSet();
	CT_ITEM("ItemDescShow: ident=%d uniq=%d quest=%d extColor=%d",
	           (int)isIdent_dbg, (int)isUnique_dbg, (int)isQuest_dbg, extColor_dbg);

	if (p_item->IsIdentified()
#if __CONTENTS(__TUNING_ITEM)
		|| p_item->IsTuningItem()
#endif //__TUNING_ITEM
		)
	{
		CT_ITEM("ItemDescShow: identified branch ENTER");
		if(p_item->IsUniqueItem() || p_item->IsQuestItem()
#if __CONTENTS(__TUNING_ITEM)
		|| p_item->IsTuningItem()
#endif //__TUNING_ITEM
			) // ||  (p_item->GetItemOptionColorSet() >= OPTIONEXT_ITEM_COLOR)) //      p_item->IsItemOptionExt())
		{
			CT_ITEM("ItemDescShow: unique/quest/tuning branch ENTER");
			WORD temp_color;

			/*if( (p_item->GetItemClass() == ITEM_CLASS_SWORD  && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_BLADE  && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_CROSS  && (p_item->GetItemType() >= 13  && p_item->GetItemType() <= 15 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_SG     && (p_item->GetItemType() >= 14  && p_item->GetItemType() <= 16 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_SMG    && (p_item->GetItemType() >= 14  && p_item->GetItemType() <= 16 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_AR     && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_SR     && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_MACE   && (p_item->GetItemType() >= 13  && p_item->GetItemType() <= 15 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_PORTAL_ITEM && (p_item->GetItemType() >= 18  && p_item->GetItemType() <= 23 )) ||
				(p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_WEAPON   && (p_item->GetItemType() >= 20  && p_item->GetItemType() <= 22)) ||
				(p_item->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM   && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17)) ||
				(p_item->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET   && (p_item->GetItemType() >= 45  && p_item->GetItemType() <= 53))
			)
			{	
				temp_color  =(unsigned short) RGB(0,69,255) ; // RGB_ORANGE ;
				name_color  = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3); 
				ename_color = name_color ; 
				shadow_color = 
				
				//if( p_item->IsQuestItem() )
				//	temp_color = CIndexSprite::ColorSet[MItem::GetQuestItemColorset()][29];			
				//else
				//	temp_color = CIndexSprite::ColorSet[MItem::GetUniqueItemColorset()][29];				
				//shadow_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
			}
			else
			{*/
			//if( p_item->IsItemOptionExtension())
			//	temp_color = CIndexSprite::ColorSet[p_item->GetItemOptionExtensionColorSet()][15];
				//temp_color = CIndexSprite::ColorSet[MItem::GetQuestItemColorset()][13];
			//else
#if __CONTENTS(__TUNING_ITEM)
			if( p_item->IsTuningItem() )
				temp_color = SafeColorSet(MItem::GetTuningItemColorSet(), 8);
			else
#endif //__TUNING_ITEM
			if( p_item->IsQuestItem()) {
				int qcs = MItem::GetQuestItemColorset();
				CT_ITEM("ItemDescShow: pre ColorSet[Quest=%d][15]", qcs);
				if (qcs >= 0 && qcs < 795) {
					temp_color = CIndexSprite::ColorSet[qcs][15];
				} else {
					CT_ITEM("ItemDescShow: Quest cs OOB (%d), fallback 0", qcs);
					temp_color = 0;
				}
				CT_ITEM("ItemDescShow: post ColorSet Quest OK");
			} else if( p_item->IsUniqueItem()) {
				int ucs = MItem::GetUniqueItemColorset();
				CT_ITEM("ItemDescShow: pre ColorSet[Uniq=%d][15] g_pClientConfig=%p", ucs, g_pClientConfig);
				if (ucs >= 0 && ucs < 795) {
					temp_color = CIndexSprite::ColorSet[ucs][15];
				} else {
					CT_ITEM("ItemDescShow: Uniq cs OOB (%d), fallback 0", ucs);
					temp_color = 0;
				}
				CT_ITEM("ItemDescShow: post ColorSet Uniq OK temp_color=%d", (int)temp_color);
			}
			//else
			//	temp_color = CIndexSprite::ColorSet[p_item->GetItemOptionColorSet()][15];


			else {
				int extCS = p_item->GetItemOptionExtColorSet();
				CT_ITEM("ItemDescShow: pre ColorSet[ExtCS=%d][15]", extCS);
				if (extCS >= 0 && extCS < 795) {
					temp_color = CIndexSprite::ColorSet[extCS][15];
				} else {
					CT_ITEM("ItemDescShow: ExtCS OOB (%d), fallback 0", extCS);
					temp_color = 0;
				}
				CT_ITEM("ItemDescShow: post ColorSet ExtCS lookup OK");
			}
	
			name_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);

			//ename_color = name_color;
			
			//if( p_item->IsItemOptionExtension())
			//	temp_color = CIndexSprite::ColorSet[p_item->GetItemOptionExtensionColorSet()][29];
			//else

			{
				int cs2 = -1;
#if __CONTENTS(__TUNING_ITEM)
				if (p_item->IsTuningItem()) cs2 = MItem::GetTuningItemColorSet();
				else
#endif
				if (p_item->IsQuestItem()) cs2 = MItem::GetQuestItemColorset();
				else if (p_item->IsUniqueItem()) cs2 = MItem::GetUniqueItemColorset();
				else cs2 = p_item->GetItemOptionExtColorSet();
				CT_ITEM("ItemDescShow: pre ColorSet[cs2=%d][25]", cs2);
				if (cs2 >= 0 && cs2 < 795) {
					temp_color = CIndexSprite::ColorSet[cs2][25];
				} else {
					CT_ITEM("ItemDescShow: cs2 OOB (%d), fallback 0", cs2);
					temp_color = 0;
				}
				CT_ITEM("ItemDescShow: post ColorSet cs2 OK");
			}


			shadow_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
			CT_ITEM("ItemDescShow: post shadow_color RGB");


			//}
		}
		else if(p_item->GetItemClass() == ITEM_CLASS_PET_ITEM)
		{
			if(p_item->GetSilver() > 0)
			{
				name_color = ITEM_NAME_OPTION_COLOR;
				shadow_color = ITEM_NAME_OPTION_SHADE;
				ename_color = ITEM_ENAME_OPTION_COLOR;

				MPetItem *pPetItem = (MPetItem *)p_item;
				
				strcat(sz_name, pPetItem->GetPetOptionName().c_str());
//				strcat(sz_name, " "); // add 'space'

				strcat(sz_ename, pPetItem->GetPetOptionEName().c_str());
//				strcat(sz_ename, " "); // add 'space'
			}
		}
		else if((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].DropItemNameTag)
		{
			name_color  = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].DropItemNameTag;
			ename_color = name_color;
		}
		else if (!p_item->IsEmptyItemOptionList() && p_item->GetItemClass() != ITEM_CLASS_COUPLE_RING &&
				p_item->GetItemClass() != ITEM_CLASS_VAMPIRE_COUPLE_RING && p_item->GetItemClass() != ITEM_CLASS_OUSTERS_HARMONIC_PENDENT
				&& p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
		{
			for(int i=0;i<min(2,p_item->GetItemOptionListCount());i++)
			{
				//if(p_item->GetItemOptionName(i)!=NULL)
				//{
				//	strcat(sz_name, p_item->GetItemOptionName(i));
				//	strcat(sz_name, " "); // add 'space'
				//}
				//if(p_item->GetItemOptionEName(i)!=NULL)
				//{
				//	strcat(sz_ename, p_item->GetItemOptionEName(i));
				//	strcat(sz_ename, " "); // add 'space'
				//}
			}
			if(p_item->GetItemOptionListCount() > 1)			// Rare �������� ���
			{
				name_color = g_pClientConfig->COLOR_NAME_ITEM_RARE_OPTION;
				shadow_color = ITEM_NAME_RARE_OPTION_SHADE;
				ename_color = ITEM_ENAME_RARE_COLOR;
			} else
			{
				name_color = ITEM_NAME_OPTION_COLOR;
				shadow_color = ITEM_NAME_OPTION_SHADE;
				ename_color = ITEM_ENAME_OPTION_COLOR;
			}			
		}
		// FAZ 4 (2026-05-04 oturum 2): May 2 ABI fix sonrasi cantadaki BLADE
		// item (class=15, uniq=1) hover'da IsItemNameColorChange virtual call'da
		// crash izlendi. Crash trace son satir: "pre IsItemNameColorChange check".
		// Branch sirasi reorder edildi — uniq/quest kontrolu basa alindi:
		//   - Original: A() && !B() && !C() — A virtual call HER ZAMAN evaluate
		//   - Yeni:     !B() && !C() && A() — uniq/quest item icin kisa devre,
		//                                      A() asla cagrilmaz
		// Mantik aynidir, ama unique item (uniq=1) icin IsItemNameColorChange
		// (virtual GetItemNameColorSet) artik cagrilmaz. Davranis aynen korunur:
		// branch sadece "non-unique && non-quest && rare-name-color" itemlar icin
		// girerdi zaten.
		//
		// Granuler trace eklendi — gelecek crashlerde tam virtual call izlenebilir.
		CT_ITEM("ItemDescShow: pre uniq/quest/itemNameColor check this=%p", (void*)p_item);
		bool isUniq = p_item->IsUniqueItem();
		CT_ITEM("ItemDescShow: post IsUniqueItem=%d", (int)isUniq);
		bool isQuest = isUniq ? false : p_item->IsQuestItem();
		CT_ITEM("ItemDescShow: post IsQuestItem=%d", (int)isQuest);
		bool isItemColor = (!isUniq && !isQuest) ? p_item->IsItemNameColorChange() : false;
		CT_ITEM("ItemDescShow: post IsItemNameColorChange=%d", (int)isItemColor);
		if( !isUniq && !isQuest && isItemColor )
		{
			WORD temp_color;
			int incs = p_item->GetItemNameColorSet();
			CT_ITEM("ItemDescShow: pre ColorSet[NameCS=%d][15/25]", incs);
			if (incs >= 0 && incs < 795) {
				temp_color = CIndexSprite::ColorSet[incs][15];
				name_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);

				temp_color = CIndexSprite::ColorSet[incs][25];
				shadow_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
			} else {
				CT_ITEM("ItemDescShow: NameCS OOB (%d), skip color update", incs);
			}
			CT_ITEM("ItemDescShow: post NameCS block");

			temp_color = SafeColorSet(p_item->GetItemNameColorSet(), 18);
			ename_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
		}

	}

	char sz_buf[50];

	// FAZ 4 (2026-05-05): GetName/GetEName NULL/length guard + sz_name buffer overflow guard.
	// Hover-on-equipped-gear crash devam eden site. MItem::GetName fix yetmedi —
	// belki m_pName non-NULL ama corrupt. Defansif NULL+strncat clamp.
	{
		const char *pName = p_item->GetName();
		CT_ITEM("ItemDescShow: pre strcat name pName=%p sz_name_len=%d", (void*)pName, (int)strlen(sz_name));
		if (pName != NULL) {
			size_t cur = strlen(sz_name);
			if (cur < NAME_STRING_LEN - 1) {
				strncat(sz_name, pName, NAME_STRING_LEN - 1 - cur);
			}
		}
		CT_ITEM("ItemDescShow: post strcat name");

		const char *pEName = p_item->GetEName();
		CT_ITEM("ItemDescShow: pre strcat ename pEName=%p sz_ename_len=%d", (void*)pEName, (int)strlen(sz_ename));
		if (pEName != NULL) {
			size_t cur = strlen(sz_ename);
			if (cur < NAME_STRING_LEN - 1) {
				strncat(sz_ename, pEName, NAME_STRING_LEN - 1 - cur);
			}
		}
		CT_ITEM("ItemDescShow: post strcat ename");
	}

	CT_ITEM("ItemDescShow: pre Grade check");
	if( p_item->GetGrade() > 0 &&  p_item->GetGrade() <= 10 &&
		p_item->IsGearItem() && !p_item->IsUniqueItem() &&
		p_item->GetItemClass() != ITEM_CLASS_CORE_ZAP &&
		p_item->GetItemClass() != ITEM_CLASS_CUE_OF_ADAM)
	{
		DescAppendItemGradeSuffix(sz_name, sizeof(sz_name), p_item->GetGrade());
		DescAppendItemGradeSuffix(sz_ename, sizeof(sz_ename), p_item->GetGrade());
	}
	CT_ITEM("ItemDescShow: post Grade check, pre paren-close");

	strcat(sz_ename, ")");
	CT_ITEM("ItemDescShow: post paren-close, pre IsIdentified");

	if(!p_item->IsIdentified())
	{
		for(int i = 0; i < strlen(sz_name); i++)
			sz_name[i] = '?';

		for(int i = 0; i < strlen(sz_ename); i++)
			sz_ename[i] = '?';
	}
	CT_ITEM("ItemDescShow: post IsIdentified, pre IsQuestItem chain");

	if(!p_item->IsQuestItem())
	{
		if( (p_item->GetItemClass() == ITEM_CLASS_SWORD  && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_BLADE  && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_CROSS  && (p_item->GetItemType() >= 13  && p_item->GetItemType() <= 15 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_SG     && (p_item->GetItemType() >= 14  && p_item->GetItemType() <= 16 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_SMG    && (p_item->GetItemType() >= 14  && p_item->GetItemType() <= 16 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_AR     && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_SR     && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_MACE   && (p_item->GetItemType() >= 13  && p_item->GetItemType() <= 15 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_PORTAL_ITEM && (p_item->GetItemType() >= 18  && p_item->GetItemType() <= 23 )) ||
			(p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_WEAPON   && (p_item->GetItemType() >= 20  && p_item->GetItemType() <= 22)) ||
			(p_item->GetItemClass() == ITEM_CLASS_OUSTERS_CHAKRAM   && (p_item->GetItemType() >= 15  && p_item->GetItemType() <= 17)) ||
			(p_item->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET   && (p_item->GetItemType() >= 45  && p_item->GetItemType() <= 53))
		)
		{	
			//temp_color  =(unsigned short) RGB(255,69,0) ; // RGB_ORANGE ;
			name_color  = (unsigned short) RGB(200,100,100); // 
			//WORD temp_color = 0 ; 
			//temp_color =   100  ;
			//temp_color +=  (100 << 5) ;
			//temp_color +=  (255 << 11) ; //   RGB(100,100,255) ;
			//name_color  = RGB(CDirectDraw::Red(temp_color), CDirectDraw::Green(temp_color), CDirectDraw::Blue(temp_color)); 
			ename_color = name_color ; 
			shadow_color = 0 ; 
			
			//if( p_item->IsQuestItem() )
			//	temp_color = CIndexSprite::ColorSet[MItem::GetQuestItemColorset()][29];			
			//else
			//	temp_color = CIndexSprite::ColorSet[MItem::GetUniqueItemColorset()][29];				
			//shadow_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
		}

	}
	CT_ITEM("ItemDescShow: post IsQuestItem block, pre g_FL2_GetDC");

	g_FL2_GetDC();
	CT_ITEM("ItemDescShow: post g_FL2_GetDC, pre EquipSet/RankGem checks");

	CT_ITEM("ItemDescShow: pre EquipSet branch nEquipSetID=%d", nEquipSetID);
	if ( nEquipSetID != -1 )
	{
	//   name_color = RGB(0, 255, 0);
		int grade = g_CurrentFrame % 28;

		if ( grade > 14 )
			grade = 28 - grade;

		int mygrade= (240)+grade;
		DWORD temp_color = SafeColorSet(mygrade, 15);
		name_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);

	   int nSize = g_vecEquipSetList[nEquipSetID].size();
	   if ( nSize == 1 )
		   name_color = RGB(165, 52, 247);
	}
	else if ( p_item->GetItemClass() == ITEM_CLASS_RANK_GEM )
	{
		int grade = g_CurrentFrame % 28;

		if ( grade > 14 )
			grade = 28 - grade;

		int mygrade= (240)+grade;
		DWORD temp_color = SafeColorSet(mygrade, 15);
		name_color = RGB(CDirectDraw::Red(temp_color)<<3, CDirectDraw::Green(temp_color)<<3, CDirectDraw::Blue(temp_color)<<3);
		ename_color = RGB_WHITE;
		shadow_color = 0;
	}
	else
	{
		CT_ITEM("ItemDescShow: else-default branch, pre PrintColorStr shadow gpC_base=%p", (void*)gpC_base);
		if (gpC_base != NULL)
			g_PrintColorStr(px+1, py+1, sz_name, gpC_base->m_item_name_pi, shadow_color);
	}

	CT_ITEM("ItemDescShow: post EquipSet/Default branch, pre GetItemOptionListCount");
	int nOptSize = p_item->GetItemOptionListCount();
	CT_ITEM("ItemDescShow: post GetItemOptionListCount=%d", nOptSize);

	if ( nOptSize == 3 )
		name_color = RGB(4, 118, 129);

	// item_style=2: sonraki ezmeleri geç, isim RGB döngüsü.
	if (p_item->IsNameColorCycleItem())
	{
		name_color = p_item->GetNameColorCycle();
		ename_color = name_color;
		shadow_color = 0;
	}

	CT_ITEM("ItemDescShow: pre PrintColorStr name (4 calls)");
	if (gpC_base != NULL)
	{
		if (p_item->IsNameColorCycleItem())
		{
			PrintNameColorCycleStr(px, py, sz_name, gpC_base->m_item_name_pi, p_item);
			py += NORMAL_FONT_Y_GAP;
			g_PrintColorStr(px+1, py+1, sz_ename, gpC_base->m_item_desc_pi, shadow_color);
			PrintNameColorCycleStr(px, py, sz_ename, gpC_base->m_item_desc_pi, p_item);
			py += SMALL_FONT_Y_GAP+5;
		}
		else
		{
			g_PrintColorStr(px, py, sz_name, gpC_base->m_item_name_pi, name_color);
			py += NORMAL_FONT_Y_GAP;
			g_PrintColorStr(px+1, py+1, sz_ename, gpC_base->m_item_desc_pi, shadow_color);
			g_PrintColorStr(px, py, sz_ename, gpC_base->m_item_desc_pi, ename_color);
			py += SMALL_FONT_Y_GAP+5;
		}
	}
	CT_ITEM("ItemDescShow: post PrintColorStr name+ename block");
	CT_ITEM("ItemDescShow: Marker A - post L943, pre int vx");

	int vx;
	CT_ITEM("ItemDescShow: Marker A2 - post int vx");

	const COLORREF required_rgb = RGB(255, 128, 64);

	// identify�� �������� ���.. by sigi
//	if (p_item->IsIdentified())
	{
		// weight
//		vx = g_PrintColorStr(px, py, "Weight", gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
//		sprintf(sz_buf, "%d", p_item->GetWeight());
//		g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

//		py += SMALL_FONT_Y_GAP;

		// gun �迭
//		if(p_item->IsGunItem())
		{

			switch(p_item->GetItemClass())
			{
			case ITEM_CLASS_SWORD:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_SWORD, "");
				break;
				
			case ITEM_CLASS_BLADE:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_BLADE, "");
				break;
				
			case ITEM_CLASS_CROSS:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_CROSS, "");
				break;
				
			case ITEM_CLASS_MACE:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_MACE, "");
				break;
				
			case ITEM_CLASS_MINE:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_MINE, "");
				break;
				
			case ITEM_CLASS_BOMB:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_BOMB, "");
				break;
				
			case ITEM_CLASS_BOMB_MATERIAL:
				if(p_item->GetItemType() < 5)
					DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_BOMB_MATERIAL, "");
				else
					DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_MINE_MATERIAL, "");
				break;
				
			case ITEM_CLASS_SG:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_SG, "");
				break;
				
			case ITEM_CLASS_SMG:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_SMG, "");
				break;
				
			case ITEM_CLASS_AR:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_AR, "");
				break;
				
			case ITEM_CLASS_SR:
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ITEM_CLASS_SR, "");
				break;
				
			default:
				wsprintf(sz_buf, "");
				break;
			}
			
			if(strlen(sz_buf) > 0)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CLASS].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
				
				py += SMALL_FONT_Y_GAP;
			}
		}
		
		// Identified �и�
		CT_ITEM("ItemDescShow: Marker B - post weapon switch, pre outer IsIdentified");
		if(p_item->IsIdentified())
		{
			CT_ITEM("ItemDescShow: Marker C - inside outer IsIdentified ENTER");
			const int itemClass = p_item->GetItemClass();
			const int itemType = p_item->GetItemType();
			CT_ITEM("ItemDescShow: Marker D - post itemClass=%d itemType=%d (outer)", itemClass, itemType);

			// �� ������ ���� ��� ���� Ư���ؼ� ���� ó��
			if(itemClass == ITEM_CLASS_PET_ITEM)
			{
				// �� ������ Num�� �ִ�
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CHAR_MANAGER_LEVEL].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "%d", p_item->GetNumber());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;

				// ���� enchant level�� ��Ӽ����� ����Ѵ�
				// enchant level �� ��þƮ�ʹ� �ƹ� ���� ������ ����� ChargeItem�� Charge���� ���� �� ����
				// ��� �������� Enchant Level�� ������ ������ Number�� �����Ѵ�. �׷��� ���� �����ۿ���
				// GetEnchantLevel�� �ص� �ƹ��͵� �ȳ��´�.
				// �װ� ���� �����۸��̰� ���� �׳� EnchantLevel �״�� ����
				// �� �Ӽ� ��ġ�� Silver���� ����.

				MPetItem *pPetItem = dynamic_cast<MPetItem *>(p_item);

				// 2�� �ɷ��� ���� ��
				if(pPetItem && pPetItem->IsCanCutHead())
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_PET_CAN_CUT_HEAD].GetString(),
						gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					py += SMALL_FONT_Y_GAP;
				}
				
				// 4���ɷ�(ĸ�� �ɷ�)
				if(pPetItem && pPetItem->GetEnchantSkillType() > 0)
				{
					DWORD			stringID		= UI_STRING_PET_EFFECT_NO_CAPSULE + pPetItem->GetEnchantSkillType();
					
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[stringID].GetString(),
						gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					py += SMALL_FONT_Y_GAP;
				}

				if(p_item->GetSilver() > 0)
				{
					int iEnchantLevel  = p_item->GetEnchantLevel();
					
					if(iEnchantLevel < ITEMOPTION_TABLE::MAX_PART)
					{
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_PET_ATTR].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

						sprintf(sz_buf, "%s +%d", g_pItemOptionTable->ITEMOPTION_PARTNAME[iEnchantLevel].GetString(), p_item->GetSilver());
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

						py += SMALL_FONT_Y_GAP;
					}
				}

				// �׾���
				if( p_item->GetGrade() != -1 )
				{
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_DESC_PET_DEAD_DAY, (int)(p_item->GetGrade()), " %d");				
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}
				else	// ���׾���
				{
					// ���� dur�� ���� ���̾����� ����Ѵ�
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_PET_DESC_DURABILITY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					TYPE_ITEM_DURATION leftTime = p_item->GetCurrentDurability();
					int timeGap = ((timeGetTime() - ((MPetItem *)p_item)->GetUpdateTime())/1000/60);
					if(timeGap > leftTime)
					{
						leftTime = 0;
					}
					else
					{
						leftTime = leftTime - timeGap;
					}
					int day,hour,minute;
					
					day = leftTime / 60 / 24;
					hour = (leftTime / 60)%24;
 					minute = leftTime % 60;

					std::string time;

					if(leftTime == 0)
					{
						time = (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NOT_EXIST].GetString();
					}
					else
					{
						if( day > 0 )
						{
							sprintf( sz_buf, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString(), day );
							time = sz_buf;
							time += " ";
						}
						if( hour > 0 )
						{
							sprintf( sz_buf, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString(), hour );
							time += sz_buf;
							time += " ";
						}
						if( minute > 0 )
						{
							sprintf( sz_buf, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString(), minute );
							time += sz_buf;					
						}
					}
					g_PrintColorStr(vx, py, time.c_str() , gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}
//				MPetItem *pPetItem = (MPetItem *)p_item;
//				if(pPetItem->GetPetKeepedDay() > 0)
//				{
//					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_PET_ATTR].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
//					sprintf(sz_buf, "%s +%d", g_pItemOptionTable->ITEMOPTION_PARTNAME[p_item->GetEnchantLevel()].GetString(), p_item->GetSilver());				
//					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
//					py += SMALL_FONT_Y_GAP;
//				}
#if __CONTENTS(__PET_MIXINGFORGE)
				// �� ���ձ� �ɼ�(���� �ɼ�) ǥ��
				if(pPetItem->GetPetMixingOptionListCount())
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PETMIXING_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					const std::list<TYPE_ITEM_OPTION> & petMixingOptionList = pPetItem->GetPetMixingOptionList();
					std::list<TYPE_ITEM_OPTION>::const_iterator itr = petMixingOptionList.begin();

					for (; itr!= petMixingOptionList.end(); itr++)
					{
						WORD mixingOption = *itr;
						
						ITEMOPTION_INFO optionInfo;
						
						optionInfo = (*g_pItemOptionTable)[mixingOption];
						
						char pPartName[20];
						strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
						if(strstr(pPartName,"MP") != NULL)
						{
							if(g_eRaceInterface == RACE_VAMPIRE)
								*strstr(pPartName,"MP") = 'H';
							else if(g_eRaceInterface == RACE_OUSTERS)
								*strstr(pPartName,"MP") = 'E';
						}
						
						int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ
						
						if (pPartName)
						{
							if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
							{
								sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
								strcat(sz_buf, "%");
							}
							else
								if(
									optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
									optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
								{
									sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
									strcat(sz_buf, "%");
								}
								else
									sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
								
								g_PrintColorStr(vx, py, sz_buf, gpC_base->m_chatting_pi, RGB_GOLD);//RGB_YELLOW);
								py += 13;
						}
					}
				}
#endif //__PET_MIXINGFORGE

			} 
		
			else
			{
				CT_ITEM("ItemDescShow: Marker H - inside else non-PET branch ENTER (class=48 EVENT_ETC expected)");
				// Durability
				if (
					p_item->GetMaxDurability() != -1	&&
					!p_item->IsUniqueItem()				&&
					!p_item->IsQuestItem()				&&
					p_item->IsRepairableItem()
					//itemClass!=ITEM_CLASS_VAMPIRE_AMULET &&
					//itemClass!=ITEM_CLASS_COUPLE_RING &&
					//itemClass!=ITEM_CLASS_VAMPIRE_COUPLE_RING &&
					//itemClass!=ITEM_CLASS_CORE_ZAP&&
					//itemClass!=ITEM_CLASS_BLOOD_BIBLE_SIGN&&
					//itemClass!=ITEM_CLASS_ETHEREAL_CHAIN
					)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DURABILITY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d/%d", p_item->GetCurrentDurability(), p_item->GetMaxDurability());				
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}

				// Silvering
				if (p_item->GetSilverMax() != -1)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_SILVERING].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d/%d", p_item->GetSilver(), p_item->GetSilverMax());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Damage
				if (p_item->GetMaxDamage() != -1) // damage�� �ִ°�?
				{
					int stringID = UI_STRING_MESSAGE_DESC_DAMAGE;
					if(p_item->GetItemClass() == ITEM_CLASS_OUSTERS_WRISTLET)
						stringID = UI_STRING_MESSAGE_DESC_MAGIC_DAMAGE;

					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[stringID].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d~%d", p_item->GetMinDamage(), p_item->GetMaxDamage());				
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
					py += SMALL_FONT_Y_GAP;
				}			

				if (p_item->GetOriginalSpeed() > 0 && p_item->IsBasicWeapon())
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ATTACK_SPEED].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetOriginalSpeed());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB(200, 200, 255));
					py += SMALL_FONT_Y_GAP;
				}

				// critical hit
				if (p_item->GetCriticalHit() != -1)//ũ��Ʈ�� ��Ʈ�� �ִ°�?
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CRITICALHIT].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetCriticalHit());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
					py += SMALL_FONT_Y_GAP;
				}		
				// Defense
				if (p_item->GetDefenseValue() != -1)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DEFENSE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetDefenseValue());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Protection
				if (p_item->GetProtectionValue() != -1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PROTECTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetProtectionValue());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;
				}			
				// TOHIT
				if (p_item->GetToHit() != -1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ACCURACY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetToHit());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Heal point
				if (p_item->GetHealPoint() != -1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_HP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "+%d", p_item->GetHealPoint());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Mana point
				if (p_item->GetManaPoint() != -1)
				{
					if( g_eRaceInterface == RACE_OUSTERS )
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_EP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					else
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "+%d", p_item->GetManaPoint());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}
				
				// Attack range
				if (p_item->GetReach() != -1 && p_item->GetReach() != 1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_RANGE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d %s", p_item->GetReach(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_TILE_PIECE].GetString());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Bullet
				if (p_item->GetMagazineSize() != -1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d %s", p_item->GetMagazineSize(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_COUNT].GetString());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
					py += SMALL_FONT_Y_GAP;
				}
				// Pocket size
				if (p_item->GetPocketNumber() != -1)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_POCKET_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d %s", p_item->GetPocketNumber(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
					py += SMALL_FONT_Y_GAP;
				}			
				// Pile size
				if (p_item->IsPileItem() == true)
				{
					vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ITEM_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d %s", p_item->GetNumber(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);			
					py += SMALL_FONT_Y_GAP;
				}
				
				// Charge size
				// Sjheon 2005.05.30  ChargeItem ������ ������ ���� ��� ȸ�� ���� Start
				if (p_item->IsChargeItem() == true)
				{
					

					//if (!(gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID())) && p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
					//{	
					//}
					//else

			
					if(p_item->GetItemClass() != ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
					{
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEFT_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
						sprintf(sz_buf, "%d/%d", p_item->GetNumber(), p_item->GetMaxNumber());
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
						py += SMALL_FONT_Y_GAP;
					}

					if (!(gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID())) && p_item->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM)
					{	
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_CHARGE_ITEM_NOUSE].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
						py += SMALL_FONT_Y_GAP;
					}

				}
				// Sjheon 2005.05.30  ChargeItem ������ ������ ���� ��� ȸ�� ���� Start End 

				// lucky value
				CT_ITEM("ItemDescShow: Marker J - pre Lucky check (post stat checks L1230-L1372)");
				if (p_item->GetLucky() != - 9999 &&
					itemClass != ITEM_CLASS_COUPLE_RING && itemClass != ITEM_CLASS_VAMPIRE_COUPLE_RING && itemClass != ITEM_CLASS_OUSTERS_HARMONIC_PENDENT &&
					itemClass != ITEM_CLASS_CORE_ZAP && itemClass != ITEM_CLASS_ETHEREAL_CHAIN && itemClass != ITEM_CLASS_CUE_OF_ADAM )
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LUCKY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetLucky());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
					py += SMALL_FONT_Y_GAP;
				}
#if __CONTENTS(__ADD_MAGICPROTECTION)
				if (p_item->HasMagicProtection())//>GetMagicProtection() != -1)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGICPROTECTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					sprintf(sz_buf, "%d", p_item->GetMagicProtection());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);
					py += SMALL_FONT_Y_GAP;
				}
#endif
				// �ƿ콺���� ���ɼӼ�
				if(itemClass == ITEM_CLASS_OUSTERS_WRISTLET || itemClass == ITEM_CLASS_OUSTERS_STONE)
				{
					static COLORREF stoneRGB[5] = { RGB(255, 100, 100), RGB(100, 100, 255), RGB(255, 180, 100), RGB(100, 100, 255), ITEM_DESC_RGB };
					ITEMTABLE_INFO::ELEMENTAL_TYPE eType = (*g_pItemTable)[itemClass][itemType].ElementalType;
					WORD eNum = (*g_pItemTable)[itemClass][itemType].Elemental;

					if(eType != ITEMTABLE_INFO::ELEMENTAL_TYPE_ANY)
					{
						{
							// FAZ 4 (2026-05-06): OUSTERS_STONE — 1 string arg.
							// sed yanlışlıkla 1d helper uyguladı; inline güvenli fix.
							int sid = UI_STRING_MESSAGE_OUSTERS_STONE;
							const char* fmt = (g_pGameStringTable && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
							                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
							const char* eStr = (g_pGameStringTable && g_ELEMENTAL_STRING_ID[eType] >= 0 && g_ELEMENTAL_STRING_ID[eType] < (int)(*g_pGameStringTable).GetSize())
							                   ? (*g_pGameStringTable)[g_ELEMENTAL_STRING_ID[eType]].GetString() : "";
							if (fmt != NULL && strchr(fmt, '%') != NULL && strchr(fmt, '%')[1] == 's') {
								snprintf(sz_buf, sizeof(sz_buf), fmt, eStr ? eStr : "");
							} else {
								snprintf(sz_buf, sizeof(sz_buf), "%s Stone", eStr ? eStr : "");
							}
						}
						vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, stoneRGB[eType]);
						sprintf(sz_buf, "%d", eNum);
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
						py += SMALL_FONT_Y_GAP;
					}
				}

				CT_ITEM("ItemDescShow: Marker K - pre CORE_ZAP check (post Lucky+MagicProt+OustersStone)");
				if(itemClass == ITEM_CLASS_CORE_ZAP && itemType >= 0 && itemType <= 3)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					// �ھ��� �ɼ�
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_BLACK+p_item->GetItemType(), (int)(p_item->GetGrade()), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;

					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_STAT, (int)(3), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_REG, (int)(9), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					
				}

				CT_ITEM("ItemDescShow: Marker L - pre RANK_GEM check (post CORE_ZAP)");
				if ( itemClass == ITEM_CLASS_RANK_GEM && itemType >=0 && itemType < 104 )
				{
					vx = g_PrintColorStr(px, py, "Rune effects:", gpC_base->m_item_desc_pi, RGB_GREEN);
					//py += SMALL_FONT_Y_GAP;

					int nGemOptClass = p_item->GetRankGemOptionClass();
					int nGemGrade = p_item->GetGrade();

					std::string strDesc = g_mapGemDesc[nGemOptClass];
					std::string fuhao = "%";

					int nPercent = strDesc.find("BAKILACAK");
					if ( nPercent != -1 )
					{
						sprintf( sz_buf, "%s%d%s", strDesc.c_str(), nGemGrade,fuhao.c_str() );
					}
					else
					{
						sprintf( sz_buf, "%s +%d", strDesc.c_str(), nGemGrade );
					}

					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);
					py += SMALL_FONT_Y_GAP;
					py += 10;
				}
				//if(itemClass == ITEM_CLASS_CORE_ZAP && itemType >= 4 && itemType <= 5)//Display the name of the set
				//{
				//	vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				//	// �ھ��� �ɼ�
				//	DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_BLACK+p_item->GetItemType(), (int)(p_item->GetGrade()), " %d");
				//	g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
				//	py += SMALL_FONT_Y_GAP;

				//	sprintf(sz_buf,"�����ȡ����ֵ20%%");
				//	g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
				//	py += SMALL_FONT_Y_GAP;
				//	sprintf(sz_buf,"�Թ����˺����20%%");
				//	g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
				//	py += SMALL_FONT_Y_GAP;
				//	
				//}
				/*if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN && itemType == 0 )
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					// ���׸��� ü�� �ɼ�
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_BLACK+p_item->GetItemType(), (int)(p_item->GetGrade()), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;

					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_STAT, (int)(3), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_REG, (int)(9), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					
				}*/

				if(itemClass == ITEM_CLASS_CUE_OF_ADAM && itemType >= 0 && itemType <= 3)
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					// ť����ƴ� �ɼ�
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_RED+p_item->GetItemType(), (int)(p_item->GetGrade()), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;

					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_ALL_STAT, (int)(2), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_MP, (int)(20), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_ACCURACY, (int)(7), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_DAMAGE, (int)(7), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
				}
				if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN && itemType == 0)// Set display name
				{
					vx = g_PrintColorStr(px, py,"Reward effect: Takes effect upon equipping", gpC_base->m_item_desc_pi, RGB_GREEN);			
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Gain an additional experience points: 20%%");
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
					sprintf(sz_buf,"Increases damage against monsters: 20%%");
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;
				}
				CT_ITEM("ItemDescShow: Marker M - pre ETHEREAL_CHAIN check (post RANK_GEM)");
				if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN && itemType >= 3  && itemType <= 12)//Display the name of the set
				{
					int num = itemType-2;
					int attr = 10+(itemType-3)*5;
					int vis = itemType-2;
					int ice = itemType-7;

					if(itemType == 11)
						num = 10;
					if(itemType == 12)
					{
						num = 15;
						attr = 60;
					}

					vx = g_PrintColorStr(px, py,"Reward effect: Takes effect upon equipping", gpC_base->m_item_desc_pi, RGB_GREEN);			
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Physical damage:%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Magical damage+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Physical Defence+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Magical Defence+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"All Ability+%d",attr);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
					py += SMALL_FONT_Y_GAP;

					if(itemType>=8 && itemType<=12)
					{
						sprintf(sz_buf,"Chance of Chill+%d",ice);
						g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_DARKGRAY);				
						py += SMALL_FONT_Y_GAP;
					}
				}
			}
		} else
		{
			// Durability
			if (p_item->GetMaxDurability() != -1 &&
				!p_item->IsUniqueItem() &&
				//p_item->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET &&
				!p_item->IsQuestItem() &&
				p_item->IsRepairableItem() )
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DURABILITY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "MINDUR/MAXDUR");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			// Silvering
			if (p_item->GetSilverMax() != -1)
			{
				vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_SILVERING].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "MINSILVER/MAXSILVER");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Damage
			if (p_item->GetMaxDamage() != -1) // Is there any damage?
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DAMAGE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "MINDAM~MAXDAM");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
				py += SMALL_FONT_Y_GAP;
			}
			
			// Damage
			if (p_item->GetOriginalSpeed() > 0)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ATTACK_SPEED].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Attack Speed");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB(200, 200, 255));
				py += SMALL_FONT_Y_GAP;
			}

			// critical hit
			if (p_item->GetCriticalHit() != -1)//Is there a critical hit?
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CRITICALHIT].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Critical Hit");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
				py += SMALL_FONT_Y_GAP;
			}
			
			// Defense
			if (p_item->GetDefenseValue() != -1)
			{
				vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DEFENSE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Defense");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Protection
			if (p_item->GetProtectionValue() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PROTECTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Protection");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// TOHIT
			if (p_item->GetToHit() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ACCURACY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "To Hit", p_item->GetToHit());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Heal point
			if (p_item->GetHealPoint() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_HP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "+HP");
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Mana point
			if (p_item->GetManaPoint() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "+MP");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Attack range
			if (p_item->GetReach() != -1 && p_item->GetReach() != 1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_RANGE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Attack Range %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_TILE_PIECE].GetString());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			
			// Bullet
			if (p_item->GetMagazineSize() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Bullet %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_COUNT].GetString());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				py += SMALL_FONT_Y_GAP;
			}
			
			// Pocket size
			if (p_item->GetPocketNumber() != -1)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_POCKET_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Pocket size %s", (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}			
			// Pile size
			if (p_item->IsPileItem() == true)
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ITEM_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Pile %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);			
				py += SMALL_FONT_Y_GAP;
			}
			
			// Charge size
			// Remove remaining uses from ChargeItem item descriptions Start
			if (p_item->IsChargeItem() == true)
			{
				vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEFT_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "MinCharge/MaxCharge");				
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);				
				py += SMALL_FONT_Y_GAP;
			}
			// Remove remaining uses from the ChargeItem description Start End
			// lucky value
			if (p_item->GetLucky() != - 9999)
			{
				vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LUCKY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "Lucky Value");
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
				py += SMALL_FONT_Y_GAP;
			}
		}

		/*// Durability
		if (p_item->GetMaxDurability() != -1&&!p_item->IsUniqueItem()&&p_item->GetItemClass()!=ITEM_CLASS_VAMPIRE_AMULET)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DURABILITY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d/%d", p_item->GetCurrentDurability(), p_item->GetMaxDurability());
			else
				sprintf(sz_buf, "???/???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			
			py += SMALL_FONT_Y_GAP;
		}
		
		// Silvering
		if (p_item->GetSilverMax() != -1)
		{
			vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_SILVERING].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d/%d", p_item->GetSilver(), p_item->GetSilverMax());
			else
				sprintf(sz_buf, "???/???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			
			py += SMALL_FONT_Y_GAP;
		}
		
		// Damage
		if (p_item->GetMaxDamage() != -1) // damage�� �ִ°�?
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DAMAGE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d~%d", p_item->GetMinDamage(), p_item->GetMaxDamage());
			else
				sprintf(sz_buf, "???~???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
			py += SMALL_FONT_Y_GAP;
		}
		
		// critical hit
		if (p_item->GetCriticalHit() != -1)//ũ��Ʈ�� ��Ʈ�� �ִ°�?
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CRITICALHIT].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d", p_item->GetCriticalHit());
			else
				sprintf(sz_buf, "???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
			py += SMALL_FONT_Y_GAP;
		}
		
		
		// Defense
		if (p_item->GetDefenseValue() != -1)
		{
			vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DEFENSE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d", p_item->GetDefenseValue());
			else
				sprintf(sz_buf, "???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);
			
			py += SMALL_FONT_Y_GAP;
		}
		
		// Protection
		if (p_item->GetProtectionValue() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PROTECTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d", p_item->GetProtectionValue());
			else
				sprintf(sz_buf, "???");
			
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);

			py += SMALL_FONT_Y_GAP;
		}

		// TOHIT
		if (p_item->GetToHit() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ACCURACY].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d", p_item->GetToHit());
			else
				sprintf(sz_buf, "???", p_item->GetToHit());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);

			py += SMALL_FONT_Y_GAP;
		}

		// Heal point
		if (p_item->GetHealPoint() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_HP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "+%d", p_item->GetHealPoint());
			else
				sprintf(sz_buf, "+???");
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			py += SMALL_FONT_Y_GAP;
		}

		// Mana point
		if (p_item->GetManaPoint() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "+%d", p_item->GetManaPoint());
			else
				sprintf(sz_buf, "+???");

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			py += SMALL_FONT_Y_GAP;
		}

		// Attack range
		if (p_item->GetReach() != -1 && p_item->GetReach() != 1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_RANGE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d %s", p_item->GetReach(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_TILE_PIECE].GetString());
			else
				sprintf(sz_buf, "??? %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_TILE_PIECE].GetString());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			py += SMALL_FONT_Y_GAP;
		}

		// Bullet
		if (p_item->GetMagazineSize() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d %s", p_item->GetMagazineSize(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_COUNT].GetString());
			else
				sprintf(sz_buf, "??? %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_MAGAZINE_COUNT].GetString());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			py += SMALL_FONT_Y_GAP;
		}

		// Pocket size
		if (p_item->GetPocketNumber() != -1)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_POCKET_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d %s", p_item->GetPocketNumber(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());
			else
				sprintf(sz_buf, "??? %s", (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			py += SMALL_FONT_Y_GAP;
		}
		
		// Pile size
		if (p_item->IsPileItem() == true)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ITEM_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d %s", p_item->GetNumber(),(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());
			else				
				sprintf(sz_buf, "??? %s",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);			
			py += SMALL_FONT_Y_GAP;
		}
		
		// Charge size
		if (p_item->IsChargeItem() == true)
		{
			vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEFT_NUM].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(p_item->IsIdentified())
				sprintf(sz_buf, "%d/%d", p_item->GetNumber(), p_item->GetMaxNumber());
			else
				sprintf(sz_buf, "???/???");

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			
			py += SMALL_FONT_Y_GAP;
		}*/
		CT_ITEM("ItemDescShow: Marker N - pre VAMPIRE_PORTAL_ITEM check (post IsIdentified outer if/else close, post big comment block L1730-L1920)");
		if (p_item->IsIdentified() && p_item->GetItemClass() == ITEM_CLASS_VAMPIRE_PORTAL_ITEM)
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ARRIVAL_LOCATION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			MVampirePortalItem *temp_item = (MVampirePortalItem *)p_item;

			if(temp_item->IsMarked())
			{
				sprintf(sz_buf, "%s", g_pZoneTable->Get(temp_item->GetZoneID())->Name.GetString());
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				py += SMALL_FONT_Y_GAP;
				
				DescSafeSprintf2d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ZONEINFO_XY, (int)temp_item->GetZoneX(), (int)temp_item->GetZoneY(), "(%d, %d)");
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			}
			else
			{
				DescSafeSprintf0(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_DESC_NOT_EXIST, "(not exist)");
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

			}
			
			if (!(gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID()))   )
			{
				
				if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5))
				{
					py += SMALL_FONT_Y_GAP;
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_PORTAL_ITEM_DESC].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
				}
			}
			py += SMALL_FONT_Y_GAP;
		}
		// Sjheon 2005.05.30  ChargeItem ������ �����߰�  Start	
		//if (p_item->IsChargeItem() == true)
		//{
		//	vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_PORTAL_ITEM_DESC].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
		//	//sprintf(sz_buf, "%d/%d", p_item->GetNumber(), p_item->GetMaxNumber());
			//g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);				
		//	py += SMALL_FONT_Y_GAP;
		//}
		// Sjheon 2005.05.30  ChargeItem ������ �����߰�  End 	

		CT_ITEM("ItemDescShow: Marker O - pre Add option block (post VAMPIRE_PORTAL check, IsIdentified outer)");
		// Add option
		if(p_item->IsIdentified())
		{
			CT_ITEM("ItemDescShow: Marker O2 - inside Add option IsIdentified ENTER");
			/*if (p_item->GetItemOptionListCount() != 0)
			{
				int optionCount = p_item->GetItemOptionListCount();
				ITEMOPTION_INFO& optionInfo = (*g_pItemOptionTable)[option];
				
				char pPartName[20];
				strcpy(pPartName, ITEMOPTION_INFO::ITEMOPTION_PARTNAME[optionInfo.Part]); // Option�� ����Ǵ� ��ġ�� �̸�
				if(p_item->IsVampireItem() && strstr(pPartName,"MP") != NULL)
					*strstr(pPartName,"MP") = 'H';
				
				int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ
				
				if (pPartName)
				{
					fOptionCheck=TRUE;
					vx = g_PrintColorStr(px, py, "�ɼ� : ", gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					if(optionInfo.Part == ITEMOPTION_INFO::PART_DURABILITY)
					{
						sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
						strcat(sz_buf, "%");
					}
					else
						sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GOLD);//RGB_YELLOW);
				}
				py += SMALL_FONT_Y_GAP;
			}*/
			// FAZ 4 (2026-05-11) Bug 7: tooltip option uygulanmiyorsa GRI renk + "(inactive)"
			// MItem::IsAffectStatus() = false ise server isRealWearing false dondu
			// (stat requirement karsilanmiyor, durability=0, vb.) -> option DEGER VERMIYOR.
			// Tooltip cilgin renkte (gold) gostermek YANILTICI; gri renk + suffix daha dogru.
			CT_ITEM("ItemDescShow: Marker O3 - pre IsAffectStatus() call (post block comment, in Add option IsIdentified)");
			const bool bug7_optionActive = p_item->IsAffectStatus();
			CT_ITEM("ItemDescShow: Marker O4 - post IsAffectStatus=%d", (int)bug7_optionActive);
			const DWORD bug7_optColor = bug7_optionActive ? RGB_GOLD : RGB_DARKGRAY;

			BOOL fOptionCheck=FALSE;
			if(!p_item->IsEmptyItemOptionList()&& p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
			{
				const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
				std::list<TYPE_ITEM_OPTION>::const_iterator itr=optionList.begin();

				while(itr != optionList.end() && *itr < g_pItemOptionTable->GetSize())
				{
					ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
					char pPartName[20];
					strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
					if(strstr(pPartName,"MP") != NULL)
					{
						if(g_eRaceInterface == RACE_VAMPIRE)
							*strstr(pPartName,"MP") = 'H';
						else if(g_eRaceInterface == RACE_OUSTERS)
							*strstr(pPartName,"MP") = 'E';
					}

					int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ

					if (pPartName)
					{
						if(!fOptionCheck)
						{
							vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							fOptionCheck=TRUE;
						}

						if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
						{
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
							strcat(sz_buf, "%");
						}
						else
						if(
							optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
							optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
						{
							sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
							strcat(sz_buf, "%");
						}
						else
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
						// Bug 7: inactive ise GRI renk; suffix sondaki tek satira (sade tutmak icin)
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bug7_optColor);//RGB_YELLOW);
					}
					py += SMALL_FONT_Y_GAP;
					itr++;
				}
				// FAZ 4 (2026-05-11 it.2 Bug 10): "(stats not applied - requirement unmet)"
				// hint mesajı kaldırıldı. Original davranışa donduk; ileride istenirse
				// AYNI line_count'a eklenecek sekilde geri getirilebilir. Renk gri zaten
				// kullaniciya stat'in inactive oldugunu kaifi belirtiyor.
			}

			///////////////////////����///////////////
			fOptionCheck = false;
			CT_ITEM("ItemDescShow: Marker O5 - pre IsEmptyItemGTwoOptionList check");
			if(!p_item->IsEmptyItemGTwoOptionList()&& p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
			{			
				const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemGTwoOptionList();
				std::list<TYPE_ITEM_OPTION>::const_iterator itr=optionList.begin();				
				
				while(itr != optionList.end() && *itr < g_pItemOptionTable->GetSize())
				{
					ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
					char pPartName[20];
					strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
					if(strstr(pPartName,"MP") != NULL)
					{
						if(g_eRaceInterface == RACE_VAMPIRE)
							*strstr(pPartName,"MP") = 'H';
						else if(g_eRaceInterface == RACE_OUSTERS)
							*strstr(pPartName,"MP") = 'E';
					}
					
					int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ
					
					if (pPartName)
					{
						if(!fOptionCheck)
						{
							vx = g_PrintColorStr(px, py,"Modify properties:", gpC_base->m_item_desc_pi, RGB_RED);
							fOptionCheck=TRUE;
						}
								
						if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
						{
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
							strcat(sz_buf, "%");
						}
						else
						if(
							optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
							optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
						{
							sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
							strcat(sz_buf, "%");
						}
						else
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);//RGB_YELLOW);
					}
					py += SMALL_FONT_Y_GAP;
					itr++;
				}
			}
			fOptionCheck = false;
			CT_ITEM("ItemDescShow: Marker O6 - pre IsEmptyItemThirdOptionList check");
			if(!p_item->IsEmptyItemThirdOptionList()&& p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
			{			
				const std::list<TYPE_ITEM_OPTION> &thirdOptionList=p_item->GetItemThirdOptionList();
				BYTE thirdOptionType = p_item->GetItemThirdOptionType();
				std::list<TYPE_ITEM_OPTION>::const_iterator itr=thirdOptionList.begin();				
				
				while(itr != thirdOptionList.end() && *itr < g_pItemOptionTable->GetSize())
				{
					ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
					char pPartName[20];
					strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
					if(strstr(pPartName,"MP") != NULL)
					{
						if(g_eRaceInterface == RACE_VAMPIRE)
							*strstr(pPartName,"MP") = 'H';
						else if(g_eRaceInterface == RACE_OUSTERS)
							*strstr(pPartName,"MP") = 'E';
					}
					
					int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ
					
					if (pPartName)
					{
						if(!fOptionCheck)
						{
							vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_THIRD_OPTION].GetString(), gpC_base->m_item_desc_pi, 
								ITEM_DESC_RGB);
							fOptionCheck=TRUE;
						}
								
						if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
						{
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
							strcat(sz_buf, "%");
						}
						else
						if(
							optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
							optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
						{
							sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
							strcat(sz_buf, "%");
						}
						else
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi,
							(thirdOptionType == 0)? RGB_LIGHTBLUE: (thirdOptionType == 1)? RGB_RED: RGB_PURPLE);//RGB_YELLOW);
					}
					itr++;
					py += SMALL_FONT_Y_GAP;
				}
			}

			//if(!(*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].DefaultOptionList.empty())
			// Bug 7: DefaultOptionList template-level; isRealWearing false ise yine
			// stat'a uygulanmaz. IsAffectStatus ile gri renk gostermek dogru.
			CT_ITEM("ItemDescShow: Marker O7 - pre IsEmptyItemDefaultOption check");
			if(!p_item->IsEmptyItemDefaultOption())
			{
				CT_ITEM("ItemDescShow: Marker O7a - inside IsEmptyItemDefaultOption=false branch, pre GetItemDefaultOptionList");
				const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_item->GetItemDefaultOptionList();
				CT_ITEM("ItemDescShow: Marker O7a2 - post GetItemDefaultOptionList() returned, list ref @%p", (void*)&DefaultOptionList);
				std::list<TYPE_ITEM_OPTION>::const_iterator itr=DefaultOptionList.begin();
				CT_ITEM("ItemDescShow: Marker O7b - post DefaultOptionList.begin() (NO std::distance)");

				while(itr != DefaultOptionList.end())
				{
					CT_ITEM("ItemDescShow: Marker O7c - inside while iter, *itr=%u (PRE g_pItemOptionTable access, table size=%u)", (unsigned)*itr, (unsigned)(g_pItemOptionTable ? g_pItemOptionTable->GetSize() : 0));
					ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
					CT_ITEM("ItemDescShow: Marker O7d - post optionInfo obtained, optionInfo.Part=%u (PRE ITEMOPTION_PARTNAME access)", (unsigned)optionInfo.Part);
					char pPartName[20];
					strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
					CT_ITEM("ItemDescShow: Marker O7e - post strcpy pPartName=%s", pPartName);

					if(strstr(pPartName,"MP") != NULL)
					{
						if(p_item->IsVampireItem())
							*strstr(pPartName,"MP") = 'H';
						else if(p_item->IsOustersItem())
							*strstr(pPartName,"MP") = 'E';
					}

					int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ

					if (pPartName)
					{
						if(!fOptionCheck)
						{
							vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							fOptionCheck=TRUE;
						}
						if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
						{
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
							strcat(sz_buf, "%");
						}
						else
						if(
							optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
							optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
						{
							sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
							strcat(sz_buf, "%");
						}
						else
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
						// Bug 7: IsAffectStatus false ise GRI renk
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bug7_optColor);//RGB_YELLOW);
					}
					py += SMALL_FONT_Y_GAP;
					itr++;
				}				
			}
			
			CT_ITEM("ItemDescShow: Marker O8 - pre p_AddItem!=NULL check, p_AddItem=%p", (void*)p_AddItem);
			if(p_AddItem!=NULL)
			{
// 20080904 �����忡�� add�����۵鵵 �ɼ��� ������ �Ѵ�. Ŭ���� ������ �ұ� ���� ���� �ߴ�. 
//itemtable�� additem���� �ƴ��� �����ϴ� �Ӽ��� �߰��� �̿��ϸ� �� ������ �ʹ�.
//add start

				int addItemClass = p_AddItem->GetItemClass();
				int addItemType = p_AddItem->GetItemType();


				const std::list<TYPE_ITEM_OPTION> &optionList = p_AddItem->GetItemOptionList();
				const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_AddItem->GetItemDefaultOptionList();
				std::list<TYPE_ITEM_OPTION>::const_iterator itr=optionList.begin();
				
				if(optionList.size() ==0 && DefaultOptionList.size() == 0)
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				
				
				while(itr != optionList.end())
				{
					ITEMOPTION_INFO& optionInfo=(*g_pItemOptionTable)[*itr];
					char pPartName[20];
					strcpy(pPartName,g_pItemOptionTable->ITEMOPTION_PARTNAME[optionInfo.Part].GetString());
					
					if(strstr(pPartName,"MP") != NULL)
					{
						if(p_item->IsVampireItem())
							*strstr(pPartName,"MP") = 'H';
						else if(p_item->IsOustersItem())
							*strstr(pPartName,"MP") = 'E';
					}
					
					int PlusPoint = optionInfo.PlusPoint; // ����Ǵ� ��ġ
					
					if (pPartName)
					{
						if(!fOptionCheck)
						{
							vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							fOptionCheck=TRUE;
						}
						if(optionInfo.Part == ITEMOPTION_TABLE::PART_DURABILITY)
						{
							sprintf(sz_buf, "%s +%d", pPartName, PlusPoint-100);
							strcat(sz_buf, "%");
						}
						else
							if(
								optionInfo.Part >= ITEMOPTION_TABLE::PART_STR_TO_DEX &&
								optionInfo.Part <= ITEMOPTION_TABLE::PART_INT_TO_DEX)
							{
								sprintf(sz_buf, "%s %d", pPartName, PlusPoint);
								strcat(sz_buf, "%");
							}
							else
								sprintf(sz_buf, "%s +%d", pPartName, PlusPoint);
							g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GOLD);//RGB_YELLOW);
					}
					py += SMALL_FONT_Y_GAP;
					itr++;
				}	

//add end
				if(addItemClass == ITEM_CLASS_CORE_ZAP && addItemType>=0 && addItemType<=3)
				{
					const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
					const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_item->GetItemDefaultOptionList();
					if(optionList.size() ==0 && DefaultOptionList.size() == 0)
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_BLACK+p_AddItem->GetItemType(), (int)(p_AddItem->GetGrade()), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;
					
					DWORD TempColor = RGB_DARKGRAY;
					if(gC_vs_ui.IsHasAllCoreZap())
						TempColor = RGB_GREEN;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_STAT, (int)(3), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, TempColor);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CORE_ZAP_REWARD_ALL_REG, (int)(9), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, TempColor);				
					py += SMALL_FONT_Y_GAP;
					
				}
				if(addItemClass == ITEM_CLASS_ETHEREAL_CHAIN && addItemType == 0)
				{
					const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
					const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_item->GetItemDefaultOptionList();
					if(optionList.size() ==0 && DefaultOptionList.size() == 0)
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					
					sprintf(sz_buf, "Reward effects: Now active");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;
					
					DWORD TempColor = RGB_GREEN;
					sprintf(sz_buf,  "Gain an additional experience points : 20%%");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, TempColor);				
					py += SMALL_FONT_Y_GAP;
					sprintf(sz_buf, "Increases damage against monsters by : 20%%");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, TempColor);				
					py += SMALL_FONT_Y_GAP;
					
				}
				if(addItemClass == ITEM_CLASS_ETHEREAL_CHAIN && addItemType >= 3  && addItemType <= 12)//��װ��ʾ����
				{
					int num = addItemType-2;
					int attr = 10+(addItemType-3)*5;
					int vis = addItemType-2;
					int ice = addItemType-7;

					if(addItemType == 11)
						num = 10;
					if(addItemType == 12)
					{
						num = 15;
						attr = 60;
					}
					const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
					const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_item->GetItemDefaultOptionList();
					if(optionList.size() ==0 && DefaultOptionList.size() == 0)
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

					sprintf(sz_buf, "Reward effects: Now active");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Physical damage+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Magical damage+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Physical Defence+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"Magical Defence+%d",num);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;

					sprintf(sz_buf,"All Ability+%d",attr);
					g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
					py += SMALL_FONT_Y_GAP;

					if(addItemType>=8 && addItemType<=12)
					{
						sprintf(sz_buf,"Chance of Chill+%d",ice);
						g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);				
						py += SMALL_FONT_Y_GAP;
					}

				}
				if(addItemClass == ITEM_CLASS_CUE_OF_ADAM && addItemType >= 0 && addItemType <= 3)
				{
					const std::list<TYPE_ITEM_OPTION> &optionList=p_item->GetItemOptionList();
					const std::list<TYPE_ITEM_OPTION> &DefaultOptionList = p_item->GetItemDefaultOptionList();
					
					if(optionList.size() ==0 && DefaultOptionList.size() == 0)
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_RED+p_AddItem->GetItemType(), (int)(p_AddItem->GetGrade()), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_PEARL);				
					py += SMALL_FONT_Y_GAP;

					bool	bCueOption[4] = {false,};
					DWORD	dwTempColor[4] = {RGB_DARKGRAY,RGB_DARKGRAY,RGB_DARKGRAY,RGB_DARKGRAY};

					if(gC_vs_ui.IsHasAllCue(bCueOption))
					{
						for(int i = 0; i < 4; ++i)
							if(bCueOption[i]) dwTempColor[i] = RGB_GREEN;
					}
					
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_ALL_STAT, (int)(2), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, dwTempColor[0]);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_MP, (int)(20), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, dwTempColor[1]);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_ACCURACY, (int)(7), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, dwTempColor[2]);				
					py += SMALL_FONT_Y_GAP;
					DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_CUE_OF_ADAM_REWARD_DAMAGE, (int)(7), " %d");
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, dwTempColor[3]);				
					py += SMALL_FONT_Y_GAP;
				}
			}
		}
		else
		{
			sprintf(sz_buf,"%sOPTIONE-WHAT",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString());
			vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			
			py += SMALL_FONT_Y_GAP;
		}

//		if(-1 != p_item->GetGrade() &&
//			(p_item->GetItemClass() == ITEM_CLASS_RING ||
//			p_item->GetItemClass() == ITEM_CLASS_BRACELET ||
//			p_item->GetItemClass() == ITEM_CLASS_NECKLACE ||
//			)
//			}
//		{
//			sprintf(sz_buf, "%s +%d", g_pItemOptionTable->ITEMOPTION_PARTNAME[21].GetString(), p_item->GetGrade()-4);
//			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GOLD);//RGB_YELLOW);
//			py += SMALL_FONT_Y_GAP;
//		}
			
		CT_ITEM("ItemDescShow: Marker Q - pre group/quest requirements check (post Add option block + compare item)");
		bool bl_required = false;

		if(p_item->GetItemClass() != ITEM_CLASS_COUPLE_RING && p_item->GetItemClass() != ITEM_CLASS_VAMPIRE_COUPLE_RING && p_item->GetItemClass() != ITEM_CLASS_OUSTERS_HARMONIC_PENDENT &&
			!p_item->IsQuestItem())
		{
			
			if(p_item->IsIdentified())
			{
				if(p_item->IsGearItem())
				{
					// FAZ 4 (2026-05-11) Bug 10: karakterin mevcut stat'i ile karsilastir
					// requirement'i karsilamiyorsa value KIRMIZI + "(you: NN)" suffix
					// 3 irk pariteti: STR/DEX/INT/SUM = Slayer+Ousters; Level = Vampire+Ousters
					const int dbgMy_STR   = (int)g_char_slot_ingame.STR_CUR;
					const int dbgMy_DEX   = (int)g_char_slot_ingame.DEX_CUR;
					const int dbgMy_INT   = (int)g_char_slot_ingame.INT_CUR;
					const int dbgMy_SUM   = dbgMy_STR + dbgMy_DEX + dbgMy_INT;
					const int dbgMy_LEVEL = (int)g_char_slot_ingame.level;

					// Required
					if(p_item->IsSlayerItem() || p_item->IsOustersItem())
					{
						char sz_and[] = ",";
						if (p_item->GetRequireSTR() > 0)
						{
							if(!bl_required)
								vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							else
								vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
							bl_required = true;

							vx = g_PrintColorStr(vx, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_STR].GetString(), gpC_base->m_item_desc_pi, required_rgb);
							// FAZ 4 (2026-05-05): Sabit format — game string corrupt format string riski
							// FAZ 4 (2026-05-11) Bug 10: STR yetersizse value KIRMIZI + "(you: NN)"
							{
								const int reqSTR = (int)p_item->GetRequireSTR();
								const bool bMeet = (dbgMy_STR >= reqSTR);
								if (bMeet)
									snprintf(sz_buf, sizeof(sz_buf), " %d", reqSTR);
								else
									snprintf(sz_buf, sizeof(sz_buf), " %d (you: %d)", reqSTR, dbgMy_STR);
								vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bMeet ? RGB_GREEN : RGB_RED);
							}
							
							if (p_item->GetRequireDEX() > 0 ||
								p_item->GetRequireINT() > 0 ||
								//					p_item->GetRequireLevel() > 0 ||
								(p_item->GetRequireSUM() > 0 && p_item->GetRequireSTR() < p_item->GetRequireSUM()))
								g_PrintColorStr(vx+3, py, sz_and, gpC_base->m_item_desc_pi, required_rgb);
							
							py += SMALL_FONT_Y_GAP;
						}
						
						if (p_item->GetRequireDEX() > 0)
						{
							if(!bl_required)
								vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							else
								vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
							bl_required = true;

							vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DEX].GetString(), gpC_base->m_item_desc_pi, required_rgb);
							// FAZ 4 (2026-05-11) Bug 10: DEX yetersizse value KIRMIZI + "(you: NN)"
							{
								const int reqDEX = (int)p_item->GetRequireDEX();
								const bool bMeet = (dbgMy_DEX >= reqDEX);
								if (bMeet)
									snprintf(sz_buf, sizeof(sz_buf), " %d", reqDEX);
								else
									snprintf(sz_buf, sizeof(sz_buf), " %d (you: %d)", reqDEX, dbgMy_DEX);
								vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bMeet ? RGB_GREEN : RGB_RED);
							}

							if (p_item->GetRequireINT() > 0 ||
								//					p_item->GetRequireLevel() > 0 ||
								(p_item->GetRequireSUM() > 0 && p_item->GetRequireDEX() < p_item->GetRequireSUM()))
								g_PrintColorStr(vx+3, py, sz_and, gpC_base->m_item_desc_pi, required_rgb);

							py += SMALL_FONT_Y_GAP;
						}
						
						if (p_item->GetRequireINT() > 0)
						{
							if(!bl_required)
								vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							else
								vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
							bl_required = true;

							vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_INT].GetString(), gpC_base->m_item_desc_pi, required_rgb);
							// FAZ 4 (2026-05-11) Bug 10: INT yetersizse value KIRMIZI + "(you: NN)"
							{
								const int reqINT = (int)p_item->GetRequireINT();
								const bool bMeet = (dbgMy_INT >= reqINT);
								if (bMeet)
									snprintf(sz_buf, sizeof(sz_buf), " %d", reqINT);
								else
									snprintf(sz_buf, sizeof(sz_buf), " %d (you: %d)", reqINT, dbgMy_INT);
								vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bMeet ? RGB_GREEN : RGB_RED);
							}

							if (//p_item->GetRequireLevel() > 0 ||
								(p_item->GetRequireSUM() > 0 && p_item->GetRequireINT() < p_item->GetRequireSUM()))
								g_PrintColorStr(vx, py, sz_and, gpC_base->m_item_desc_pi, required_rgb);

							py += SMALL_FONT_Y_GAP;
						}
						
						if (p_item->GetRequireSUM() > 0 &&
							p_item->GetRequireSUM() > p_item->GetRequireSTR() &&
							p_item->GetRequireSUM() > p_item->GetRequireDEX() &&
							p_item->GetRequireSUM() > p_item->GetRequireINT() )
						{
							if(!bl_required)
								vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							else
								vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
							bl_required = true;

							vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ALL_STAT_SUM].GetString(), gpC_base->m_item_desc_pi, required_rgb);
							// FAZ 4 (2026-05-11) Bug 10: SUM yetersizse value KIRMIZI + "(you: NN)"
							{
								const int reqSUM = (int)p_item->GetRequireSUM();
								const bool bMeet = (dbgMy_SUM >= reqSUM);
								if (bMeet)
									snprintf(sz_buf, sizeof(sz_buf), " %d", reqSUM);
								else
									snprintf(sz_buf, sizeof(sz_buf), " %d (you: %d)", reqSUM, dbgMy_SUM);
								vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bMeet ? RGB_GREEN : RGB_RED);
							}
							
							//				if (p_item->GetRequireLevel() > 0)
							//					g_PrintColorStr(vx+3, py, sz_and, gpC_base->m_item_desc_pi, required_rgb);
							
							py += SMALL_FONT_Y_GAP;
						}
					}
					if(p_item->IsVampireItem() || p_item->IsOustersItem())
					{
						if (p_item->GetRequireLevel() > 0)
						{
							if(!bl_required)
								vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
							else
								vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
							bl_required = true;
							
							vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString(), gpC_base->m_item_desc_pi, required_rgb);
							// FAZ 4 (2026-05-06): 5. atlanmış Requirement render site.
							// gameString format'ı bozuk → garbage hex/float dump
							// (örn. "0:1-a2f0a000000001p-534"). Önceki 4 site (STR/DEX/INT/SUM)
							// snprintf+" %d" ile düzeltildi; bu 5. (Level) atlandı.
							// FAZ 4 (2026-05-11) Bug 10: Level yetersizse value KIRMIZI + "(you: NN)"
							{
								const int reqLVL = (int)p_item->GetRequireLevel();
								const bool bMeet = (dbgMy_LEVEL >= reqLVL);
								if (bMeet)
									snprintf(sz_buf, sizeof(sz_buf), " %d", reqLVL);
								else
									snprintf(sz_buf, sizeof(sz_buf), " %d (you: %d)", reqLVL, dbgMy_LEVEL);
								vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, bMeet ? RGB_GREEN : RGB_RED);
							}
							
							py += SMALL_FONT_Y_GAP;
						}
					}
					// 2005, 1, 18, sobeit add start - �ʿ� ���� ����
					int RequireAdvancementLevel = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].GetRequireAdvancementLevel();
					if( RequireAdvancementLevel > 0 )
					{
						if(!bl_required)
							vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
						else
							vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
						bl_required = true;
						//by viva
						// FAZ 4 (2026-05-06): garbage hex/float dump (örn. "0:1-a2f0a..."
						// "%a format çıktısı") AdvancementLevel render'da. Kök sebep:
						// game string ID UI_STRING_MESSAGE_REQUIRE_ADVANCEMENT_LEVEL_0+LevelGrade
						// table'da yok veya bozuk format ("%a" gibi karakter içerir).
						// Defansif fallback: format'ta "%d" varsa kullan, yoksa hardcoded.
#ifdef __DESIGNED_CHS
						int LevelGrade = (RequireAdvancementLevel)/10;
						int LevelUnits = (RequireAdvancementLevel)%10;
						{
							int sid = UI_STRING_MESSAGE_REQUIRE_ADVANCEMENT_LEVEL_0 + LevelGrade;
							const char* fmt = (g_pGameStringTable && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
							                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
							if (fmt != NULL) {
								const char* pct = strchr(fmt, '%');
								if (pct != NULL && pct[1] == 'd') {
									snprintf(sz_buf, sizeof(sz_buf), fmt, LevelUnits);
								} else {
									snprintf(sz_buf, sizeof(sz_buf), " Adv %d.%d", LevelGrade, LevelUnits);
								}
							} else {
								snprintf(sz_buf, sizeof(sz_buf), " Adv %d.%d", LevelGrade, LevelUnits);
							}
						}
#else
						int LevelGrade = (RequireAdvancementLevel-1)/10;
						int LevelUnits = (RequireAdvancementLevel-1)%10;
						{
							int sid = UI_STRING_MESSAGE_REQUIRE_ADVANCEMENT_LEVEL_0 + LevelGrade;
							const char* fmt = (g_pGameStringTable && sid >= 0 && sid < (int)(*g_pGameStringTable).GetSize())
							                  ? (*g_pGameStringTable)[sid].GetString() : NULL;
							if (fmt != NULL) {
								const char* pct = strchr(fmt, '%');
								if (pct != NULL && pct[1] == 'd') {
									snprintf(sz_buf, sizeof(sz_buf), fmt, LevelUnits+1);
								} else {
									snprintf(sz_buf, sizeof(sz_buf), " Adv %d.%d", LevelGrade, LevelUnits+1);
								}
							} else {
								snprintf(sz_buf, sizeof(sz_buf), " Adv %d.%d", LevelGrade, LevelUnits+1);
							}
						}
#endif
						//end

						if(g_char_slot_ingame.m_AdvancementLevel >= RequireAdvancementLevel)
							vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_GREEN);
						else
							vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_RED);
						
						py += SMALL_FONT_Y_GAP;

					}
					// 2005, 1, 18, sobeit add end - �ʿ� ���� ����
				}
				
				bool bSlayer = !p_item->IsSlayerItem() && g_eRaceInterface == RACE_SLAYER;
				bool bVampire = !p_item->IsVampireItem() && g_eRaceInterface == RACE_VAMPIRE;
				bool bOusters = !p_item->IsOustersItem() && g_eRaceInterface == RACE_OUSTERS;
//					if(!p_item->IsSlayerItem() && g_eRaceInterface == RACE_SLAYER ||
//						!p_item->IsVampireItem() && g_eRaceInterface == RACE_VAMPIRE ||
//						!p_item->IsOustersItem() && g_eRaceInterface == RACE_OUSTERS
//						)
				if( bSlayer||bVampire||bOusters)
				{
					if(!bl_required)
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					else
						vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
					bl_required = true;
					
					vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OTHER_TRIBE].GetString(), gpC_base->m_item_desc_pi, required_rgb);
					py += SMALL_FONT_Y_GAP;
				}	
				
				if (p_item->IsGenderForMale())
				{
					if(!bl_required)
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					else
						vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
					bl_required = true;
					
					vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ONLY_MALE].GetString(), gpC_base->m_item_desc_pi, required_rgb);
					py += SMALL_FONT_Y_GAP;
				}
				else if (p_item->IsGenderForFemale())
				{
					if(!bl_required)
						vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
					else
						vx = px + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString(), gpC_base->m_item_desc_pi.hfont);
					bl_required = true;
					
					vx = g_PrintColorStr(vx, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ONLY_FEMALE].GetString(), gpC_base->m_item_desc_pi, required_rgb);
					py += SMALL_FONT_Y_GAP;
				}
			}
			else
			{
				sprintf(sz_buf,"%sUI_STRING_MESSAGE_DESC_REQUIRE",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString());
				vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				py += SMALL_FONT_Y_GAP;
			}
		}
	}

//	if(p_item->GetItemClass() != ITEM_CLASS_EVENT_GIFT_BOX)
	{
		
		// Price
		// by csm

		if(gC_vs_ui.IsRunningPersnalShop() && p_item->GetPersnalPrice() !=-1 ) 
		{
			
			std::string sstr;
			//vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB); �����ǽ�ң������޸�Ϊ���
			vx = g_PrintColorStr(px, py, "Voucher price:", gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			
			if(p_item->IsPileItem() && p_item->GetNumber() >= 1)
			{
				//int price = max( 0, g_pPriceManager->GetItemPrice(p_item, MPriceManager::PC_TO_NPC) );
				int TempPrice = p_item->GetPersnalPrice(); 
				
				if( g_pTimeItemManager->IsExist( p_item->GetID() ) )
					TempPrice = 50;
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
				if(p_item->GetCashItem())
					TempPrice = 1;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
				if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
				{
					sstr = "$";
					sstr += g_GetStringByMoney(TempPrice);
					vx = g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
					sstr = TempPrice/p_item->GetNumber();
					sprintf(sz_buf, "(%sx%d)", sstr.c_str(), max(0, p_item->GetNumber()));
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
				}
				else
				{
					sprintf(sz_buf, "%d", TempPrice);
					std::string sstr = sz_buf;
					for(int i = 3; i <= 13; i += 4)
						if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
					sprintf(sz_buf, "$%s", sstr.c_str());
					vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
					sprintf(sz_buf, "(%dx%d)", TempPrice/p_item->GetNumber(),max(0, p_item->GetNumber()));
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
				}
			}
			else 
			{
				int TempPrice = g_pTimeItemManager->IsExist(p_item->GetID()) ? 50 : max( 0, p_item->GetPersnalPrice() );
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
				if(p_item->GetCashItem())
					TempPrice = 1;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM


				if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
				{
					sstr = "$";
					sstr += g_GetStringByMoney(TempPrice);
					g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
				}
				else
				{
					sprintf(sz_buf, "%d", TempPrice);

					sstr = sz_buf;
					for(int i = 3; i <= 13; i += 4)
						if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
					sprintf(sz_buf, "$%s", sstr.c_str());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				}
			}
			
			py += SMALL_FONT_Y_GAP;
		}


		if (gbl_sell_running == true)
		{
			assert(gbl_buy_running == false);
			
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			
			if(p_item->IsPileItem() && p_item->GetNumber() >= 1)
			{
				int TempPrice = max( 0, g_pPriceManager->GetItemPrice(p_item, MPriceManager::PC_TO_NPC) );
				
				if( g_pTimeItemManager->IsExist( p_item->GetID() ) )
					TempPrice = 50;
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
				if(p_item->GetCashItem())
					TempPrice = 1;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
				std::string sstr;
				if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
				{
					sstr = "$";
					sstr += g_GetStringByMoney(TempPrice*max(0,p_item->GetNumber()));
					vx = g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
					sstr = g_GetStringByMoney(TempPrice);
					sprintf(sz_buf, "(%sx%d)", sstr.c_str(), max(0, p_item->GetNumber()));
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
				}
				else
				{
					sprintf(sz_buf, "%d", TempPrice*max(0,p_item->GetNumber()));
					sstr = sz_buf;
					for(int i = 3; i <= 13; i += 4)
						if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
					sprintf(sz_buf, "$%s", sstr.c_str());
					vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
					
					sprintf(sz_buf, "(%dx%d)", TempPrice,max(0, p_item->GetNumber()));
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
				}
			}
			else 
			{
				std::string sstr;
				int TempPrice = g_pTimeItemManager->IsExist(p_item->GetID()) ? 50 : max( 0, g_pPriceManager->GetItemPrice(p_item, MPriceManager::PC_TO_NPC)) ;
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
				if(p_item->GetCashItem())
					TempPrice = 1;
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
				if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
				{
					sstr = "$";
					sstr += g_GetStringByMoney(TempPrice);
					g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
				}
				else
				{
					sprintf(sz_buf, "%d", TempPrice);
					sstr = sz_buf;
					for(int i = 3; i <= 13; i += 4)
						if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
					sprintf(sz_buf, "$%s", sstr.c_str());
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				}
			}
			
			py += SMALL_FONT_Y_GAP;
		}
		
		if (gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID() ) )
		{
			assert(gbl_sell_running == false);			
			// check mysterious
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			if(C_VS_UI_SHOP::m_shop_type == MShop::SHOP_EVENT_STAR && left)
			{
				STAR_ITEM_PRICE price;
				g_pPriceManager->GetItemPrice(p_item, price);
				
				if(price.type != -1 && price.number > 0)
				{
					wsprintf(sz_buf, "%s %d%s", (*g_pItemTable)[ITEM_CLASS_EVENT_STAR][price.type].HName.GetString(), max( 0, price.number) , (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_NUMBER].GetString());
				}
				else
				{
					DescSafeSprintf0(sz_buf, sizeof(sz_buf), STRING_MESSAGE_CANNOT_BUY, "");
				}
			}
			else
			{
//				sprintf(sz_buf, "%d", g_pPriceManager->GetItemPrice(p_item, MPriceManager::NPC_TO_PC, C_VS_UI_SHOP::m_bl_mysterious_tab));
				if(p_item->IsPileItem() && p_item->GetNumber() >= 1)
				{
					int TempPrice = max( 0, g_pPriceManager->GetItemPrice(p_item, MPriceManager::NPC_TO_PC) );
					std::string sstr;
					if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
					{
						sstr = "$";
						sstr += g_GetStringByMoney(TempPrice*p_item->GetNumber());
						vx = g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
						sstr = g_GetStringByMoney(TempPrice);
						sprintf(sz_buf, "(%sx%d)", sstr.c_str(), max(p_item->GetNumber(),0) );
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
					}
					else
					{
						sprintf(sz_buf, "%d", TempPrice*p_item->GetNumber());
						sstr = sz_buf;
						for(int i = 3; i <= 13; i += 4)
							if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
						sprintf(sz_buf, "$%s", sstr.c_str());
						vx = g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
						sprintf(sz_buf, "(%dx%d)", TempPrice, max(p_item->GetNumber(),0) );
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, required_rgb);
					}
				}
				else 
				{
					std::string sstr;
					int TempPrice = max( 0, g_pPriceManager->GetItemPrice(p_item, MPriceManager::NPC_TO_PC) );
					if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
					{
						sstr = "$";
						sstr += g_GetStringByMoney(TempPrice);
						g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
					}
					else
					{
						sprintf(sz_buf, "%d", TempPrice);
						sstr = sz_buf;
						for(int i = 3; i <= 13; i += 4)
							if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
						sprintf(sz_buf, "$%s", sstr.c_str());
						g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
					}
				}
			}
			
			py += SMALL_FONT_Y_GAP;

			//if ((gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID())) && p_item->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM)
			if (p_item->GetItemClass() == ITEM_CLASS_SLAYER_PORTAL_ITEM)
			{
				if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5))
				{
					vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_CHARGE_DESC].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
					py += SMALL_FONT_Y_GAP;
				}

			}

		}
		
		if (gbl_repair_running == true &&
			!g_pTimeItemManager->IsExist( p_item->GetID() ) &&
			//p_item->GetItemClass() != ITEM_CLASS_VAMPIRE_AMULET &&
			//p_item->GetItemClass() != ITEM_CLASS_CORE_ZAP &&
			//p_item->GetItemClass() != ITEM_CLASS_ETHEREAL_CHAIN
			p_item->IsRepairableItem()	&&	
			!p_item->IsUniqueItem() &&
			!p_item->IsQuestItem()
			)
		{
			assert(gbl_buy_running == false);
			assert(gbl_sell_running == false);
			
			DWORD repairPrice = g_pPriceManager->GetItemPrice(p_item, MPriceManager::REPAIR);
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
			MPriceManager::REPAIRPIRCEPAIR repairPricePair(g_pPriceManager->GetRepairPricePair(p_item->GetID())); 
			repairPrice = repairPricePair.first;
			DWORD repairPriceDC = repairPricePair.second;
#endif //__REPAIRSYSTEM_RENEWAL
			if(p_item->IsChargeItem())
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CHARGE_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			else
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REPAIR_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

			std::string sstr;   
			if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
			{
				sstr = "$";
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
				if(!g_pPriceManager->IsRepairPriceListHave(p_item->GetID()))
					sstr += "?????????";
				else
#endif //__REPAIRSYSTEM_RENEWAL
					sstr += g_GetStringByMoney(repairPrice);
				g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
			}
			else
			{
				sprintf(sz_buf, "%d", repairPrice);
				std::string sstr;
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
				if(!g_pPriceManager->IsRepairPriceListHave(p_item->GetID()))
					sstr += "?????????";
				else
#endif //__REPAIRSYSTEM_RENEWAL
					sstr = sz_buf;
				// ���ڸ����� �޸� ���
				for(int i = 3; i <= 13; i += 4)
					if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
				sprintf(sz_buf, "$%s", sstr.c_str());
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			}			
			py += SMALL_FONT_Y_GAP;

			//���ΰ�
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_DC_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

			if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
			{
				sstr = "$";
				if(!g_pPriceManager->IsRepairPriceListHave(p_item->GetID()))
					sstr += "IsRepairPriceListHave1";
				else

				sstr += g_GetStringByMoney(repairPriceDC);
				g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
			}
			else
			{
				sprintf(sz_buf, "%d", repairPriceDC);
				std::string sstr;
				if(!g_pPriceManager->IsRepairPriceListHave(p_item->GetID()))
					sstr += "IsRepairPriceListHave2";
				else
					sstr = sz_buf;
				// ���ڸ����� �޸� ���
				for(int i = 3; i <= 13; i += 4)
					if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
				sprintf(sz_buf, "$%s", sstr.c_str());
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			}			
			py += SMALL_FONT_Y_GAP;
#endif //#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)

		}
		
		if (gbl_silvering_running == true)
		{
			assert(gbl_buy_running == false);
			assert(gbl_sell_running == false);
			assert(gbl_repair_running == false);			
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_SILVERING_PRICE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

			std::string sstr;
			int TempPrice = g_pPriceManager->GetItemPrice(p_item, MPriceManager::SILVERING);
			if(gC_ci->IsKorean() && g_pUserOption->ShowGameMoneyWithHANGUL)
			{
				sstr = "$";
				sstr += g_GetStringByMoney(TempPrice);
				g_PrintColorStr(vx, py, sstr.c_str(), gpC_base->m_item_desc_pi, RGB_WHITE);
			}	
			else
			{
				sprintf(sz_buf, "%d", max( 0, TempPrice));
				std::string sstr = sz_buf;
				for(int i = 3; i <= 13; i += 4)
					if(sstr.size() > i)sstr.insert(sstr.size()-i, ",");
				sprintf(sz_buf, "$%s", sstr.c_str());
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			}			
			py += SMALL_FONT_Y_GAP;
		}
		
	}

	CT_ITEM("ItemDescShow: Marker R - pre Description text print, g_pItemTable=%p class=%d type=%d (SUSPECT: array OOB for EVENT_ETC?)", (void*)g_pItemTable, (int)p_item->GetItemClass(), (int)p_item->GetItemType());
//	MString temp_string = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description;
	if((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetLength() != 0)
	{
		CT_ITEM("ItemDescShow: Marker R2 - inside Description block (table access OK, GetLength != 0)");
		int descriptionLines = 1;
		if(p_item->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE_SIGN)
		{
			int bibleType = p_item->GetItemType();
			if(bibleType < 12)
				g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibleType].GetString(), gpC_base->m_item_desc_pi, required_rgb);
			else if(bibleType > 11 && bibleType < 85)
			{
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
				char biblebuffer[128];
				sprintf(biblebuffer, "%s %s",(*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibleType%12].GetString(),
					(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_BONUS_GRUN_ARMEGA+(bibleType-12)].GetString());

				g_PrintColorStr(px, py, biblebuffer, gpC_base->m_item_desc_pi, required_rgb);
#endif //__CONTRIBUTE_SYSTEM
			}
		}
		else
		{
			descriptionLines = DescPrintFormattedDescriptionLines(px, py, (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetString(), gpC_base->m_item_desc_pi, required_rgb);
			if (descriptionLines < 1)
				descriptionLines = 1;
		}
		py += descriptionLines * SMALL_FONT_Y_GAP;
	}

	Race race = g_pPlayer->GetRace();
	int nEquipDescriptCount = 0;

	if ( nEquipSetID != -1 )//��ʾ��װ��Ϣ
	{
		int savePX = px;
		int savePY = py;
		//g_PrintColorStr(px, py, "��װ����:", gpC_base->m_item_desc_pi, RGB(0, 255, 0));
		py += SMALL_FONT_Y_GAP;

		int nSize = g_vecEquipSetList[nEquipSetID].size();
		MPlayerGear* pGear = g_pPlayer->GetGear();

		if ( nSize == 1 )
		{
			for ( int z=0; z<MAX_EQUIP_DESCRIP; z++ )
			{
				if ( g_strEquipDescription[nEquipSetID][z].c_str() > 0 )
				g_PrintColorStr(px, py, g_strEquipDescription[nEquipSetID][z].c_str(),gpC_base->m_item_desc_pi, RGB(165, 52, 247));
				py += SMALL_FONT_Y_GAP;
			}
		}
		else
		{
			py += SMALL_FONT_Y_GAP;

			for (int i=0; i<nSize; i++)
			{
				stEquipUnit stUnit = g_vecEquipSetList[nEquipSetID][i];
				int nGearSize = pGear->GetSize();
				MItem* pGearItem = NULL;
				const char* setItemName = DescGetItemTableHName(stUnit.itemClass, stUnit.itemType);

				if (setItemName != NULL)
				{
					for ( int x=0; x<nGearSize; x++ )
					{
						pGearItem = pGear->GetItem(x);
						if ( pGearItem && pGearItem->GetItemClass() == stUnit.itemClass && pGearItem->GetItemType() == stUnit.itemType )
						{
							nEquipDescriptCount++;
							break;
						}
					}
				}

				if (setItemName == NULL)
				{
					char missingEquipMember[64];
					DescFormatMissingEquipSetMember(missingEquipMember, sizeof(missingEquipMember), stUnit.itemClass, stUnit.itemType);
					g_PrintColorStr(px, py, missingEquipMember, gpC_base->m_item_desc_pi, RGB(255, 80, 80));
				}
				else if ( pGearItem ) //��װ��
					g_PrintColorStr(px, py, setItemName, gpC_base->m_item_desc_pi, RGB(255, 255, 255));
				else   //ûװ��
					g_PrintColorStr(px, py, setItemName, gpC_base->m_item_desc_pi, RGB_DARKGRAY);

				py += SMALL_FONT_Y_GAP;
			}

			char buff[125];
			sprintf(buff, "%s Set : (%d/6)",g_strEquipName[nEquipSetID].c_str(), nEquipDescriptCount);
			g_PrintColorStr(savePX, savePY, buff, gpC_base->m_item_desc_pi, RGB(156, 40, 206));

			for ( int z=0; z<MAX_EQUIP_DESCRIP; z++ )
			{
				if ( z+1 < nEquipDescriptCount )
					g_PrintColorStr(px, py, g_strEquipDescription[nEquipSetID][z].c_str(),gpC_base->m_item_desc_pi, RGB(0, 255, 0));
				else
					g_PrintColorStr(px, py, g_strEquipDescription[nEquipSetID][z].c_str(),gpC_base->m_item_desc_pi, RGB_DARKGRAY);
				py += SMALL_FONT_Y_GAP;
			}
		}
		

	}
	


	if(pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && p_item->IsGearItem())
	{
		const std::list<TYPE_ITEM_OPTION> &optionList = p_item->GetItemOptionList();
		std::list<TYPE_ITEM_OPTION>::const_iterator itr = optionList.begin();	
		
		bool bCanUpgrade = false;
	
		if(p_item->IsUniqueItem() || p_item->IsQuestItem())
		{
			bCanUpgrade = false;
		}
		else		
		if(pMouseItem->GetItemType() == 6)
		{
			bCanUpgrade = false;
		}
		else		//������ ������ 2
		if(pMouseItem->GetItemType() == 7 || pMouseItem->GetItemType() == 15  || pMouseItem->GetItemType() == 78  || pMouseItem->GetItemType() == 79)
		{
			//if(p_item->GetItemOptionListCount() == 1)
			const std::list<TYPE_ITEM_OPTION> &optionList = p_item->GetItemOptionList();
			std::list<TYPE_ITEM_OPTION>::const_iterator itr = optionList.begin();	
			while(itr!=optionList.end())
			{
				if((*g_pItemOptionTable)[*itr].UpgradeOptionType != 0)
				{
				bCanUpgrade = true;
					break;
				}
				itr++;
			}	
		}
		else		
		if(pMouseItem->GetItemType() >= 8 && pMouseItem->GetItemType() <=14	&& pMouseItem->GetItemType() != 12	||
			pMouseItem->GetItemType() >= 17 && pMouseItem->GetItemType() <= 21	||
			pMouseItem->GetItemType() >= 49 && pMouseItem->GetItemType() <= 53 	|| 
			pMouseItem->GetItemType() >= 63 && pMouseItem->GetItemType() <= 67 	|| 
			pMouseItem->GetItemType() == 59)
		{
			if(p_item->GetItemOptionListCount() == 0)
				bCanUpgrade = true;
		}
		else
		if(pMouseItem->GetItemType() == 12 || pMouseItem->GetItemType() == 82)					// ���� ����� �ɼ� �ΰ���
		{
			if(p_item->GetItemOptionListCount() != 2)
				bCanUpgrade = false;
			else
			{
				bCanUpgrade = true;
				itr = optionList.begin();
				while(itr != optionList.end() )
				{
					if((*g_pItemOptionTable)[*itr].UpgradeOptionType == 0)
					{
						bCanUpgrade = false;
						break;
					}
					itr++;
				}
				
			}
		}
		else
		if(pMouseItem->GetItemType() == 16 )			// Ʈ���� ������ ŰƮ
		{
			if(!p_item->IsGenderForAll())
				bCanUpgrade = true;
			else
				bCanUpgrade = false;
		}
		else
		if( ((pMouseItem->GetItemType() >= 26) && (pMouseItem->GetItemType() <= 31)) ||
			 ((pMouseItem->GetItemType() >= 38) && (pMouseItem->GetItemType() <= 46))	
			)
		{
			if(!p_item->GetItemOptionListCount())
				bCanUpgrade = true;
		}
		else
		if(pMouseItem->GetItemType() == 47)
		{
			bCanUpgrade = false;
		}
		//2006.05.24 sjheon Add
		else if( pMouseItem->GetItemType() >= 32 && pMouseItem->GetItemType() <= 37 )
		{
			if( p_item->GetItemClass() == ITEM_CLASS_RING && p_item->GetItemOptionListCount() == 0)
				bCanUpgrade = true;
		}
		//2006.05.24 sjheon End
		else
		if( pMouseItem->GetItemType() == 22) // ���ο� ���
		{
			ITEM_CLASS curitemclass = p_item->GetItemClass();
			if( curitemclass != ITEM_CLASS_CORE_ZAP			&&
				curitemclass != ITEM_CLASS_ETHEREAL_CHAIN	&&
				curitemclass != ITEM_CLASS_CUE_OF_ADAM		&&
				p_item->GetGrade() > 0 &&  p_item->GetGrade() < 6)
				bCanUpgrade = true;
		} 
		else
		if(pMouseItem->GetItemType() == 48) // ���ο� ���
		{
			ITEM_CLASS curitemclass = p_item->GetItemClass();
			if( curitemclass != ITEM_CLASS_CORE_ZAP			&&
				curitemclass != ITEM_CLASS_ETHEREAL_CHAIN	&&
				curitemclass != ITEM_CLASS_CUE_OF_ADAM		&&
				p_item->GetGrade() > 0 &&  p_item->GetGrade() < 8)
				bCanUpgrade = true;
		}
		else
		if (pMouseItem->GetItemType() == 57 || pMouseItem->GetItemType() == 69)
		{
			ITEM_CLASS curitemclass = p_item->GetItemClass();
			if( curitemclass != ITEM_CLASS_CORE_ZAP			&&
				curitemclass != ITEM_CLASS_ETHEREAL_CHAIN	&&
				curitemclass != ITEM_CLASS_CUE_OF_ADAM		&&
				p_item->GetGrade() > 0 &&  p_item->GetGrade() < 10)
				bCanUpgrade = true;
		}
		else
			if (pMouseItem->GetItemType() == 91)
			{
				ITEM_CLASS curitemclass = p_item->GetItemClass();
				if( curitemclass == ITEM_CLASS_CORE_ZAP	&& p_item->GetGrade() > 0 &&  p_item->GetGrade() < 50)
					bCanUpgrade = true;
			}	
		else
		if(pMouseItem->GetItemType() >= 54 && pMouseItem->GetItemType() <= 56
#if __CONTENTS(__THIRD_ENCHANT_2)
			|| pMouseItem->GetItemType() >= 60 && pMouseItem->GetItemType() <= 62
			|| pMouseItem->GetItemType() >= 70 && pMouseItem->GetItemType() <= 72
#endif //__THIRD_ENCHANT_2
			)
		{
			int itmeType = pMouseItem->GetItemType();
			switch(itmeType)
			{
				case 54:
				case 60:
				case 70:
					if (p_item->IsBasicWeapon() && p_item->IsMeleeWeapon() && 
						(p_item->GetItemOptionListCount() == 2))
						bCanUpgrade = true;
					break;
				case 55:
				case 61:
				case 71:
					if (p_item->IsBasicWeapon() && p_item->IsMagicWeapon()&& 
						(p_item->GetItemOptionListCount() == 2))
						bCanUpgrade = true;
					break;
				case 56:
				case 62:
				case 72:
					if (!p_item->IsBasicWeapon() && p_item->IsCanAddThirdOptionItem()&& 
						(p_item->GetItemOptionListCount() == 2))
						bCanUpgrade = true;
					break;
			}
		}
#if	__CONTENTS (__2008_FIRST_CHARGE_ITEM)
		else if(pMouseItem->GetItemType() == 58)		//��ˮ��58�� �����Կ���ʹ��
		{
			int itemGrade = p_item->GetGrade();
			int itemOptionCount = p_item->GetItemOptionListCount();
			if(	itemGrade > 0 &&  itemGrade < 10 &&
				itemOptionCount >= 0  && itemOptionCount <= 1)
				bCanUpgrade = true;
		}
		else if(pMouseItem->GetItemType() == 68 || pMouseItem->GetItemType() == 81)			//��ˮ��68�� 81�� ����2���Կ���ʹ��
		{
			int itemGrade = p_item->GetGrade();
			int itemOptionCount = p_item->GetItemOptionListCount();
			if(	itemGrade > 0 &&  itemGrade < 10 &&
				itemOptionCount == 2)
				bCanUpgrade = true;
		}
		else if(pMouseItem->GetItemType() == 95)		//
		{
			int itemGrade = p_item->GetGrade();
			int itemOptionCount = p_item->GetItemGTwoOptionListCount();
			if(	itemGrade > 0 &&  itemGrade < 10 &&
				itemOptionCount >= 0  && itemOptionCount <= 1)
				bCanUpgrade = true;
		}
#endif // __2008_FIRST_CHARGE_ITEM
		else
		{
			while(itr!=optionList.end())
			{
				if((*g_pItemOptionTable)[*itr].UpgradeOptionType != 0)
				{
					bCanUpgrade = true;
					break;
				}
				itr++;
			}
		}
		//else
		//if(p_item->GetItemOptionListCount()>1)
		//	bCanUpgrade = false;

		if(bCanUpgrade)
		{
			if(pMouseItem->GetNumber() != 1)
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_ONLY_PICK_UP_ITEM_ONE].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));
			else
			{
				if(pMouseItem->GetItemType() == 16 ) 
					g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_TRANS].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));				
//				else if( pMouseItem->GetItemType() >= 26 && pMouseItem->GetItemType() <= 31)
//					g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_ITEM_TO_ITEM_IMPOSIBLE].GetString(),gpC_base->m_item_desc_pi, RGB(255, 150, 150));
				else
				{
					g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_ENCHANT].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));
				}
			}
		}
		else
		{
			if( pMouseItem->GetItemType() == 16)
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_CANNOT_USE].GetString(),gpC_base->m_item_desc_pi, RGB(255, 150, 150));
//			else if( pMouseItem->GetItemType() >= 26 && pMouseItem->GetItemType() <= 31)
//				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_ENCHANT].GetString(),gpC_base->m_item_desc_pi, RGB(150, 150, 255));

			else
				g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_ITEM_TO_ITEM_IMPOSIBLE].GetString(), gpC_base->m_item_desc_pi, RGB(255, 150, 150));
		}

		py += SMALL_FONT_Y_GAP;
	}

	if( pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_PET_ENCHANT_ITEM &&
		p_item->GetItemClass() == ITEM_CLASS_PET_ITEM )
	{		
		MPetItem* pPetItem = (MPetItem*)p_item;
		int bCanUpgrade = 0;
		
		if((pMouseItem->GetItemType() < 13 || 
			pMouseItem->GetItemType() >= 16 && pMouseItem->GetItemType() <= 19) &&	// 12������ ���� ���
			pPetItem->GetSilver() == 0 &&		// silver�� AttrLevel�� 0�� ���� �Ӽ��� ���ٰ� ���� ��þƮ ����
			pPetItem->GetNumber() >= 10)		// �� ���� 10�̻��ΰ��
			bCanUpgrade = 1;
		
		if(pMouseItem->GetItemType() == 13 || pMouseItem->GetItemType() == 14)
		{
			if(pPetItem->GetCurrentDurability() == 0)		// ���� HP�� 0�϶�
				bCanUpgrade = 2;
			else
				bCanUpgrade = 3;
		}
		
		if(pMouseItem->GetItemType() == 15 && // 15�� ���� ����
			pPetItem->GetNumber() == 49 && pPetItem->GetItemOptionListCount() == 0)
			bCanUpgrade = 1;
		
		//if(pMouseItem->GetItemType() >= 22 || pMouseItem->GetItemType() <= 24)	// ������ �� ü���� ��� ����


		if(pMouseItem->GetItemType() >= 22 && pMouseItem->GetItemType() <= 25)

		{
			bCanUpgrade = 1;
		}

		if(PhoenixCanUseADeLantePetInjection(pMouseItem, pPetItem))
		{
			bCanUpgrade = 1;
		}


//		if( !pPetItem->IsCanGamble() )
//			bCanUpgrade = 0;
		
		switch(bCanUpgrade)
		{
		case 1:
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_ENCHANT].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));			
			py += SMALL_FONT_Y_GAP;
			break;

		case 2:
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_PET_REVIVAL].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));			
			py += SMALL_FONT_Y_GAP;
			break;

		case 3:
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_PET_REVIVAL].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));			
			py += SMALL_FONT_Y_GAP;
			break;

		default:
			g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_ITEM_TO_ITEM_IMPOSIBLE].GetString(), gpC_base->m_item_desc_pi, RGB(255, 150, 150));
			py += SMALL_FONT_Y_GAP;
			break;
		}
		
	}

#if __CONTENTS(__TUNING_ITEM)
	if(pMouseItem!=NULL&&pMouseItem->IsTuningItem())
	{
		int itemTclass = pMouseItem->GetTargetClass();
		int itemTType = pMouseItem->GetTargetType();

		int thisitem = p_item->GetItemClass();
		int thistype = p_item->GetItemType();

		BOOL b_item_Unique = p_item->IsUniqueItem();
		BOOL b_item_Quest = p_item->IsQuestItem();

		int AdvancementLevel = (*g_pItemTable)[pMouseItem->GetItemClass()][pMouseItem->GetItemType()].GetRequireAdvancementLevel();
		bool b_item_Advance = (AdvancementLevel!=0 && g_char_slot_ingame.m_AdvancementLevel < AdvancementLevel);

		//////////////////////////////////////////////////////////////////////////
		// ����� �䱸����
		bool b_item_NetCapacity = false;
//		char _race = (*g_pItemTable)[pMouseItem->GetItemClass()][pMouseItem->GetItemType()].Race;
		char _race = g_char_slot_ingame.Race;

		if(_race==RACE_SLAYER)
		{
			int str_cur = g_char_slot_ingame.STR_CUR;
			int dex_cur = g_char_slot_ingame.DEX_CUR;
			int int_cur = g_char_slot_ingame.INT_CUR;
			
			int sum = str_cur+dex_cur+int_cur;

			int reqSUM = (*g_pItemTable)[pMouseItem->GetItemClass()][pMouseItem->GetItemType()].GetRequireSUM();

			b_item_NetCapacity = (sum<reqSUM);
		}
		else if(_race==RACE_VAMPIRE||_race==RACE_OUSTERS)
		{
			int reqLevel = (*g_pItemTable)[pMouseItem->GetItemClass()][pMouseItem->GetItemType()].GetRequireLevel();
			b_item_NetCapacity = (g_char_slot_ingame.level<reqLevel);
		}
		//////////////////////////////////////////////////////////////////////////

		if(pMouseItem->GetNumber() != 1)
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_ONLY_PICK_UP_ITEM_ONE].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));
		}
		else
		{
			if(itemTclass==-1||itemTType==-1||thisitem==-1||thistype==-1||itemTclass!=thisitem||itemTType!=thistype||b_item_Unique||b_item_Quest||b_item_Advance||b_item_NetCapacity)
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_NOT_TUNING].GetString(), gpC_base->m_item_desc_pi, RGB(255, 150, 150));
			else
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_TUNING].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));
		}
		
		py += SMALL_FONT_Y_GAP;
	}
#endif	//__TUNING_ITEM

	if(pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM	&&
		( ( pMouseItem->GetItemType() >= 9	&& pMouseItem->GetItemType() <= 17 ) ||
		  ( pMouseItem->GetItemType() >= 19 && pMouseItem->GetItemType() <= 24 ) ||
		    pMouseItem->GetItemType() == 28 ) &&
		p_item != NULL && !p_item->IsEmptyItemOptionList() && !p_item->IsQuestItem() && !p_item->IsUniqueItem())
	{
		if(pMouseItem->GetNumber() != 1)
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_ONLY_PICK_UP_ITEM_ONE].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));
		else
		{
//			if((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetLength() != 0)
//				py += SMALL_FONT_Y_GAP;
			if( C_VS_UI_REMOVE_OPTION::IsCanRemoveOption_Puritas( pMouseItem, p_item ) )
			{
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_REMOVE_OPTION].GetString(), gpC_base->m_item_desc_pi, RGB(150, 150, 255));			
			} else
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSGAE_CANNOT_REMOVE_OPTION].GetString(), gpC_base->m_item_desc_pi, RGB(255, 150, 150));
		}
		py += SMALL_FONT_Y_GAP;
	}
	
	if(g_pTimeItemManager->IsExist( p_item->GetID() ) )
	{
		char	temp[256] = {0,};	
		bool	bContinue = false;
		TYPE_OBJECTID objectID = p_item->GetID();		
		
		int tempInt;
		vx=px;
		
		if( g_pTimeItemManager->GetDay( objectID ) == 0 && g_pTimeItemManager->GetHour (objectID ) ==0 &&
			g_pTimeItemManager->GetMinute( objectID ) == 0 )
		{
			if( g_pTimeItemManager->GetSecond( objectID ) == 0 )
			{
				strcpy(temp, (*g_pGameStringTable)[UI_STRING_MESSAGE_EXPIRED_ITEM].GetString() );
			} else
			{
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB );
				strcat(temp, " ");
				char tempSecond[10];
				wsprintf(tempSecond, (*g_pGameStringTable)[UI_STRING_MESSAGE_SECOND].GetString() , g_pTimeItemManager->GetSecond( objectID ) );
				strcat(temp, tempSecond );
			}
		} else
		{
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB );
			memset( temp, 0, sizeof( temp ) );

			if((tempInt = g_pTimeItemManager->GetDay( objectID ))>0  || bContinue )
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString() , tempInt );
				strcat ( temp, tempDay );
				bContinue = true;
			}
			if((tempInt = g_pTimeItemManager->GetHour( objectID ))>0 || bContinue)
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString() , tempInt );
				strcat ( temp, tempDay );
				bContinue = true;
			}
			if((tempInt = g_pTimeItemManager->GetMinute( objectID ))>0  || bContinue)
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString() , tempInt );
				strcat ( temp, tempDay );
				bContinue = true;
			}			
		}		
		g_PrintColorStr(vx, py, temp,gpC_base->m_item_desc_pi,RGB(255,255,255));
		py += SMALL_FONT_Y_GAP;
	}
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	else if( p_item->IsTimeLimitStatic() )
	{
		bool bContinue = false;		
		char temp[256];
		vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString(), gpC_base->m_item_desc_pi, RGB(192, 192, 255) );
		memset( temp, 0, sizeof( temp ) );
		
		DWORD TimeLimit = p_item->GetTimeLimitStatic();
		
		int days = TimeLimit / 60 / 60 / 24;
		int hours = (TimeLimit / 60 / 60) % 24;
		int minutes = (TimeLimit / 60 ) % 60;
		int seconds = TimeLimit % 60;
		
		
		if(days > 0|| bContinue )
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString() , days );
			strcat ( temp, tempDay );				
		}
		if(hours > 0|| bContinue)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString() , hours );
			strcat ( temp, tempDay );				
		}
		if(minutes || bContinue)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString() , minutes );
			strcat ( temp, tempDay );				
		}
		if(seconds)
		{
			char tempDay[30] = {0,};
			strcat(temp, " ");
			wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_SECOND].GetString() , seconds );
			strcat ( temp, tempDay );				
		}
		g_PrintColorStr(vx,py, temp,gpC_base->m_item_desc_pi,RGB(255,255,255));
		py += SMALL_FONT_Y_GAP;
	}
#endif //__PREMIUM_GIVE_ITEM_UI

	if(p_item->GetItemClass() == ITEM_CLASS_PET_FOOD)
	{
		PETINFO *pPetInfo = gC_vs_ui.GetMyPetInfo();
		if(pPetInfo != NULL && pPetInfo->EXIST)
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_USE_PET_FOOD].GetString(),gpC_base->m_item_desc_pi,RGB(255,255,255));
		}
		else
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DETACH_PET_FOOD].GetString(),gpC_base->m_item_desc_pi,RGB(255,255,255));
		}
		py += SMALL_FONT_Y_GAP;
	}
	
	CT_ITEM("ItemDescShow: Marker EXIT - pre g_FL2_ReleaseDC at function end");
	g_FL2_ReleaseDC();
	CT_ITEM("ItemDescShow: EXIT ok - function completed normally");
}

//-----------------------------------------------------------------------------
// _Skill_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_Skill_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption NULL guard.
	// 2026-05-22 FIX: void_ptr == NULL kosulu kaldirildi (Kural 4.3 ihlali olan
	// yuzeysel crash guard duzeltildi). _Skill_Description_Show void_ptr'i HIC
	// KULLANMIYOR — sadece "left" (skill ID) ile (*g_pSkillInfoTable)[left]
	// uzerinden calisir. Tum mesru cagiranlar bilerek NULL geciyor:
	//   - VS_UI_GameCommon.cpp:13426 (C_VS_UI_SKILL::MouseControl M_MOVING — F1-F12
	//     HotKey QuickSlot bar tooltip)
	//   - VS_UI_GameCommon.cpp:14258
	//   - VS_UI_GameCommon.cpp:14620
	// Eski guard tum mesru cagrilari reddederek crash_trace.log'da 134 SAFE-RETURN
	// entry uretiyordu — F1-F12 HotKey QuickSlot tooltip'leri ve diger skill
	// tooltip'leri (K menusu, skill info) gozukmuyordu. Sadece gercek bagimlilik
	// (g_pUserOption — DrawAlphaBox->ALPHA_DEPTH icin) kontrol ediliyor.
	if (g_pUserOption == NULL) { CT_Done("SkillDescShow: SAFE-RETURN g_pUO NULL"); return; }
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif

//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;

//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// level infomation
	//
	int domain_level;
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		if((*g_pSkillInfoTable)[left].DomainType >= 0)
			domain_level = (*g_pSkillManager)[(*g_pSkillInfoTable)[left].DomainType].GetDomainLevel();
		else domain_level = 0;
		break;
		
	case RACE_VAMPIRE:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel();
		break;
		
	case RACE_OUSTERS:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetDomainLevel();
		break;
	}

	const int advance_level		= g_char_slot_ingame.m_AdvancementLevel;
	const int sum_level			= domain_level + advance_level;

	const int skill_learn_level = (*g_pSkillInfoTable)[left].GetLearnLevel();

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	char sz_buf[50];

	g_FL2_GetDC();
	// name
	// option�� �پ������� �ٸ� ������ ����Ѵ�.
	const char *szSkillName;
	char szTemp2[10] = "???";
	bool bEnable = true;

	if( g_pSystemAvailableManager->GetLimitLearnSkillLevel() >= (*g_pSkillInfoTable)[left].GetLearnLevel() )
		// 2026-05-15: GetHName (Hangul) -> GetName (English); CLAUDE.md kural 3
		szSkillName = (*g_pSkillInfoTable)[left].GetName();
	else
	{
		szSkillName = szTemp2;
		bEnable = false;
	}

	g_PrintColorStr(px+1, py+1, szSkillName, gpC_base->m_item_name_pi, RGB(100,100,0));
	g_PrintColorStr(px, py, szSkillName, gpC_base->m_item_name_pi, RGB_WHITE);

	py += NORMAL_FONT_Y_GAP;
	std::string SkillEName;
	SkillEName = "(";
	if( bEnable )
		SkillEName += (*g_pSkillInfoTable)[left].GetName();
	else
		SkillEName += szTemp2;
	SkillEName += ")";

	g_PrintColorStr(px, py, SkillEName.c_str(), gpC_base->m_item_desc_pi, RGB_GRAY);
	py += NORMAL_FONT_Y_GAP;

	if( !bEnable )
	{
		g_FL2_ReleaseDC();
		return;
	}
	
	int vx;

	if(left == SKILL_LOVE_CHAIN)
	{
		vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_LOVE_CHAIN].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		
		g_FL2_ReleaseDC();
		return;
	}

	if (g_eRaceInterface != RACE_VAMPIRE && (!(*g_pSkillInfoTable)[left].IsPassive() || g_eRaceInterface == RACE_OUSTERS))
	{
		// level
#if __CONTENTS(__SECOND_RARE_SKILL)	
		int level = -1;		
		
		if(skill_learn_level > 150)
			level = min(max(sum_level - skill_learn_level + 1, 0), 10);
		else
			level = (*g_pSkillInfoTable)[left].GetExpLevel();

		if(level>0)
		{
			if(g_eRaceInterface == RACE_OUSTERS && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_OUSTERS_RARE
				|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
			{
			}
			else if(g_eRaceInterface == RACE_SLAYER && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_SLAYER_RARE
				|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
			{
			}
			else
			{
				sprintf(sz_buf,"%s: ",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString());
				vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				sprintf(sz_buf, "%d", level);
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				py += SMALL_FONT_Y_GAP;
			}
		}
#else
		sprintf(sz_buf,"%s: ",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString());
		vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		
		if(skill_learn_level > 150)
			sprintf(sz_buf, "%d", min(max(sum_level - skill_learn_level + 1, 0), 10));
		else
			sprintf(sz_buf, "%d", (*g_pSkillInfoTable)[left].GetExpLevel());

		g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);

		py += SMALL_FONT_Y_GAP;
#endif //__SECOND_RARE_SKILL
	}

	// MP
	if(!(*g_pSkillInfoTable)[left].IsPassive())
	{
			switch(g_eRaceInterface)
			{
			case RACE_SLAYER:
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_MP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				break;

			case RACE_VAMPIRE:
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_HP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				break;

			case RACE_OUSTERS:
				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_EP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				break;
			}

		if( left == SKILL_WILL_OF_LIFE )
			sprintf(sz_buf, "%d",5 + (g_char_slot_ingame.level / 7));
		else
			sprintf(sz_buf, "%d", (*g_pSkillInfoTable)[left].GetMP());

		//sprintf(sz_buf, "%d ~ %d", (*g_pSkillInfoTable)[left].GetMP() , (*g_pSkillInfoTable)[left].GetMP());

		g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
		py += SMALL_FONT_Y_GAP;
	}
	else
	{


		if(left >= SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA && left <= SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA)
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+left-SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		}

		else
		if(left >= SKILL_SWEEPER_BONUS_1 && left <= SKILL_SWEEPER_BONUS_12 )
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_SWEEPER_BONUS_1+(left-SKILL_SWEEPER_BONUS_1)].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		else
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_HAN_PASSIVE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		py += SMALL_FONT_Y_GAP;
	}

	if(g_eRaceInterface == RACE_OUSTERS)
	{
		SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[left];
		if(sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND
			)
		{
			int elemental_point[5] = { sInfo.Fire, sInfo.Water, sInfo.Earth, sInfo.Wind, sInfo.Sum };
			
			{
				// FAZ 4 (2026-05-06): ELEMENTAL_LEVEL — string + int. Defansif.
				int eSid = g_ELEMENTAL_STRING_ID[sInfo.ElementalDomain];
				const char* eStr = (g_pGameStringTable && eSid >= 0 && eSid < (int)(*g_pGameStringTable).GetSize())
				                   ? (*g_pGameStringTable)[eSid].GetString() : "";
				DescSafeSprintf1s1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_REQUIRE_ELEMENTAL_LEVEL,
				                    eStr ? eStr : "", (int)elemental_point[sInfo.ElementalDomain],
				                    "%s Lvl %d");
			}
			g_PrintColorStr(px, py, sz_buf, gpC_base->m_chatting_pi, g_ELEMENTAL_COLOR[sInfo.ElementalDomain]);
			py += SMALL_FONT_Y_GAP;
		}
	}
	//py += SMALL_FONT_Y_GAP;

	g_FL2_ReleaseDC();
}

//-----------------------------------------------------------------------------
// _BloodBible_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_BloodBible_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption/gpC_base NULL guard.
	if (void_ptr == NULL || g_pUserOption == NULL || gpC_base == NULL || gpC_base->m_p_DDSurface_back == NULL) {
		CT_Done("BloodBibleDescShow: SAFE-RETURN void_ptr=%p g_pUO=%p gpC_base=%p", void_ptr, (void*)g_pUserOption, (void*)gpC_base);
		return;
	}
	S_BLOOD_BIBLE_DESCRIPTION *desc = (S_BLOOD_BIBLE_DESCRIPTION*)void_ptr;
	COLORREF color = (COLORREF) right;
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);

	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
	gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);

	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;


	g_FL2_GetDC();
	// name
	// option�� �پ������� �ٸ� ������ ����Ѵ�.
	
	if(desc->sz_name != NULL)
	{
		int vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CHAR_MANAGER_NAME].GetString() , gpC_base->m_item_name_pi, RGB_YELLOW);
		g_PrintColorStr(vx, py, desc->sz_name , gpC_base->m_item_name_pi, RGB_YELLOW);
		py += NORMAL_FONT_Y_GAP;
	}

	if(desc->sz_place != NULL)
	{		
		int vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_POSITION].GetString(),gpC_base->m_item_desc_pi,ITEM_DESC_RGB);
		g_PrintColorStr(vx, py, desc->sz_place , gpC_base->m_item_desc_pi, RGB_WHITE);
		py += SMALL_FONT_Y_GAP;
	}
	
	if(desc->sz_shrine != NULL)
	{		
		int tempColor;
		int vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_SHRINE_RACE].GetString(), gpC_base->m_item_desc_pi,ITEM_DESC_RGB);
		if(strcmp(desc->sz_shrine, (*g_pGameStringTable)[STRING_MESSAGE_SLAYER].GetString()) == 0)
			tempColor = RGB(23,107,255);
		else if (strcmp( desc->sz_shrine, (*g_pGameStringTable)[STRING_MESSAGE_VAMPIRE].GetString()) == 0 )
			tempColor = RGB_RED;
		else tempColor = RGB_GREEN;

		g_PrintColorStr(vx, py, desc->sz_shrine , gpC_base->m_item_desc_pi, tempColor);
		py += SMALL_FONT_Y_GAP;
	}
	
	if(desc->sz_player != NULL)
	{		
		int vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_PLAYER].GetString(),gpC_base->m_item_desc_pi,ITEM_DESC_RGB);
		g_PrintColorStr(vx, py, desc->sz_player , gpC_base->m_item_desc_pi, MAGENTA);
		py += SMALL_FONT_Y_GAP;
	}
	
	if(desc->sz_option != NULL)
	{
		int vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(),gpC_base->m_item_desc_pi,ITEM_DESC_RGB);
		g_PrintColorStr(vx, py, desc->sz_option , gpC_base->m_item_desc_pi, RGB_GOLD);
		py += SMALL_FONT_Y_GAP;		
	}

	if(desc->sz_status != NULL)
	{
		int vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_STATUS].GetString(),gpC_base->m_item_desc_pi,ITEM_DESC_RGB);
		g_PrintColorStr(vx, py, desc->sz_status , gpC_base->m_item_desc_pi, color);
		py += SMALL_FONT_Y_GAP;
	}
	
	g_FL2_ReleaseDC();
}


//-----------------------------------------------------------------------------
// _Strings_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_Strings_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption NULL guard.
	if (void_ptr == NULL || g_pUserOption == NULL) { CT_Done("StringsDescShow: SAFE-RETURN void_ptr=%p g_pUO=%p", void_ptr, (void*)g_pUserOption); return; }
	LPSTR* ptr = (LPSTR *)void_ptr;

//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif
//
//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;
//
//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	g_FL2_GetDC();

	for(int i = 0; i < left; i++)
	{
		g_PrintColorStr(px, py, ptr[i], gpC_base->m_item_name_pi, RGB_WHITE);
		py += NORMAL_FONT_Y_GAP;
	}

	g_FL2_ReleaseDC();
}

//-----------------------------------------------------------------------------
// _Info_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_Info_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption NULL guard.
	if (void_ptr == NULL || g_pUserOption == NULL) { CT_Done("InfoDescShow: SAFE-RETURN void_ptr=%p g_pUO=%p", void_ptr, (void*)g_pUserOption); return; }
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif
//
//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;
//
//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	if(left == 0)left = RGB_WHITE;
	g_PrintColorStr(px, py, (LPSTR)void_ptr, gpC_base->m_item_name_pi, left);
	py += NORMAL_FONT_Y_GAP;
}

//-----------------------------------------------------------------------------
// _SkillTree_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_SkillTree_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption NULL guard.
	if (void_ptr == NULL || g_pUserOption == NULL) { CT_Done("SkillTreeDescShow: SAFE-RETURN void_ptr=%p g_pUO=%p", void_ptr, (void*)g_pUserOption); return; }
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif
//
//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;
//
//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	
	//
	// level infomation
	//
	int domain_level;
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		if((*g_pSkillInfoTable)[left].DomainType >= 0)
			domain_level = (*g_pSkillManager)[(*g_pSkillInfoTable)[left].DomainType].GetDomainLevel();
		else domain_level = 0;
		break;
		
	case RACE_VAMPIRE:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel();
		break;
		
	case RACE_OUSTERS:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetDomainLevel();
		break;
	}

	const int advance_level		= g_char_slot_ingame.m_AdvancementLevel;
	const int sum_level			= domain_level + advance_level;

	const int skill_learn_level = (*g_pSkillInfoTable)[left].GetLearnLevel();


	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	char sz_buf[50];
	bool bEnable = true;
	const char *szSkillName;
	char szTemp2[10] = "???";

	if( g_pSystemAvailableManager->GetLimitLearnSkillLevel() < (*g_pSkillInfoTable)[left].GetLearnLevel() )
	{
		bEnable = false;
		szSkillName = szTemp2;
	} else
	{
		// 2026-05-15: GetHName (Hangul) -> GetName (English); CLAUDE.md kural 3
		szSkillName = (*g_pSkillInfoTable)[left].GetName();
	}

	g_FL2_GetDC();
	// name
	// option�� �پ������� �ٸ� ������ ����Ѵ�.
	g_PrintColorStr(px+1, py+1, szSkillName, gpC_base->m_item_name_pi, RGB(100,100,0) );
	g_PrintColorStr(px, py, szSkillName, gpC_base->m_item_name_pi, RGB_WHITE );

	py += NORMAL_FONT_Y_GAP;
	std::string SkillEName;
	SkillEName = "(";
	if( bEnable )
		SkillEName += (*g_pSkillInfoTable)[left].GetName();
	else
		SkillEName += szTemp2;
	SkillEName += ")";
	g_PrintColorStr(px, py, SkillEName.c_str(), gpC_base->m_item_desc_pi, RGB_GRAY);
	py += NORMAL_FONT_Y_GAP;
	int vx;

	if( !bEnable ) 
	{
		g_FL2_ReleaseDC();
		return;
	}

	MSkillDomain::SKILLSTATUS status = (*g_pSkillManager)[*(SKILLDOMAIN *)void_ptr].GetSkillStatus((ACTIONINFO)left);

	// level
	if(g_eRaceInterface != RACE_OUSTERS)
	{
		vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_REQUIRE_LEVEL].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

		int nLearnLevel = (*g_pSkillInfoTable)[left].GetLearnLevel();

		if(nLearnLevel >= 0)
		{
			if(nLearnLevel > 150)	// ���� ��ų 
			{
				//by viva
#ifdef __DESIGNED_CHS
				int LevelGrade = (nLearnLevel-150)/10;
				int LevelUnits = (nLearnLevel-150)%10;
				DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ADVANTE_0+LevelGrade, (int)(LevelUnits), " %d");
#else
				int LevelGrade = (nLearnLevel-151)/10;
				int LevelUnits = (nLearnLevel-151)%10;
				DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_ADVANTE_0+LevelGrade, (int)(LevelUnits+1), " %d");
#endif
				//end
			}
			else					// �Ϲ� ��ų
			{
				sprintf(sz_buf, "%d", nLearnLevel);
			}
		
			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
		}
		py += SMALL_FONT_Y_GAP;
	}
	
	if(status==MSkillDomain::SKILLSTATUS_LEARNED || g_eRaceInterface == RACE_OUSTERS)
	{

#if __CONTENTS(__SECOND_RARE_SKILL)
		if (g_eRaceInterface != RACE_VAMPIRE && !(*g_pSkillInfoTable)[left].IsPassive() || g_eRaceInterface == RACE_OUSTERS)
		{
			int level = -1;
			if(skill_learn_level > 150)
			{
				if(status==MSkillDomain::SKILLSTATUS_LEARNED)
					level = min(max(sum_level - skill_learn_level + 1, 0), 10);
				else
					level = 0;
			}
			else
				level = (*g_pSkillInfoTable)[left].GetExpLevel();

			if(level>0)
			{
				if(!(g_eRaceInterface == RACE_OUSTERS && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_OUSTERS_RARE
					|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE)))
				{
					sprintf(sz_buf,"%s: ",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString());
					vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				
					sprintf(sz_buf, "%d", level);
					g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
					py += SMALL_FONT_Y_GAP;
				}
			}
		}
#else
		if (g_eRaceInterface != RACE_VAMPIRE && !(*g_pSkillInfoTable)[left].IsPassive() || g_eRaceInterface == RACE_OUSTERS)
		{
			sprintf(sz_buf,"%s: ",(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString());
			vx = g_PrintColorStr(px, py, sz_buf, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);

			if(skill_learn_level > 150) {
				int level;
				if(status==MSkillDomain::SKILLSTATUS_LEARNED)
					level = min(max(sum_level - skill_learn_level + 1, 0), 10);
				else
					level = 0;

				sprintf(sz_buf, "%d", level);
			}
			else
				sprintf(sz_buf, "%d", (*g_pSkillInfoTable)[left].GetExpLevel());

			g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
			py += SMALL_FONT_Y_GAP;
		}
#endif //__SECOND_RARE_SKILL
	}
	if(g_eRaceInterface == RACE_OUSTERS)
	{
		// ���� ��ų�� ��ų ����Ʈ �ʿ� ����
		if(skill_learn_level <= 150
#if __CONTENTS(__SECOND_RARE_SKILL)
			&& (*g_pSkillInfoTable)[left].GetSkillStep() != SKILL_STEP_OUSTERS_RARE
#endif //__SECOND_RARE_SKILL
		)
		{
			if(status==MSkillDomain::SKILLSTATUS_LEARNED)
				DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_REQUIRE_SKILL_POINT, (int)((*g_pSkillInfoTable)[left].LevelUpPoint ), " %d");
			else
				DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_REQUIRE_SKILL_POINT, (int)( (*g_pSkillInfoTable)[left].SkillPoint), " %d");
			g_PrintColorStr(px, py, sz_buf, gpC_base->m_chatting_pi, ITEM_DESC_RGB);
			py += SMALL_FONT_Y_GAP;
		}

		int nLearnLevel = (*g_pSkillInfoTable)[left].GetLearnLevel();

		// 2004, 10, 16, sobeit add start
		if(nLearnLevel > 1)
		{
			if(nLearnLevel > 150)
			{
				char sz_buf2[64];
				//by viva
#ifdef __DESIGNED_CHS
				int LevelGrade = (nLearnLevel-150)/10;
				int LevelUnits = (nLearnLevel-150)%10;
				sprintf(sz_buf2, (*g_pGameStringTable)[UI_STRING_MESSAGE_ADVANTE_0+LevelGrade].GetString(), LevelUnits);
#else
				int LevelGrade = (nLearnLevel-151)/10;
				int LevelUnits = (nLearnLevel-151)%10;
				sprintf(sz_buf2, (*g_pGameStringTable)[UI_STRING_MESSAGE_ADVANTE_0+LevelGrade].GetString(), LevelUnits+1);
#endif
				//end
				DescSafeSprintf1s(sz_buf, sizeof(sz_buf), UI_STRING_LEARN_SKILL_LEVEL_FOR_STRING, sz_buf2, " %s");
			}
			else
			{
				DescSafeSprintf1d(sz_buf, sizeof(sz_buf), UI_STRING_LEARN_SKILL_LEVEL, (int)((*g_pSkillInfoTable)[left].GetLearnLevel()), " %d");
			}

			g_PrintColorStr(px, py, sz_buf, gpC_base->m_chatting_pi, ITEM_DESC_RGB);
			py += SMALL_FONT_Y_GAP;
		}
		// 2004, 10, 16, sobeit add end
	}

	// MP
	if(!(*g_pSkillInfoTable)[left].IsPassive())
	{
		switch(g_eRaceInterface)
		{
		case RACE_SLAYER:
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_MP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			break;
			
		case RACE_VAMPIRE:
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_HP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			break;
			
		case RACE_OUSTERS:
			vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CONSUME_EP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			break;
		}
		
		sprintf(sz_buf, "%d", (*g_pSkillInfoTable)[left].GetMP());
		g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
	}
	else
	{
		//g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_HAN_PASSIVE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		if(left >= SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA && left <= SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA)
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+left-SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		}
		else if (left >= SKILL_SWEEPER_BONUS_1 && left <= SKILL_SWEEPER_BONUS_12 )
		{
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_SWEEPER_BONUS_1+left-SKILL_SWEEPER_BONUS_1].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		}
		else
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_HAN_PASSIVE].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
		
	}
	py += SMALL_FONT_Y_GAP;
	
	if(g_eRaceInterface == RACE_OUSTERS)
	{
		SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[left];
		if(sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND
			)
		{
			int elemental_point[5] = { sInfo.Fire, sInfo.Water, sInfo.Earth, sInfo.Wind, sInfo.Sum };
			
			{
				// FAZ 4 (2026-05-06): ELEMENTAL_LEVEL — string + int. Defansif.
				int eSid = g_ELEMENTAL_STRING_ID[sInfo.ElementalDomain];
				const char* eStr = (g_pGameStringTable && eSid >= 0 && eSid < (int)(*g_pGameStringTable).GetSize())
				                   ? (*g_pGameStringTable)[eSid].GetString() : "";
				DescSafeSprintf1s1d(sz_buf, sizeof(sz_buf), UI_STRING_MESSAGE_REQUIRE_ELEMENTAL_LEVEL,
				                    eStr ? eStr : "", (int)elemental_point[sInfo.ElementalDomain],
				                    "%s Lvl %d");
			}
			g_PrintColorStr(px, py, sz_buf, gpC_base->m_chatting_pi, g_ELEMENTAL_COLOR[sInfo.ElementalDomain]);
			py += SMALL_FONT_Y_GAP;
		}
	}
	

//	assert(status != MSkillDomain::SKILLSTATUS_NULL);

	// ���ο� skill�� ��� �� �ִ� �������� üũ�ؾ���... by sigi
#if __CONTENTS(__SECOND_RARE_SKILL)
	if((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_SLAYER_RARE 
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_VAMPIRE_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_OUSTERS_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE) {}
	else
#endif //__SECOND_RARE_SKILL
	if(status==MSkillDomain::SKILLSTATUS_LEARNED)
	{
		if(g_eRaceInterface == RACE_SLAYER)
		{
			if((*g_pSkillInfoTable)[(ACTIONINFO)left].IsPassive())
			{
//				g_PrintColorStr(px, py, "Passive", gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
			}
			else
			{
				int domain = (*g_pSkillInfoTable)[(ACTIONINFO)left].DomainType;
				int max_exp = (*g_pSkillInfoTable)[(ACTIONINFO)left].maxExp;
				int exp = (*g_pSkillInfoTable)[(ACTIONINFO)left].GetSkillExp();
				int exp_percent;
				if(max_exp > 0)
					exp_percent = (exp % (max_exp/100))*100/(max_exp/100);
				else exp_percent = 0;

				if(exp == max_exp)exp_percent = 100;

				vx = g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_EXP].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
				wsprintf(sz_buf, "%d", exp_percent);
				strcat(sz_buf, "%");
				g_PrintColorStr(vx, py, sz_buf, gpC_base->m_item_desc_pi, RGB_WHITE);
				py += SMALL_FONT_Y_GAP;

				if(domain >= 0)
				{
					int domain_grade = 	max((*g_pSkillManager)[domain].GetDomainLevel()/25,
						(*g_pSkillInfoTable)[(ACTIONINFO)left].GetLearnLevel()/25);
					//int learn_level_grade = (*g_pSkillInfoTable)[(ACTIONINFO)left].GetLearnLevel()/25;
										
//					int skill_grade = (*g_pSkillInfoTable)[(ACTIONINFO)left].GetSkillStep() - SKILL_STEP_APPRENTICE;					
//					if(domain_grade != SKILL_STEP_GRAND_MASTER - SKILL_STEP_APPRENTICE && domain_grade - skill_grade >= 0 && domain_grade - skill_grade <= 2)
					
					if(domain_grade>= 0 && domain_grade< 3)
					{
						vx = g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LIMIT_LEVEL].GetString(), gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
						switch(domain_grade)
						{
						case 0:
							g_PrintColorStr(vx, py, "49", gpC_base->m_item_desc_pi, RGB_YELLOW);
							break;
							
						case 1:
							g_PrintColorStr(vx, py, "74", gpC_base->m_item_desc_pi, RGB_YELLOW);
							break;
							
						case 2:
							g_PrintColorStr(vx, py, "99", gpC_base->m_item_desc_pi, RGB_YELLOW);
							break;
						}
					}
				}
			}
		}
	}
	else// if ((*g_pSkillManager)[*(SKILLDOMAIN *)void_ptr].HasNewSkill())
	{
//		switch (status)
//		{
//			default: // ���� ��� �� �ִ�.

//#ifdef _DEBUG
//		(*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].SetDomainLevel(21);
//#endif

		// ���� ��ų�� ���
		if(skill_learn_level > 150)
		{
			if(skill_learn_level >= 0	&& domain_level >= 150	&& advance_level > 0 && skill_learn_level <= sum_level)
				g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CAN_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_GREEN);
			else
				g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CANNOT_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
		}
		// ���� ��ų�� �ƴ� ���
		else
		{
			if(g_eRaceInterface == RACE_OUSTERS)
			{
				SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[left];
				if(sInfo.SkillPoint <= g_char_slot_ingame.skill_point&& sInfo.GetLearnLevel() <= g_char_slot_ingame.level )
				{
					bool bCanLearn = false;
					
					if(sInfo.SkillTypeList.empty())
						bCanLearn = true;
					else
					{
						SKILLINFO_NODE::SKILLTYPE_LIST::iterator itr = sInfo.SkillTypeList.begin();
						SKILLINFO_NODE::SKILLTYPE_LIST::iterator endItr = sInfo.SkillTypeList.end();
						
						while(itr != endItr)
						{
							if((*g_pSkillInfoTable)[*itr].GetSkillStep() == SKILL_STEP_OUSTERS_ETC && (*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetSkillStatus((ACTIONINFO)*itr) == MSkillDomain::SKILLSTATUS_LEARNED)
							{
								SKILLINFO_NODE::SKILLTYPE_LIST::iterator itr2 = (*g_pSkillInfoTable)[*itr].SkillTypeList.begin();
								SKILLINFO_NODE::SKILLTYPE_LIST::iterator endItr2 = (*g_pSkillInfoTable)[*itr].SkillTypeList.end();
								while(itr2 != endItr2)
								{
									if((*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetSkillStatus((ACTIONINFO)*itr2) == MSkillDomain::SKILLSTATUS_LEARNED
										&& (sInfo.GetSkillStep() == (*g_pSkillInfoTable)[*itr2].GetSkillStep() || (*g_pSkillInfoTable)[*itr2].GetSkillStep() == SKILL_STEP_OUSTERS_ETC))
									{
										bCanLearn = true;
										break;
									}
									itr2++;
								}
								if(bCanLearn)
									break;
							}
							else
							{
								if((*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetSkillStatus((ACTIONINFO)*itr) == MSkillDomain::SKILLSTATUS_LEARNED)
								{
									bCanLearn = true;
									break;
								}
							}
							itr++;
						}
					}
					
					if(bCanLearn)
						g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CAN_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_GREEN);
					else
						g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CANNOT_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
				} else
					g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CANNOT_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
			}
			else
			{
				if(skill_learn_level <= domain_level && skill_learn_level >= 0)
					g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CAN_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_GREEN);
				else
					g_PrintColorStr(px, py,(*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_CANNOT_LEARN_SKILL].GetString(), gpC_base->m_item_desc_pi, RGB_RED);
			}

		}
		py += SMALL_FONT_Y_GAP;
	}
//	else if (status==MSkillDomain::SKILLSTATUS_OTHER		// by sigi
//			|| status==MSkillDomain::SKILLSTATUS_NEXT)
//				g_PrintColorStr(px, py, "\"���� ��� �� �����ϴ�.\"", gpC_base->m_item_desc_pi, RGB_RED);				
	

	if(g_eRaceInterface == RACE_OUSTERS && left <365)
	{
		if((*g_pSkillInfoTable)[left].CanDelete)
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_SKILL_DELETE].GetString(), gpC_base->m_chatting_pi, RGB_GREEN);
		else
			g_PrintColorStr(px, py, (*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_SKILL_DELETE].GetString(), gpC_base->m_chatting_pi, RGB_RED);
		py += SMALL_FONT_Y_GAP;
	}
	g_FL2_ReleaseDC();
	
}

//-----------------------------------------------------------------------------
// _Help_Description_Show
//
//
//-----------------------------------------------------------------------------
void	_Help_Description_Show(Rect rect, void * void_ptr, long left, long right)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/g_pUserOption NULL guard.
	if (void_ptr == NULL || g_pUserOption == NULL) { CT_Done("HelpDescShow: SAFE-RETURN void_ptr=%p g_pUO=%p", void_ptr, (void*)g_pUserOption); return; }
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif
//
//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;
//
//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
////		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	g_FL2_GetDC();

	g_PrintColorStr(px, py, ((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str, gpC_base->m_item_name_pi, RGB_WHITE);
	py += NORMAL_FONT_Y_GAP;

	if (((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str2 != NULL)
	{
		if(left)
			g_PrintColorStr(px, py, ((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str2, gpC_base->m_item_desc_pi, left);
		else
			g_PrintColorStr(px, py, ((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str2, gpC_base->m_item_desc_pi, RGB(200, 200, 200));
		
		py += SMALL_FONT_Y_GAP;
	}

	if (((S_DEFAULT_HELP_STRING *)void_ptr)->sz_sub_str != NULL)
	{
		if(right)
			g_PrintColorStr(px, py, ((S_DEFAULT_HELP_STRING *)void_ptr)->sz_sub_str, gpC_base->m_item_desc_pi, right);
		else
			g_PrintColorStr(px, py, ((S_DEFAULT_HELP_STRING *)void_ptr)->sz_sub_str, gpC_base->m_item_desc_pi, ITEM_DESC_RGB);
	}

	g_FL2_ReleaseDC();
}

//-----------------------------------------------------------------------------
// _Item_Description_Calculator
//
// void_ptr = MItem *
//-----------------------------------------------------------------------------
// 2026-05-12: Calculator buffer overflow guards. sz_name[NAME_STRING_LEN] + sz_ename
// surekli strcat ile birikiyordu (CJK + multi-option = stack smash). Show varianti
// L810/820'de zaten strncat kullaniyor; Calculator burada bounded.
static inline void _ItemDescSafeCat(char *dst, const char *src, size_t dstSize) {
    if (dst == NULL || src == NULL || dstSize == 0) return;
    size_t cur = strlen(dst);
    if (cur >= dstSize - 1) return;
    strncat(dst, src, dstSize - 1 - cur);
    dst[dstSize - 1] = '\0';
}
#define SAFE_STRCAT_NAME(src)  _ItemDescSafeCat(sz_name,  (src), NAME_STRING_LEN)
#define SAFE_STRCAT_ENAME(src) _ItemDescSafeCat(sz_ename, (src), NAME_STRING_LEN)

void _Item_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): hover crash devamı — _Item_Description_Calculator
	// içinde NULL deref'leri (gpC_mouse_pointer, g_pTimeItemManager) korumasız.
	// p_item NULL check ÇOK GEÇ (line 4448), önündeki gpC_mouse_pointer deref
	// p_item NULL ise nasılsa unrelated, ama kendi pointer'ı NULL ise crash.
	CT_Done("ItemDescCalc: ENTER void_ptr=%p right=%p x=%d y=%d gpC_mouse_pointer=%p g_pTimeItemManager=%p",
	           void_ptr, (void*)right, x, y, (void*)gpC_mouse_pointer, (void*)g_pTimeItemManager);

	Rect rect(0,0,0,0);
	char sz_name[NAME_STRING_LEN];
	char sz_ename[NAME_STRING_LEN];

	//////////////////////////////// start calculation
	MItem * p_item = (MItem *)void_ptr;

	if (p_item == NULL) {
		CT_Done("ItemDescCalc: p_item NULL — return");
		return;
	}

	MItem * pAddItem = (MItem*)right;

	MItem* pMouseItem = (gpC_mouse_pointer != NULL) ? gpC_mouse_pointer->GetPickUpItem() : NULL;
	CT_Done("ItemDescCalc: pMouseItem=%p p_item=%p, pre GetItemClass/Type", (void*)pMouseItem, (void*)p_item);

	int nEquipSetID = GetEquipSetID(p_item->GetItemClass(), p_item->GetItemType());
	CT_Done("ItemDescCalc: nEquipSetID=%d, pre TimeItemManager IsExist", nEquipSetID);

	int line_count = 2;//2; // default: name, weight -> ����

	if (g_pTimeItemManager != NULL && g_pTimeItemManager->IsExist( p_item->GetID() ))
		line_count++;
	CT_Done("ItemDescCalc: post TimeItemManager, line_count=%d", line_count);

	// 2004, 7, 1, sobeit add start - corezap ����
	if(pAddItem != NULL)
	{
// 20080904 �����忡�� add�����۵鵵 �ɼ��� ������ �Ѵ�.
// add start
		int addItemClass = pAddItem->GetItemClass();
		int addItemType = pAddItem->GetItemType();

		int addItemOptionListCount = pAddItem->GetItemOptionListCount();
		line_count += addItemOptionListCount;
//add end
		if(addItemClass == ITEM_CLASS_CORE_ZAP)
		{
			if(addItemType >= 0 && addItemType <=5) // ���巹 �ھ���
				line_count+= 3; // ������ ��ü �ɼ� 1��, 4�� ������� �ɼ� 2��
		//	if(addItemType == 4 || addItemType == 5) // ť ���� �ƴ�
		//		line_count+= 9; // ������ ��ü �ɼ� 1��, ������� �ɼ� 4��
		}

		if(addItemClass == ITEM_CLASS_CUE_OF_ADAM)
		{
			if(addItemType >= 0 && addItemType <=3) // ť ���� �ƴ�
				line_count+= 5; // ������ ��ü �ɼ� 1��, ������� �ɼ� 4��
		}
		if(addItemClass == ITEM_CLASS_ETHEREAL_CHAIN)//������װ������Ŀ����
		{
			if(addItemType == 0) // ť ���� �ƴ�
				line_count+= 3; // ������ ��ü �ɼ� 1��, ������� �ɼ� 4��
			if(addItemType>=3 && addItemType<=12)
				line_count+= 9;
		}
	}
	// 2004, 7, 1, sobeit add end - corezap ����

	// identify�� �������� ���.. by sigi
//	if (p_item->IsIdentified())
	{
		CT_Done("ItemDescCalc: pre GetItemClass/Type p_item=%p", (void*)p_item);
		const int itemClass = p_item->GetItemClass();
		const int itemType = p_item->GetItemType();
		CT_Done("ItemDescCalc: itemClass=%d itemType=%d, pre IsIdentified", itemClass, itemType);

		bool bIdentified = p_item->IsIdentified();
		CT_Done("ItemDescCalc: bIdentified=%d, pre option list checks", (int)bIdentified);

		if ( bIdentified)
		{
			if (!p_item->IsEmptyItemOptionList() &&
				itemClass != ITEM_CLASS_CODE_SHEET
				)
				line_count+=p_item->GetItemOptionListCount();
		}
		else
		{
			line_count++;
		}
		CT_Done("ItemDescCalc: post option list, line_count=%d, pre third option", line_count);

		if ( bIdentified)
		{
			if (!p_item->IsEmptyItemThirdOptionList() &&
				itemClass != ITEM_CLASS_CODE_SHEET
				)
				line_count+=p_item->GetItemThirdOptionListCount();
		}
		else
		{
			line_count++;
		}
		CT_Done("ItemDescCalc: post third option, line_count=%d, pre gtwo option", line_count);

		if ( bIdentified)
		{
			if (!p_item->IsEmptyItemGTwoOptionList() &&
				itemClass != ITEM_CLASS_CODE_SHEET
				)
				line_count+=p_item->GetItemGTwoOptionListCount();
		}
		else
		{
			line_count++;
		}
		CT_Done("ItemDescCalc: post gtwo option, line_count=%d, pre g_pItemTable[%d][%d] access", line_count, itemClass, itemType);
		//if( itemClass != ITEM_CLASS_ETHEREAL_CHAIN )
		// FAZ 4 (2026-05-06): g_pItemTable double-index OOB guard.
		// Crash trace gosterdi: line_count=2 sonrasi crash burada (4558).
		// itemClass veya itemType OOB ise (*g_pItemTable)[X][Y] crash.
		if (g_pItemTable != NULL &&
		    itemClass >= 0 && itemClass < (int)(*g_pItemTable).GetSize() &&
		    itemType  >= 0 && itemType  < (int)(*g_pItemTable)[itemClass].GetSize()) {
			line_count+=(*g_pItemTable)[itemClass][itemType].DefaultOptionList.size();
			CT_Done("ItemDescCalc: post g_pItemTable access, line_count=%d", line_count);
		} else {
			CT_Done("ItemDescCalc: g_pItemTable OOB skip — tbl=%p itemClass=%d itemType=%d classSize=%d",
			           (void*)g_pItemTable, itemClass, itemType,
			           g_pItemTable ? (int)(*g_pItemTable).GetSize() : -1);
		}


		// ���� ���� Ư���ؼ� ����ó��
		if(itemClass == ITEM_CLASS_PET_ITEM)
		{
			// ���� dur�� ���� ���̾����� ����Ѵ�
			line_count++;

			// �� ������ Num�� �ִ�
			line_count++;


			MPetItem *pPetItem = dynamic_cast<MPetItem *>(p_item);

			// 2�� �ɷ��� ���� ��
			if(pPetItem && pPetItem->IsCanCutHead())
				line_count++;

			if(pPetItem && pPetItem->GetEnchantSkillType() > 0)
				line_count++;

			// ���� enchant level�� ��Ӽ����� ����Ѵ�
			// enchant level �� ��þƮ�ʹ� �ƹ� ���� ������ ����� ChargeItem�� Charge���� ���� �� ����
			// ��� �������� Enchant Level�� ������ ������ Number�� �����Ѵ�. �׷��� ���� �����ۿ���
			// GetEnchantLevel�� �ص� �ƹ��͵� �ȳ��´�.
			// �װ� ���� �����۸��̰� ���� �׳� EnchantLevel �״�� ����
			// �� �Ӽ� ��ġ�� Silver���� ����.

			if(g_pTimeItemManager->IsExist( p_item->GetID() ) )
			{
				if( gbl_sell_running || gbl_silvering_running )
					line_count++;
			} else
			{
				if ((gbl_sell_running || gbl_buy_running || gbl_silvering_running
#if __CONTENTS(!__REPAIRSYSTEM_RENEWAL)
					//Ÿ ���� ���� ���� �۵� ���� �ʴ´�.
					|| gbl_repair_running
#endif //gbl_repair_running ||
					))
					line_count++;
			}

			if( p_item->GetGrade() != -1 )
			{
				TYPE_ITEM_DURATION leftTime = p_item->GetCurrentDurability();
				int day,hour,minute;
				
				day = leftTime / 60 / 24;
				hour = (leftTime / 60)%24;
				minute = leftTime % 60;
				
				std::string time;

				time = (*g_pGameStringTable)[UI_STRING_MESSAGE_PET_DESC_DURABILITY].GetString();
				time += " ";
				
				if( day > 0 )
				{
					sprintf( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString(), day );
					time += sz_name;
					time += " ";
				}
				if( hour > 0 )
				{
					sprintf( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString(), hour );
					time += sz_name;
					time += " ";
				}
				if( minute > 0 )
				{
					sprintf( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString(), minute );
					time += sz_name;					
				}

				rect.w = max(rect.w, g_GetStringWidth(time.c_str(), gpC_base->m_item_desc_pi.hfont));
			}

			if(p_item->GetSilver() > 0 &&
			   p_item->GetEnchantLevel() < ITEMOPTION_TABLE::MAX_PART)
				line_count++;

			// FIX 2026-05-12: gpC_mouse_pointer NULL guard (Pet description Calculator hot path).
			const MItem* pMouseItem = (gpC_mouse_pointer != NULL) ? gpC_mouse_pointer->GetPickUpItem() : NULL;

			if( pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_PET_ENCHANT_ITEM )
			{
				line_count++;			
			}
#if __CONTENTS(__PET_MIXINGFORGE)
			if(pPetItem->GetPetMixingOptionListCount() > 0)
			{
				line_count += pPetItem->GetPetMixingOptionListCount();
			}
#endif //__PET_MIXINGFORGE
			if (pPetItem->GetPersnalPrice() != -1)
			{
				line_count++;
			}
		}
		else if(itemClass == ITEM_CLASS_CORE_ZAP)
		{
			line_count += 3;
			if(gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
				line_count++;
			
		}
		else if ( itemClass == ITEM_CLASS_RANK_GEM )
		{
			line_count ++;
		}
		else if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN && itemType == 0)//������װ��Ŀ����
		{
			line_count += 3;
			if(gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
				line_count++;
		}
		else if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN && itemType >= 3 && itemType <= 12)//������װ��Ŀ����
		{
			line_count += 9;
			if(gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
				line_count++;
		}
		else if(itemClass == ITEM_CLASS_ETHEREAL_CHAIN )
		{
			//line_count++;
			if(gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
					line_count++;
		}
		else if(itemClass == ITEM_CLASS_CUE_OF_ADAM )
		{
			if(itemType >= 0 && itemType <= 3)
			{
				line_count += 5;
				if( gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
					line_count++;
			}
			//20080214 diesirace ���� �������� ������ ���� �Ѿ�� �ȴ�.
			//���� ���� �½� sell�̳� buy�� ture�� ���� ������ ������ ���� ������ ǥ�� �Ǹ鼭 �̻�����
			//���� ���� ������ ��� ���� ǥ�����ִ� gbl�� �����  ������ ++ ���ִ� �ڵ常 �߰����ش�. 
			//���� ���� �������� ������ ���ڶ��� �̿��� ����
			else if(gbl_persnalshop_running ||gbl_sell_running || gbl_buy_running )
					line_count++;
		}
		else
		{
			// FAZ 4 (2026-05-06): generic weapon/equip branch — 30+ virtual call.
			// p_item state corruption olursa herhangi biri crash. Checkpoint
			// marker'lar grup grup hangi virtual call zincirinde patladığını söyler.
			CT_Done("ItemDescCalc: ELSE branch enter (itemClass=%d), pre weapon-class+IsGunItem", itemClass);
			if(itemClass == ITEM_CLASS_SWORD ||
				itemClass == ITEM_CLASS_BLADE ||
				itemClass == ITEM_CLASS_CROSS ||
				itemClass == ITEM_CLASS_MACE ||
				itemClass == ITEM_CLASS_MINE ||
				itemClass == ITEM_CLASS_BOMB ||
				itemClass == ITEM_CLASS_BOMB_MATERIAL ||
				p_item->IsGunItem())
				line_count++;
			CT_Done("ItemDescCalc: post IsGunItem, pre damage/speed/crit");
			if (p_item->GetMaxDamage() != -1)
				line_count++;
			if (p_item->GetOriginalSpeed() > 0
#if __CONTENTS(__ADD_MAGICPROTECTION)
				||p_item->HasMagicProtection()
#endif
				)
				line_count++;
			if (p_item->GetCriticalHit() != -1)
				line_count++;
			CT_Done("ItemDescCalc: post damage/speed/crit, pre durability/repair check");
			if (p_item->GetMaxDurability() != -1 &&
				!p_item->IsUniqueItem() &&
				//itemClass != ITEM_CLASS_VAMPIRE_AMULET &&
				//itemClass != ITEM_CLASS_COUPLE_RING && itemClass != ITEM_CLASS_VAMPIRE_COUPLE_RING &&
				//&&itemClass != ITEM_CLASS_BLOOD_BIBLE_SIGN
				!p_item->IsQuestItem() &&
				p_item->IsRepairableItem() )
				line_count++;
			CT_Done("ItemDescCalc: post durability/repair, pre defense stats");
			if (p_item->GetProtectionValue() != -1)
				line_count++;
			if (p_item->GetDefenseValue() != -1)
				line_count++;
			if (p_item->GetToHit() != -1)
				line_count++;
			CT_Done("ItemDescCalc: post defense stats, pre HP/MP/Silver/Reach");
			if (p_item->GetHealPoint() != -1)
				line_count++;
			if (p_item->GetManaPoint() != -1)
				line_count++;
			if (p_item->GetSilverMax() != -1)
				line_count++;
			if (p_item->GetReach() != -1 && p_item->GetReach() != 1)
				line_count++;
			CT_Done("ItemDescCalc: post HP/MP/Silver/Reach, pre Magazine/Pocket/Pile");
			if (p_item->GetMagazineSize() != -1)
				line_count++;
			if (p_item->GetPocketNumber() != -1)
				line_count++;
			if (p_item->IsPileItem() == true || p_item->IsChargeItem())
				line_count++;
			if (p_item->GetPersnalPrice() != -1)
				line_count++;
			CT_Done("ItemDescCalc: post Magazine/Pocket/Pile/PersnalPrice, pre Lucky");
			// 2004, 04, 3 sobeit add start
			if(p_item->GetLucky() != -9999 && itemClass != ITEM_CLASS_COUPLE_RING && itemClass != ITEM_CLASS_VAMPIRE_COUPLE_RING && itemClass != ITEM_CLASS_OUSTERS_HARMONIC_PENDENT)
				line_count++;
			CT_Done("ItemDescCalc: post Lucky, pre TimeItemManager IsExist");


			// 2004, 04, 3 sobeit add end
			// FAZ 4 (2026-05-06): TimeItemManager NULL guard (else branch'te ikinci use-site).
			CT_Done("ItemDescCalc: ELSE TimeItemManager check, g_pTimeItemManager=%p", (void*)g_pTimeItemManager);
			bool bTimeItemExist = (g_pTimeItemManager != NULL && g_pTimeItemManager->IsExist( p_item->GetID() ));
			CT_Done("ItemDescCalc: bTimeItemExist=%d, pre IsTimeLimitStatic", (int)bTimeItemExist);
			if(bTimeItemExist)
			{
				if( gbl_sell_running || gbl_silvering_running )
					line_count++;
			}
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
			else if( p_item->IsTimeLimitStatic() )
			{
				line_count++;
			}
#endif //__PREMIUM_GIVE_ITEM_UI
			else
			{
				CT_Done("ItemDescCalc: ELSE-of-Time, pre IsRepairableItem/IsUnique/IsQuest");
				if ((gbl_sell_running || gbl_buy_running ||
					(gbl_repair_running && (itemClass != ITEM_CLASS_VAMPIRE_AMULET) && p_item->IsRepairableItem()) ||
					gbl_silvering_running
					&& !p_item->IsUniqueItem() &&
					!p_item->IsQuestItem()
					))
					line_count++;
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
				if(gbl_repair_running && p_item->IsRepairableItem()
					&& !p_item->IsUniqueItem() &&
					!p_item->IsQuestItem()
					)
					line_count++;
#endif //__REPAIRSYSTEM_RENEWAL
			}
			CT_Done("ItemDescCalc: post Time/Repair block, pre PortalItem checks");
			
			
			if (itemClass == ITEM_CLASS_VAMPIRE_PORTAL_ITEM && p_item->IsIdentified())
			{
				if (!(gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID()))   )
				{
					rect.w = max(rect.w,g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_PORTAL_ITEM_DESC].GetString(), gpC_base->m_item_desc_pi.hfont));
					
				}
				
				//line_count++; 
				if(((MVampirePortalItem *)p_item)->IsMarked())
				{
					//if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5))
					//{
					//	line_count++;
					//}
					//else	
					//{
					//	if ((gbl_buy_running == true && !g_pTimeItemManager->IsExist( p_item->GetID())))
					//		line_count++;
						 
					//}

					if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5) && gbl_buy_running != true)
					{
						line_count++;
					}
					line_count ++;
				}
				else
				{	
					if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5) && gbl_buy_running != true)
					{
						line_count++;
					}

					//{
					//if (gbl_buy_running != true)
					//	line_count++;
					//}
				}
			}

			
			if (itemClass == ITEM_CLASS_SLAYER_PORTAL_ITEM) //  && p_item->IsIdentified())
			{
				if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5) && (gbl_buy_running == true))
				{
					line_count++;
				}
				else if(!(p_item->GetGridX() > 9 || p_item->GetGridY() > 5) && (gbl_buy_running == false))
				{
					line_count++;
				}		
			}
			
			CT_Done("ItemDescCalc: pre QuestItem-block (race-aware Require checks)");
			if(itemClass != ITEM_CLASS_COUPLE_RING && itemClass != ITEM_CLASS_VAMPIRE_COUPLE_RING && itemClass != ITEM_CLASS_OUSTERS_HARMONIC_PENDENT &&
				!p_item->IsQuestItem())
			{
				if(p_item->IsIdentified())
				{
					CT_Done("ItemDescCalc: pre IsGearItem virtual");
					if(p_item->IsGearItem())
					{
						CT_Done("ItemDescCalc: IsGearItem=1, pre Slayer/Ousters Require chain");
						// FAZ 4 (2026-05-11 it.2) Bug 10: requirement satirlarini Calculator'a
						// kazandirmak — "(you: NN)" suffix horizontal overflow yapmasin.
						// Render text'inin width'ini ol+rect.w'ye uygula. line_count zaten
						// orijinal mantikla artiyor (per-stat 1 line) — degisiklik yok.
						const int bug10_my_STR   = (int)g_char_slot_ingame.STR_CUR;
						const int bug10_my_DEX   = (int)g_char_slot_ingame.DEX_CUR;
						const int bug10_my_INT   = (int)g_char_slot_ingame.INT_CUR;
						const int bug10_my_SUM   = bug10_my_STR + bug10_my_DEX + bug10_my_INT;
						const int bug10_my_LEVEL = (int)g_char_slot_ingame.level;
						const char* bug10_szRequire = (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_REQUIRE].GetString();
						const int bug10_baseW = g_GetStringWidth(bug10_szRequire, gpC_base->m_item_desc_pi.hfont);
						char bug10_buf[64];

						if(p_item->IsSlayerItem() || p_item->IsOustersItem())
						{
							if (p_item->GetRequireSTR() > 0) {
								line_count++;
								const int req = (int)p_item->GetRequireSTR();
								const int my  = bug10_my_STR;
								const char* sLabel = (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_STR].GetString();
								if (my >= req) snprintf(bug10_buf, sizeof(bug10_buf), "%s %d,", sLabel, req);
								else snprintf(bug10_buf, sizeof(bug10_buf), "%s %d (you: %d),", sLabel, req, my);
								rect.w = max(rect.w, bug10_baseW + g_GetStringWidth(bug10_buf, gpC_base->m_item_desc_pi.hfont));
							}
							if (p_item->GetRequireDEX() > 0) {
								line_count++;
								const int req = (int)p_item->GetRequireDEX();
								const int my  = bug10_my_DEX;
								const char* sLabel = (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DEX].GetString();
								if (my >= req) snprintf(bug10_buf, sizeof(bug10_buf), "%s %d,", sLabel, req);
								else snprintf(bug10_buf, sizeof(bug10_buf), "%s %d (you: %d),", sLabel, req, my);
								rect.w = max(rect.w, bug10_baseW + g_GetStringWidth(bug10_buf, gpC_base->m_item_desc_pi.hfont));
							}
							if (p_item->GetRequireINT() > 0) {
								line_count++;
								const int req = (int)p_item->GetRequireINT();
								const int my  = bug10_my_INT;
								const char* sLabel = (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_INT].GetString();
								if (my >= req) snprintf(bug10_buf, sizeof(bug10_buf), "%s %d,", sLabel, req);
								else snprintf(bug10_buf, sizeof(bug10_buf), "%s %d (you: %d),", sLabel, req, my);
								rect.w = max(rect.w, bug10_baseW + g_GetStringWidth(bug10_buf, gpC_base->m_item_desc_pi.hfont));
							}
							if (p_item->GetRequireSUM() > 0 &&
								p_item->GetRequireSUM() > p_item->GetRequireSTR() &&
								p_item->GetRequireSUM() > p_item->GetRequireDEX() &&
								p_item->GetRequireSUM() > p_item->GetRequireINT() ) {
								line_count++;
								const int req = (int)p_item->GetRequireSUM();
								const int my  = bug10_my_SUM;
								const char* sLabel = (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_ALL_STAT_SUM].GetString();
								if (my >= req) snprintf(bug10_buf, sizeof(bug10_buf), "%s %d", sLabel, req);
								else snprintf(bug10_buf, sizeof(bug10_buf), "%s %d (you: %d)", sLabel, req, my);
								rect.w = max(rect.w, bug10_baseW + g_GetStringWidth(bug10_buf, gpC_base->m_item_desc_pi.hfont));
							}
						}
						if(p_item->IsVampireItem() || p_item->IsOustersItem())
						{
							if (p_item->GetRequireLevel() > 0) {
								line_count++;
								const int req = (int)p_item->GetRequireLevel();
								const int my  = bug10_my_LEVEL;
								const char* sLabel = (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString();
								if (my >= req) snprintf(bug10_buf, sizeof(bug10_buf), "%s %d", sLabel, req);
								else snprintf(bug10_buf, sizeof(bug10_buf), "%s %d (you: %d)", sLabel, req, my);
								rect.w = max(rect.w, bug10_baseW + g_GetStringWidth(bug10_buf, gpC_base->m_item_desc_pi.hfont));
							}
						}
						CT_Done("ItemDescCalc: pre g_pItemTable RequireAdvancementLevel access");
						// 2005, 1, 18, sobeit add start - �ʿ� ���� ����
						// FAZ 4 (2026-05-06): table double-index OOB guard.
						{
							int icc = p_item->GetItemClass();
							int itc = p_item->GetItemType();
							if (g_pItemTable != NULL &&
							    icc >= 0 && icc < (int)(*g_pItemTable).GetSize() &&
							    itc >= 0 && itc < (int)(*g_pItemTable)[icc].GetSize() &&
							    (*g_pItemTable)[icc][itc].GetRequireAdvancementLevel()>0)
								line_count++;
						}
						// 2005, 1, 18, sobeit add end - �ʿ� ���� ����
					}

					CT_Done("ItemDescCalc: pre race-mismatch + Gender check");
					if(!p_item->IsSlayerItem() && g_eRaceInterface == RACE_SLAYER ||
						!p_item->IsVampireItem() && g_eRaceInterface == RACE_VAMPIRE ||
						!p_item->IsOustersItem() && g_eRaceInterface == RACE_OUSTERS)
						line_count++;

					if (p_item->IsGenderForMale() || p_item->IsGenderForFemale())
						line_count++;
				}
				else
					line_count++;
			}
			CT_Done("ItemDescCalc: post QuestItem-block");
		}

		CT_Done("ItemDescCalc: pre Description.GetLength access (g_pItemTable[%d][%d])", itemClass, itemType);
		// FAZ 4 (2026-05-06): g_pItemTable double-index OOB guard.
		bool bHasDescription = false;
		if (g_pItemTable != NULL &&
		    itemClass >= 0 && itemClass < (int)(*g_pItemTable).GetSize() &&
		    itemType  >= 0 && itemType  < (int)(*g_pItemTable)[itemClass].GetSize()) {
			bHasDescription = ((*g_pItemTable)[itemClass][itemType].Description.GetLength() != 0);
		}
		if(bHasDescription
#if __CONTENTS(__TUNING_ITEM)
			|| p_item->IsTuningItem()
#endif //__TUNING_ITEM
			)
		{
			int descriptionLines = 1;
			if (bHasDescription)
			{
				descriptionLines = DescCountFormattedDescriptionLines((*g_pItemTable)[itemClass][itemType].Description.GetString());
				if (descriptionLines < 1)
					descriptionLines = 1;
			}
			line_count += descriptionLines;
		}
		CT_Done("ItemDescCalc: post Description.GetLength");

		if(pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && p_item->IsGearItem() )
		{
			line_count++;
		}
#if __CONTENTS(__TUNING_ITEM)
		if(pMouseItem != NULL&&pMouseItem->IsTuningItem())
		{
			line_count++;
		}
#endif //__TUNING_ITEM
		if(pMouseItem != NULL && pMouseItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM	&&
			( ( pMouseItem->GetItemType() >= 9	&& pMouseItem->GetItemType() <= 17 ) ||
			  ( pMouseItem->GetItemType() >= 19 && pMouseItem->GetItemType() <= 24 ) ||
			    pMouseItem->GetItemType() == 28)	&&
			p_item != NULL &&	!p_item->IsEmptyItemOptionList() && !p_item->IsQuestItem() && !p_item->IsUniqueItem())
		{
			line_count++;
			if( C_VS_UI_REMOVE_OPTION::IsCanRemoveOption_Puritas( pMouseItem, p_item) )
			{				
				rect.w = max(rect.w, g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_REMOVE_OPTION].GetString(), gpC_base->m_item_desc_pi.hfont));
			} else
				rect.w = max(rect.w, g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSGAE_CANNOT_REMOVE_OPTION].GetString(), gpC_base->m_item_desc_pi.hfont));
		}
		if((itemClass == ITEM_CLASS_OUSTERS_WRISTLET || itemClass == ITEM_CLASS_OUSTERS_STONE)
			&& (*g_pItemTable)[itemClass][itemType].ElementalType != ITEMTABLE_INFO::ELEMENTAL_TYPE_ANY)
			line_count++;
		
	}
	

	if(p_item->GetItemClass() == ITEM_CLASS_PET_FOOD)
	{
		line_count++;
		PETINFO *pPetInfo = gC_vs_ui.GetMyPetInfo();
		if(pPetInfo != NULL && pPetInfo->EXIST)
		{
			rect.w = max(rect.w,g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_USE_PET_FOOD].GetString(), gpC_base->m_item_desc_pi.hfont));
		}
		else
		{
			rect.w = max(rect.w,g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DETACH_PET_FOOD].GetString(), gpC_base->m_item_desc_pi.hfont));
		}
	}
	
	sz_name[0] = '\0'; // for strcat()
	sz_ename[0] = '\0'; // for strcat()
	SAFE_STRCAT_ENAME("(");

	if (g_pTimeItemManager->IsExist( p_item->GetID() ) )
	{
		strcat( sz_name, (*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_HAN].GetString() );
		SAFE_STRCAT_NAME(" "); // add 'space'
		strcat( sz_ename,(*g_pGameStringTable)[UI_STRING_MESSAGE_QUEST_ENG].GetString() );
		SAFE_STRCAT_ENAME(" "); // add 'space'
	}
	
	if(p_item->GetItemClass() == ITEM_CLASS_PET_ITEM)
	{
		if(p_item->GetSilver() > 0)
		{
			MPetItem *pPetItem = (MPetItem *)p_item;
			
			SAFE_STRCAT_NAME(pPetItem->GetPetOptionName().c_str());
//				strcat(sz_name, " "); // add 'space'

			SAFE_STRCAT_ENAME(pPetItem->GetPetOptionEName().c_str());
//				strcat(sz_ename, " "); // add 'space'
		}
	}
	else if (!p_item->IsEmptyItemOptionList() && p_item->GetItemClass() != ITEM_CLASS_CODE_SHEET )
	{
		for(int i=0;i<min(2,p_item->GetItemOptionListCount());i++)
		{
			if(p_item->GetItemOptionName(i)!=NULL)
			{
				SAFE_STRCAT_NAME(p_item->GetItemOptionName(i));
				SAFE_STRCAT_NAME(" "); // add 'space'
			}
			if(p_item->GetItemOptionEName(i)!=NULL)
			{
				SAFE_STRCAT_ENAME(p_item->GetItemOptionEName(i));
				SAFE_STRCAT_ENAME(" "); // add 'space'
			}
		}
	}

	
	if ( nEquipSetID != -1 ) //����װ��Ϣ
	{
		int nSize = g_vecEquipSetList[nEquipSetID].size();

		if ( nSize == 1)
			line_count += 8;
		else
			line_count += 15;
	}
//	strcat(sz_name, p_item->GetName());
//	strcat(sz_ename, p_item->GetEName());
//	strcat(sz_ename, ")");
//
//	rect.w = max(g_GetStringWidth(sz_name, gpC_base->m_item_name_pi.hfont), g_GetStringWidth(sz_ename, gpC_base->m_item_desc_pi.hfont));
//	rect.h = NORMAL_FONT_Y_GAP+(line_count-1)*SMALL_FONT_Y_GAP+5;
//
//	if((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetLength() != 0)
//		rect.w = max(rect.w, g_GetStringWidth((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetString(), gpC_base->m_item_desc_pi.hfont));

	
	// ����� �ٺ� ������
	if(g_pTimeItemManager->IsExist( p_item->GetID() ) )
	{
		char	temp[256] = {0,};	
		bool	bContinue = false;
		TYPE_OBJECTID objectID = p_item->GetID();		
		
		int tempInt;
		
		if( g_pTimeItemManager->GetDay( objectID ) == 0 && g_pTimeItemManager->GetHour (objectID ) ==0 &&
			g_pTimeItemManager->GetMinute( objectID ) == 0 )
		{
			if( g_pTimeItemManager->GetSecond( objectID ) == 0 )
			{
				strcpy(temp, (*g_pGameStringTable)[UI_STRING_MESSAGE_EXPIRED_ITEM].GetString() );
			} else
			{
				strcpy(temp, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString() );
				strcat(temp, " ");
				char tempSecond[10];
				wsprintf(tempSecond, (*g_pGameStringTable)[UI_STRING_MESSAGE_SECOND].GetString() , g_pTimeItemManager->GetSecond( objectID ) );
				strcat(temp, tempSecond );
			}
		} else
		{
			strcpy(temp, (*g_pGameStringTable)[UI_STRING_MESSAGE_LEFT_TIME].GetString() );

			if((tempInt = g_pTimeItemManager->GetDay( objectID ))>0  || bContinue )
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_DAY].GetString() , tempInt );
				strcat(temp, tempDay );
				bContinue = true;
			}
			if((tempInt = g_pTimeItemManager->GetHour( objectID ))>0 || bContinue)
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_HOUR].GetString() , tempInt );
				strcat ( temp, tempDay );
				bContinue = true;
			}
			if((tempInt = g_pTimeItemManager->GetMinute( objectID ))>0  || bContinue)
			{
				char tempDay[30] = {0,};
				strcat(temp, " ");
				wsprintf(tempDay, (*g_pGameStringTable)[UI_STRING_MESSAGE_MINUTE].GetString() , tempInt );
				strcat ( temp, tempDay );
				bContinue = true;
			}			
		}
		rect.w = max (rect.w, g_GetStringWidth( temp, gpC_base->m_item_desc_pi.hfont ) );
	}

	if(gpC_mouse_pointer != NULL && pMouseItem != NULL &&
		pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR )
	{	
		rect.w = max (rect.w, g_GetStringWidth( (*g_pGameStringTable)[UI_STRING_MESSAGE_CAN_TRANS].GetString(), 
			gpC_base->m_item_desc_pi.hfont) );
	}

	SAFE_STRCAT_NAME(p_item->GetName());
	SAFE_STRCAT_ENAME(p_item->GetEName());
	SAFE_STRCAT_ENAME(")");

	rect.w = max(rect.w,max(g_GetStringWidth(sz_name, gpC_base->m_item_name_pi.hfont), g_GetStringWidth(sz_ename, gpC_base->m_item_desc_pi.hfont)));
	// 2026-05-22 FIX: Bracelet/wearable item tooltip alt satir (Requirements) yarim
	// kesik gozukuyor (Phoenix D3D9 native font rendering metric drift hipotezi —
	// SMALL_FONT_Y_GAP constant'i VS6 GDI font height'i icin, D3D9 native font
	// birkac piksel daha fazla yer kapliyor; toplam 7-8 stat satirinda ~7 piksel
	// taskin pencere alt kenarini asiyor). (line_count-1) -> (line_count) ile
	// 1 ek SMALL_FONT_Y_GAP padding eklendi. Bu degisiklik VS_UI_Description'daki
	// diger 7 rect.h hesabi (5556, 5645, 5688, 5814, 5931, 5985, 6133) ile zaten
	// tutarli — onlar hep (line_count) kullaniyor, sadece ItemDesc + 6080 satir
	// (line_count-1) idi. Referans VS6 ayni formulu kullaniyor ama o donem font
	// metric'i farkliydi, sorun gorunmuyordu.
	rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP+5;

#if __CONTENTS(__TUNING_ITEM)
	if(p_item->IsTuningItem())
	{
		MString td_message;
		MString td_item;
		MString id_Grade;

		int AdvancementLevel = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].GetRequireAdvancementLevel();
		//by viva
#ifdef __DESIGNED_CHS
		int LevelGrade = (AdvancementLevel)/10;
		int LevelUnits = (AdvancementLevel)%10;
#else
		int LevelGrade = (AdvancementLevel-1)/10;
		int LevelUnits = (AdvancementLevel-1)%10;
#endif
		//end

		td_item.Init((*g_pItemTable)[p_item->GetTargetClass()][p_item->GetTargetType()].HName.GetLength()+
						(*g_pItemTable)[p_item->GetTargetClass()][p_item->GetTargetType()].EName.GetLength()+2);
		td_item.Format("%s(%s)",(*g_pItemTable)[p_item->GetTargetClass()][p_item->GetTargetType()].HName.GetString(),
						(*g_pItemTable)[p_item->GetTargetClass()][p_item->GetTargetType()].EName.GetString());

		if(AdvancementLevel==0) 
		{
	//		char _race = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Race;
			char _race = g_char_slot_ingame.Race;
			if(_race==RACE_SLAYER)
			{
				int reqSUM = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].GetRequireSUM();

				td_message.Init((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION_DOMAIN].GetLength()+td_item.GetLength());
				td_message.Format((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION_DOMAIN].GetString(),td_item.GetString(),reqSUM);
			}
			else if(_race==RACE_VAMPIRE||_race==RACE_OUSTERS)
			{
				int reqLevel = (*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].GetRequireLevel();

				td_message.Init((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION_LEVEL].GetLength()+td_item.GetLength());
				td_message.Format((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION_LEVEL].GetString(),td_item.GetString(),reqLevel);
			}
		}
		else
		{
			id_Grade.Init((*g_pGameStringTable)[UI_STRING_MESSAGE_ADVANTE_00_NAME+LevelGrade].GetLength());
			id_Grade.Format((*g_pGameStringTable)[UI_STRING_MESSAGE_ADVANTE_00_NAME+LevelGrade].GetString());
	
			td_message.Init((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION].GetLength()+td_item.GetLength()+id_Grade.GetLength());
			td_message.Format((*g_pGameStringTable)[UI_STRING_MESSAGE_TUNING_DESCRIPTION].GetString(),td_item.GetString(),id_Grade.GetString());
		}

		(*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description = td_message;
	}	
#endif //__TUNING_ITEM

	if((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetLength() != 0)
	{
		int MaxWidth = 0;
		if(p_item->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE_SIGN)
		{
			int bibleType = p_item->GetItemType();
			if(bibleType < 12) 
				MaxWidth = g_GetStringWidth((*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibleType].GetString(), 
				gpC_base->m_item_desc_pi.hfont);
			else if(bibleType > 11 && bibleType < 85)
			{
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
				char biblebuffer[128];
				sprintf(biblebuffer, "%s %s",(*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibleType%12].GetString(),
					(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_BONUS_GRUN_ARMEGA+(bibleType-12)].GetString());
				MaxWidth = g_GetStringWidth(biblebuffer,gpC_base->m_item_desc_pi.hfont);
#endif //__CONTRIBUTE_SYSTEM
			}
		}


		else
			MaxWidth = DescGetFormattedDescriptionMaxWidth((*g_pItemTable)[p_item->GetItemClass()][p_item->GetItemType()].Description.GetString(), gpC_base->m_item_desc_pi.hfont);
		rect.w = max(rect.w, MaxWidth);
	}

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// revision?
	if (rect.w < DEFAULT_BOX_WIDTH)
		rect.w = DEFAULT_BOX_WIDTH;

	// get (x, y)
	rect.x = x;
	rect.y = y-(rect.h+1);

	if (rect.x < 0)
		rect.x = 0;

	//if (rect.x+rect.w > RESOLUTION_X)
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;
		//rect.x = RESOLUTION_X-rect.w;
	if (rect.y < 0)
		rect.y = y+(p_item->GetGridHeight()*C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y);
	//if (rect.y+rect.h > RESOLUTION_Y)
	if (rect.y+rect.h > g_pUserInformation->iResolution_y)
		rect.y = g_pUserInformation->iResolution_y-rect.h;
		//rect.y = RESOLUTION_Y-rect.h;

	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Skill_Description_Calculator
//
// left = ACTIONINFO
// right = icon height
//-----------------------------------------------------------------------------
void _Skill_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — table/manager NULL+OOB guard.
	if (g_pSkillInfoTable == NULL || g_pSystemAvailableManager == NULL ||
	    left < 0 || left >= (long)(*g_pSkillInfoTable).GetSize()) {
		CT_Done("SkillDescCalc: SAFE-RETURN tbl=%p mgr=%p left=%ld size=%d",
		           (void*)g_pSkillInfoTable, (void*)g_pSystemAvailableManager, left,
		           g_pSkillInfoTable ? (int)(*g_pSkillInfoTable).GetSize() : -1);
		return;
	}
	Rect rect;

	int line_count = 3;

	//////////////////////////////// start calculation
	if( (*g_pSkillInfoTable)[left].GetLearnLevel() > g_pSystemAvailableManager->GetLimitLearnSkillLevel() )
	{
		line_count = 2;
		rect.w = DEFAULT_BOX_WIDTH;
		rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP;
		rect.x = x;
		rect.y = y-rect.h;
		
		g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
		return;
	}	
	if (g_eRaceInterface == RACE_VAMPIRE || (*g_pSkillInfoTable)[left].IsPassive() && g_eRaceInterface != RACE_OUSTERS || left == SKILL_LOVE_CHAIN)
	{
		line_count--;
	}

#if __CONTENTS(__SECOND_RARE_SKILL)
//////////////////////////////////////////////////////////////////////////
	
	int level = -1;
	
	int domain_level;
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		if((*g_pSkillInfoTable)[left].DomainType >= 0)
			domain_level = (*g_pSkillManager)[(*g_pSkillInfoTable)[left].DomainType].GetDomainLevel();
		else domain_level = 0;
		break;
		
	case RACE_VAMPIRE:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel();
		break;
		
	case RACE_OUSTERS:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetDomainLevel();
		break;
	}
	
	const int advance_level		= g_char_slot_ingame.m_AdvancementLevel;
	const int sum_level			= domain_level + advance_level;
	
	const int skill_learn_level = (*g_pSkillInfoTable)[left].GetLearnLevel();
	
	if(skill_learn_level > 150)
		level = min(max(sum_level - skill_learn_level + 1, 0), 10);
	else
		level = (*g_pSkillInfoTable)[left].GetExpLevel();
	
	if(level<=0 && g_eRaceInterface!=RACE_VAMPIRE)
	{
		line_count--;
	}
	else if(g_eRaceInterface == RACE_OUSTERS && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_OUSTERS_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
	{
		line_count--;
	}
	else if(g_eRaceInterface == RACE_SLAYER && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_SLAYER_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
	{
		line_count--;
	}

//////////////////////////////////////////////////////////////////////////
#endif //__SECOND_RARE_SKILL

	if(g_eRaceInterface == RACE_OUSTERS)
	{
		SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[left];
		if(sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND
			)
		{
			line_count++;
		}
	}

	if(left >= SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA && left <= SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA)
		rect.w = g_GetStringWidth((*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+left-SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].GetString(), gpC_base->m_item_desc_pi.hfont);
	else if(left >= SKILL_SWEEPER_BONUS_1 && left <= SKILL_SWEEPER_BONUS_12 )
		rect.w = g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_SWEEPER_BONUS_1+left-SKILL_SWEEPER_BONUS_1].GetString(), gpC_base->m_item_desc_pi.hfont);
	else if(left == SKILL_LOVE_CHAIN)
		rect.w = g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_LOVE_CHAIN].GetString(), gpC_base->m_item_desc_pi.hfont);
	else
		rect.w = 0;


	// 2026-05-15: GetHName (Hangul) -> GetName (English); CLAUDE.md kural 3
	rect.w = max(rect.w, g_GetStringWidth((*g_pSkillInfoTable)[left].GetName(), gpC_base->m_item_name_pi.hfont));
	rect.w = max(rect.w, g_GetStringWidth((*g_pSkillInfoTable)[left].GetName(), gpC_base->m_item_desc_pi.hfont));
	rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// revision?
	if (rect.w < DEFAULT_BOX_WIDTH)
		rect.w = DEFAULT_BOX_WIDTH;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;

	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;

	if (rect.y < 0)
		rect.y = y+right;
	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Info_Description_Calculator
//
// void_ptr = string
//-----------------------------------------------------------------------------
void _Info_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/gpC_base NULL guard.
	if (void_ptr == NULL || gpC_base == NULL) { CT_Done("InfoDescCalc: SAFE-RETURN void_ptr=%p gpC_base=%p", void_ptr, (void*)gpC_base); return; }
	Rect rect;

	//////////////////////////////// start calculation
	int line_count = 0;

	rect.w = g_GetStringWidth((const char *)void_ptr, gpC_base->m_item_name_pi.hfont);
	rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// revision?
//	if (rect.w < DEFAULT_BOX_WIDTH)
//		rect.w = DEFAULT_BOX_WIDTH;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;
	if (rect.y < 0)
		rect.y = y+right;
	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Strings_Description_Calculator
//
// void_ptr = string
//-----------------------------------------------------------------------------
void _Strings_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr NULL guard (LPSTR* deref ediliyor).
	if (void_ptr == NULL) { CT_Done("StringsDescCalc: SAFE-RETURN void_ptr NULL"); return; }

	Rect rect;

	//////////////////////////////// start calculation
	int line_count = left-1;

	LPSTR* ptr = (LPSTR *)void_ptr;

	rect.w = 0;
	for(int i = 0; i < left; i++)
	{
		rect.w = max(g_GetStringWidth( (const char *)ptr[i], gpC_base->m_item_name_pi.hfont), rect.w);
	}

	rect.h = NORMAL_FONT_Y_GAP+(line_count)*NORMAL_FONT_Y_GAP;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// revision?
//	if (rect.w < DEFAULT_BOX_WIDTH)
//		rect.w = DEFAULT_BOX_WIDTH;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;

	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;
	if (rect.y < 0)
		rect.y = y+right;
	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}
//-----------------------------------------------------------------------------
// _SkillTree_Description_Calculator
//
// left = ACTIONINFO
// right = icon height
// void_ptr = SKILLDOMAIN
//-----------------------------------------------------------------------------
void _SkillTree_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — table NULL+OOB + g_pSkillManager NULL guard.
	if (g_pSkillInfoTable == NULL || g_pSkillManager == NULL ||
	    left < 0 || left >= (long)(*g_pSkillInfoTable).GetSize()) {
		CT_Done("SkillTreeDescCalc: SAFE-RETURN tbl=%p mgr=%p left=%ld",
		           (void*)g_pSkillInfoTable, (void*)g_pSkillManager, left);
		return;
	}
	//
	// level infomation
	//
	int domain_level;
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		if((*g_pSkillInfoTable)[left].DomainType >= 0)
			domain_level = (*g_pSkillManager)[(*g_pSkillInfoTable)[left].DomainType].GetDomainLevel();
		else domain_level = 0;
		break;
		
	case RACE_VAMPIRE:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel();
		break;
		
	case RACE_OUSTERS:
		domain_level = (*g_pSkillManager)[SKILLDOMAIN_OUSTERS].GetDomainLevel();
		break;
	}

	const int advance_level		= g_char_slot_ingame.m_AdvancementLevel;
	const int sum_level			= domain_level + advance_level;

	const int skill_learn_level = (*g_pSkillInfoTable)[left].GetLearnLevel();

	Rect rect;
	int line_count = 4;

	if(left < 0)return;
	//////////////////////////////// start calculation
	if( (*g_pSkillInfoTable)[left].GetLearnLevel() > g_pSystemAvailableManager->GetLimitLearnSkillLevel() )
	{
		line_count = 2;
		rect.w = DEFAULT_BOX_WIDTH;
		rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP;
		rect.x = x;
		rect.y = y-rect.h;		
		g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
		return;
	}
	
	MSkillDomain::SKILLSTATUS status = (*g_pSkillManager)[*(SKILLDOMAIN *)void_ptr].GetSkillStatus((ACTIONINFO)left);
//	assert(status != MSkillDomain::SKILLSTATUS_NULL);
//	if (status == MSkillDomain::SKILLSTATUS_NEXT ||
//		 status == MSkillDomain::SKILLSTATUS_OTHER)
//		line_count++;

#if __CONTENTS(__SECOND_RARE_SKILL)
	if(status==MSkillDomain::SKILLSTATUS_LEARNED || g_eRaceInterface == RACE_OUSTERS)
	{
		if (g_eRaceInterface != RACE_VAMPIRE && !(*g_pSkillInfoTable)[left].IsPassive() || g_eRaceInterface == RACE_OUSTERS)
		{
			int level = -1;
			if(skill_learn_level > 150)
			{
				if(status==MSkillDomain::SKILLSTATUS_LEARNED)
					level = min(max(sum_level - skill_learn_level + 1, 0), 10);
				else
					level = 0;
			}
			else
				level = (*g_pSkillInfoTable)[left].GetExpLevel();
			
			if(level<=0)
			{
				line_count--;
			}
			else if(g_eRaceInterface == RACE_OUSTERS && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_OUSTERS_RARE
				|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
			{
				line_count--;
			}
		}
	}
	else if(g_eRaceInterface == RACE_SLAYER && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_SLAYER_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
	{
		line_count--;
	}
	else if(g_eRaceInterface == RACE_VAMPIRE && ((*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_VAMPIRE_RARE
		|| (*g_pSkillInfoTable)[left].GetSkillStep() == SKILL_STEP_COMMON_RARE))
	{
		line_count--;
	}
#endif //__SECOND_RARE_SKILL

	if ((g_eRaceInterface != RACE_SLAYER || (*g_pSkillInfoTable)[left].IsPassive()) && status == MSkillDomain::SKILLSTATUS_LEARNED)
	{
		line_count--;
	}
	else if(status == MSkillDomain::SKILLSTATUS_LEARNED && g_eRaceInterface == RACE_SLAYER)
	{
		line_count++;
		int domain = (*g_pSkillInfoTable)[(ACTIONINFO)left].DomainType;
		if(domain >= 0)
		{
			int domain_grade = max((*g_pSkillManager)[domain].GetDomainLevel()/25,
				(*g_pSkillInfoTable)[(ACTIONINFO)left].GetLearnLevel()/25);
//			int learn_level_grade = (*g_pSkillInfoTable)[(ACTIONINFO)left].GetLearnLevel()/25;
			//int skill_grade = ;//(*g_pSkillInfoTable)[(ACTIONINFO)left].GetSkillStep() - SKILL_STEP_APPRENTICE;
			
			//if(domain_grade - skill_grade >= 0
			//	&& domain_grade - skill_grade <= 2 
			//	&& domain_grade != SKILL_STEP_GRAND_MASTER - SKILL_STEP_APPRENTICE)
			if(domain_grade>= 0 && domain_grade<=2)
				line_count++;
		}
	}
	if(g_eRaceInterface == RACE_OUSTERS)
	{
		SKILLINFO_NODE sInfo = (*g_pSkillInfoTable)[left];
		if(sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_FIRE ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WATER ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_EARTH ||
			sInfo.ElementalDomain == ITEMTABLE_INFO::ELEMENTAL_TYPE_WIND
			)
		{
			line_count++;
		}

		// ���� ��ų�� ��ų ����Ʈ �ʿ� ����
		if(skill_learn_level <= 150
#if __CONTENTS(__SECOND_RARE_SKILL)
			&& (*g_pSkillInfoTable)[left].GetSkillStep() != SKILL_STEP_OUSTERS_RARE
#endif //__SECOND_RARE_SKILL
		)
		{
			line_count++;
		}

		line_count++;
		
		// 2004, 10, 16, sobiet add start
		if(sInfo.GetLearnLevel()!= 1 && left <365 )
			line_count++;
		// 2004, 10, 16, sobiet add end
	}

	// 2026-05-15: GetHName (Hangul) -> GetName (English); CLAUDE.md kural 3
	rect.w = g_GetStringWidth((*g_pSkillInfoTable)[left].GetName(), gpC_base->m_item_name_pi.hfont);
	rect.w = max( rect.w,g_GetStringWidth((*g_pSkillInfoTable)[left].GetName(), gpC_base->m_item_desc_pi.hfont));

	if(left >= SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA && left <= SKILL_HOLYLAND_BLOOD_BIBLE_CHASPA)
	{
		rect.w = max(rect.w, g_GetStringWidth((*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+left-SKILL_HOLYLAND_BLOOD_BIBLE_ARMEGA].GetString(), gpC_base->m_item_desc_pi.hfont));
	}
	else if (left >= SKILL_SWEEPER_BONUS_1 && left <= SKILL_SWEEPER_BONUS_12 )
	{
		rect.w = max(rect.w, g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_SWEEPER_BONUS_1+left-SKILL_SWEEPER_BONUS_1].GetString(), gpC_base->m_item_desc_pi.hfont));
	}

	rect.h = NORMAL_FONT_Y_GAP+(line_count)*SMALL_FONT_Y_GAP;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// revision?
	if (rect.w < DEFAULT_BOX_WIDTH)
		rect.w = DEFAULT_BOX_WIDTH;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;

	if (rect.y < 0)
		rect.y = y+right;
	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Help_Description_Calculator
//
// void_ptr = S_DEFAULT_HELP_STRING *
// left = item count
//-----------------------------------------------------------------------------
void _Help_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr/gpC_base NULL guard (deref ediliyor aşağıda).
	if (void_ptr == NULL || gpC_base == NULL) { CT_Done("HelpDescCalc: SAFE-RETURN void_ptr=%p gpC_base=%p", void_ptr, (void*)gpC_base); return; }
	Rect rect;

	//////////////////////////////// start calculation
	int line_count = 0;
	int sub_w = 0, main_w2 = 0;
	if (((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str2 != NULL)
	{
		line_count += 1;
		main_w2 = g_GetStringWidth(((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str2, gpC_base->m_item_desc_pi.hfont);
	}
	if (((S_DEFAULT_HELP_STRING *)void_ptr)->sz_sub_str != NULL)
	{
		line_count += 1;
		sub_w = g_GetStringWidth(((S_DEFAULT_HELP_STRING *)void_ptr)->sz_sub_str, gpC_base->m_item_desc_pi.hfont);
	}
	rect.w = g_GetStringWidth(((S_DEFAULT_HELP_STRING *)void_ptr)->sz_main_str, gpC_base->m_item_name_pi.hfont);
	rect.h = NORMAL_FONT_Y_GAP+line_count*SMALL_FONT_Y_GAP;

	if (rect.w < main_w2)
		rect.w = main_w2;
	if (rect.w < sub_w)
		rect.w = sub_w;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;

	if (rect.y < 0)
		rect.y = 0;
//		rect.y = y+right;

	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Help_Description_Calculator
//
// void_ptr = S_DEFAULT_HELP_STRING *
// left = item count
//-----------------------------------------------------------------------------
void _BloodBible_Description_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	// FAZ 4 (2026-05-06): Description audit — void_ptr NULL guard (cast/deref ediliyor).
	if (void_ptr == NULL) { CT_Done("BloodBibleDescCalc: SAFE-RETURN void_ptr NULL"); return; }
	Rect rect;

	S_BLOOD_BIBLE_DESCRIPTION *desc = (S_BLOOD_BIBLE_DESCRIPTION*) void_ptr;

	//////////////////////////////// start calculation
	int line_count = 0;
	int width = 0;//, height = 0;


	g_FL2_GetDC();
	if(desc->sz_name != NULL)
	{
		line_count ++;
//		height += g_GetStringHeight(desc->sz_name,gpC_base->m_item_name_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_name,gpC_base->m_item_name_pi.hfont) + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_CHAR_MANAGER_NAME].GetString(),gpC_base->m_item_name_pi.hfont));
	}
	
	if(desc->sz_place != NULL)
	{
		line_count++;
//		height += g_GetStringHeight(desc->sz_place,gpC_base->m_item_desc_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_place,gpC_base->m_item_desc_pi.hfont) + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_POSITION].GetString(), gpC_base->m_item_desc_pi.hfont));
	}
	
	if(desc->sz_shrine != NULL)
	{		
		line_count++;
//		height += g_GetStringHeight(desc->sz_shrine,gpC_base->m_item_desc_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_shrine,gpC_base->m_item_desc_pi.hfont) + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_POSITION].GetString(), gpC_base->m_item_desc_pi.hfont));
	}

	if(desc->sz_player != NULL)
	{
		line_count++;
//		height += g_GetStringHeight(desc->sz_player,gpC_base->m_item_desc_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_player,gpC_base->m_item_desc_pi.hfont) + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_PLAYER].GetString(), gpC_base->m_item_desc_pi.hfont));
	}
	
	if(desc->sz_option != NULL)
	{
		line_count++;
//		height += g_GetStringHeight(desc->sz_option,gpC_base->m_item_desc_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_option,gpC_base->m_item_desc_pi.hfont) + g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_OPTION].GetString(), gpC_base->m_item_desc_pi.hfont));
	}

	if(desc->sz_status != NULL)
	{
		line_count++;
//		height += g_GetStringHeight(desc->sz_status, gpC_base->m_item_desc_pi.hfont);
		width = max(width, g_GetStringWidth(desc->sz_status, gpC_base->m_item_desc_pi.hfont) +g_GetStringWidth((*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_DESC_STATUS].GetString(), gpC_base->m_item_desc_pi.hfont));
	}
	g_FL2_ReleaseDC();
	
	rect.w = width + SIDE_GAP;
	rect.h = NORMAL_FONT_Y_GAP + SMALL_FONT_Y_GAP *( line_count -1)+ SIDE_GAP;
	

	// get (x, y)
	rect.x = x;
	rect.y = y - rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;
	if (rect.y < 0)
		rect.y = 0;
//		rect.y = y+right;

	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}


//-----------------------------------------------------------------------------
// _Multiline_Description_Calculator
//
// void_ptr = S_DEFAULT_HELP_STRING *
// left = item count
//-----------------------------------------------------------------------------
void _Multiline_Info_Calculator(void (*fp_show)(Rect, void *, long, long), int x, int y, void * void_ptr, long left, long right, int z_order)
{
	Rect rect;

	//////////////////////////////// start calculation
	
	int line_count = 0;

	if(right != 0) // �Ѷ��ο� ǥ���� ��Ʈ��
	{
		if(NULL != void_ptr)
		{	
			line_count = (strlen((char*)void_ptr)/right) + 1;

			char szTempBuf[128] = {0,};
			memcpy(szTempBuf, void_ptr, right);
			rect.w = g_GetStringWidth((const char *)szTempBuf, gpC_base->m_item_name_pi.hfont);
		}
		else
			rect.w = g_GetStringWidth((const char *)void_ptr, gpC_base->m_item_name_pi.hfont);
	}
	else
		rect.w = g_GetStringWidth((const char *)void_ptr, gpC_base->m_item_name_pi.hfont);

	rect.h = (line_count)*NORMAL_FONT_Y_GAP;

	rect.w += SIDE_GAP;
	rect.h += SIDE_GAP;

	// get (x, y)
	rect.x = x;
	rect.y = y-rect.h;

	if (rect.x < 0)
		rect.x = 0;
	//if (rect.x+rect.w > RESOLUTION_X)
	//	rect.x = RESOLUTION_X-rect.w;
	if (rect.x+rect.w > g_pUserInformation->iResolution_x)
		rect.x = g_pUserInformation->iResolution_x-rect.w;

	if (rect.y < 0)
		rect.y = y+right;
	//////////////////////////////// end calculation

	g_descriptor_manager.RectCalculationFinished(fp_show, rect, void_ptr, left, right, z_order);
}

//-----------------------------------------------------------------------------
// _Info_Description_Show
//
// 
//-----------------------------------------------------------------------------
void	_Multiline_Info_Show(Rect rect, void * void_ptr, long left, long right)
{
//#ifdef _LIB // Client������ 3D������ ����� �� �ִ�. -.-
	if(NULL == void_ptr)
		return;
	RECT _rect;
	SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h);
	DrawAlphaBox(&_rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
//#endif
//
//	S_RECT dest_rect;
//	S_SURFACEINFO	dest_surface;
//
//#ifndef _LIB
//	if (gpC_base->m_DDSurface_offscreen.Lock())
//	{
//		S_SURFACEINFO src_surface;
//		S_RECT src_rect;
//
//		SetSurfaceInfo(&src_surface, gpC_base->m_DDSurface_offscreen.GetDDSD());
//
//		//SetRect(src_rect, 0, 0, rect.w, rect.h);
//		//filledRect(&src_surface, &src_rect, BLACK);
//#endif
//		if (gpC_base->m_p_DDSurface_back->Lock()) // Descriptor common background�� ����Ѵ�.
//		{
//			SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
//			SetRect(dest_rect, rect.x, rect.y, rect.w, rect.h);
//
//#ifndef _LIB
//			//TransparentBlt16(&dest_surface, &src_surface, &dest_rect, &src_rect);
//			filledRect(&dest_surface, &dest_rect, BLACK);
//#endif
			SetRect(&_rect, rect.x, rect.y, rect.x+rect.w, rect.y+rect.h); // -.- TransparentBlt16()���� ���� ������.. �ƴ� �̷�����!
			gpC_base->m_p_DDSurface_back->DrawRect(&_rect, LIGHT_MAGENTA);
//			rectangle(&dest_surface, dest_rect.x, dest_rect.y, dest_rect.x+dest_rect.w-1, dest_rect.y+dest_rect.h-1, LIGHT_MAGENTA);
//
//			gpC_base->m_p_DDSurface_back->Unlock();
//		}
//
//#ifndef _LIB
//		gpC_base->m_DDSurface_offscreen.Unlock();
//	}
//#endif

	//
	// show contents
	//
	int px = rect.x+SIDE_GAP_HALF;
	int py = rect.y+SIDE_GAP_HALF;

	if(left == 0)left = RGB_WHITE;

	int CurrentPos = right;

	int LineCount = (strlen((char*)void_ptr)/right) + 1;

	char sz_temp[4048];
	char *cur = (char*)void_ptr;
	char char_temp;
	
	strcpy(sz_temp, cur);
	int TempCount = 0;
	while(TempCount<LineCount)
	{
		int check;
		if(g_PossibleStringCut(cur, CurrentPos))
			check = 0; 
		else 
			check = 1;
		
		char_temp = cur[CurrentPos - check];
		cur[CurrentPos - check] = '\0';
		
		g_PrintColorStr(px, py, cur, gpC_base->m_item_name_pi, left);
		py += NORMAL_FONT_Y_GAP;
		
		if(strlen(cur) < CurrentPos-check) 
			break;
		cur += CurrentPos-check;
		*cur = char_temp;
		if(*cur == ' ')cur++;
		TempCount ++;
	}
}
//-----------------------------------------------------------------------------
// g_InstallDescriptor
//
// 
//-----------------------------------------------------------------------------
void	g_InstallDescriptor()
{
	g_descriptor_manager.AddDescribedUnit(DID_ITEM, _Item_Description_Calculator, _Item_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_SKILL, _Skill_Description_Calculator, _Skill_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_HELP, _Help_Description_Calculator, _Help_Description_Show, false);
	g_descriptor_manager.AddDescribedUnit(DID_SKILLTREE, _SkillTree_Description_Calculator, _SkillTree_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_INFO, _Info_Description_Calculator, _Info_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_STRINGS, _Strings_Description_Calculator, _Strings_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_BLOOD_BIBLE_STATUS, _BloodBible_Description_Calculator, _BloodBible_Description_Show);
	g_descriptor_manager.AddDescribedUnit(DID_MULTILINE_INFO, _Multiline_Info_Calculator, _Multiline_Info_Show);

	InitEquipSetData();
	InitGemDescData();
}

