//-------------------------------------------------------------------------------- // 
// Filename    : GCChangeGearSlot.cpp 
// Written By  : rappi76 
// Description : 
// 
//--------------------------------------------------------------------------------

// include files
#include "GCChangeGearSlot.h"
#include "Assert.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCChangeGearSlot::GCChangeGearSlot ()
	throw ()
: m_GearSlotID(0), m_pGearInfo(NULL)
{
}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCChangeGearSlot::~GCChangeGearSlot ()
	throw ()
{
	SAFE_DELETE(m_pGearInfo);
}

//--------------------------------------------------------------------------------
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//--------------------------------------------------------------------------------
void GCChangeGearSlot::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read(m_GearSlotID);

	m_pGearInfo = new GearInfo();
	m_pGearInfo->read( iStream );
		
	__END_CATCH
}

		    
//--------------------------------------------------------------------------------
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//--------------------------------------------------------------------------------
void GCChangeGearSlot::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write(m_GearSlotID );

	m_pGearInfo->write( oStream );
		
	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCChangeGearSlot::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCChangeGearSlotHandler::execute( this , pPlayer );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCChangeGearSlot::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCChangeGearSlot("
		<< "GearInfo:" << m_pGearInfo->toString()
		<< ")";
	return msg.toString();

	__END_CATCH
}


