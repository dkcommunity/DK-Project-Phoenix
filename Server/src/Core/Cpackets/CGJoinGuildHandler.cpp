////////////////////////////////////////////////////////////////////////////// // Filename    : CGJoinGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGJoinGuild.h"

#ifdef __GAME_SERVER__
	#include "SystemAvailabilitiesManager.h"
	#include "DB.h"
	#include "SharedServerManager.h"
	#include "GamePlayer.h"
	#include "Slayer.h"
	#include "Vampire.h"
	#include "Ousters.h"
	#include "GuildManager.h"
	#include "Guild.h"
	#include "Properties.h"
	#include "Gpackets/GSAddGuildMember.h"
	#include "Gpackets/GCNPCResponse.h"
	#include "VariableManager.h"

	#include <stdio.h>

#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGJoinGuildHandler::execute (CGJoinGuild* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	SYSTEM_ASSERT(SYSTEM_GUILD);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Creature* pCreature = pGamePlayer->getCreature();
	Assert( pCreature != NULL );

	Player* pPlayer = pCreature->getPlayer();
	Assert( pPlayer != NULL );
	
	Statement* pStmt;
	Result* pResult;

	//cout << pPacket->toString() << endl;

	BEGIN_DB
	{
		// �������� ������ ���ƴٸ� ���⼭ üũ�Ҷ� �ɸ��� �ȵȴ�.
		// �׷� ������ �����޽����� Ŭ���̾�Ʈ�� ������ �ʴ´�.
		// �ٸ� ��� �Ҽ����� üũ
		pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
		pResult = pStmt->executeQuery( "SELECT GuildID, `Rank`, ExpireDate FROM GuildMember WHERE Name = '%s'", pCreature->getName().c_str() );

		if ( pResult->next() )
		{
			
			GuildID_t   GuildID    = pResult->getInt(1);
			int    		Rank       = pResult->getInt(2);
			string 		ExpireDate = pResult->getString(3);

			if ( ExpireDate.size() == 7 )
			{
				time_t daytime = time(0);

				tm Time;

				Time.tm_year = atoi( ExpireDate.substr(0,3).c_str() );
				Time.tm_mon = atoi( ExpireDate.substr(3,2).c_str() );
				Time.tm_mday = atoi( ExpireDate.substr(5,2).c_str() );
				Time.tm_hour = 0;
				Time.tm_min = 0;
				Time.tm_sec = 0;

//				if ( difftime( daytime, mktime(&Time) ) < 604800 )	// �ǽð� 7���� �����°�?
				if ( difftime( daytime, mktime(&Time) ) < g_pVariableManager->getVariable(QUIT_GUILD_PENALTY_TERM)*24*3600 )	// �ǽð� 7���� �����°�?
				{
/*					if (Rank==GuildMember::GUILDMEMBER_RANK_DENY
						&& GuildID != pPacket->getGuildID())
					{
						// rank4==�߹�/�ź�..�� �ֵ��� �ٸ� ��忡�� �� �� �ִ�.
						// ������ �ִ� GuildMember���� �����Ѵ�.
						pStmt->executeQuery( "DELETE FROM GuildMember WHERE Name = '%s'",
												pCreature->getName().c_str() );
					}
					else
					{
						SAFE_DELETE( pStmt );

						return;
					}*/

					// �ǽð� 7���� ������ ������ ������ �� ����. ������
					// 2003. 6. 25 by bezz
					SAFE_DELETE( pStmt );

					return;
				}
			}
			else
			{
				// �̹� �ٸ� ��忡 �ҼӵǾ� ����
				SAFE_DELETE( pStmt );

				return;
			}
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt)


	// ��Ÿ�� ����� ������ ���
	if ( pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER )
	{
		// ��尡 �̹� ���� ���� ��ϵǾ����� Ȯ���Ѵ�.
		Guild* pGuild = g_pGuildManager->getGuild( pPacket->getGuildID() );
		if ( pGuild == NULL ) return;

		if ( pGuild->getState() != Guild::GUILD_STATE_WAIT )
			return;

		if ( pCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			Assert( pSlayer != NULL );

			SkillDomainType_t highest = pSlayer->getHighestSkillDomain();

			// ��� ���� ���� üũ
			if ( ( pSlayer->getGold() >= REQUIRE_SLAYER_SUBMASTER_GOLD ) &&				// ��Ϻ� 5õ��
				 ( pSlayer->getFame() >= REQUIRE_SLAYER_SUBMASTER_FAME[highest]) &&		// ����ġ
				 ( pSlayer->getSkillDomainLevel(highest) >= REQUIRE_SLAYER_SUBMASTER_SKILL_DOMAIN_LEVEL )	// ���� 40 �̻�
			   )
			{
				GSAddGuildMember gsAddGuildMember;

				gsAddGuildMember.setGuildID( pPacket->getGuildID() );
				gsAddGuildMember.setName( pSlayer->getName() );
				gsAddGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
				gsAddGuildMember.setGuildMemberIntro( pPacket->getGuildMemberIntro() );
				gsAddGuildMember.setServerGroupID( g_pConfig->getPropertyInt( "ServerID" ) );

				g_pSharedServerManager->sendPacket( &gsAddGuildMember );
			}

		}
		else if ( pCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			Assert( pVampire != NULL );

			// ��� ���� ���� üũ
			if ( ( pVampire->getGold() >= REQUIRE_VAMPIRE_SUBMASTER_GOLD ) &&		// ��Ϻ� 5õ��
				 ( pVampire->getLevel() >= REQUIRE_VAMPIRE_SUBMASTER_LEVEL )		// ���� 40 �̻�
			   )
			{
				GSAddGuildMember gsAddGuildMember;

				gsAddGuildMember.setGuildID( pPacket->getGuildID() );
				gsAddGuildMember.setName( pVampire->getName() );
				gsAddGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
				gsAddGuildMember.setGuildMemberIntro( pPacket->getGuildMemberIntro() );
				gsAddGuildMember.setServerGroupID( g_pConfig->getPropertyInt( "ServerID" ) );

				g_pSharedServerManager->sendPacket( &gsAddGuildMember );

			}
		}
		else if ( pCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			Assert( pOusters != NULL );

			// ��� ���� ���� üũ
			if ( ( pOusters->getGold() >= REQUIRE_OUSTERS_SUBMASTER_GOLD ) &&		// ��Ϻ� 5õ��
				 ( pOusters->getLevel() >= REQUIRE_OUSTERS_SUBMASTER_LEVEL )		// ���� 40 �̻�
			   )
			{
				GSAddGuildMember gsAddGuildMember;

				gsAddGuildMember.setGuildID( pPacket->getGuildID() );
				gsAddGuildMember.setName( pOusters->getName() );
				gsAddGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
				gsAddGuildMember.setGuildMemberIntro( pPacket->getGuildMemberIntro() );
				gsAddGuildMember.setServerGroupID( g_pConfig->getPropertyInt( "ServerID" ) );

				g_pSharedServerManager->sendPacket( &gsAddGuildMember );

			}
		}
	}
	else if ( pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_WAIT )
	{
		// ��� ���� ��û, ��� ����
		GSAddGuildMember gsAddGuildMember;

		gsAddGuildMember.setGuildID( pPacket->getGuildID() );
		gsAddGuildMember.setName( pCreature->getName() );
		gsAddGuildMember.setGuildMemberRank( pPacket->getGuildMemberRank() );
		gsAddGuildMember.setGuildMemberIntro( pPacket->getGuildMemberIntro() );
		gsAddGuildMember.setServerGroupID( g_pConfig->getPropertyInt( "ServerID" ) );

		g_pSharedServerManager->sendPacket( &gsAddGuildMember );
	}

	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer->sendPacket( &response );

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

