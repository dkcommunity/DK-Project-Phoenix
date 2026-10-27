//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRepairInfo.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRepairInfo.h"


//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCRepairInfo::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_ItemOID );
	iStream.read( m_RepairPrice );
	iStream.read( m_RepairDiscountPrice );


	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCRepairInfo::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_ItemOID );
	oStream.write( m_RepairPrice );
	oStream.write( m_RepairDiscountPrice );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCRepairInfo::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCRepairInfoHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCRepairInfo::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCRepairInfo("
		<< "ObjectID:" << m_ItemOID 
		<< ",RepairPrice:" << m_RepairPrice
		<< ",RepairDiscountPrice:" << m_RepairDiscountPrice 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}


