//----------------------------------------------------------------------
// MCreatureTable.h
//----------------------------------------------------------------------
// Creature�� ������ ���� ������ �����ϴ� class
//----------------------------------------------------------------------
//
// [ �������� �ִ� ���� ] 
//
// - ���� FrameID
// - Sprite FilePosition
// - First SpriteID
// - �̸�... ���...
// 
//----------------------------------------------------------------------

#ifndef	__MCREATURETABLE_H__
#define	__MCREATURETABLE_H__

#pragma warning(disable:4786)

#include "MObject.h"
#include "SpriteLib\DrawTypeDef.h"
#include "CTypeTable.h"
#include "MString.h"
#include <list>
#include <vector>
std::ifstream;
std::ofstream;

//----------------------------------------------------------------------
// Creature�� ����
//----------------------------------------------------------------------
enum CREATURETRIBE
{
	CREATURETRIBE_SLAYER,			// 0  Slayer
	CREATURETRIBE_VAMPIRE,			// 1  Vampire
	CREATURETRIBE_NPC,				// 2  NPC
	CREATURETRIBE_SLAYER_NPC,		// 3  Slayer-sprite NPC
	CREATURETRIBE_OUSTERS,			// 4  Ousters
	CREATURETRIBE_OUSTERS_NPC,		// 5  Ousters-sprite NPC
	// 2026-05-07: orijinal Korean source (Referans Proje/.../MCreatureTable.h:41)
	// ile sync — Umbra creature.inf bu tribe değerlerini kullanıyor (39 + 1 entry).
	// Hepsi IsVampire() döner → ACTION_MAX_VAMPIRE (=17).
	CREATURETRIBE_UNKNOW1,			// 6
	CREATURETRIBE_UNKNOW2,			// 7
	CREATURETRIBE_UNKNOW3,			// 8
	CREATURETRIBE_UNKNOW4,			// 9
	CREATURETRIBE_UNKNOW5,			// 10
	MAX_CREATURETRIBE
};

//----------------------------------------------------------------------
// CreatureType
//----------------------------------------------------------------------
#define	CREATURETYPE_SLAYER_MALE		0
#define	CREATURETYPE_SLAYER_FEMALE		1
#define	CREATURETYPE_VAMPIRE_MALE1		2
#define	CREATURETYPE_VAMPIRE_FEMALE1	3
#define	CREATURETYPE_VAMPIRE_MALE2		514
#define	CREATURETYPE_VAMPIRE_FEMALE2	515
#define	CREATURETYPE_VAMPIRE_MALE3		558
#define	CREATURETYPE_VAMPIRE_FEMALE3	559
#define	CREATURETYPE_BAT				185
#define	CREATURETYPE_WOLF				186
#define	CREATURETYPE_SLAYER_OPERATOR	367
#define	CREATURETYPE_VAMPIRE_OPERATOR	369
#define	CREATURETYPE_OUSTERS_OPERATOR	652
#define CREATURETYPE_OUSTERS			651
#define CREATURETYPE_WER_WOLF			671
#define CREATURETYPE_GHOST				716
#define CREATURETYPE_INSTALL_TURRET		742
#define CREATURETYPE_ROCKET_LUNCHER		767
#define CREATURETYPE_WILD_WOLF			787
#define	CREATURETYPE_VAMPIRE_MALE4		804
#define	CREATURETYPE_VAMPIRE_FEMALE4	805
#define	CREATURETYPE_VAMPIRE_GHOST		813
#define CREATURETYPE_BAT_STORM_1		924
#define CREATURETYPE_BAT_STORM_2		925
#define CREATURETYPE_XRL_ROCKET_LUNCHER	1027
#define CREATURETYPE_FLITTERMOUSE		1028
#define CREATURETYPE_SHAPE_OF_DEMON		1042
#define CREATURETYPE_INSTALL_MK1                1357
#define CREATURETYPE_NEDE                       1208
#define CREATURETYPE_KLTL                       1712
#define CREATURETYPE_DARK_WOLF                  1730


