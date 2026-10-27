//////////////////////////////////////////////////////////////////////////////
// Filename    : PlayerCreature.h
// Written by  : excel96
// Description : 
// Slayer ?? Vampire ??????? ????????? ?? ?????? ?¥ê??? ?????? 
// ???????. abstract ?????????, ??? ??????? ?? ?? ?????.
//////////////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_CREATURE_H__
#define __PLAYER_CREATURE_H__

#include "Creature.h"
#include "ObjectRegistry.h"
#include "InventorySlot.h"
#include "Garbage.h"
#include "RankBonus.h"
#include "ExpFwd.h"
//#include "RankExpTable.h"
//#include "ItemNameInfo.h"
//#include "quest/Squest/QuestManager.h"
#include "Gpackets/GCMonsterKillQuestInfo.h"
#include "OptionInfo.h"
#include "VSDateTime.h"
#include "GearInfo.h"

#include "NewRankGem.h"

#include <map>
#include <bitset>
#include <list>
#include <vector>

static const GuildID_t SlayerCommon	=99;
static const GuildID_t VampireCommon=0;
static const GuildID_t OustersCommon=66;


//////////////////////////////////////////////////////////////////////////////
// class PlayerCreature
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class InventorySlot;
class Stash;
class Player;
class FlagSet;
class QuestManager;
class TimeLimitItemManager;
class GoodsInventory;
class PetInfo;
class PetItem;
class Pet;
class SMSAddressBook;
class NicknameBook;
class NicknameInfo;
class GQuestManager;
class BloodBibleSignInfo;
class Store;
//class GCMonsterKillQuestInfo;
//struct GCMonsterKillQuestInfo::QuestInfo;
class WeekItemListManager;
class NPC;
class SkillOutput;
class Monster;

typedef map<DWORD, RankBonus*> HashMapRankBonus;
typedef HashMapRankBonus::iterator HashMapRankBonusItor;
typedef HashMapRankBonus::const_iterator HashMapRankBonusConstItor;

typedef map<DWORD, NewRankGem*> HashMapRankGem;
typedef HashMapRankGem::iterator HashMapRankGemItor;
typedef HashMapRankGem::const_iterator HashMapRankGemConstItor;

typedef vector<Item *> WearItemList;
typedef map<string, void *> TParameterList;

class PresentItem
{
public :
	PresentItem()
	{
		m_strEventTitle = "";
		m_iGoodsID = 0;
		m_iGoodsCount = 0;
		m_iPeriod = 0;
	}
	
	string m_strEventTitle;
	int m_iGoodsID;
	int m_iGoodsCount;
	int m_iPeriod;
};

class NewStartItem
{
public :
	NewStartItem()
	{
		m_iGoodsID = 0;
		m_iGoodsCount = 0;
	}
	
	int m_iGoodsID;
	int m_iGoodsCount;
};

class PlayerCreature : public Creature
{
////////////////////////////////////////////////////////////
// ?????? ?? ?????
////////////////////////////////////////////////////////////
public:
	PlayerCreature(ObjectID_t objectID = 0, Player* pPlayer = NULL) throw();
	virtual ~PlayerCreature() throw();

	virtual void initWearItemSlot(int iMaxWearItemSlot, int iMaxGearSlot);

	virtual bool load() throw (InvalidProtocolException, Error);
	virtual void tinysave(const string & field) const throw (Error) = 0;
//	virtual void tinysave(const char* field) const throw (Error) = 0;

////////////////////////////////////////////////////////////
// OID ??? ???? ????
////////////////////////////////////////////////////////////
protected:
	virtual void registerItem(Item* pItem, ObjectRegistry& OR) throw();

public:
	virtual void registerInventory(ObjectRegistry& OR) throw();
	virtual void registerInitInventory(ObjectRegistry& OR) throw();
	virtual void registerStash(void) throw();

	virtual void registerGoodsInventory(ObjectRegistry& OR) throw();

//////////////////////////////////////////////////////////////
// ?©£????? ?????? ???? ???
//////////////////////////////////////////////////////////////
public:
	bool wasteIfTimeLimitExpired(Item* pItem) throw (Error);
	virtual void checkItemTimeLimit() throw (Error) = 0;
	void sendTimeLimitItemInfo() throw(Error);
	void addTimeLimitItem(Item* pItem, DWORD time) throw(Error);
	void sellItem( Item* pItem ) throw(Error);
	void deleteItemByMorph( Item* pItem ) throw(Error);
	void updateItemTimeLimit( Item* pItem, DWORD time ) throw(Error);
	virtual void updateEventItemTime( DWORD time ) throw(Error) = 0;
	void loadTimeLimitItem() throw(Error);
	DWORD 		getLimitTime(Item* pItem) throw(Error); // 20080913

//////////////////////////////////////////////////////////////
// ???? ??? ?????? ???? ???
//////////////////////////////////////////////////////////////
public:
	void loadGoods() throw(Error);

//////////////////////////////////////////////////////////////
// ????? ????? ???? ???
//////////////////////////////////////////////////////////////
public:
	QuestManager*	getQuestManager() const { return m_pQuestManager; }
	void			sendCurrentQuestInfo() const throw(Error);

