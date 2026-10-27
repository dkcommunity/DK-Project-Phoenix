//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGQuitGuild.h"

#ifdef __GAME_SERVER__
	#include "SystemAvailabilitiesManager.h"
	#include "Gpackets/GSQuitGuild.h"
	#include "Guild.h"
	#include "GuildManager.h"
	#include "Player.h"
	#include "PlayerCreature.h"
	#include "SharedServerManager.h"
	#include "StringPool.h"
	#include "Gpackets/GCSystemMessage.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGQuitGuildHandler::execute (CGQuitGuild* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	SYSTEM_ASSERT(SYSTEM_GUILD);

	PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pPlayer);
	Assert( pPlayerCreature != NULL );

	// 플레이어가 속한 길드를 가져온다.
	Guild* pGuild = g_pGuildManager->getGuild( pPacket->getGuildID() );
	try { Assert( pGuild != NULL ); } catch ( Throwable& t ) { return; }

	// 플레이어가 길드의 멤버인지 확인한다.
	GuildMember* pGuildMember = pGuild->getMember( pPlayerCreature->getName() );
	if ( pGuildMember == NULL )
		return;

	// 길드가 전쟁 신청 중이면 탈퇴할 수 없다.
	if ( g_pGuildManager->hasWarSchedule( pGuild->getID() ) )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CANNOT_LEAVE_DURING_JOIN_WAR) ); // 20070814 전쟁신청중에는 탈퇴 할 수 없다.
		pPlayer->sendPacket( &gcSystemMessage );

		return;
	}

	GSQuitGuild gsQuitGuild;
	gsQuitGuild.setGuildID( pGuild->getID() );
	gsQuitGuild.setName( pPlayerCreature->getName() );

	g_pSharedServerManager->sendPacket( &gsQuitGuild );

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

