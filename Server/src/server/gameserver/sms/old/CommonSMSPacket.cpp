//////////////////////////////////////////////////////////////////////////////
// Filename    : CommonSMSPacket.cpp 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CommonSMSPacket.h"
#include "Assert.h"
#include "Properties.h"
#include <netinet/in.h>
#include "DB.h"
#include "VSDateTime.h"

void CommonSMSPacket::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	int len = getPacketSize();
	SMSInfo* pSMSInfo = this;

	memset((char*)pSMSInfo, 0, sizeof(SMSInfo));
	iStream.read((char*)pSMSInfo, len);

#ifdef __COUT_SMS_SYSTEM__
	cout << "[read] " << toString().c_str() << endl;
#endif

	__END_CATCH
}

void CommonSMSPacket::write (SocketOutputStream & oStream) const
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
#ifdef __COUT_SMS_SYSTEM__
	cout << "[write] " << toString().c_str() << endl;
#endif

	int len = getPacketSize();

	const SMSInfo* pSMSInfo = this;
	oStream.write((const char*)pSMSInfo, len);

	__END_CATCH
}

void CommonSMSPacket::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	CommonSMSPacketHandler::execute(this , pPlayer);

	__END_CATCH
}


string CommonSMSPacket::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;

	return msg.toString();

	__END_CATCH
}
