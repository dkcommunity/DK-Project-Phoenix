//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectRegenZone.cpp 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGSelectRegenZone.h"

void CGSelectRegenZone::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	iStream.read( m_RegenZoneID );
		
	__END_CATCH
}
		    
void CGSelectRegenZone::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_RegenZoneID );

	__END_CATCH
}

void CGSelectRegenZone::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGSelectRegenZoneHandler::execute(this , pPlayer);

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGSelectRegenZone::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGSelectRegenZone("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