//----------------------------------------------------------------------
// �����̾� NPC�� ���忡 ���� ����
//----------------------------------------------------------------------
class ITEM_WEARINFO
{
	public :
		WORD		skinColor;
		WORD		hairColor;		
		WORD		jacketColor;
		WORD		pantsColor;
		WORD		helmetColor;
		WORD		weaponColor;
		WORD		shieldColor;
		WORD		motorcycleColor;
		
		BYTE		hair;		
		BYTE		jacket;
		BYTE		pants;
		BYTE		helmet;
		BYTE		weapon;
		BYTE		shield;
		BYTE		motorcycle;

	public :
		ITEM_WEARINFO();

		void		operator = (const ITEM_WEARINFO& info);
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(ivfstream& file);
};


//----------------------------------------------------------------------
//
// �� ������ Creature�� ���� ����
//
//----------------------------------------------------------------------
class CREATURETABLE_INFO {
	public :
//�Ʒ� csv������ ũ��ó �̸��� csv���Ϸ� �̾� ���� ���� �����̴�. ���� ���࿡�� ������ �����Ƿ� ��쿡 ���� ���� �ص� ������.
//���� �Լ��� ���������̴�.
//		const char*				CSV_NULL_VALUE;
//		const char*				CSV_TOKEN;

		MString					Name;					// ĳ���� �̸�
		CTypeTable<int>			SpriteTypes;			// SpriteTable������ ID
		bool					bMale;					// �����ΰ�?		
		BYTE					MoveTimes;				// �̵� ȸ��
		BYTE					MoveRatio;				// Frame��/�̵� ȸ��... �׸��� 12frame�̶� 6���� �̵��� ���� �ִ�.
		BYTE					MoveTimesMotor;			// ������� ���� �� �̵� ȸ��
		int						Height;					// Ű (ä�� ��� ��ġ�� ���ؼ�)
		int						Width;					// �ʺ� 
		int						DeadHeight;				// �׾������� Ű
		TYPE_ACTIONINFO			DeadActionInfo;			// �������� actionInfo
		int						ColorSet;				// �ٲ�� ���� ��ȣ
		bool					bFlyingCreature;		// ���ƴٴϴ� ĳ���ΰ�?
		int						FlyingHeight;			// ���ƴٴϴ� ����
		bool					bHeadCut;				// �Ӹ� �߸��°�? -_-;
		int						HPBarWidth;				// HP bar ũ��
		ITEM_WEARINFO*			pItemWearInfo;			// �����̾� NPC�� ���
		WORD					ChangeColorSet;			// ĳ���� ��ü�� �ٲ�� ���
		int						ShadowCount;			// �� ����ٴϴ� ���� �ܻ� ����
		int						EffectStatus;			// ���� ���� �ٴ� ����Ʈ
		int						Level;					// ���� ����
		bool					bFade;					// ��Ӱ� ������ ����
		bool					bFadeShadow;			// �׸��ڰ� ��Ӱ� ������ ����
		int					EffectSpriteType;

	protected :
		CREATURETRIBE			m_CreatureTribe;		// ����
		TYPE_SOUNDID*			m_pActionSound;			// Sound ID
		int*					m_pActionCount;			// Action Count
		
	public :
		CREATURETABLE_INFO();
		~CREATURETABLE_INFO();

		//-------------------------------------------------------
		// Set / Get
		//-------------------------------------------------------
		void				SetCreatureTribe(enum CREATURETRIBE ct, int nMaxAction = 0);
		bool				IsSlayer() const						{ return m_CreatureTribe==CREATURETRIBE_SLAYER || m_CreatureTribe==CREATURETRIBE_SLAYER_NPC; }
		// 2026-08-19: Referans restorasyonu — UNKNOW1..4 tribe'ları vampire sayılır (bat/ghost/wolf formları). UNKNOW5 referansta da hariç.
		bool				IsVampire() const						{ return m_CreatureTribe==CREATURETRIBE_VAMPIRE || m_CreatureTribe==CREATURETRIBE_UNKNOW1 || m_CreatureTribe==CREATURETRIBE_UNKNOW2 || m_CreatureTribe==CREATURETRIBE_UNKNOW3 || m_CreatureTribe==CREATURETRIBE_UNKNOW4; }
		bool				IsOusters() const						{ return m_CreatureTribe==CREATURETRIBE_OUSTERS || m_CreatureTribe==CREATURETRIBE_OUSTERS_NPC; }
		bool				IsNPC() const							{ return m_CreatureTribe==CREATURETRIBE_NPC || m_CreatureTribe==CREATURETRIBE_SLAYER_NPC|| m_CreatureTribe==CREATURETRIBE_OUSTERS_NPC; }
		void				SetActionSound(int n, int count, TYPE_SOUNDID sid)	{ m_pActionCount[n] = count;  m_pActionSound[n] = sid; }
		void				SetActionSound(int n, TYPE_SOUNDID sid)	{ m_pActionSound[n] = sid; }
		void				SetActionCount(int n, int count)		{ m_pActionCount[n] = count; }

