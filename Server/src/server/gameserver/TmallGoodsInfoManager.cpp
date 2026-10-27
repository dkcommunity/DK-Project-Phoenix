//////////////////////////////////////////////////////////////////////////////
// Filename    : GoodsInfo.cpp
// Written By  : beowulf
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "TmallGoodsInfoManager.h"
#include "Assert.h"
#include "DB.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"

//////////////////////////////////////////////////////////////////////////////
// class GoodsInfo member methods
//////////////////////////////////////////////////////////////////////////////

TmallGoodsInfow::TmallGoodsInfow()
{
	__BEGIN_TRY
	__END_CATCH
}

TmallGoodsInfow::~TmallGoodsInfow()
{
	__BEGIN_TRY
	__END_CATCH
}

string TmallGoodsInfow::toString() const
{
	__BEGIN_TRY
	
	StringStream msg;
	msg << "TmallGoodsInfo ("
		<< "Type:"			<< (int)m_ItemType
		<< ",Name:"			<< m_Name
		<< ",ItemClass:"	<< (int)m_ItemClass
		<< ",ItemType:"		<< (int)m_ItemType
		<< ",TimeLimit:"	<< (m_bTimeLimit?"Y":"N")
		<< ",Hour:"			<< m_Hour
		<< ")";
	return msg.toString();

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// class GoodsInfoManager member methods
//////////////////////////////////////////////////////////////////////////////

TmallGoodsInfoManager::TmallGoodsInfoManager()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

TmallGoodsInfoManager::~TmallGoodsInfoManager()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

void TmallGoodsInfoManager::init()
	throw(Error)
{
	__BEGIN_TRY

	load();

	__END_CATCH
}

void TmallGoodsInfoManager::clear()
	throw()
{
	__BEGIN_TRY

	HashMapTmallGoodsInfoItr itr = m_GoodsInfos.begin();

	for ( ; itr != m_GoodsInfos.end(); itr++ )
	{
		SAFE_DELETE( itr->second );
	}

	m_GoodsInfos.clear();
	
	__END_CATCH
}

void TmallGoodsInfoManager::load()
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	clear();

	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
		pResult = pStmt->executeQuery("SELECT GoodsID, Name, ItemClass, ItemType, Grade, OptionType, Num, Limited+0, Hour FROM GoodsListInfo WHERE Kind<>'SET'");

		while (pResult->next()) 
		{
			TmallGoodsInfow* pGoodsInfo = new TmallGoodsInfow();
			int i = 0;

			pGoodsInfo->setID( pResult->getInt(++i) );
			pGoodsInfo->setName( pResult->getString(++i) );
			pGoodsInfo->setItemClass( (Item::ItemClass)(pResult->getInt(++i)) );
			pGoodsInfo->setItemType( pResult->getInt(++i) );
			pGoodsInfo->setGrade( pResult->getInt(++i) );
			string optionField = pResult->getString(++i);
			pGoodsInfo->setNum( pResult->getInt(++i) );
			pGoodsInfo->setTimeLimit( pResult->getInt(++i) == 1 ); // enum( 'LIMITED'(1), 'UNLIMITED'(2) ) 
			pGoodsInfo->setHour( pResult->getInt(++i) );

			list<OptionType_t> optionTypes;
			setOptionTypeFromField( optionTypes, optionField );
			pGoodsInfo->setOptionTypeList( optionTypes );

			addGoodsInfo(pGoodsInfo);
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_DEBUG
	__END_CATCH
}

void TmallGoodsInfoManager::addGoodsInfo(TmallGoodsInfow* pGoodsInfo)
	throw(DuplicatedException, Error)
{
	__BEGIN_TRY

  	Assert (pGoodsInfo != NULL);

	HashMapTmallGoodsInfoItr itr = m_GoodsInfos.find( pGoodsInfo->getID() );

	if ( itr != m_GoodsInfos.end() )
		throw DuplicatedException();

	m_GoodsInfos[ pGoodsInfo->getID() ] = pGoodsInfo;

	__END_CATCH
}

TmallGoodsInfow* TmallGoodsInfoManager::getGoodsInfo( DWORD id ) const
	throw()
{
	__BEGIN_TRY

	HashMapTmallGoodsInfoConstItr itr = m_GoodsInfos.find( id );

	if ( itr == m_GoodsInfos.end() )
		return NULL;

	return itr->second;

	__END_CATCH
}

string TmallGoodsInfoManager::toString() const
	throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GoodsInfoManager(\n";

	HashMapTmallGoodsInfoConstItr itr = m_GoodsInfos.begin();

	for ( ; itr != m_GoodsInfos.end(); itr++ )
	{
		msg << "TmallGoodsInfo[" << itr->second->getID() << "] == ";
		msg	<< itr->second->getName();
		msg << "\n";
		msg << itr->second->toString() << "\n";
	}

	return msg.toString();

	__END_CATCH
}

// Global Variable definition
TmallGoodsInfoManager* g_pTmallGoodsInfoManager = NULL;