	virtual	int		getQuestLevel() const = 0;
	virtual void	whenQuestLevelUpgrade();

////////////////////////////////////////////////////////////
// ?¥ê??? ???? ????
////////////////////////////////////////////////////////////
public:
	virtual Inventory* getInventory() const throw() { return m_pInventory; }
	virtual void setInventory(Inventory* pInventory) throw() { m_pInventory = pInventory; }

	virtual InventorySlot* getExtraInventorySlot() throw() { return m_pExtraInventorySlot; }
	virtual Item* getExtraInventorySlotItem() throw(Error) { return m_pExtraInventorySlot->getItem(); }
	virtual void deleteItemFromExtraInventorySlot() throw(Error) { m_pExtraInventorySlot->deleteItem(); }
	virtual void addItemToExtraInventorySlot(Item* pItem) throw(Error) { m_pExtraInventorySlot->addItem(pItem); }

	// Extra slot'taki tuketilmemis mouse item'i envantere geri yazar.
	// bSendCreatePacket=true: canli oturumda GCCreateItem gonderir (protocol ERROR restore).
	bool restoreExtraInventorySlotToInventory(bool bSendCreatePacket) throw();

	// 2003.04.04. by Sequoia
	virtual void loadItem() throw (InvalidProtocolException, Error);

	virtual GoodsInventory* getGoodsInventory() const throw() { return m_pGoodsInventory; }

	int getItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType ) throw();
	int decreaseItemClassTypeNum( Item::ItemClass itemClass, ItemType_t itemType, int num ) throw();

//////////////////////////////////////////////////////////////
// Phoenix counter
//////////////////////////////////////////////////////////////
public:
	void loadPhoenixMonsterCounter() throw(Error);
	void sendPhoenixMonsterCounter() throw(Error);
	void increasePhoenixMonsterCounter(Monster* pMonster, int playerLevel, int monsterLevel) throw(Error);

protected:
	bool givePhoenixMonsterCounterReward() throw(Error);
	bool resetPhoenixMonsterCounterCycle() throw(Error);


////////////////////////////////////////////////////////////
// ????? ????
////////////////////////////////////////////////////////////
public :
	virtual GearInfo *getGearInfo() const throw();

	void  addWearItem(int Part, Item* pItem) throw();
	void  deleteWearItem(int Part);
	void  deleteWearItem(GearSlotID_t GearSlotID, int Part);
	Item* getWearItem(GearSlotID_t GearSlotID, int Part);
	Item* getWearItem(int Part);
	
	virtual int getWearPartOfLeftHand() = 0;
	virtual int getWearPartOfRightHand() = 0;
	
	virtual bool addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color) = 0;
	virtual bool removeShape(Item::ItemClass IClass, bool bSendPacket = true) = 0;
	virtual Color_t getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo=NULL) const = 0;
	
	virtual bool  isRealWearing(Item* pItem) throw() = 0;
	virtual DWORD sendRealWearingInfo(void) throw() = 0;
		
////////////////////////////////////////////////////////////
// ?????? ???? ????
////////////////////////////////////////////////////////////
public:
	virtual Stash* getStash(void) const throw() { return m_pStash; }
	virtual void setStash(Stash* pStash) throw() { m_pStash = pStash; }

	virtual BYTE getStashNum(void) const throw() { return m_StashNum; }
	virtual void setStashNum(BYTE num) throw() { m_StashNum = num; }
	virtual void setStashNumEx(BYTE num) throw();

	virtual Gold_t getStashGold(void) const throw() { return m_StashGold; }
	virtual void setStashGold(Gold_t gold) throw() { m_StashGold = gold; }
	virtual void setStashGoldEx(Gold_t gold) throw();
	virtual void increaseStashGoldEx(Gold_t gold) throw();
	virtual void decreaseStashGoldEx(Gold_t gold) throw();
	virtual bool checkStashGoldIntegrity() = 0;

	virtual bool getStashStatus(void) const throw() { return m_bStashStatus; }
	virtual void setStashStatus(bool s) throw() { m_bStashStatus = s; }

	virtual void deleteStash(void) throw();


////////////////////////////////////////////////////////////
// ?????? ???? ????
////////////////////////////////////////////////////////////
public:
	void addItemToGarbage(Item* pItem) { m_Garbage.addItem(pItem); }
	void addItemToGarbageEx(Item* pItem) { m_Garbage.addItemEx(pItem, getName()); }
	Item* popItemFromGarbage(void) { return m_Garbage.popItem(); }
	void saveGarbage(void) { m_Garbage.save(getName()); }
	int getGarbageSize(void) { return m_Garbage.size(); }

