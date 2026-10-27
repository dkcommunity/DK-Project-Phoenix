//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddInstalledMineToZone.cc 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddInstalledMineToZone.h"
#include "Assert.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCAddInstalledMineToZone::GCAddInstalledMineToZone() 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

	
//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCAddInstalledMineToZone::~GCAddInstalledMineToZone() 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddInstalledMineToZone::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCAddInstalledMineToZoneHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCAddInstalledMineToZone::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCAddInstalledMineToZone("
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",PCItemInfo:" << m_PCItemInfo.toString()
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
