//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemUtil.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ITEMUTIL_H__
#define __ITEMUTIL_H__

#include "Item.h"
#include <list>
#include <string>

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;
class Inventory;
class PlayerCreature;
class Slayer;
// 2007 01 05
class Vampire;
class Ousters;
class Corpse;

struct ITEM_TEMPLATE;

enum ItemTraceLogType
{
	ITEM_LOG_CREATE = 0,		// ������ ������ ���� �α�
	ITEM_LOG_TRADE,				// ������ TRADE �� ���� �α�
	ITEM_LOG_MOVE,				// ������ �̵��� ���� �α�
	ITEM_LOG_DELETE,			// ������ ������ ���� �α�
             
	ITEM_LOG_MAX
};

enum ItemTraceDetailType
{
	DETAIL_COMMAND = 0,		// command ���ɾ�� ����						: ITEM_LOG_CREATE
	DETAIL_SHOPBUY,			// �������� �� ���								: ITEM_LOG_CREATE
	DETAIL_PICKUP,			// �ٴڿ��� ���� �ֿ��� ���					: ITEM_LOG_MOVE
	DETAIL_DROP,			// �ٴڿ� ���� ���								: ITEM_LOG_MOVE
	DETAIL_EVENTNPC,		// �̺�Ʈ�� ���� ������							: ITEM_LOG_CREATE
	DETAIL_SHOPSELL,		// ������ ���� �� ���							: ITEM_LOG_DELETE
	DETAIL_TIMEOUT,			// �ٴڿ� ������ �� timeout ���� ����� ���	: ITEM_LOG_DELETE
	DETAIL_ENCHANT,			// ��þƮ�� ����� ���							: ITEM_LOG_DELETE
	DETAIL_OPCLEAR,			// commnad �� ����								: ITEM_LOG_DELETE
	DETAIL_TRADE,			// ������ TRADE									: ITEM_LOG_TRADE
	DETAIL_MALLBUY,			// ������ �� ������								: ITEM_LOG_CREATE

	DETAIL_MAX				
};

enum ITLType
{
	ITL_GET = 0,			// ������ GET
	ITL_DROP,				// ������ DROP
	ITL_ETC,				// ������ ETC
             
	ITL_MAX	
};

enum ITLDType
{
	ITLD_PICKUP = 0, 		// ������ PICKUP		GET
	ITLD_TRADE,				// ������ TRADE			GET/DROP
	ITLD_EVENTNPC,			// �̺�Ʈ�� clear�ϰ� NPC���� �޴� ������ 		GET
	ITLD_PETITEM,			// GET
	ITLD_ENCHANT,			// GET/DROP
	ITLD_MIXING,			// GET/DROP
	ITLD_OPTION,			// GET/DROP/ETC

	ITLD_NPCSHOP,			// NPC������ �ŷ�	GET/DROP
	ITLD_WEBSHOP,			// �����Ͽ����� �ŷ� GET
	ITLD_PRIVATESHOP,		// ���λ������� GET/DROP

	ITLD_GM,				// ���Ӹ������� ACTION GET/DROP/MOVE/ETC
	ITLD_TIMEOUT,			// Time�ƿ�	DROP
	ITLD_DELETE,			// Delete		// ETC+DELETE �� ���� �����״�.
	ITLD_MOVE,				// Mode 		GET/DROP/ETC

	ITLD_MAX
};

const string ItemTraceLogType2String[] =
{
	"CREATE",       // 0
	"TRADE",        // 1
	"MOVE",         // 2
	"DELETE",       // 3
};

const string ItemTraceLogDetailType2String[] =
{
	"COMMAND",      // 0
	"SHOPBUY",      // 1
	"PICKUP",       // 2
	"DROP",         // 3
	"EVENTNPC",     // 4
	"SHOPSELL",     // 5
	"TIMEOUT",      // 6
	"ENCHANT",      // 7
	"OPCLEAR",      // 8
	"TRADE",        // 9
	"MALLBUY",      // 10
};

