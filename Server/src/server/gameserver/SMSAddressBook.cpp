#include "SMSAddressBook.h"
#include "DB.h"
#include "PlayerCreature.h"
#include "Gpackets/GCAddressListVerify.h"

AddressUnit* SMSAddressElement::getAddressUnit() const 
{
	AddressUnit* pRet = new AddressUnit;
	pRet->ElementID = m_ElementID;
	pRet->CharacterName = m_CharacterName;
	pRet->CustomName = m_CustomName;
	pRet->Number = m_Number;

	return pRet;
}

SMSAddressBook::~SMSAddressBook()
{
	map<DWORD, SMSAddressElement*>::iterator itr = m_Addresses.begin();
	map<DWORD, SMSAddressElement*>::iterator endItr = m_Addresses.end();

	for ( ; itr != endItr ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}

	m_Addresses.clear();
}

void SMSAddressBook::load() throw(Error)
{
	__BEGIN_TRY

	m_Addresses.clear();
	Assert( m_pOwner != NULL );

	m_NextEID = 1;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT eID, CharacterName, CustomName, Number FROM SMSAddressBook WHERE OwnerID='%s'",
				m_pOwner->getName().c_str() );

		while ( pResult->next() )
		{
			SMSAddressElement* pElement = new SMSAddressElement(
					pResult->getInt(1),
					pResult->getString(2),
					pResult->getString(3),
					pResult->getString(4) );

//			Assert( addAddressElement( pElement ) );
			Assert( m_Addresses[pElement->getID()] == NULL );
			m_Addresses[pElement->getID()] = pElement;
			if ( m_NextEID <= pElement->getID() ) m_NextEID = pElement->getID()+1;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt);

	__END_CATCH
}

GCSMSAddressList* SMSAddressBook::getGCSMSAddressList() const
{
	GCSMSAddressList* pRet = new GCSMSAddressList;

	map<DWORD,SMSAddressElement*>::const_iterator itr = m_Addresses.begin();
	map<DWORD,SMSAddressElement*>::const_iterator endItr = m_Addresses.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( itr->second != NULL ) pRet->getAddresses().push_back( itr->second->getAddressUnit() );
	}
	
	return pRet;
}

int SMSAddressBook::addAddressElement( SMSAddressElement* pElement )
{
	if ( !IsValidName(pElement->m_CharacterName) )
	{
		return GCAddressListVerify::ADD_FAIL_INVALID_DATA;
	}
	
	if ( !IsValidName(pElement->m_CustomName) )
	{
		return GCAddressListVerify::ADD_FAIL_INVALID_DATA;
	}
	
	if ( !IsValidNumber(pElement->m_Number) )
	{
		return GCAddressListVerify::ADD_FAIL_INVALID_DATA;
	}
	
	if ( m_Addresses.size() > MAX_ADDRESS_NUM ) return GCAddressListVerify::ADD_FAIL_MAX_NUM_EXCEEDED;
	if ( m_Addresses[pElement->getID()] != NULL ) return GCAddressListVerify::ADD_FAIL_INVALID_DATA;

	m_Addresses[pElement->getID()] = pElement;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "INSERT INTO SMSAddressBook (eID, OwnerID, CharacterName, CustomName, Number) VALUES (%u, '%s', '%s', '%s', '%s')",
				pElement->m_ElementID, m_pOwner->getName().c_str(),
				pElement->m_CharacterName.c_str(), pElement->m_CustomName.c_str(),
				pElement->m_Number.c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt );

	return GCAddressListVerify::ADDRESS_LIST_ADD_OK;
}

int SMSAddressBook::removeAddressElement( DWORD eID )
{
	map<DWORD, SMSAddressElement*>::iterator itr = m_Addresses.find( eID );

	if ( itr == m_Addresses.end() ) return GCAddressListVerify::DELETE_FAIL_NO_SUCH_EID;

	m_Addresses.erase( itr );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "DELETE FROM SMSAddressBook WHERE eID = %u AND OwnerID = '%s'",
				eID, m_pOwner->getName().c_str() );

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt );

	return 0;
}

bool SMSAddressBook::IsValidName(string& strName)
{
	int iIndex;
	
	if ( strName.size() == 0 )
	{
		return false;
	}
	
	iIndex = strName.find("'");
	if ( iIndex >= 0 )
	{
		return false;
	}
	
	iIndex = strName.find("\"");
	if ( iIndex >= 0 )
	{
		return false;
	}
	
	iIndex = strName.find("\\");
	if ( iIndex >= 0 )
	{
		return false;
	}
	
	return true;
}

bool SMSAddressBook::IsValidNumber(string& strNumber)
{
	int i, iLen;
	
	iLen = strNumber.size();
	
	if ( iLen == 0 )
	{
		return false;
	}
	
	for(i = 0; i < iLen ; ++i)
	{
		if ( strNumber.at(i) < '0' || strNumber.at(i) > '9' )
		{
			return false;
		}
	}
	
	return true;
}