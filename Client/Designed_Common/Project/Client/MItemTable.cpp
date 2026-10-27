//----------------------------------------------------------------------
// MItemTable.cpp
//----------------------------------------------------------------------
// Item?뼥?뼱吏??뒗嫄? ?븯怨? ?떢?쑝硫?..
// 媛? info?뿉?떎媛? DropFrameID( ...)瑜? ?젣???濡? ?꽕?젙?븯硫? ?맂?떎.
// 臾쇰줎 .. MTopView?뿉?뒗 ItemDropFPK??? ItemDropFPK媛? ?젣???濡? ?맂寃? ?엳?뼱?빞 寃좎??..
//----------------------------------------------------------------------
// Edit Log
//
//	- dj 2006.12.21 ?궎耳??떎 臾쇰씪?떎?씪 ?뱶?씪?씠?뼱?뱶由? ?궗?씠?젋留? ?젅?돩?렂?뜕?듃 ?넗由щ뱶 由ъ뒪???由? ?씠誘몄?? 援먯껜瑜? ?쐞?븳 踰덊샇 (蹂?寃?)?븷?떦
//
#include "Client_PCH.h"
#include "MItem.h"
#include "MItemTable.h"
#include "AddonDef.h"
#include "SkillDef.h"
#include "SoundDef.h"
#include "MGameStringTable.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef __GAME_CLIENT__
	#include "DebugInfo.h"
#endif

//#if !defined(__GAME_CLIENT__) || defined(OUTPUT_DEBUG) || defined(_DEBUG)
#if defined(__INIT_INFO__) || !defined(__GAME_CLIENT__)
	#define __INIT_ITEM__
#endif

#ifdef __INIT_ITEM__
	#include "InitInfo.h"
#endif

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
ITEMCLASS_TABLE	*	g_pItemTable = NULL;
COLORREF g_ELEMENTAL_COLOR[5] = { RGB(255, 100, 100), RGB(100, 100, 255), RGB(255, 180, 100), RGB(100, 100, 255), RGB(192, 192, 255) };
int g_ELEMENTAL_STRING_ID[5] = { UI_STRING_MESSAGE_ELEMENTAL_FIRE, UI_STRING_MESSAGE_ELEMENTAL_WATER, UI_STRING_MESSAGE_ELEMENTAL_EARTH, UI_STRING_MESSAGE_ELEMENTAL_WIND, UI_STRING_MESSAGE_ELEMENTAL_SUM};

static void WriteItemTypeTableDiagnostic(const char* format, ...)
{
	static const bool kEnabled = false;

	if (!kEnabled)
		return;

	FILE* fp = fopen("diagnostics_typetable.txt", "a");
	if (fp == NULL)
		return;

	va_list args;
	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);

	fclose(fp);
}

