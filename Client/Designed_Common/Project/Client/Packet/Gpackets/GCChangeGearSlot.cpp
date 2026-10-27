//-------------------------------------------------------------------------------- // 
// Filename    : GCChangeGearSlot.cpp 
// Written By  : shootkj 
// Description : 
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCChangeGearSlot.h"
#include "Client.h"
#include "DarkAssert.h"
#if __CONTENTS(__GEAR_SWAP_CHANGE) // GCChangeGearSlot.cpp
//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCChangeGearSlot::GCChangeGearSlot ()
	
: m_GearSlotID(0), m_pGearInfo(NULL)
{
}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCChangeGearSlot::~GCChangeGearSlot() {
	m_GearSlotID	= 0;
	SAFE_DELETE(m_pGearInfo);
}

//--------------------------------------------------------------------------------
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//--------------------------------------------------------------------------------
void GCChangeGearSlot::read ( SocketInputStream & iStream ) 
	 
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
	 
{
	__BEGIN_TRY
		
	GCChangeGearSlotHandler::execute( this , pPlayer );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
std::string GCChangeGearSlot::toString () const
       
{
	__BEGIN_TRY

//	m_pGearInfo->toString();
		
	StringStream msg;
	msg << "GCChangeGearSlot("
// 릴리즈에서는 에러가 나옴.
//		<< "GearInfo:" << m_pGearInfo->toString();
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif //__GEAR_SWAP_CHANGE
