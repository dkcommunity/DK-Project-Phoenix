//////////////////////////////////////////////////////////////////////////////
// Filename    : CGGlobalChatHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGGlobalChat.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "IncomingPlayerManager.h"
	#include "Creature.h"
	#include "Zone.h"
	#include "Gpackets/GCGlobalChat.h"
	#include "LogNameManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// �Ӹ����� GlobalChat ��Ʈ���� ��ﶧ ����ϴ� ��Ŷ�̴�.
// �� ũ��ó�� �� �� �ִ� ��� �÷��̾�� ��ε�ĳ��Ʈ�Ѵ�.
// �ϴ��� ���� ���� ��� �÷��̾�(�Ǵ� ��ü �÷��̾�)���� 
// GCGlobalChat ��Ŷ�� ��ε�ĳ��Ʈ�Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void CGGlobalChatHandler::execute (CGGlobalChat* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	if (pGamePlayer->getPlayerStatus() == GPS_NORMAL) {

		if (pGamePlayer->isPenaltyFlag(PENALTY_TYPE_MUTE)) {
			return;
		}
		Creature* pCreature = pGamePlayer->getCreature();

		Assert(pCreature != NULL);

		// �������� Ŭ���̾�Ʈ�� �����ϹǷ� GC- ��Ŷ�� ����ؾ� �Ѵ�.
		GCGlobalChat gcGlobalChat;

		// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
		size_t i = pPacket->getMessage().find_first_of('*' , 0);

		if (i == 0) return;

		// text color setting
		gcGlobalChat.setColor( pPacket->getColor() );

		// ũ��ó �̸��� �޽����� ��Ŷ�� �����Ѵ�.
		StringStream msg;
		msg << pCreature->getName() << " " << pPacket->getMessage();
	
		gcGlobalChat.setMessage(msg.toString());
		gcGlobalChat.setRace(pCreature->getRace());
	
		// �ֺ� PC�鿡�� ��ε�ĳ��Ʈ�Ѵ�.
		pCreature->getZone()->broadcastPacket(&gcGlobalChat , pCreature);


		// ä�� �α׸� �����. by sigi. 2002.10.30
		if (LogNameManager::getInstance().isExist( pCreature->getName() ))
		{
			filelog("chatLog.txt", "[Global] %s> %s", pCreature->getName().c_str(), pPacket->getMessage().c_str());
		}
	}
	
#endif
		
	__END_DEBUG_EX __END_CATCH
}