//----------------------------------------------------------------------
//
//				ITEMTABLE_INFO
//
//----------------------------------------------------------------------
ITEMTABLE_INFO::ITEMTABLE_INFO()
{
	// Frame ID
	TileFrameID			= FRAMEID_NULL;		// Tile?뿉?꽌?쓽 FrameID
	InventoryFrameID	= FRAMEID_NULL;		// Inventory?뿉?꽌?쓽 Frame ID
	GearFrameID			= FRAMEID_NULL;		// Gear?뿉?꽌?쓽 Frame ID
	AddonMaleFrameID	= FRAMEID_NULL;		// ?옣李⑺뻽?쓣 ?븣?쓽 ?룞?옉 FrameID - ?궓?옄
	AddonFemaleFrameID	= FRAMEID_NULL;		// ?옣李⑺뻽?쓣 ?븣?쓽 ?룞?옉 FrameID - ?뿬?옄

	// Sound ID
	UseSoundID			= SOUNDID_NULL;		// Item ?궗?슜 SoundID			
	TileSoundID			= SOUNDID_NULL;		// Item 以띻린 SoundID
	InventorySoundID	= SOUNDID_NULL;		// Inventory?뿉?꽌?쓽 Sound
	GearSoundID			= SOUNDID_NULL;		// Gear?뿉?꽌?쓽 Sound

	bMaleOnly			= false;
	bFemaleOnly			= false;
	
	// inventory?뿉?꽌?쓽 Grid?겕湲?
	GridWidth			= 1;
	GridHeight			= 1;

	// item ?옄泥댁뿉 ????븳 怨좎젙?맂 ?젙蹂?
	Weight				= 0;				// 臾닿쾶	

	// 媛믩뱾.. --> Protection, 怨듦꺽?젰, ?궗?젙嫄곕━
	Value1				= 0;
	Value2				= 0;
	Value3				= 0;
	Value4				= 0;
	Value5				= 0;
	Value6				= 0;
	Value7				= 0;

	// ?븘?슂?뒫?젰
	RequireSTR			= 0;
	RequireDEX			= 0;
	RequireINT			= 0;
	RequireSUM			= 0;
	RequireAdvancementLevel = 0;
	RequireLevel		= 0;

	// 湲곕낯 怨듦꺽 ActionInfo
	UseActionInfo		= ACTIONINFO_NULL;

	// silver coating
	SilverMax			= 0;

	ToHit				= 0;

	MaxNumber			= 1;

	CriticalHit			= 0;

	ItemStyle			= 0;
#if __CONTENTS(__ONIBLA_ITEM)
	NormalItemGrade		= 0;
#endif //__ONIBLA_ITEM

	ElementalType		= ELEMENTAL_TYPE_ANY;
	Elemental			= 0;

	DescriptionFrameID = 0;
	DropItemNameTag		= 0;
	tx		= 0;
	//tx1		= 0;
	//tx2		= 0;
	//tx3		= 0;
}

ITEMTABLE_INFO::~ITEMTABLE_INFO()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set SoundID
//----------------------------------------------------------------------
void	
ITEMTABLE_INFO::SetSoundID(TYPE_SOUNDID tile, TYPE_SOUNDID inventory, TYPE_SOUNDID gear, TYPE_SOUNDID use)
{
	TileSoundID = tile;
	InventorySoundID = inventory;
	GearSoundID = gear;
	UseSoundID = use;	
}

//----------------------------------------------------------------------
// Set FrameID
//----------------------------------------------------------------------
void	
ITEMTABLE_INFO::SetFrameID(TYPE_FRAMEID tile, TYPE_FRAMEID inventory, TYPE_FRAMEID gear, TYPE_FRAMEID use)
{
	TileFrameID = tile;
	InventoryFrameID = inventory;
	GearFrameID = gear;	
	UseFrameID = use;
}

//----------------------------------------------------------------------
// Set DropFrameID
//----------------------------------------------------------------------
void	
ITEMTABLE_INFO::SetDropFrameID(TYPE_FRAMEID drop)
{
	DropFrameID	= drop;
}

//----------------------------------------------------------------------
// Set Addon FrameID
//----------------------------------------------------------------------
void
ITEMTABLE_INFO::SetAddonFrameID(TYPE_FRAMEID male, TYPE_FRAMEID female)
{
	AddonMaleFrameID = male;
	AddonFemaleFrameID = female;
}

//----------------------------------------------------------------------
// Set Grid
//----------------------------------------------------------------------
void	
ITEMTABLE_INFO::SetGrid(BYTE width, BYTE height)
{
	GridWidth = width;
	GridHeight = height;
}

//----------------------------------------------------------------------
// Set Values
//----------------------------------------------------------------------
void	
ITEMTABLE_INFO::SetValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7)
{
	Value1 = v1;
	Value2 = v2;
	Value3 = v3;
	Value4 = v4;
	Value5 = v5;
	Value6 = v6;
	Value7 = v7;
}

