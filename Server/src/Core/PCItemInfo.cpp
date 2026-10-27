//////////////////////////////////////////////////////////////////////////////
// Filename    : PCItemInfo.cpp 
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PCItemInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "StringStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
PCItemInfo::PCItemInfo () 
     throw ()
{
	__BEGIN_TRY

	m_ObjectID     = 0;
	m_IClass       = 0;
	m_ItemType     = 0;
	m_Durability   = 0;
	m_Silver       = 0;
	m_Grade        = 0;
	m_EnchantLevel = 0;
	m_ItemNum      = 0;
	m_MainColor    = 0;
	m_ListNum      = 0;
#ifdef VERSION_THIRD_ENCHANT_1
	m_ThirdEnchantType = 99;
#endif
	m_AddedInfo1 = 255;	//1208 wlzzi
	m_AddedInfo2 = 255;	// - default 값으로 255를... 
	m_AddedInfo3 = 255;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
PCItemInfo::~PCItemInfo () 
    throw ()
{
	__BEGIN_TRY

	while (!m_SubItemInfoList.empty()) 
	{
		SubItemInfo* pSubItemInfo = m_SubItemInfoList.front();
		SAFE_DELETE(pSubItemInfo);
		m_SubItemInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void PCItemInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );
	iStream.read( m_IClass );
	iStream.read( m_ItemType );

	BYTE optionSize;
	iStream.read( optionSize );

	m_OptionType.clear();
	for (int i = 0; i < optionSize; i++) 
	{
		OptionType_t optionType;
		iStream.read( optionType );
		m_OptionType.push_back( optionType );
	}

	BYTE GTwooptionSize;
	iStream.read( GTwooptionSize );

	m_GTwoOptionType.clear();
	for (int i = 0; i < GTwooptionSize; i++) 
	{
		OptionType_t GTwooptionType;
		iStream.read( GTwooptionType );
		m_GTwoOptionType.push_back( GTwooptionType );
	}
	iStream.read( m_Durability );
	iStream.read( m_Silver );
	iStream.read( m_Grade );
	iStream.read( m_EnchantLevel );
	iStream.read( m_ItemNum );
	iStream.read( m_MainColor );
	iStream.read( m_ListNum );

	for (int i = 0; i < m_ListNum; i++) 
	{
		SubItemInfo* pSubItemInfo = new SubItemInfo();
		pSubItemInfo->read(iStream);
		m_SubItemInfoList.push_back( pSubItemInfo );
	}
#ifdef VERSION_THIRD_ENCHANT_1
	BYTE thirdoptionSize;
	iStream.read( thirdoptionSize );
	m_ThirdOptionType.clear();
	for (int i = 0; i < thirdoptionSize; i++) 
	{
		OptionType_t thirdoptionType;
		iStream.read( thirdoptionType );
		m_ThirdOptionType.push_back( thirdoptionType );
	}
	iStream.read( m_ThirdEnchantType );
#endif
	//1208 wlzzi
	iStream.read( m_AddedInfo1 );
	iStream.read( m_AddedInfo2 );
	iStream.read( m_AddedInfo3 );
	
	//20090601 ksym555
	BYTE mixOptionSize;
	iStream.read( mixOptionSize );
	
	m_MixOptionType.clear();
	
	for ( int k=0; k < mixOptionSize ; ++k )
	{
		OptionType_t mixOptionType;
		iStream.read( mixOptionType );
		m_MixOptionType.push_back( mixOptionType );
	}
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void PCItemInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_ObjectID );
	oStream.write( m_IClass );
	oStream.write( m_ItemType );

	BYTE optionSize = m_OptionType.size();
	oStream.write( optionSize );

	list<OptionType_t>::const_iterator iOption = m_OptionType.begin();
    for (; iOption!= m_OptionType.end(); iOption++) 
	{
		OptionType_t optionType = *iOption;
		oStream.write( optionType );
	}
    

	BYTE GTwooptionSize = m_GTwoOptionType.size();
	oStream.write( GTwooptionSize );

	list<OptionType_t>::const_iterator GTwoiOption = m_GTwoOptionType.begin();
    for (; GTwoiOption!= m_GTwoOptionType.end(); GTwoiOption++) 
	{
		OptionType_t GTwooptionType = *GTwoiOption;
		oStream.write( GTwooptionType );
	}


	oStream.write( m_Durability );
	oStream.write( m_Silver );
	oStream.write( m_Grade );
	oStream.write( m_EnchantLevel );
	oStream.write( m_ItemNum );
	oStream.write( m_MainColor );
	oStream.write( m_ListNum );

	list<SubItemInfo*>::const_iterator itr = m_SubItemInfoList.begin();
    for (; itr!= m_SubItemInfoList.end(); itr++) 
		(*itr)->write(oStream);
#ifdef VERSION_THIRD_ENCHANT_1
	BYTE thirdoptionSize = m_ThirdOptionType.size();
	oStream.write( thirdoptionSize );
	list<OptionType_t>::const_iterator iThirdOption = m_ThirdOptionType.begin();
	for (; iThirdOption!= m_ThirdOptionType.end(); iThirdOption++) 
	{
		OptionType_t thirdoptionType = *iThirdOption;
		oStream.write( thirdoptionType );
	}
	oStream.write( m_ThirdEnchantType );
#endif
	//1208 wlzzi
	oStream.write( m_AddedInfo1 );
	oStream.write( m_AddedInfo2 );
	oStream.write( m_AddedInfo3 );
	
	//20090601 ksym555
	BYTE mixOptionSize = m_MixOptionType.size();
	oStream.write( mixOptionSize );
	std::list<OptionType_t>::const_iterator mixitr = m_MixOptionType.begin();
	for (; mixitr!=m_MixOptionType.end(); mixitr++)
	{
		OptionType_t mixOptionType = *mixitr;
		oStream.write( mixOptionType );
	}
	//cout << toString().c_str() << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// debug string
//////////////////////////////////////////////////////////////////////////////
string PCItemInfo::toString() const
	throw()
{
	StringStream msg;
	msg << "PCItemInfo("
		<< "ObjectID:" << (int)m_ObjectID
		<< "ItemClass:" << (int)m_IClass
		<< "ItemType:" << (int)m_ItemType
		<< "OptionTypeSize:" << (int)m_OptionType.size()
		<< "GTwoOptionTypeSize:" << (int)m_OptionType.size()
		<< "Durability:" << (int)m_Durability
		<< "Silver:" << (int)m_Silver
		<< "Grade:" << (int)m_Grade
		<< "EnchantLevel:" << (int)m_EnchantLevel
		<< "ItemNum:" << (int)m_ItemNum
		<< "MainColor:" << (int)m_MainColor
		<< "ListNum:" << (int)m_ListNum
		<< ")";
	return msg.toString();
}
