//////////////////////////////////////////////////////////////////////////////
// Filename    : GCOTHERGEARINFO.cpp 
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCOTHERGEARINFO.h"


GCOTHERGEARINFO::GCOTHERGEARINFO ()
	throw ()
: m_ObjectID(0), m_pGearInfo(NULL)
{
}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCOTHERGEARINFO::~GCOTHERGEARINFO ()
	throw ()
{
	SAFE_DELETE(m_pGearInfo);
}

void GCOTHERGEARINFO::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );
	m_pGearInfo = new GearInfo();
	m_pGearInfo->read( iStream );

	__END_CATCH
}
		    
void GCOTHERGEARINFO::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_ObjectID );
	m_pGearInfo->write( oStream );
	
	__END_CATCH
}

void GCOTHERGEARINFO::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCOTHERGEARINFOHandler::execute( this , pPlayer );
		
	__END_CATCH
}

string GCOTHERGEARINFO::toString () const 
	throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCOTHERGEARINFO("
		<< "ObjectID::" << (int)m_ObjectID << ","
		<< ")";
	return msg.toString();

	__END_CATCH
}