////////////////////////////////////////////////////////////
// ?????? ??? ???
////////////////////////////////////////////////////////////
public:
	virtual Item* findItemOID(ObjectID_t id) throw() = 0;
	virtual Item* findItemOID(ObjectID_t id, int& storage, int& x, int& y) throw() = 0;

	virtual Item* findItemIID(ItemID_t id) throw() = 0;
	virtual Item* findItemIID(ItemID_t id, int& storage, int& x, int& y) throw() = 0;

	virtual Item* findBeltOID(ObjectID_t id) throw() = 0;
	virtual Item* findBeltOID(ObjectID_t id, int& storage, int& x, int& y) throw() = 0;

	virtual Item* findBeltIID(ItemID_t id) throw() = 0;
	virtual Item* findBeltIID(ItemID_t id, int& storage, int& x, int& y) throw() = 0;


////////////////////////////////////////////////////////////
// ?¡À??? ?? ???? ???
////////////////////////////////////////////////////////////
public:
	FlagSet* getFlagSet(void) const throw() { return m_pFlagSet; }
	void setFlagSet(FlagSet* pSet) throw() { m_pFlagSet = pSet; } 
	void deleteFlagSet(void) throw();

//////////////////////////////////////////////////////////////
// Blood Burst Point
//////////////////////////////////////////////////////////////
public:
	virtual Attr_t getAttackBloodBurstPoint() const = 0;
	virtual void setAttackBloodBurstPoint( Attr_t point ) = 0;
	virtual void increaseAttackBloodBurstPoint( Attr_t point ) = 0;
	virtual void resetAttackBloodBurstPoint() = 0;
	virtual Attr_t getDefenseBloodBurstPoint() const = 0;
	virtual void setDefenseBloodBurstPoint( Attr_t point ) = 0;
	virtual void increaseDefenseBloodBurstPoint( Attr_t point ) = 0;
	virtual void resetDefenseBloodBurstPoint() = 0;
	
	virtual void sendSkillInfo() throw() = 0;

////////////////////////////////////////////////////////////
// ??? ???
////////////////////////////////////////////////////////////
public:
	virtual void setXY(ZoneCoord_t x, ZoneCoord_t y) throw();

    virtual const string& getName() const throw() { return m_Name; }
	virtual void setName(const string & name) throw() { m_Name = name; }

	virtual Fame_t getFame() const throw() = 0;
	virtual void setFame(Fame_t fame) throw() = 0;

	virtual Gold_t getGold() const throw() = 0;
	virtual void setGold(Gold_t gold) throw() = 0;
	virtual void setGoldEx(Gold_t gold) throw() = 0;
	virtual void   increaseGoldEx(Gold_t gold) throw() = 0;
    virtual void   decreaseGoldEx(Gold_t gold) throw() = 0;
	virtual bool checkGoldIntegrity() = 0;

	virtual Gold_t getDianJuan() const throw() = 0;
	virtual void setDianJuan(Gold_t DianJuan) throw() = 0;
	virtual void   increaseDianJuanEx(Gold_t DianJuan) throw() = 0;
  	virtual void   decreaseDianJuanEx(Gold_t DianJuan) throw() = 0;

	virtual Gold_t getJiFen() const throw() = 0;
	virtual void setJiFen(Gold_t JiFen) throw() = 0;
	virtual void   increaseJiFenEx(Gold_t JiFen) throw() = 0;
  	virtual void   decreaseJiFenEx(Gold_t JiFen) throw() = 0;

	bool checkDBGold( Gold_t gold )
	{
		Gold_t temp = getGold();
		setGold(gold);
		bool ret = checkGoldIntegrity();
		setGold(temp);
		return ret;
	}


	virtual Sex	getSex() const throw() = 0;

	virtual ZoneID_t getResurrectZoneID(void) const throw() = 0;
	virtual void setResurrectZoneID(ZoneID_t id) throw() = 0;
	virtual void setResurrectZoneIDEx(ZoneID_t id) throw() = 0;

	//virtual Race_t getRace() const = 0; - Creature?? ?¡À??.
	virtual GuildID_t getCommonGuildID() const = 0;

	IP_t getIP(void) const;
	virtual Level_t getLevel() const throw() = 0;

////////////////////////////////////////////////////////////
// ???? ????? ????
////////////////////////////////////////////////////////////
	// enemy specific methods
	void addEnemy(const string& Name) throw(Error);
	void deleteEnemy(const string& Name) throw(NoSuchElementException, Error);

	// ?? ??? ?????? ??? ?????? ????¡Æ??
	bool hasEnemy(const string& Name) const throw();
	uint getMaxEnemies() const throw();

	list<string>& getEnemies(void) throw() { return m_Enemies;}

	bool isPK() throw() { return m_isPK; }

	void setPK(bool isPK) throw() { m_isPK = isPK; }

	void setGuildID( GuildID_t GuildID ) throw() { m_GuildID = GuildID; }
	GuildID_t getGuildID() const throw() { return m_GuildID; }

	string getGuildName() const throw();
	GuildMemberRank_t getGuildMemberRank() const throw();

	Rank_t  	getRank() const throw();
	RankExp_t   getRankExp() const throw();
	RankExp_t   getRankGoalExp() const throw();

