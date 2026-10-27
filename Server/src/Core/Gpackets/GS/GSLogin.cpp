//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSLogin.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSLogin.h"


GSLogin::GSLogin()
{
	m_Sender.SetMaxLength(20);
	m_ServerName.SetMaxLength(20);
}

GSLogin::~GSLogin() throw()
{
	
}

void GSLogin::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Sender.read(iStream);
	m_ServerName.read(iStream);
	
	__END_CATCH
}

void GSLogin::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Sender.write(oStream);
	m_ServerName.write(oStream);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSLogin::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSLoginHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSLogin::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSLogin ("
		<< "Sender:" << GetSender().c_str() << ","
		<< "ServerName:" << GetServerName().c_str()
		<< " )";

	return msg.toString();
}

