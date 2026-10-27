//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddInstalledMineToZone.cc 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
// include files
#include "GCAddInstalledMineToZone.h"
#include "DarkAssert.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCAddInstalledMineToZone::GCAddInstalledMineToZone() 
     
{
	__BEGIN_TRY
	__END_CATCH
}

	
//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCAddInstalledMineToZone::~GCAddInstalledMineToZone() {
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddInstalledMineToZone::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddInstalledMineToZoneHandler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__	
//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string GCAddInstalledMineToZone::toString () const
       
{
	__BEGIN_TRY

	StringStream msg;
#if __CONTENTS(__PCITEMINFO)
	msg << "GCAddInstalledMineToZone("
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",PCItemInfo:" << m_PCItemInfo.toString()
		<< ")" ;
#else //__PCITEMINFO
	msg << "GCAddInstalledMineToZone("
		<< "ObjectID:" << m_ObjectID
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",ItemClass:" << (int)m_ItemClass
		<< ",ItemType:" << (int)m_ItemType
		<< ",OptionTypeSize:" << (int)m_OptionType.size()
		<< ",Durability:" << (int)m_Durability
		<< ",ItemNum:" << (int)m_ItemNum
		<< ")" ;
#endif //__PCITEMINFO
	return msg.toString();

	__END_CATCH
}
#endif
