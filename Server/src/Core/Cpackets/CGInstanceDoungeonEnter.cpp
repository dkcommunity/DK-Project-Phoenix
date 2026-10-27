//////////////////////////////////////////////////////////////////////////////
// Filename    : CGInstanceDoungeonEnter.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGInstanceDoungeonEnter.h"

CGInstanceDoungeonEnter::CGInstanceDoungeonEnter () 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

CGInstanceDoungeonEnter::~CGInstanceDoungeonEnter () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

void CGInstanceDoungeonEnter::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_SelectDoungeonIndex);

	__END_CATCH
}
		    
void CGInstanceDoungeonEnter::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_SelectDoungeonIndex);

	__END_CATCH
}

void CGInstanceDoungeonEnter::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGInstanceDoungeonEnterHandler::execute (this , pPlayer);
		
	__END_CATCH
}

string CGInstanceDoungeonEnter::toString () const
    throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGInstanceDoungeonEnter("
		<< "ObjectID:" << (int)m_ObjectID 
		<< "SelectDoungeonIndex:" << (int)m_SelectDoungeonIndex 
		<< ")";
	return msg.toString();

	__END_CATCH
}