//	virtual Rank_t getRank() const throw() = 0;
	void increaseRankExp(RankExp_t Point, bool isItemBouns = false);

//	void increaseItemRankExp(RankExp_t Point);//by rallser

	WORD getRankExpSaveCount(void) const { return m_RankExpSaveCount; }
	void setRankExpSaveCount(WORD count) { m_RankExpSaveCount = count; }

	virtual Alignment_t getAlignment() const throw()=0;
	virtual void setAlignment(Alignment_t Alignment) throw()=0;

////////////////////////////////////////////////////////////
// Rank Bonus ????
////////////////////////////////////////////////////////////
	void loadRankBonus() throw();
	bool hasRankBonus( RankBonus::RankBonusType type ) throw() { return m_RankBonusFlag.test(type); }
	RankBonus* getRankBonus( RankBonus::RankBonusType type ) const throw();
	RankBonus* getRankBonusByRank( Rank_t rank ) const throw();
	bool learnRankBonus( DWORD type ) throw();
	void clearRankBonus() throw();
	void clearRankBonus( Rank_t rank ) throw();
	HashMapRankBonus& getRankBonuses() { return m_RankBonuses; }

	void sendRankBonusInfo() throw();
	void loadDianJuan() throw();
	void loadJiFen() throw();
	bool ensureMarketCurrencyRow(const char* context) throw();
	bool increaseMarketCashEx(Gold_t amount) throw();
	bool decreaseMarketCashEx(Gold_t amount) throw();
	bool increaseMarketPointEx(Gold_t amount) throw();
	bool decreaseMarketPointEx(Gold_t amount) throw();


	void loadRankGem() throw();//by rallser
	bool hasRankGem( NewRankGem::RankGemType type ) throw() { return m_RankGemFlag.test(type); }
	//void removeRankGem( NewRankGem::RankGemType type ) throw() { m_RankGemFlag.reset(type); }
	void clearRankGem( NewRankGem::RankGemType type ) throw();
	NewRankGem* getRankGem( NewRankGem::RankGemType type ) const throw();
	HashMapRankGem& getRankGem() { return m_RankGem; }

protected:
	void addRankBonus( RankBonus* rankBonus ) throw();

	void addRankGem( NewRankGem* RankGem ) throw();//by rallser
	bool updateMarketCurrency(const char* fieldName, const char* currencyName, Gold_t amount, bool increase, Gold_t& newValue, const char* context) throw();

public :
	// by sigi. 2002.11.19
	bool isBillingPlayAvaiable() throw(Error);
	virtual bool isPayPlayAvaiable() throw(Error);
	virtual bool canPlayFree() throw(Error);

public:
	Item*	getQuestItem() const { return m_pQuestItem; }
	void	setQuestItem( Item* pItem ) { m_pQuestItem = pItem; }

public :
	// by sigi. 2002.12.3
/*	void	loadQuest() throw (Error);
	bool 	hasQuest() const 	{ return m_pQuestManager!=NULL && !m_pQuestManager->isEmpty(); }
	bool 	addQuest(Quest* pQuest) throw (Error);
	bool 	checkEvent(QuestEvent* pQuest) throw (Error);
	Quest* 	removeCompleteQuest() throw (Error);
	void 	removeAllQuest() throw (Error)	{ if (m_pQuestManager!=NULL) m_pQuestManager->release(); }*/

	virtual void initAllStatAndSend() = 0;
	virtual void initAllStat(int numPartyMember = -1) throw() = 0;

	virtual void computeStatOffset(void) throw() = 0;
	virtual void computeItemStat(Item* pItem) throw() = 0;
	virtual void computeOptionStat(Item* pItem) throw() = 0;
	virtual void computeOptionStat(OptionType_t optionType) throw() = 0;
	virtual void computeOptionClassStat( OptionClass OClass, int PlusPoint ) = 0;
	
	void heartbeat(const Timeval& currentTime) throw();

	virtual bool canSee( Object* pObject ) const;

