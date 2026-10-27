//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddNewItemToZone.cc 
// Written By  : elca
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddNewItemToZone.h"
#include "DarkAssert.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCAddNewItemToZone::GCAddNewItemToZone() 
     
{
	__BEGIN_TRY
	__END_CATCH
}

	
//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCAddNewItemToZone::~GCAddNewItemToZone() {
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddNewItemToZone::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddNewItemToZoneHandler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__	
//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string GCAddNewItemToZone::toString () const
       
{
	__BEGIN_TRY

	StringStream msg;
#if __CONTENTS(__PCITEMINFO)
		msg << "GCAddNewItemToZone("
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",PCItemInfo: " << m_PCItemInfo.toString()
		<< ")" ;
#else //__PCITEMINFO
	msg << "GCAddNewItemToZone("
		<< "ObjectID:" << m_ObjectID
		<< ",X:" << (int)m_X 
		<< ",Y:" << (int)m_Y 
		<< ",ItemClass: " << (int)m_ItemClass
		<< ",ItemType: " << (int)m_ItemType
		<< ",OptionTypeSize: " << (int)m_OptionType.size()
		<< ",Durability: " << (int)m_Durability
		<< ",ItemNum: " << (int)m_ItemNum
		<< ")" ;
#endif//__PCITEMINFO
	return msg.toString();

	__END_CATCH
}
#endif