const string ITLType2String[] =
{
	"GET",       // 0
	"DROP",        // 1
	"MOVE",         // 2
	"ETC",       // 3
};

const string ITLDType2String[] =
{
	"PICKUP",     // 0
	"TRADE",      // 1
	"EVENTNPC",   // 2
	"PETITEM",    // 3
	"ENCHANT",    // 4
	"MIXING",     // 5
	"OPTION",     // 6

	"NPCSHOP",    // 7
	"WEBSHOP",    // 8
	"PRIVATESHOP",// 9

	"GM",  		  // 10
	"TIMEOUT",	  // 11
	"DELETE",	  // 12
	"MOVE"        // 13
};

//////////////////////////////////////////////////////////////////////////////
// ���� �� �ִ� �������ΰ�? 
//////////////////////////////////////////////////////////////////////////////
bool isStackable(Item::ItemClass IClass);
bool isStackable(const Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// ���� Ŭ����, Ÿ���� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSameItem(Item::ItemClass IClass1, Item::ItemClass IClass2, ItemType_t type1, ItemType_t type2);
bool isSameItem(const Item* pItem1, const Item* pItem2);

//////////////////////////////////////////////////////////////////////////////
// �� �������� ���� �� �ִ°�?
//////////////////////////////////////////////////////////////////////////////
bool canStack(const Item* pItem1, const Item* pItem2);

//////////////////////////////////////////////////////////////////////////////
// �� �� �����ΰ�?
// ������ �����ΰ�?
// ����,����, ������ �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isTwohandWeapon(const Item* pItem);
bool isMeleeWeapon(const Item* pItem);
bool isFighterWeapon(const Item* pItem);
bool isArmsWeapon(const Item* pItem);
bool isClericWeapon(const Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// �ѿ� �´� źâ�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSuitableMagazine(const Item* pGun, const Item* pMagazine, bool hasVivid);

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �����ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerWeapon(Item::ItemClass IClass);
bool isAdvancedSlayerWeapon(Item::ItemClass IClass);
bool isVampireWeapon(Item::ItemClass IClass);
bool isOustersWeapon(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////
// �����̾�� ���ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerArmor(Item::ItemClass IClass);
bool isVampireArmor(Item::ItemClass IClass);
bool isOustersArmor(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////
// �����̾�� �Ǽ��縮�ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isSlayerAccessory(Item::ItemClass IClass);
bool isVampireAccessory(Item::ItemClass IClass);
bool isOustersAccessory(Item::ItemClass IClass);

//////////////////////////////////////////////////////////////////////////////
// ������ �� �ִ� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isRepairableItem(const Item* pItem);
OptionType_t IsHiddenOptionType(Item::ItemClass IClass);
//////////////////////////////////////////////////////////////////////////////
// �������� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void repairItem(Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// �������� �ִ� �������� ����.
//////////////////////////////////////////////////////////////////////////////
Durability_t computeMaxDurability(Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// źâ�� ���Ƴ����ش�.
//////////////////////////////////////////////////////////////////////////////
Bullet_t reloadArmsItem(Item* pWeapon, Item* pMagazine);

//////////////////////////////////////////////////////////////////////////////
// �Ѿ��� ���ش�.
//////////////////////////////////////////////////////////////////////////////
Bullet_t decreaseBullet(Item* pWeapon) throw();

//////////////////////////////////////////////////////////////////////////////
// ���� �Ѿ��� ������ ����
//////////////////////////////////////////////////////////////////////////////
Bullet_t getRemainBullet(Item* pWeapon) throw();

//////////////////////////////////////////////////////////////////////////////
// ���� �� �ִ� �������ΰ�?
//////////////////////////////////////////////////////////////////////////////
bool isPortableItem(Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// ����� �� �ִ� �������ΰ�?
// ���⼭�� ����̶�, ����ؼ� ������� �Ҹ� �������� ���Ѵ�.
//////////////////////////////////////////////////////////////////////////////
bool isUsableItem(Item* pItem, Creature* pUser);

//////////////////////////////////////////////////////////////////////////////
// �������� ���ڸ� ���δ�.
//////////////////////////////////////////////////////////////////////////////
ItemNum_t decreaseItemNum(Item* pItem, Inventory* pInventory, 
	const string& OwnerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y);

//////////////////////////////////////////////////////////////////////////////
// �������� ��ġ�� ���׸� �α��ϴ� �Լ���.
//////////////////////////////////////////////////////////////////////////////
void processItemBug(Creature* pCreature, Item* pItem);
void processItemBugEx(Creature* pCreature, Item* pItem);

//////////////////////////////////////////////////////////////////////////////
// Mysterious item ����
//////////////////////////////////////////////////////////////////////////////
Item* getRandomMysteriousItem(Creature* pCreature, Item::ItemClass itemClass, int maxLevel=0)
	    throw(Error);

//////////////////////////////////////////////////////////////////////////////
// Option�� ���õ� ��������
//////////////////////////////////////////////////////////////////////////////
// Ư���� OptionType�� �پ��ִ°�?
bool hasOptionType(const list<OptionType_t>& optionTypes, OptionType_t optionType);

// Ư���� OptionClass(STR, DEX...)�� �پ��ִ°�?
bool hasOptionClass(const list<OptionType_t>& optionTypes, OptionType_t optionType);

// DB���� �о�ö�
void setOptionTypeFromField(list<OptionType_t>& optionTypes, const string& optionField);

// DB�� ������ ��
void setOptionTypeToField(const list<OptionType_t>& optionTypes, string& optionField);

// cout�� ���
string getOptionTypeToString(const list<OptionType_t>& optionTypes);

string turnToString(const list<OptionType_t>& optionTypes);
//////////////////////////////////////////////////////////////////////////////
// ���� �ɼ��� ���� Ȯ�� üũ : ���� ������ ���� ����
//////////////////////////////////////////////////////////////////////////////
bool isPossibleNextOption(ITEM_TEMPLATE* pTemplate);

ItemType_t getUpgradeItemType(Item::ItemClass IClass, ItemType_t itemType, ItemType_t upgradeCount);
ItemType_t getDowngradeItemType(Item::ItemClass IClass, ItemType_t itemType);
bool isPossibleUpgradeItemType(Item::ItemClass IClass);
bool isPossibleHiddenOption(Item::ItemClass IClass);
//////////////////////////////////////////////////////////////////////////////
// ũ�������� Ʈ�� �̺�Ʈ��
//////////////////////////////////////////////////////////////////////////////
// Ʈ�� ������ �˻�
//TPOINT checkEventTree( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );
//TPOINT checkEventDocument( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );
//TPOINT checkEventDoll( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY );

//1219 wlzzi -- �Լ� ����.. ������ ũ�����.. ���ڷ�.. default����.. ������ ������� ũ���.... 
//TPOINT checkEventPuzzle( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY, int start);
TPOINT checkEventPuzzle( PlayerCreature* pPC, CoordInven_t iX, CoordInven_t iY, int start, int width = 3, int height = 4);

// �κ��丮�� (X0, Y0) - (X1, y1) ������ �������� �����.
void deleteInventoryItem( Inventory* pInventory, CoordInven_t invenX0, CoordInven_t invenY0, CoordInven_t invenX1, CoordInven_t invenY1 );

// �κ��丮�� �ʺ��ڿ� �������� �־��ش�.
bool addNewbieItemToInventory( Slayer* pSlayer, bool sendPacket = false ) throw(Error);
bool addNewbieGoldToInventory( Slayer* pSlayer, bool sendPacket = false ) throw(Error);
bool addNewbieItemToGear( Slayer* pSlayer, bool sendPacket = false ) throw(Error);

bool addNewbieItemToInventory( Ousters* pOusters, bool sendPacket = false ) throw(Error);
bool addNewbieGoldToInventory( Ousters* pOusters, bool sendPacket = false ) throw(Error);
bool addNewbieItemToGear( Ousters* pOusters, bool sendPacket = false ) throw(Error);

// Sifir Domain Level baslangic paketi (sinifa uygun ilk silah + tuketilebilirler, ItemType=0).
bool addNewbieClassStarterItems( Slayer*  pSlayer,  bool sendPacket = false ) throw(Error);
bool addNewbieClassStarterItems( Vampire* pVampire, bool sendPacket = false ) throw(Error);
bool addNewbieClassStarterItems( Ousters* pOusters, bool sendPacket = false ) throw(Error);
bool addNewbieGoldToInventory ( Vampire* pVampire, bool sendPacket = false ) throw(Error);

Item::ItemClass getBestNewbieWeaponClass(Slayer* pSlayer) throw (Error);

// 2007 01 04
bool addNewbieEventItemToInventory( Slayer* pSlayer, bool sendPacket = false ) throw(Error);
bool addNewbieEventItemToInventory( Vampire* pVampire, bool sendPacket = false ) throw(Error);
bool addNewbieEventItemToInventory( Ousters* pOusters, bool sendPacket = false ) throw(Error);

bool addNewbieItemToGear( Vampire* pVampire, bool sendPacket = false ) throw(Error);

// �ɼ� string���κ��� �ɼ� list�� �����.
void makeOptionList(const string& options, list<OptionType_t>& optionList) throw (Error);

void saveDissectionItem(Creature* pCreature, Item* pTreasure, int x, int y) throw (Error);

bool canDecreaseDurability( Item* pItem ) throw(Error);

bool canSellStore( Item* pItem );
bool canSell( Item* pItem );
bool canPutInStash( Item* pItem );
bool canTrade( Item* pItem );
bool canTradeInventoryItem( Item* pItem );
bool isCoupleRing( Item* pItem );

bool suitableItemClass( Item::ItemClass iClass, SkillDomainType_t domainType );

// �������� ������ �´� ������ ���������� �ٲ��ش�. ������ Ÿ���� ���ڿ� ������ �ٷ� ���� ���� ���ڿ��� �´ٰ� ����
// �̰� �ҷ��� �ڿ� �ݵ�� ������ Ÿ���� ���̺����ִ��� pItem->save() �� �ҷ���� �ȴ�.
void setItemGender( Item* pItem, GenderRestriction gender );

// Item Trace Log �� ���� �������ΰ�?
bool bTraceLog( Item* pItem );

// Item Trace Log ����� �Լ� 
void remainTraceLog ( Item* pItem, const string& preOwner, const string& owner, ItemTraceLogType logType, ItemTraceDetailType detailType ) throw (Error);
void remainTraceLogNew ( Item* pItem, const string& owner, ITLType logType, ITLDType detailType, ZoneID_t zid=0, int x=0, int y=0 ) throw (Error);

// Meney Trace Log ����� �Լ�
void remainMoneyTraceLog ( const string& preOwner, const string& owner, ItemTraceLogType logType, ItemTraceDetailType detailType, int amount ) throw (Error);

// Web ���� �� �������� ����� �Լ�
Item* createItemByGoodsID( DWORD goodsID );

// ���Ǵ�÷�� ���� �� �Լ� (Ư���� �� ���� ��� ���� �д� --;) 2003.04.29 by DEW
bool bWinPrize( DWORD rewardID, DWORD questLevel );

void deleteFlagEffect( Corpse* pFlagPole, Item* pFlag );

Item* fitToPC( Item* pItem, PlayerCreature* pPC );

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

ItemType_t getItemTypeByItemLimit( Item::ItemClass itemClass, ItemType_t itemType );

#endif	// __THAILAND_SERVER__

void increaseEventItemCount( Item* pItem, int num = 1 );
Item *GiveItemToInventory(string strLog, 
		PlayerCreature *pPlayerCreature, 
		Item::ItemClass ItemClass, 
		int iItemType, 
		int iItemCount, 
		list<OptionType_t> Options, 
		DWORD LimitedTime);

#endif
