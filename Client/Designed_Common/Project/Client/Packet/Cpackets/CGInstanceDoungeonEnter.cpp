//////////////////////////////////////////////////////////////////////////////
// Filename    : CGInstanceDoungeonEnter.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGInstanceDoungeonEnter.h"
#if __CONTENTS(__TIPOJYU_CASTLE)
CGInstanceDoungeonEnter::CGInstanceDoungeonEnter () 
     
{
	__BEGIN_TRY
	__END_CATCH
}

CGInstanceDoungeonEnter::~CGInstanceDoungeonEnter() {
	__BEGIN_TRY
	__END_CATCH
}

void CGInstanceDoungeonEnter::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_SelectDoungeonIndex);

	__END_CATCH
}
		    
void CGInstanceDoungeonEnter::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_SelectDoungeonIndex);

	__END_CATCH
}

void CGInstanceDoungeonEnter::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

	CGInstanceDoungeonEnterHandler::execute (this , pPlayer);
		
	__END_CATCH
}

std::string CGInstanceDoungeonEnter::toString () const
    
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
#endif // __TIPOJYU_CASTLE
