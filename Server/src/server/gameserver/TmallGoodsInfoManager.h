//////////////////////////////////////////////////////////////////////////////
// Filename    : GoodsInfo.h
// Written By  : bezz
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __TMALL_GOODS_INFOW_H__
#define __TMALL_GOODS_INFOW_H__

#include "Exception.h"
#include "Types.h"
#include "Item.h"

#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////////////
// Class TmallGoodsInfo
//////////////////////////////////////////////////////////////////////////////

class TmallGoodsInfow 
{
public:
	TmallGoodsInfow();
	~TmallGoodsInfow();

public:
	DWORD getID() const { return m_ID; }
	void setID(DWORD id) { m_ID = id; }

	string getName() const { return m_Name ; }
	void setName(string name) { m_Name = name ; }

	Item::ItemClass getItemClass() const { return m_ItemClass; }
	void setItemClass( Item::ItemClass itemClass ) { m_ItemClass = itemClass; }

	ItemType_t getItemType() const { return m_ItemType; }
	void setItemType( ItemType_t itemType ) { m_ItemType = itemType; }

	Grade_t getGrade() const { return m_Grade; }
	void setGrade( Grade_t grade ) { m_Grade = grade; }

	const list<OptionType_t>& getOptionTypeList() const { return m_OptionTypes; }
	void setOptionTypeList( const list<OptionType_t>& optionTypes ) { m_OptionTypes = optionTypes; }

	int getNum() const { return m_Num; }
	void setNum( int num ) { m_Num = num; }

	bool isTimeLimit() const { return m_bTimeLimit; }
	void setTimeLimit( bool bTimeLimit = true ) { m_bTimeLimit = bTimeLimit; }

	int getHour() const { return m_Hour; }
	void setHour( int hour ) { m_Hour = hour; }

	// toString
	string toString() const ;

private:
	DWORD					m_ID;			// Type
	string					m_Name;			// Name
	Item::ItemClass			m_ItemClass;	// ItemClass
	ItemType_t				m_ItemType;		// ItemType
	Grade_t					m_Grade;		// Grade
	list<OptionType_t>		m_OptionTypes;	// OptionType list
	int 					m_Num;
	bool					m_bTimeLimit;	// °£ Ÿ°¡?
	int						m_Hour;			// »ç¿ë  °£
};

//////////////////////////////////////////////////////////////////////////////
// Class GoodsInfoManager
//////////////////////////////////////////////////////////////////////////////

class TmallGoodsInfoManager 
{
public:
	typedef map<DWORD, TmallGoodsInfow*>			HashMapTmallGoodsInfo;
	typedef HashMapTmallGoodsInfo::iterator			HashMapTmallGoodsInfoItr;
	typedef HashMapTmallGoodsInfo::const_iterator	HashMapTmallGoodsInfoConstItr;

public:
	TmallGoodsInfoManager() throw();
	~TmallGoodsInfoManager() throw();

public:

	// initialize Manager
	void init() throw(Error);

	// void load() throw();
	void load() throw(Error);
	
	// clear list
	void clear() throw();

	// add GoodsInfo
	void addGoodsInfo(TmallGoodsInfow* pGoodsInfo) throw(DuplicatedException, Error);

	// get GoodsInfo
	TmallGoodsInfow* getGoodsInfo( DWORD id ) const throw();

	// toString for Debug
	string toString() const throw();

private:
	HashMapTmallGoodsInfo		m_GoodsInfos;

};

// Global Variable Definition
extern TmallGoodsInfoManager* g_pTmallGoodsInfoManager;

#endif	// __SKILL_INFO_MANAGER_H__
