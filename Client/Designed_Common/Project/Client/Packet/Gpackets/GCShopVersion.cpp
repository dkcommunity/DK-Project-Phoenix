//--------------------------------------------------------------------------------
// 
// Filename    : GCShopVersion.cpp 
// Written By  : 김성민
// Description : 플레이어에게 서버 측의 상점 버전을 알려줄 때 쓰이는 패킷이다.
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCShopVersion.h"
#include "DarkAssert.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopVersion::GCShopVersion() 
	
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopVersion::~GCShopVersion() {
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//--------------------------------------------------------------------
void GCShopVersion::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	// read object id
	iStream.read( m_ObjectID );
#if __CONTENTS(__GLOBAL_NPC)
	iStream.read(m_NPCID);
#endif //__GLOBAL_NPC

	// read versions
	for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
		iStream.read(m_Version[i]);

	iStream.read(m_MarketCondSell);
	__END_CATCH
}

		    
//--------------------------------------------------------------------------------
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//--------------------------------------------------------------------------------
void GCShopVersion::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	// write object id
	oStream.write( m_ObjectID );
#if __CONTENTS(__GLOBAL_NPC)
	oStream.write(m_NPCID);
#endif//__GLOBAL_NPC
	// write versions
	for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
		oStream.write(m_Version[i]);
	
	oStream.write( m_MarketCondSell );
	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopVersion::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCShopVersionHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCShopVersion::toString () const
		   
	{
		__BEGIN_TRY
			
		StringStream msg;
		
		msg << "GCShopVersion(" << "ObjectID:" << (int)m_ObjectID;
#if __CONTENTS(__GLOBAL_NPC)
		msg << "NPCID" << (NPCID_t)m_NPCID;
#endif//__GLOBAL_NPC
		msg << "Ratio:" << (int)m_MarketCondSell;
		
		for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
			msg << " Version[" << (int)i << "] : " << (int)m_Version[i];

		//msg << ")" << std::endl;
		
		return msg.toString();
			
		__END_CATCH
	}
#endif

