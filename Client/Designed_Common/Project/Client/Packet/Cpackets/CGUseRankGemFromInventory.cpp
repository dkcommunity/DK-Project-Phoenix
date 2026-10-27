//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseRankGemFromInventory.cpp 
// Written By  : excel96
// Description : 
// 牢亥配府 救狼 酒捞袍阑 荤侩且 锭, 努扼捞攫飘啊 X, Y 棺 ObjectID甫
// 焊郴搁 酒捞袍 努贰胶俊 蝶扼辑, 辑滚啊 捞俊 嘎绰 内靛甫 贸府茄促.
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGUseRankGemFromInventory.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "DarkAssert.h"


void CGUseRankGemFromInventory::read (SocketInputStream & iStream) 

{
	__BEGIN_TRY

	iStream.read(m_ObjectID);
	iStream.read(m_SlotID);

	__END_CATCH
}

void CGUseRankGemFromInventory::write (SocketOutputStream & oStream) const 

{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_SlotID);

	__END_CATCH
}

void CGUseRankGemFromInventory::execute (Player* pPlayer) 

{
	__BEGIN_TRY

		CGUseRankGemFromInventoryHandler::execute (this , pPlayer);

	__END_CATCH
}

std::string CGUseRankGemFromInventory::toString () 
const 
{
	__BEGIN_TRY

		StringStream msg;
	msg << "CGUseRankGemFromInventory("
		<< "ObjectID:" << (int)m_ObjectID 
		<< ",SlotID:" << (int)m_SlotID
		<< ")";
	return msg.toString();

	__END_CATCH
}

