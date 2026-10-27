//////////////////////////////////////////////////////////////////////////////
// Filename    : GCRemoveInjuriousCreature.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCRemoveInjuriousCreature.h"

void GCRemoveInjuriousCreature::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
	int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
	localLongName = 22;
#endif //__LOCALIZING_LONGNAME		
	// 이름 읽기
	BYTE szName;

	iStream.read(szName);

	if (szName == 0)
		throw InvalidProtocolException("szName == 0");

	if (szName > 100 + localLongName)
		throw InvalidProtocolException("too large name length");

	iStream.read(m_Name , szName);

	__END_CATCH
}
		    
void GCRemoveInjuriousCreature::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY
	int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
	localLongName = 22;
#endif //__LOCALIZING_LONGNAME		
	// 이름 쓰기
	BYTE szName = m_Name.size();

	if (szName == 0)
		throw InvalidProtocolException("szName == 0");

	if (szName > 100 + localLongName)
		throw InvalidProtocolException("too large name length");

	oStream.write(szName);

	oStream.write(m_Name);

	__END_CATCH
}

void GCRemoveInjuriousCreature::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
	GCRemoveInjuriousCreatureHandler::execute(this , pPlayer);

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
	std::string GCRemoveInjuriousCreature::toString () const
		   
	{
		__BEGIN_TRY
			
		StringStream msg;
		msg << "GCRemoveInjuriousCreature(Name :" << m_Name 
			<< ")" ;
		return msg.toString();

		__END_CATCH
	}
#endif