////////////////////////////////////////////////////////////
// ??? ??????
////////////////////////////////////////////////////////////
protected:

	// PC name
	string m_Name;

	Inventory*     m_pInventory;          // ?¥ê??? ??????
	InventorySlot* m_pExtraInventorySlot; // ???²J ??????

	GoodsInventory* m_pGoodsInventory;    // ???? ?????? ?¥ê???

	Stash*         m_pStash;              // ?????? ??????
	BYTE           m_StashNum;            // ???????? ????
	Gold_t         m_StashGold;           // ?????? ???? ?? ???
	bool           m_bStashStatus;        // ?????? ?????? OID ??? ????

	Garbage        m_Garbage;             // ??????...

	FlagSet*       m_pFlagSet;            // ?¡À??? ????

	// ???? ?????? ??????? ????? ???????.
	// ObjectID?? ?????? ???? ??????, ??? ??? ???? ????????? ???????? ??? ?????? ??????? ?????????.
    list<string> m_Enemies;

	// PK?? ??????? ???©§? ???????.
	bool m_isPK;

	// GuildID
	GuildID_t		m_GuildID;

	// Rank Bonus map
	HashMapRankBonus m_RankBonuses;
	bitset<RankBonus::RANK_BONUS_MAX> m_RankBonusFlag;

	HashMapRankGem m_RankGem;
	bitset<NewRankGem::RANK_GEM_MAX> m_RankGemFlag;

	Rank*	m_pRank;
	WORD	m_RankExpSaveCount;

	bool	m_PhoenixMonsterCounterEnabled;
	bool	m_PhoenixMonsterCounterRewarded;
	DWORD	m_PhoenixMonsterCounter;
	DWORD	m_PhoenixMonsterCounterGoal;
	DWORD	m_PhoenixMonsterCounterRewardGoodsID;
	int		 m_PhoenixMonsterCounterRewardGoodsCount;
	int		 m_PhoenixMonsterCounterMaxLevelGap;

//	QuestManager*	m_pQuestManager;
	QuestManager*			m_pQuestManager;
	TimeLimitItemManager*	m_pTimeLimitItemManager;

	Item*					m_pQuestItem;
	vector<Item*>			m_PetStash;		// pet ??????

public:
	/////////////////////////////////////////////////////////
	// pet ??????? ????? method
	/////////////////////////////////////////////////////////
	Item*				getPetStashItem(int idx);	
	void				addPetStashItem(int idx, Item* pPetItem);

	/////////////////////////////////////////////////////////
	// BloodBible ???? ????? ?????
	/////////////////////////////////////////////////////////
public:
	int					getConsumeMPRatio() const { return m_ConsumeMPRatio; }
	void				setConsumeMPRatio( int ratio ) { m_ConsumeMPRatio = ratio; }

	int					getGamblePriceRatio() const { return m_GamblePriceRatio; }
	void				setGamblePriceRatio( int ratio ) { m_GamblePriceRatio = ratio; }

	int					getPotionPriceRatio() const { return m_PotionPriceRatio; }
	void				setPotionPriceRatio( int ratio ) { m_PotionPriceRatio = ratio; }

	Damage_t			getMagicBonusDamage() const { return m_MagicBonusDamage; }
	void				setMagicBonusDamage( Damage_t damage ) { m_MagicBonusDamage = damage; }

	Damage_t			getPhysicBonusDamage() const { return m_PhysicBonusDamage; }
	void				setPhysicBonusDamage( Damage_t damage ) { m_PhysicBonusDamage = damage; }

	Damage_t			getMagicDamageReduce() const { return m_MagicDamageReduce; }
	void				setMagicDamageReduce( Damage_t damage ) { m_MagicDamageReduce = damage; }

	Damage_t			getPhysicDamageReduce() const { return m_PhysicDamageReduce; }
	void				setPhysicDamageReduce( Damage_t damage ) { m_PhysicDamageReduce = damage; }

/*	bool				isEmptyItemNameInfoList() { return m_ItemNameInfoList.empty(); }
	void				clearItemNameInfoList() { m_ItemNameInfoList.clear(); }
	const list<ItemNameInfo*>& getItemNameInfoList() const { return m_ItemNameInfoList; }
	void				addItemNameInfoList( ItemNameInfo* itemNameInfo ) { m_ItemNameInfoList.push_back(itemNameInfo); }
	bool				deleteItemNameInfoList( ObjectID_t objectID ) throw(Error);
	string				getItemName( ObjectID_t objectID ) throw(Error);*/

protected:
	int						m_ConsumeMPRatio;
	int						m_GamblePriceRatio;
	int						m_PotionPriceRatio;
	Damage_t				m_MagicBonusDamage;
	Damage_t				m_PhysicBonusDamage;

	Damage_t				m_MagicDamageReduce;
	Damage_t				m_PhysicDamageReduce;

//	list<ItemNameInfo*> 	m_ItemNameInfoList;

// ??????? ??¡À ???¥è? ?????? ???? ??¢¯ ?? ???? ???? ?????? ???

