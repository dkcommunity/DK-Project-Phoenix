//----------------------------------------------------------------------
//
// Filename    : SubOustersSkillInfo.cpp
// Written By  :
// Description :
//
//----------------------------------------------------------------------

#include "Client_PCH.h"
// include files
#include "SubOustersSkillInfo.h"

//----------------------------------------------------------------------
// read data from socket input stream
//----------------------------------------------------------------------
void SubOustersSkillInfo::read ( SocketInputStream & iStream ) 
	
{
	__BEGIN_TRY
	try {

	iStream.read( m_SkillType );
	iStream.read( m_ExpLevel );
	iStream.read( m_Interval );
	iStream.read( m_CastingTime );

	} catch( Throwable & t ) {
		std::cout << t.toString().c_str() << std::endl;
	}

	__END_CATCH
}

//----------------------------------------------------------------------
// write data to socket output stream
//----------------------------------------------------------------------
void SubOustersSkillInfo::write ( SocketOutputStream & oStream ) const 
	
{
	__BEGIN_TRY

	try {

	oStream.write( m_SkillType );
	oStream.write( m_ExpLevel );
	oStream.write( m_Interval );
	oStream.write( m_CastingTime );

	} catch (Throwable & t ) {
		std::cout << t.toString().c_str() << std::endl;
	}

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//----------------------------------------------------------------------
// get debug std::string
//----------------------------------------------------------------------
std::string SubOustersSkillInfo::toString () const 
	
{
	StringStream msg;

	msg << "SubOustersSkillInfo("
		<< ",SkillType:" << (int)m_SkillType
		<< ",Interval:" << (int)m_Interval
		<< ",CastingTime:" << (int)m_CastingTime
		<< ")";
	
	return msg.toString();
}
#endif
