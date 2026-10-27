//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCGlobalChat.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCGlobalChat.h"


//////////////////////////////////////////////////////////////////////
// ??¨ö????(????)???¥ê??? ??????? ?¬à? ????? ???????.
//////////////////////////////////////////////////////////////////////
void GCGlobalChat::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_Color );

	BYTE szMessage;

	iStream.read( szMessage );

	if ( szMessage == 0 )
		throw InvalidProtocolException("szMessage == 0");

	if ( szMessage > 255 )
		throw InvalidProtocolException("too large message length");

	iStream.read( m_Message , szMessage );

	iStream.read( m_Race );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// ??¨ö????(????)???? ????? ?????? ??????? ??????.
//////////////////////////////////////////////////////////////////////
void GCGlobalChat::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_Color );

	if ( m_Message.size() == 0 )
		throw InvalidProtocolException("szMessage == 0");

	if ( m_Message.size() > 255 )
		throw InvalidProtocolException("too large message length");

	BYTE szMessage = (BYTE)m_Message.size();

	oStream.write( szMessage );

	oStream.write( m_Message );

	oStream.write( m_Race );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCGlobalChat::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCGlobalChatHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCGlobalChat::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCGlobalChat("
		<< "Color:" << m_Color
		<< ",Message:" << m_Message 
		<< ",Race:" << (int)m_Race
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