public:
	DWORD				getLottoRewardID() const { return m_LottoRewardID; }
	void				setLottoRewardID( DWORD lottoRewardID ) { m_LottoRewardID = lottoRewardID; }

	DWORD				getLottoQuestLevel() const { return m_LottoQuestLevel; }
	void 				setLottoQuestLevel( DWORD lottoQuestLevel ) { m_LottoQuestLevel = lottoQuestLevel; }

	bool				isLotto() const { return m_bLotto; }
	void				setLotto( bool lotto ) { m_bLotto = lotto; }

protected:
	DWORD				m_LottoRewardID;
	DWORD				m_LottoQuestLevel;
	bool				m_bLotto;

	//////////////////////////////////////////////
	// Default Option Set Info
	//////////////////////////////////////////////
public:
	void	addDefaultOptionSet( DefaultOptionSetType_t type ) throw();
	void	removeDefaultOptionSet( DefaultOptionSetType_t type ) throw();

	void	addDefaultOptionType( OptionType_t type );
	void	removeDefaultOptionType( OptionType_t type );
	void	clearDefaultOptionTypes();

protected:
	list<DefaultOptionSetType_t>		m_DefaultOptionSet;
	list<OptionType_t>					m_DefaultOptions;

	//////////////////////////////////////////////
	// Effect Option
	//////////////////////////////////////////////
public:
	void addEffectOption( ObjectID_t oid, OptionType_t type );
	void removeEffectOption( ObjectID_t oid );
	void clearEffectOption();

protected:
	typedef map<ObjectID_t, OptionType_t>		HashMapObjectOption;
	typedef HashMapObjectOption::iterator			HashMapObjectOptionItor;
	typedef HashMapObjectOption::const_iterator		HashMapObjectOptionConstItor;

	HashMapObjectOption m_EffectOptions;

public:
	PetInfo*	getPetInfo() const;
	void		setPetInfo(PetInfo* pPetInfo);

	list<PetItem*>& getPetItems() { return m_PetItems; }
	const list<PetItem*>& getPetItems() const { return m_PetItems; }

	Pet*		getPet() const { return m_pPet; }

protected:
	PetInfo* m_pPetInfo;
	list<PetItem*> m_PetItems;
	Pet*	m_pPet;

	// 2?? ?? ????? ???? 
public:
	SpriteType_t	getTargetMonsterSType() const { return m_TargetMonster; }
	virtual void	initPetQuestTarget() = 0;
	GCMonsterKillQuestInfo::QuestInfo*	getPetQuestInfo() const;

protected:
	SpriteType_t	m_TargetMonster;
	DWORD			m_TargetNum;
	DWORD			m_TimeLimit;

	// SMS ????
public:
	SMSAddressBook*	getAddressBook() const { return m_pSMSAddressBook; }
	uint			getSMSCharge() const { return m_SMSCharge; }
	void			setSMSCharge(uint charge) { m_SMSCharge = charge; }

	NicknameBook*	getNicknameBook() const { return m_pNicknameBook; }

	NicknameInfo*	getNickname() const { return m_pNickname; }
	void			setNickname(NicknameInfo* pNickname) { m_pNickname = pNickname; }
	
	GQuestManager*	getGQuestManager() const { return m_pGQuestManager; }

	BloodBibleSignInfo* getBloodBibleSign() const { return m_pBloodBibleSign; }

	virtual int		getBloodBibleSignOpenNum() const = 0;
	void			applyBloodBibleSign();

private:
	SMSAddressBook*	m_pSMSAddressBook;
	uint			m_SMSCharge;

	NicknameBook*	m_pNicknameBook;
	NicknameInfo*	m_pNickname;

	GQuestManager*	m_pGQuestManager;
	BloodBibleSignInfo*	m_pBloodBibleSign;

public:
	void			setBaseLuck(Luck_t luck) { m_BaseLuck = luck; }
	Luck_t			getBaseLuck() const { return m_BaseLuck; }
protected:
	Luck_t			m_BaseLuck;

public:
	Store*			getStore() const { return m_pStore; }

protected:
	Store*			m_pStore;

public:
    // get / set PowerPoint
    int getPowerPoint() const { return m_PowerPoint; }
    void setPowerPoint( int powerpoint ) { m_PowerPoint = powerpoint; }

protected:
    // ?¨¨?? ?????
    int             m_PowerPoint;

public:
	bool			isAdvanced() const { return m_bAdvanced; }
	
	Level_t			getAdvancementClassLevel() const;
	Exp_t			getAdvancementClassGoalExp() const;
	bool			increaseAdvancementClassExp(Exp_t exp, bool bApplyExpBount = true);

protected:
	bool				m_bAdvanced;
	AdvancementClass*	m_pAdvancementClass;
	WORD				m_AdvancementClassExpSaveCount;