//----------------------------------------------------------------------
// Save
//----------------------------------------------------------------------
void			
ITEMTABLE_INFO::SaveToFile(std::ofstream& file)
{
	// Ad캇 kaydet
	EName.SaveToFile( file );
	HName.SaveToFile( file );
	Description.SaveToFile( file );

	// Frame ID
	file.write((const char*)&TileFrameID, SIZE_FRAMEID);
	file.write((const char*)&InventoryFrameID, SIZE_FRAMEID);
	file.write((const char*)&GearFrameID, SIZE_FRAMEID);
	file.write((const char*)&DropFrameID, SIZE_FRAMEID);
	file.write((const char*)&AddonMaleFrameID, SIZE_FRAMEID);
	file.write((const char*)&AddonFemaleFrameID, SIZE_FRAMEID);
	file.write((const char*)&UseFrameID, SIZE_FRAMEID);
	
	// Sound ID
	file.write((const char*)&UseSoundID, SIZE_SOUNDID);
	file.write((const char*)&TileSoundID, SIZE_SOUNDID);
	file.write((const char*)&InventorySoundID, SIZE_SOUNDID);
	file.write((const char*)&GearSoundID, SIZE_SOUNDID);

	// inventory?뿉?꽌?쓽 Grid?겕湲?
	file.write((const char*)&GridWidth, 1);
	file.write((const char*)&GridHeight, 1);

	// 媛?寃?
	file.write((const char*)&Price, SIZE_ITEM_PRICE);

	// 臾닿쾶
	file.write((const char*)&Weight, SIZE_ITEM_WEIGHT);

	// 媛믩뱾
	file.write((const char*)&Value1, 4);
	file.write((const char*)&Value2, 4);
	file.write((const char*)&Value3, 4);
	file.write((const char*)&Value4, 4);
	file.write((const char*)&Value5, 4);
	file.write((const char*)&Value6, 4);
	file.write((const char*)&Value7, 4);
	
	// ?븘?슂?뒫?젰
	file.write((const char*)&RequireSTR, 1);
	file.write((const char*)&RequireDEX, 1);
	file.write((const char*)&RequireINT, 1);		
	file.write((const char*)&RequireSUM, 2);
	file.write((const char*)&RequireLevel, 1);
	file.write((const char*)&RequireAdvancementLevel, 1);
	file.write((const char*)&bMaleOnly, 1);
	file.write((const char*)&bFemaleOnly, 1);
  //  int tmp2;
     //file.write((const char*)&tmp2, 2);
	// UseActionInfo
	file.write((const char*)&UseActionInfo, 4);
	//if (UseActionInfo < 3452829695)
	//{
	//	UseActionInfo += 23;
	//}
	file.write((const char*)&SilverMax, 4);

	file.write((const char*)&ToHit, 4);

	file.write((const char*)&MaxNumber, SIZE_ITEM_NUMBER);

	file.write((const char*)&CriticalHit, 4);	

	BYTE DefaultOptionListSize = DefaultOptionList.size();
	file.write((const char*)&DefaultOptionListSize, 1);
	std::list<TYPE_ITEM_OPTION>::iterator itr = DefaultOptionList.begin();

	while(itr != DefaultOptionList.end())
	{
		TYPE_ITEM_OPTION_2 Option = *itr;
		file.write((const char*)&Option, sizeof(TYPE_ITEM_OPTION_2));
		//	file.write((const char*)&Option, sizeof(TYPE_ITEM_OPTION));
		//file.write((const char*)&kimneww,1);
		//file.write((const char*)&kimneww,1);
 	//	file.write((const char*)&kimneww,1);

		itr++;
	}

	file.write((const char*)&ItemStyle, 4);

	file.write((const char*)&ElementalType, 4);
	file.write((const char*)&Elemental, 2);

	file.write((const char*)&Race, 1);

	// 2005, 1, 14, sobeit add start - ItemDescription.spk ?뿉?꽌 ?벐?뒗 frameID
	file.write((const char*)&DescriptionFrameID, SIZE_FRAMEID);
	// 2005, 1, 14, sobeit add end
	
	// 2006.12.26 chyaya add start - ?븘?씠?뀥 ?씠?룞 ?젣?븳 ?냽?꽦
	file.write((const char*)&ItemMoveControl, ItemMoveControl.GetValueSize());
	// 2006.12.26 chyaya add end

	// by diesirace 20070314 add start
	file.write((const char*)&ItemCanAdvance, ItemCanAdvance.GetValueSize());

	file.write((const char*)&DropItemNameTag, 4);

#if __CONTENTS(__ONIBLA_ITEM)
	file.write((const char*)&NormalItemGrade, 4);
	file.write((const char*)&tx,4);
	//file.write((const char*)&tx1,4);
	//file.write((const char*)&tx2,4);
	//file.write((const char*)&tx3,4);
#endif //__ONIBLA_ITEM


	//end
}

