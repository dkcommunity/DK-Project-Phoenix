//////////////////////////////////////////////////////////////////////////////
// Filename    : GearSlotInfo.cpp
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GearSlotInfo.h"

//////////////////////////////////////////////////////////////////////////////
// read data from socket input stream
//////////////////////////////////////////////////////////////////////////////
void GearSlotInfo::read ( SocketInputStream & iStream ) 
	
{
	__BEGIN_TRY

	// 먼저 하위 클래스에서 읽어들이고...
	PCItemInfo::read(iStream);

	// 이 클래스의 데이터를 읽어들인다.
	iStream.read(m_SlotID);

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo::read 착용 중인 아이템이 활성활 될 수 있는지의 값을 서버에게 받는다.
	iStream.read(m_bActiveSlot);
#endif	// __GEAR_SWAP_CHANGE
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// write data to socket output stream
//////////////////////////////////////////////////////////////////////////////
void GearSlotInfo::write ( SocketOutputStream & oStream ) const 
	
{
	__BEGIN_TRY

	// 먼저 하위 클래스에서 읽어들이고...
	PCItemInfo::write(oStream);

	// 이 클래스의 데이터를 읽어들인다.
	oStream.write(m_SlotID);

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo::write  착용 중인 아이템이 활성활 될 수 있는지의 값을 서버에게 보낸다.
	oStream.write(m_bActiveSlot);
#endif	// __GEAR_SWAP_CHANGE

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GearSlotInfo::toString () const 
	
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GearSlotInfo("
		<< PCItemInfo::toString()
		<< "SlotID:" << (int)m_SlotID

#if __CONTENTS(__GEAR_SWAP_CHANGE)
		<< "ActiveSlot:" << (int)m_SlotID
#endif	// __GEAR_SWAP_CHANGE

		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif
