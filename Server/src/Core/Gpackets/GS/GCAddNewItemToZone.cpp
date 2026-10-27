//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddNewItemToZone.cc 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddNewItemToZone.h"
#include "Assert.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCAddNewItemToZone::GCAddNewItemToZone() 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

	
//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCAddNewItemToZone::~GCAddNewItemToZone() 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddNewItemToZone::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCAddNewItemToZoneHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCAddNewItemToZone::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GCAddNewItemToZone("
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",PCItemInfo: " << m_PCItemInfo.toString()
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