//----------------------------------------------------------------------
// Load
//----------------------------------------------------------------------
void			
ITEMTABLE_INFO::LoadFromFile(ivfstream& file)
{
	EName.LoadFromFile( file );
	HName.LoadFromFile( file );
	Description.LoadFromFile( file );

	// Frame ID
	file.read((char*)&TileFrameID, SIZE_FRAMEID);
	file.read((char*)&InventoryFrameID, SIZE_FRAMEID);
	file.read((char*)&GearFrameID, SIZE_FRAMEID);
	file.read((char*)&DropFrameID, SIZE_FRAMEID);
	file.read((char*)&AddonMaleFrameID, SIZE_FRAMEID);
	file.read((char*)&AddonFemaleFrameID, SIZE_FRAMEID);
	file.read((char*)&UseFrameID, SIZE_FRAMEID);

	// Sound ID
	file.read((char*)&UseSoundID, SIZE_SOUNDID);
	file.read((char*)&TileSoundID, SIZE_SOUNDID);
	file.read((char*)&InventorySoundID, SIZE_SOUNDID);
	file.read((char*)&GearSoundID, SIZE_SOUNDID);	

	// grid ?겕湲?
	file.read((char*)&GridWidth, 1);	
	file.read((char*)&GridHeight, 1);	

	// 媛?寃?
	file.read((char*)&Price, SIZE_ITEM_PRICE);

	// 臾닿쾶
	file.read((char*)&Weight, SIZE_ITEM_WEIGHT);	
	
	// 媛믩뱾 
	file.read((char*)&Value1, 4);	
	file.read((char*)&Value2, 4);
	file.read((char*)&Value3, 4);
	file.read((char*)&Value4, 4);
	file.read((char*)&Value5, 4);
	file.read((char*)&Value6, 4);
	file.read((char*)&Value7, 4);

	// ?븘?슂?뒫?젰
	file.read((char*)&RequireSTR, 1);
	file.read((char*)&RequireDEX, 1);
	file.read((char*)&RequireINT, 1);		
	file.read((char*)&RequireSUM, 2);
	file.read((char*)&RequireLevel, 1);
	file.read((char*)&RequireAdvancementLevel, 1);
	file.read((char*)&bMaleOnly, 1);
	file.read((char*)&bFemaleOnly, 1);
	//int tmp2;
    //file.read((char*)&tmp2, 2);
	// UseActionInfo
	file.read((char*)&UseActionInfo, 4);
//	if (UseActionInfo < 3452829695)
//	{
	//	UseActionInfo += 23;
	//}
	file.read((char*)&SilverMax, 4);

	file.read((char*)&ToHit, 4);

	file.read((char*)&MaxNumber, SIZE_ITEM_NUMBER);

	file.read((char*)&CriticalHit, 4);

	BYTE DefaultOptionListSize = 0;
	file.read((char*)&DefaultOptionListSize, 1);
	
	for(int i = 0; i < DefaultOptionListSize; i++)
	{
		TYPE_ITEM_OPTION_2 TempOptionType;
	//	file.read((char*)&TempOptionType, sizeof(TYPE_ITEM_OPTION));
		file.read((char*)&TempOptionType, sizeof(TYPE_ITEM_OPTION_2));
		//file.read((char*)&kimnew,1);
		//file.read((char*)&kimnew,1);
		//file.read((char*)&kimnew,1);
		DefaultOptionList.push_back(TempOptionType);
	}

	file.read((char*)&ItemStyle, 4);

	file.read((char*)&ElementalType, 4);
	file.read((char*)&Elemental, 2);

	file.read((char*)&Race, 1);

	// 2005, 1, 14, sobeit add start - ItemDescription.spk ?뿉?꽌 ?벐?뒗 frameID
	file.read((char*)&DescriptionFrameID, SIZE_FRAMEID);
	// 2005, 1, 14, sobeit add end

	// 2006.12.26 chyaya add start - ?븘?씠?뀥 ?씠?룞 ?젣?븳 ?냽?꽦
	file.read((char*)&ItemMoveControl, ItemMoveControl.GetValueSize());
	// 2006.12.26 chyaya add end

	// by diesirace 20070314 add start
	file.read((char*)&ItemCanAdvance, ItemCanAdvance.GetValueSize());
	//end
	file.read((char*)&DropItemNameTag, 4);

#if __CONTENTS(__ONIBLA_ITEM)
	file.read((char*)&NormalItemGrade, 4);
	file.read((char*)&tx, 4);
	//file.read((char*)&tx1, 4);
	//file.read((char*)&tx2, 4);
	//file.read((char*)&tx3, 4);
#endif //__ONIBLA_ITEM

}