		enum CREATURETRIBE	GetCreatureTribe() const				{ return m_CreatureTribe; }
		// FAZ 4 (2026-05-07): NULL/OOB guard. Pet creature tip 693 için
		// m_pActionCount allocate edilmemiş → m_pActionCount[n] crash.
		// PetItem rclick crash zincirinin kök nedeni.
		int					GetActionCount(int n)					{ if (m_pActionCount == NULL || n < 0 || n >= GetActionMax()) return 0; return m_pActionCount[n]; }
		TYPE_SOUNDID		GetActionSound(int n)					{ if (m_pActionSound == NULL || n < 0 || n >= GetActionMax()) return 0; return m_pActionSound[n]; }
		int					GetActionMax() const;
		
		bool				IsFlyingCreature() const				{ return bFlyingCreature; }

		//-------------------------------------------------------
		// assign
		//-------------------------------------------------------
		void				operator = (const CREATURETABLE_INFO& creatureInfo);

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void				SaveToFile(std::ofstream& file);
		void				LoadFromFile(ivfstream& file);	

//		void				SaveToCSVFile(std::ofstream& file, unsigned int CreatureID);
//		void				SaveNumberToCSVFile(std::ofstream& file, int value, int null = 65536);
//		void				SaveStringToCSVFile(std::ofstream& file, const MString& value);

		
	protected :
		void				InitActionType(int nMaxAction = 0);				
};

//----------------------------------------------------------------------
// CreatureSpriteTypeMapper
//----------------------------------------------------------------------
// SpriteID�� CreatureType�� ã�� ��쿡 ���
//----------------------------------------------------------------------
class CreatureSpriteTypeMapper {
	public :
		typedef std::vector<WORD>				CREATURE_TYPES;		// WORD.. ������..
		typedef std::vector<CREATURE_TYPES*>	CREATURE_SPRITE_TYPES;

	public :
		CreatureSpriteTypeMapper();
		~CreatureSpriteTypeMapper();

		void	Init(int num);
		void	Release();

		void	AddCreatureType(TYPE_SPRITEID spriteID, WORD creatureType);
		int		GetRandomCreatureType(TYPE_SPRITEID spriteID) const;

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void				SaveToFile(std::ofstream& file);
		void				LoadFromFile(ivfstream& file);	

	protected :
		CREATURE_SPRITE_TYPES	m_CreatureSpriteTypes;
};


typedef CTypeTable<CREATURETABLE_INFO>	CREATURE_TABLE;
extern CREATURE_TABLE* g_pCreatureTable;

extern CreatureSpriteTypeMapper* g_pCreatureSpriteTypeMapper;

// FAZ 4 (2026-05-05): SpriteTypes[0] safe accessor.
// creature.inf eksik veri sebebiyle (*g_pCreatureTable)[type].SpriteTypes
// boş olabilir. Render path'te 8+ site bu pattern'i kullaniyor — OOB
// yaratık-başına crash riski. Default 0 = no-sprite, render skip.
inline int SafeSpriteType0(int creatureType)
{
    if (g_pCreatureTable == NULL) return 0;
    if (creatureType < 0 || creatureType >= (int)g_pCreatureTable->GetSize()) return 0;
    if ((*g_pCreatureTable)[creatureType].SpriteTypes.GetSize() == 0) return 0;
    return (*g_pCreatureTable)[creatureType].SpriteTypes[0];
}

#endif




// Decryption Utility
void DecryptCreatureTable(CREATURE_TABLE* pTable);