private:
	MP_t getCurrentMPUpperBound() const throw()
	{
		MP_t maxMP = m_MP[ATTR_MAX];
		if (isOusters())
		{
			if (maxMP > 0xffffffffUL / 3) return (MP_t)0xffffffffUL;
			return (MP_t)(maxMP * 3);
		}

		return maxMP;
	}

	MP_t normalizeCurrentMP(MP_t mp) const throw()
	{
		MP_t upperBound = getCurrentMPUpperBound();
		if (mp <= upperBound) return mp;

		// Unsigned wrap from negative MP must become 0, not max MP.
		if (mp > 0x7fffffffUL) return 0;
		return upperBound;
	}

public:
	HP_t getHP(AttrType attrType = ATTR_CURRENT) const throw() { return m_HP[attrType]; }
	void setHP(HP_t hp, AttrType attrType = ATTR_CURRENT) throw() { m_HP[attrType] = hp; }
	void setHP(HP_t current, HP_t max) throw() { m_HP[ATTR_CURRENT] = current; m_HP[ATTR_MAX] = max; }
	
	MP_t getMP(AttrType attrType = ATTR_CURRENT) const throw() { return m_MP[attrType]; }
	MP_t getValidCurrentMP() const throw() { return normalizeCurrentMP(m_MP[ATTR_CURRENT]); }
	void setMP(MP_t mp, AttrType attrType = ATTR_CURRENT) throw()
	{
		m_MP[attrType] = (attrType == ATTR_CURRENT) ? normalizeCurrentMP(mp) : mp;
		if (attrType == ATTR_MAX) m_MP[ATTR_CURRENT] = normalizeCurrentMP(m_MP[ATTR_CURRENT]);
	}
	void setMP(MP_t current, MP_t max) throw() { m_MP[ATTR_MAX] = max; m_MP[ATTR_CURRENT] = normalizeCurrentMP(current); }
	
	virtual Speed_t getAttackSpeed(AttrType attrType = ATTR_CURRENT) const { return m_AttackSpeed[attrType]; }
	virtual void setAttackSpeed(Speed_t speed, AttrType attrType = ATTR_CURRENT) 
	{ 
		m_AttackSpeed[attrType] = min(250, (int)speed); 
	}
	virtual void increaseAttackSpeed(Speed_t speed, AttrType attrType = ATTR_CURRENT)
	{ 
		m_AttackSpeed[attrType] = min(250, (int)(m_AttackSpeed[attrType] + speed)); 
	}
	
	Damage_t     getDamage(AttrType attrType = ATTR_CURRENT) const throw() { return m_Damage[attrType]; }
	Protection_t getProtection(void) const throw() { return m_Protection[ATTR_CURRENT]; }
	Defense_t    getDefense(void) const throw() { return m_Defense[ATTR_CURRENT]; }
	ToHit_t      getToHit(void) const throw() { return m_ToHit[ATTR_CURRENT]; }
	
	virtual int getCriticalRatio(void) const { return m_CriticalRatio[ATTR_CURRENT]; }
	virtual void setCriticalRatio(int ratio) { m_CriticalRatio[ATTR_CURRENT] = ratio; }
	
	Attr_t				getAdvancedAttrBonus() const { return m_AdvancedAttrBonus; }
	bool				putAdvancedBonusToSTR();
	bool				putAdvancedBonusToDEX();
	bool				putAdvancedBonusToINT();
	bool				popAdvancedBonusFromSTR();
	bool				popAdvancedBonusFromDEX();
	bool				popAdvancedBonusFromINT();
	//20070911
	Attr_t				getAdvancedSTR(AttrType attrType = ATTR_CURRENT) const { return m_AdvancedSTR[attrType]; }
	Attr_t				getAdvancedDEX(AttrType attrType = ATTR_CURRENT) const { return m_AdvancedDEX[attrType]; }
	Attr_t				getAdvancedINT(AttrType attrType = ATTR_CURRENT) const { return m_AdvancedINT[attrType]; }
	void				setAdvancedSTR(Attr_t str, AttrType attrType = ATTR_CURRENT) { m_AdvancedSTR[attrType] = str; }
	void				setAdvancedDEX(Attr_t dex, AttrType attrType = ATTR_CURRENT) { m_AdvancedDEX[attrType] = dex; }
	void				setAdvancedINT(Attr_t inte, AttrType attrType = ATTR_CURRENT) { m_AdvancedINT[attrType] = inte; }
	
	virtual Bonus_t getBonus() const throw() = 0;
	virtual void    setBonus(Bonus_t bonus) throw() = 0;
	
protected:
	HP_t m_HP[3];
	MP_t m_MP[3];
	Speed_t m_AttackSpeed[2];
	
	Damage_t     m_Damage[3];
	ToHit_t      m_ToHit[2];
	Defense_t    m_Defense[2];
	Protection_t m_Protection[2];
	int          m_CriticalRatio[2];
	
	Attr_t				m_AdvancedSTR[3];
	Attr_t				m_AdvancedDEX[3];
	Attr_t				m_AdvancedINT[3];
	Attr_t				m_AdvancedAttrBonus;

