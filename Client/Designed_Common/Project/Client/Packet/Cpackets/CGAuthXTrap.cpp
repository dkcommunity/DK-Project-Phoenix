#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuthXTrap.cpp 
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGAuthXTrap.h"


void CGAuthXTrap::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	iStream.read(m_shCmdFlag);
	BYTE sizeAuth =0;
	iStream.read(sizeAuth);
	
	if ( sizeAuth == 0 )
		throw InvalidProtocolException("sizeAuth == 0");

	if ( sizeAuth > szAuth )
		throw InvalidProtocolException("sizeAuth too long name length");
	
	iStream.read((char *) m_AuthData , sizeAuth);

	__END_CATCH
}
		    
void CGAuthXTrap::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	oStream.write(m_shCmdFlag);
	oStream.write(szAuth);
	oStream.write( (char *)m_AuthData, szAuth );

	__END_CATCH
}

void CGAuthXTrap::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	CGAuthXTrapHandler::execute( this , pPlayer );

	__END_CATCH
}

std::string CGAuthXTrap::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGAuthXTrap("
		<< "Key:" << m_AuthData 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}



#endif
