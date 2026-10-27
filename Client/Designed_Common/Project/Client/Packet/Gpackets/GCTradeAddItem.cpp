////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeAddItem.cpp 
// Written By  : 김성민
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradeAddItem.h"
#include "DarkAssert.h"

////////////////////////////////////////////////////////////////////////////////
// constructor
////////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PCITEMINFO2)
GCTradeAddItem::GCTradeAddItem()
	
{
	__BEGIN_TRY
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// destructor
////////////////////////////////////////////////////////////////////////////////
GCTradeAddItem::~GCTradeAddItem() {
	__BEGIN_TRY
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read( m_TargetObjectID );
	iStream.read( m_X );
	iStream.read( m_Y );
	
	m_PCItemInfo.read(iStream);
	
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	iStream.read(m_CashItem);
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
		
	__END_CATCH
}

		    
////////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write( m_TargetObjectID );
	oStream.write( m_X );
	oStream.write( m_Y );

	m_PCItemInfo.write(oStream);

#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	oStream.write(m_CashItem);
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
	
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// execute packet's handler
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCTradeAddItemHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__	
////////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
////////////////////////////////////////////////////////////////////////////////
std::string GCTradeAddItem::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << " GCTradeAddItem("
		<< " TargetObjectID : " << (int)m_TargetObjectID 
#if __CONTENTS(__PCITEMINFO2)
		<< ",PCItemInfo: " << m_PCItemInfo.toString()
#else //__PCITEMINFO2
		<< " ItemObjectID : "   << (int)m_ItemObjectID 
		<< " ItemClass : "      << (int)m_ItemClass 
		<< " ItemType : "       << (int)m_ItemType 
		<< " OptionTypeSize : "     << (int)m_OptionType.size()
		<< " Durability : "     << (int)m_Durability 
		<< " ItemNum : "        << (int)m_ItemNum 
		<< " Silver : "         << (int)m_Silver
		<< " Grade : "          << (int)m_Grade
		<< " EnchantLevel : "   << (int)m_EnchantLevel
		<< " ListNum : "        << (int)m_ListNum 
		<< "ThirdOptionTypeSize:" << (int)m_ThirdOptionType.size()
		<< "ThirdEnchantType"	<<	(int)m_ThirdEnchantType
#endif //__PCITEMINFO2
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
		<< "CashItem"			<< (int)m_CashItem
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
		<< " )";
	return msg.toString();
		
	__END_CATCH
}
#endif //__DEBUG_OUTPUT__
#else //__PCITEMINFO2
GCTradeAddItem::GCTradeAddItem()
	
{
	__BEGIN_TRY

	m_ListNum = 0;
	m_ThirdEnchantType = 99;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// destructor
////////////////////////////////////////////////////////////////////////////////
GCTradeAddItem::~GCTradeAddItem() {
	__BEGIN_TRY

	while (!m_InfoList.empty())
	{
		SubItemInfo* pInfo = m_InfoList.front();
		delete pInfo;
		m_InfoList.pop_front();
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read( m_TargetObjectID );
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_ItemObjectID );
	iStream.read( m_ItemClass );
	iStream.read( m_ItemType );

	BYTE optionSize;
	iStream.read( optionSize );
	for (int i=0; i<optionSize; i++)
	{
		OptionType_t optionType;
		iStream.read( optionType );
		addOptionType( optionType );
	}

	iStream.read( m_Durability );
	iStream.read( m_ItemNum );
	iStream.read( m_Silver );
	iStream.read( m_Grade );
	iStream.read( m_EnchantLevel );
	iStream.read( m_ListNum );
	for (int i = 0; i < m_ListNum; i++) 
	{
		SubItemInfo* pInfo = new SubItemInfo();
		pInfo->read( iStream );
		m_InfoList.push_back( pInfo );
	}
	BYTE thirdOptionSize;
	iStream.read( thirdOptionSize );
//20071009 by diesirace 서드아이템옵션 추가 
	m_ThirdOptionType.clear();
	for (int thirdindex = 0; thirdindex < thirdOptionSize; thirdindex++) 
	{
		OptionType_t optionType;
		iStream.read( optionType );
		m_ThirdOptionType.push_back( optionType );
	}

	iStream.read( m_ThirdEnchantType );
	
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	iStream.read(m_CashItem);
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
		
	__END_CATCH
}

		    
////////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write( m_TargetObjectID );
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_ItemObjectID );
	oStream.write( m_ItemClass );
	oStream.write( m_ItemType );
	
	BYTE optionSize = m_OptionType.size();
	oStream.write( optionSize );

	std::list<OptionType_t>::const_iterator iOption = m_OptionType.begin();
	for (; iOption!=m_OptionType.end(); iOption++)
	{
		OptionType_t optionType = *iOption;
		oStream.write( optionType );
	}

	oStream.write( m_Durability );
	oStream.write( m_ItemNum );
	oStream.write( m_Silver );
	oStream.write( m_Grade );
	oStream.write( m_EnchantLevel );
	oStream.write( m_ListNum );
	std::list<SubItemInfo*>::const_iterator itr = m_InfoList.begin();
	for (; itr!= m_InfoList.end(); itr++) 
	{
		Assert(*itr != NULL);
		(*itr)->write( oStream );
	}
	BYTE thirdOptionSize = m_ThirdOptionType.size();
	oStream.write( thirdOptionSize );

	std::list<OptionType_t>::const_iterator iThirdOption = m_ThirdOptionType.begin();
    for (; iThirdOption!= m_ThirdOptionType.end(); iThirdOption++) 
	{
		OptionType_t optionType = *iThirdOption;
		oStream.write( optionType );
	}
	oStream.write( m_ThirdEnchantType );
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
	oStream.write(m_CashItem);
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
	
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// execute packet's handler
////////////////////////////////////////////////////////////////////////////////
void GCTradeAddItem::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCTradeAddItemHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__	
////////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
////////////////////////////////////////////////////////////////////////////////
std::string GCTradeAddItem::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << " GCTradeAddItem("
		<< " TargetObjectID : " << (int)m_TargetObjectID 
		<< " ItemObjectID : "   << (int)m_ItemObjectID 
		<< " ItemClass : "      << (int)m_ItemClass 
		<< " ItemType : "       << (int)m_ItemType 
		<< " OptionTypeSize : "     << (int)m_OptionType.size()
		<< " Durability : "     << (int)m_Durability 
		<< " ItemNum : "        << (int)m_ItemNum 
		<< " Silver : "         << (int)m_Silver
		<< " Grade : "          << (int)m_Grade
		<< " EnchantLevel : "   << (int)m_EnchantLevel
		<< " ListNum : "        << (int)m_ListNum 
		<< "ThirdOptionTypeSize:" << (int)m_ThirdOptionType.size()
		<< "ThirdEnchantType"	<<	(int)m_ThirdEnchantType
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
		<< "CashItem"			<< (int)m_CashItem
#endif //__INTERNATIONAL_PREMIUM_SYSTEM
		<< " )";
	return msg.toString();
		
	__END_CATCH
}
#endif //__DEBUG_OUTPUT__	
#endif //__PCITEMINFO2