public:
	WORD				getMasterEffectColor() const { return m_MasterEffectColor; }
	void				setMasterEffectColor(WORD color) { m_MasterEffectColor = color; }
	bool				canChangeMasterEffectColor();

	WorldID_t			getBorn() const { return m_Born; }
	void				setBorn( const string& str );

protected:
	WORD				m_MasterEffectColor;
	WorldID_t			m_Born;
#ifdef __CONTRIBUTION_SYSTEM__
public:
	void loadContribute() throw();
	void addContributePoint(int ContributePoint) throw();
	void saveContributePoint() const throw();
	void addHolyWarCheckPoint()	throw();
	void addLevelWarCheckPoint() throw();
	int getHolyWarCheckPoint() { return m_HolyWarCheckPoint; }
	int getLevelWarCheckPoint() { return m_LevelWarCheckPoint; }
	int getContributePoint() const { return m_ContributePoint; }
	int getContributeRank() const { return m_ContributeRank; }
	void setContributePoint(int point) throw();
	VSDateTime& getLevelWarCheckDate() { return m_LevelWarCheckDate; }

protected:
	int	m_ContributePoint;
	int m_ContributeRank;
	int	m_HolyWarCheckPoint;
	VSDateTime m_HolyWarCheckDate;
	int m_LevelWarCheckPoint;
	VSDateTime m_LevelWarCheckDate;
#endif

public :
	GearSlotID_t getCurrentGearSlotID() const { return m_CurrentGearSlotID; }
	GearSlotID_t getMaxGearSlotID() const { return m_pWearItemSwapPool.size() - 1; }

	int getMaxGearSlotNum() const { return m_pWearItemSwapPool.size(); }
	// ???? ?????? ???? ??? ?????? ???????? ???? ?????? ????
	int getMaxWearSlotNum() const { return m_pWearItemSwapPool[0].size(); }

	virtual void ChangeGearSlotID(GearSlotID_t GearSlotID);

	void putItemToGear(int iWearPart, GearSlotID_t GearSlotID, Item *pItem);
	bool IsValidWearPart(int Part);

	virtual bool isWear(int iPart) throw();

	virtual void  wearItem(int iPart) throw(Error) = 0;
	virtual void  wearItem(int iPart, Item* pItem) throw(Error) = 0;
        virtual int hasTaoZhuang() throw(Error) = 0;
	
	bool StartToTalkWithCallNPC(NPC *pNPC, int ScriptType = 0);
	void FinishToTalkWithCallNPC();
	NPC *GetTalkingCallNPC();
	
	void SendChangeShapes(bool bSendPacket);
	void CheckValidWearingItem();
	
	virtual bool ComputeSkillOutput(SkillType_t SkillType, SkillOutput& output) = 0;
	virtual bool GetSkillInterval(SkillType_t SkillType, int& iSkillInterval) = 0;
	virtual void ComputeAllSkillDelay() = 0;
	virtual int GetSkillCastingTime(SkillType_t SkillType);
	
public :
	virtual void OnChangeItemInGearSlot(Item *pWearItem, Item *pTakeOffItem) {}
	virtual void OnDead();

public :
	virtual void OnLogOut();
	
protected :
	// WearItem Field
	WearItemList m_pWearItem;
	vector<WearItemList> m_pWearItemSwapPool; // Swapping ?? ???? ?????? ??? ?????? ???? ????

	vector<bool>  m_pRealWearingCheck;

	GearSlotID_t m_CurrentGearSlotID;
	
	NPC *m_pCallNPC;
	
	TParameterList m_ParameterList;
	
public :
	WeekItemListManager *getWeekItemListManager() { return m_pWeekItemListManager; }
	bool loadWeekItemList();
	void loadEventPresentItemList();
	void loadEventNewStartSupplyItemList();
	void loadEventComebackSupplyItemList();
	
	void NotifyZoneToMove();
	
	template<typename T> 
	void SetParameter(string strParamName, T ParamValue)
	{
		TParameterList::iterator iter;
		
		iter = m_ParameterList.find(strParamName);
		
		T *pValue;
		
		if ( iter == m_ParameterList.end() )
		{
			pValue = new T;
		}
		else
		{
			pValue = static_cast<T *>(iter->second);
		}
		
		*pValue = ParamValue;
		m_ParameterList[strParamName] = pValue;
	}

	template<typename T> 
	T GetParameter(string strParamName, T defaultValue)
	{
		TParameterList::iterator iter;
		T retValue;
			
		iter = m_ParameterList.find(strParamName);
		if ( iter != m_ParameterList.end() )
		{
			T *pValue;
			
			pValue = static_cast<T *>(m_ParameterList[strParamName]);
			
			retValue = *pValue;
		}
		else
		{
			retValue = defaultValue;
		}
		
		return retValue;
	}	
	
protected :
	WeekItemListManager *m_pWeekItemListManager;
};

#endif
