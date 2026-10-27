//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRequestRankGemData.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGRequestRankGemData.h"


void CGRequestRankGemData::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
	
	//iStream.read( m_Kind );
		
	__END_CATCH
}

void CGRequestRankGemData::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
	
	//oStream.write( m_Kind );
		
	__END_CATCH
}

void CGRequestRankGemData::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	CGRequestRankGemDataHandler::execute(this , pPlayer);

	__END_CATCH
}
