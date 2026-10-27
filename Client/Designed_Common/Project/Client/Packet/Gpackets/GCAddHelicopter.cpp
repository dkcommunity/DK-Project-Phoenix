//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddHelicopter.cpp
// Written By  : excel96
// Description :
// 슬레이어가 무전기를 사용해, 헬기를 호출했을 때 날아가는 패킷
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddHelicopter.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopter member methods
//////////////////////////////////////////////////////////////////////////////

void GCAddHelicopter::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_Code);
	iStream.read(m_ExtendType);

	__END_CATCH
}
		    
void GCAddHelicopter::write ( SocketOutputStream & oStream ) 
     const 
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_Code);
	oStream.write(m_ExtendType);	
	__END_CATCH
}

void GCAddHelicopter::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddHelicopterHandler::execute( this , pPlayer );
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
	std::string GCAddHelicopter::toString () 
		const 
	{
		__BEGIN_TRY

		StringStream msg;
		msg << "GCAddHelicopter( "
			<< ",ObjectID:" << (int)m_ObjectID
			<< ",Code:" << (int)m_Code
			<< ",ExtendType:" << (int)m_ExtendType
			<< ")";
		return msg.toString();

		__END_CATCH
	}

#endif