//----------------------------------------------------------------------
// Save CSV File
//----------------------------------------------------------------------
void			
ITEMTABLE_INFO::SaveToCSVFile(std::ofstream& file, int classIdx, int typeIdx)
{
#ifdef __INIT_ITEM__

	SaveNumberToCSVFile(file, classIdx);
	SaveNumberToCSVFile(file, typeIdx);

	SaveStringToCSVFile(file, EName);
	SaveStringToCSVFile(file, HName);
	SaveStringToCSVFile(file, Description);

	// Frame ID
	SaveNumberToCSVFile(file, TileFrameID);
	SaveNumberToCSVFile(file, InventoryFrameID);
	SaveNumberToCSVFile(file, GearFrameID);
	SaveNumberToCSVFile(file, DropFrameID);
	SaveNumberToCSVFile(file, AddonMaleFrameID);
	SaveNumberToCSVFile(file, AddonFemaleFrameID);

	// Sound ID
	SaveNumberToCSVFile(file, UseSoundID);
	SaveNumberToCSVFile(file, InventorySoundID);
	SaveNumberToCSVFile(file, GearSoundID);

	// inventory?뿉?꽌?쓽 Grid?겕湲?
	SaveNumberToCSVFile(file, GridWidth);
	SaveNumberToCSVFile(file, GridHeight);

	// 媛?寃?
	SaveNumberToCSVFile(file, Price);

	// 臾닿쾶
	SaveNumberToCSVFile(file, Weight);
	
	// 媛믩뱾
	SaveNumberToCSVFile(file, Value1, -1);
	SaveNumberToCSVFile(file, Value2, -1);
	SaveNumberToCSVFile(file, Value3, -1);
	SaveNumberToCSVFile(file, Value4, -1);
	SaveNumberToCSVFile(file, Value5, -1);
	SaveNumberToCSVFile(file, Value6, -1);
	SaveNumberToCSVFile(file, Value7, -1);
	
	// ?븘?슂?뒫?젰
	SaveNumberToCSVFile(file, RequireSTR);
	SaveNumberToCSVFile(file, RequireDEX);
	SaveNumberToCSVFile(file, RequireINT);
	SaveNumberToCSVFile(file, RequireSUM);
	SaveNumberToCSVFile(file, RequireLevel);
	SaveNumberToCSVFile(file, RequireAdvancementLevel);
	SaveNumberToCSVFile(file, bMaleOnly);
	SaveNumberToCSVFile(file, bFemaleOnly);
	
	// UseActionInfo
	SaveNumberToCSVFile(file, UseActionInfo);

	SaveNumberToCSVFile(file, SilverMax, -1);
	SaveNumberToCSVFile(file, ToHit, -1);
	SaveNumberToCSVFile(file, MaxNumber);
	SaveNumberToCSVFile(file, CriticalHit);


	BYTE DefaultOptionListSize = DefaultOptionList.size();
	SaveNumberToCSVFile(file, DefaultOptionListSize);
	
	std::list<TYPE_ITEM_OPTION>::iterator itr = DefaultOptionList.begin();

	while(itr != DefaultOptionList.end())
	{
		TYPE_ITEM_OPTION Option = *itr;
		file << (int)Option << ' ';

		itr++;
	}

	file << CSV_TOKEN;


	SaveNumberToCSVFile(file, ItemStyle);

	SaveNumberToCSVFile(file, (int)ElementalType, -1);
	SaveNumberToCSVFile(file, Elemental);

	SaveNumberToCSVFile(file, Race);
	SaveNumberToCSVFile(file, DescriptionFrameID);

	SaveBoolToCSVFile(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_DROP));
	SaveBoolToCSVFile(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_DISPLAY_PERSNALSHOP));
	SaveBoolToCSVFile(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_KEEP_STORAGE));
	SaveBoolToCSVFile(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_TRADE));
	SaveBoolToCSVFile(file, ItemMoveControl.GetAttr(ITEMMOVE_PICKUP_ONCE));

	SaveBoolToCSVFile(file, ItemCanAdvance.GetAttr(ADVANCE_CANNOT_USE_ITEM));
	SaveBoolToCSVFile(file, ItemCanAdvance.GetAttr(NONADVANCE_CANNOT_USE_ITEM));
	SaveBoolToCSVFile(file, DropItemNameTag);

#if __CONTENTS(__ONIBLA_ITEM)
	SaveNumberToCSVFile(file, NormalItemGrade);
	SaveNumberToCSVFile(file, tx);
		//SaveNumberToCSVFile(file, tx1);
		//	SaveNumberToCSVFile(file, tx2);
		//		SaveNumberToCSVFile(file, tx3);
#endif //__ONIBLA_ITEM

	file << '\n';

#endif
}

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
//----------------------------------------------------------------------
// Save CSV File
//----------------------------------------------------------------------
void			
ITEMTABLE_INFO::LoadToCSVFile(std::ifstream& file, int classIdx, int typeIdx)
{
#ifdef __INIT_ITEM__

	LoadCSVFileToNumber(file, classIdx);
	LoadCSVFileToNumber(file, typeIdx);

	LoadCSVFileToString(file, EName);
	LoadCSVFileToString(file, HName);
	LoadCSVFileToString(file, Description);

	// Frame ID
	LoadCSVFileToNumber(file, TileFrameID);
	LoadCSVFileToNumber(file, InventoryFrameID);
	LoadCSVFileToNumber(file, GearFrameID);
	LoadCSVFileToNumber(file, DropFrameID);
	LoadCSVFileToNumber(file, AddonMaleFrameID);
	LoadCSVFileToNumber(file, AddonFemaleFrameID);

	// Sound ID
	LoadCSVFileToNumber(file, UseSoundID);
	LoadCSVFileToNumber(file, TileSoundID);
	LoadCSVFileToNumber(file, InventorySoundID);
	LoadCSVFileToNumber(file, GearSoundID);

	// inventory?뿉?꽌?쓽 Grid?겕湲?
	LoadCSVFileToNumber(file, GridWidth);
	LoadCSVFileToNumber(file, GridHeight);

	// 媛?寃?
	LoadCSVFileToNumber(file, Price);

	// 臾닿쾶
	LoadCSVFileToNumber(file, Weight);
	
	// 媛믩뱾
	LoadCSVFileToNumber(file, Value1, -1);
	LoadCSVFileToNumber(file, Value2, -1);
	LoadCSVFileToNumber(file, Value3, -1);
	LoadCSVFileToNumber(file, Value4, -1);
	LoadCSVFileToNumber(file, Value5, -1);
	LoadCSVFileToNumber(file, Value6, -1);
	LoadCSVFileToNumber(file, Value7, -1);
	
	// ?븘?슂?뒫?젰
	LoadCSVFileToNumber(file, RequireSTR);
	LoadCSVFileToNumber(file, RequireDEX);
	LoadCSVFileToNumber(file, RequireINT);
	LoadCSVFileToNumber(file, RequireSUM);
	LoadCSVFileToNumber(file, RequireLevel);
	LoadCSVFileToNumber(file, RequireAdvancementLevel);
	LoadCSVFileToNumber(file, bMaleOnly);
	LoadCSVFileToNumber(file, bFemaleOnly);
	
	// UseActionInfo
	LoadCSVFileToNumber(file, UseActionInfo);

	LoadCSVFileToNumber(file, SilverMax, -1);
	LoadCSVFileToNumber(file, ToHit, -1);
	LoadCSVFileToNumber(file, MaxNumber);
	LoadCSVFileToNumber(file, CriticalHit);


	BYTE DefaultOptionListSize = DefaultOptionList.size();
	LoadCSVFileToNumber(file, DefaultOptionListSize);
	
	std::list<TYPE_ITEM_OPTION>::iterator itr = DefaultOptionList.begin();

	while(itr != DefaultOptionList.end())
	{
		TYPE_ITEM_OPTION Option = *itr;
		file << (int)Option << ' ';

		itr++;
	}

	file << CSV_TOKEN;


	LoadCSVFileToNumber(file, ItemStyle);

	LoadCSVFileToNumber(file, (int)ElementalType, -1);
	LoadCSVFileToNumber(file, Elemental);

	LoadCSVFileToNumber(file, Race);
	LoadCSVFileToNumber(file, DescriptionFrameID);

	LoadCSVFileToBool(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_DROP));
	LoadCSVFileToBool(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_DISPLAY_PERSNALSHOP));
	LoadCSVFileToBool(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_KEEP_STORAGE));
	LoadCSVFileToBool(file, ItemMoveControl.GetAttr(ITEMMOVE_CANNOT_TRADE));
	LoadCSVFileToBool(file, ItemMoveControl.GetAttr(ITEMMOVE_PICKUP_ONCE));

	LoadCSVFileToBool(file, ItemCanAdvance.GetAttr(ADVANCE_CANNOT_USE_ITEM));
	LoadCSVFileToBool(file, ItemCanAdvance.GetAttr(NONADVANCE_CANNOT_USE_ITEM));
	LoadCSVFileToBool(file, DropItemNameTag);

