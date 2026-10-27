//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMixItem.cpp 
// Written By  : excel96
// Description : 
// �κ��丮 ���� �������� ����� ��, Ŭ���̾�Ʈ�� X, Y �� ObjectID��
// ������ ������ Ŭ������ ����, ������ �̿� �´� �ڵ带 ó���Ѵ�.
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGMixItem.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "Assert.h"


void CGMixItem::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_InvenX);
	iStream.read(m_InvenY);
	iStream.read(m_TargetObjectID[0]);
	iStream.read(m_TargetObjectID[1]);

	__END_CATCH
}
		    
void CGMixItem::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_InvenX);
	oStream.write(m_InvenY);
	oStream.write(m_TargetObjectID[0]);
	oStream.write(m_TargetObjectID[1]);

	__END_CATCH
}

void CGMixItem::execute (Player* pPlayer)
	 throw (ProtocolException , Error)
{
#ifndef __GAME_CLIENT__
	// Phoenix: Handler server-side; CGMixItemHandler.cpp tamamen
	// #ifndef __GAME_CLIENT__ bloğunda olduğu için Client tarafında
	// çağrılması linker hatası veriyor. Sarmalıyoruz.
	__BEGIN_TRY

	CGMixItemHandler::execute (this , pPlayer);

	__END_CATCH
#endif
}
#ifdef __DEBUG_OUTPUT__
std::string CGMixItem::toString () 
	const throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGMixItem("
		<< "ObjectID:" << (int)m_ObjectID 
		<< ",InvenX:" << (int)m_InvenX
		<< ",InvenY:" << (int)m_InvenY
		<< ",ObjectID:" << (int)m_TargetObjectID[0]
		<< ",ObjectID:" << (int)m_TargetObjectID[1]
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif