//////////////////////////////////////////////////////////////////////////////
// Filename    : GCCreateItem.cpp
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCCreateItem.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
GCCreateItem::GCCreateItem()
	throw()
{
	__BEGIN_TRY
	
	m_InvenX       = 0;
	m_InvenY       = 0;
	// 2007 03 29 Add
	m_InventoryItemObjectID = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GCCreateItem::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_InvenX );
	iStream.read( m_InvenY );
	
	m_PCItemInfo.read(iStream);
	
	iStream.read( m_InventoryItemObjectID );
		
	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GCCreateItem::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	oStream.write( m_InvenX );
	oStream.write( m_InvenY );
	
	m_PCItemInfo.write(oStream);
	
	oStream.write( m_InventoryItemObjectID );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCCreateItem::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCCreateItemHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCCreateItem::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCCreateItem("
		<< ",InvenX:"       << (int)m_InvenX
		<< ",InvenY:"       << (int)m_InvenY
		<< ",PCItemInfo:"       << m_PCItemInfo.toString()
		<< ",InventorySlotInfo:" << (int)m_InventoryItemObjectID
		<< ")";
	return msg.toString();

	__END_CATCH
}