#if __CONTENTS(__ONIBLA_ITEM)
	LoadCSVFileToNumber(file, NormalItemGrade);
	LoadCSVFileToNumber(file, tx);
		//LoadCSVFileToNumber(file, tx1)
		//		LoadCSVFileToNumber(file, tx2)
		//			LoadCSVFileToNumber(file, tx3)
#endif //__ONIBLA_ITEM

	file << '\n';

#endif
}
#endif //__ITEMINFO_TABLEATION_PROJECT


//----------------------------------------------------------------------
//
//							ITEMTYPE_TABLE
//
//----------------------------------------------------------------------
void
ITEMTYPE_TABLE::LoadFromFile(ivfstream& file)
{
	WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE::LoadFromFile ENTER\n");

	CTypeTable<ITEMTABLE_INFO>::LoadFromFile(file);

	WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE: CTypeTable base returned, m_Size=%d, m_pTypeInfo=%p\n", m_Size, (void*)m_pTypeInfo);

	if (m_pTypeInfo == NULL || m_Size <= 0) {
		WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE: SKIP AveragePrice (null/empty)\n");
		m_AveragePrice = 0;
		return;
	}

	m_AveragePrice = 0;
	int count = 0;

	WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE: Start AveragePrice loop m_Size=%d\n", m_Size);

	for (int i=0; i<m_Size; i++)
	{
		if (i % 500 == 0) {
			WriteItemTypeTableDiagnostic("    AvgLoop i=%d\n", i);
		}
		if(m_pTypeInfo[i].DefaultOptionList.empty())
		{
			m_AveragePrice += m_pTypeInfo[i].Price;
			count ++;
		}
	}

	WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE: Finished AveragePrice count=%d\n", count);

	if(count)
	{
		m_AveragePrice /= count;
		m_AveragePrice /= 1000;
		m_AveragePrice *= 100;
	}

	WriteItemTypeTableDiagnostic("  >> ITEMTYPE_TABLE::LoadFromFile EXIT OK\n");
}


void
ITEMTYPE_TABLE::SaveToCSVFile(std::ofstream& file, int classIdx)
{
	// ?븘臾? 寃껊룄 ?뾾?뒗 寃쎌슦
	if (m_pTypeInfo==NULL)
		return;

	// 媛곴컖?쓽 ?젙蹂? ????옣
	for (int i=0; i<m_Size; i++)
	{
		m_pTypeInfo[i].SaveToCSVFile(file, classIdx, i);
	}
}

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
void
ITEMTYPE_TABLE::LoadToCSVFile(std::ifstream& file, int classIdx)
{
	// ?븘臾? 寃껊룄 ?뾾?뒗 寃쎌슦
	if (m_pTypeInfo==NULL)
		return;

	// 媛곴컖?쓽 ?젙蹂? ????옣
	for (int i=0; i<m_Size; i++)
	{
		m_pTypeInfo[i].LoadToCSVFile(file, classIdx, i);
	}
}
#endif //__ITEMINFO_TABLEATION_PROJECT

//----------------------------------------------------------------------
//
//							ITEMCLASS_TABLE
//
//----------------------------------------------------------------------
ITEMCLASS_TABLE::ITEMCLASS_TABLE()
{
	int itemType = 0;
	int i = 0, j = 0;

}

ITEMCLASS_TABLE::~ITEMCLASS_TABLE()
{
}

//---------------------------------------------------------------------
// c class瑜? size媛쒕쭔?겮 珥덇린?솕?븳?떎.
//---------------------------------------------------------------------
void
ITEMCLASS_TABLE::InitClass( int c, int size )
{
	// class?뿉 size媛쒕쭔?겮 type?쓣 ?깮?꽦	
	m_pTypeInfo[c].Init( size );
}


void
ITEMCLASS_TABLE::SaveToCSVFile(std::ofstream& file)
{
	// ?븘臾? 寃껊룄 ?뾾?뒗 寃쎌슦
	if (m_pTypeInfo==NULL)
		return;

	// 媛곴컖?쓽 ?젙蹂? ????옣
	for (int i=0; i<m_Size; i++)
	{
		m_pTypeInfo[i].SaveToCSVFile(file, i);
	}
}

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
void
ITEMCLASS_TABLE::LoadToCSVFile(std::ifstream& file)
{
	// ?븘臾? 寃껊룄 ?뾾?뒗 寃쎌슦
	if (m_pTypeInfo==NULL)
		return;

	// 媛곴컖?쓽 ?젙蹂? ????옣
	for (int i=0; i<m_Size; i++)
	{
		m_pTypeInfo[i].LoadToCSVFile(file, i);
	}
}
#endif //__ITEMINFO_TABLEATION_PROJECT
