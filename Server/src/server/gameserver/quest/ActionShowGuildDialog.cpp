////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionShowGuildDialog.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionShowGuildDialog.h"
#include "DB.h"
#include "Creature.h"
#include "NPC.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildInfo.h"
#include "SharedServerManager.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCWaitGuildList.h"
#include "Gpackets/GCActiveGuildList.h"
#include "Gpackets/GSQuitGuild.h"

#include "SystemAvailabilitiesManager.h"

////////////////////////////////////////////////////////////////////////////////
// read from property buffer
////////////////////////////////////////////////////////////////////////////////
void ActionShowGuildDialog::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// ���̾�α��� ����
		m_Type = (GuildDialog_t)propertyBuffer.getPropertyInt("Type");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// �׼��� �����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
void ActionShowGuildDialog::execute (Creature * pCreature1, Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	SYSTEM_RETURN_IF_NOT(SYSTEM_GUILD);

	Player* pPlayer = pCreature2->getPlayer();
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Creature* pCreature = pGamePlayer->getCreature();
	Assert( pCreature != NULL );

	if ( m_Type == GUILD_DIALOG_REGIST )
	{
		//cout << "��� ���" << endl;
		////////////////////////////////////////////////////////////////////////////////
		// ��� ����� �������� ���
		////////////////////////////////////////////////////////////////////////////////
		Statement* pStmt = NULL;
		Result* pResult = NULL;

		BEGIN_DB
		{
			// �ٸ� ��� �Ҽ����� üũ
			pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
			pResult = pStmt->executeQuery( "SELECT `Rank`, ExpireDate FROM GuildMember WHERE Name = '%s'", pCreature->getName().c_str() );

			if ( pResult->next() )
			{
				// ��� ��� ������ �ִ�. expire date�� ���� �����Ѵ�.
				int  	Rank 		= pResult->getInt(1);
				string 	ExpireDate 	= pResult->getString(2);

				if ( ExpireDate.size() == 7 )
				{
					// �ٸ� ��忡�� Ż���� ��쿡�� ������ ���� ��带 ���� �� ����.
					if (Rank==GuildMember::GUILDMEMBER_RANK_LEAVE)
					{
						// ����� ��� �Ҽ� ���°� �ƴϴ�. ������ expire date ���� 7���� ������ �Ѵ�.
						time_t daytime = time(0);
						tm Time;
						Time.tm_year = atoi( ExpireDate.substr(0,3).c_str() );
						Time.tm_mon = atoi( ExpireDate.substr(3,2).c_str() );
						Time.tm_mday = atoi( ExpireDate.substr(5,2).c_str() );

						if ( difftime( daytime, mktime(&Time) ) < 604800 ) // �ǽð� 7���� �����°�?
						{

							SAFE_DELETE( pStmt );

							// �ð��� ������ ...��¼��
							if ( pCreature->isSlayer() )
							{
								GCNPCResponse response;
								response.setCode( NPC_RESPONSE_TEAM_REGIST_FAIL_QUIT_TIMEOUT );
								pPlayer->sendPacket( &response );
							}
							else if ( pCreature->isVampire() )
							{
								GCNPCResponse response;
								response.setCode( NPC_RESPONSE_CLAN_REGIST_FAIL_QUIT_TIMEOUT );
								pPlayer->sendPacket( &response );
							}
							else if ( pCreature->isOusters() )
							{
								GCNPCResponse response;
								response.setCode( NPC_RESPONSE_GUILD_REGIST_FAIL_QUIT_TIMEOUT );
								pPlayer->sendPacket( &response );
							}

							return;
						}
					}
				}
				else
				{
					// �ٸ� ��忡 ���ԵǾ��ִ� ���
					if ( pCreature->isSlayer() )
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_TEAM_REGIST_FAIL_ALREADY_JOIN );
						pPlayer->sendPacket( &response );
					}
					else if ( pCreature->isVampire() )
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_CLAN_REGIST_FAIL_ALREADY_JOIN );
						pPlayer->sendPacket( &response );
					}
					else if ( pCreature->isOusters() )
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_GUILD_REGIST_FAIL_ALREADY_JOIN );
						pPlayer->sendPacket( &response );
					}

					SAFE_DELETE( pStmt );
					return;
				}
			}

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)

		if ( pCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			Assert( pSlayer != NULL );

			SkillDomainType_t highest = pSlayer->getHighestSkillDomain();

			if ( pSlayer->getSkillDomainLevel(highest) < REQUIRE_SLAYER_MASTER_SKILL_DOMAIN_LEVEL )	// ���� 50 �̻�
			{
				// ������ ����
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_TEAM_REGIST_FAIL_LEVEL );
				pPlayer->sendPacket( &response );

				return;
			}
			if ( pSlayer->getGold() < REQUIRE_SLAYER_MASTER_GOLD )				// ��Ϻ� 1��
			{
				// ��Ϻ� ���ڶ�
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_TEAM_REGIST_FAIL_MONEY );
				pPlayer->sendPacket( &response );

				return;
			}
			if ( pSlayer->getFame() < REQUIRE_SLAYER_MASTER_FAME[highest] )		// ����
			{
				// ������ ���ڶ�
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_TEAM_REGIST_FAIL_FAME );
				pPlayer->sendPacket( &response );

				return;
			}
			// ��� ��� â�� ��쵵�� �޽����� ������.
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_GUILD_SHOW_REGIST );
			response.setParameter( REQUIRE_SLAYER_MASTER_GOLD );
			pPlayer->sendPacket( &response );
		}
		else if ( pCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			Assert( pVampire != NULL );

			// ��� ���� ���� üũ
			if ( pVampire->getLevel() < REQUIRE_VAMPIRE_MASTER_LEVEL )				// ���� 50�̻�
			{
				// ������ ����
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_CLAN_REGIST_FAIL_LEVEL );
				pPlayer->sendPacket( &response );

				return;
			}
			if ( pVampire->getGold() < REQUIRE_VAMPIRE_MASTER_GOLD )			// ��Ϻ� 1��
			{
				// ��Ϻ� ���ڶ�
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_CLAN_REGIST_FAIL_MONEY );
				pPlayer->sendPacket( &response );

				return;
			}
//			if ( pVampire->getFame() < 500000 )				// ���� 50��
//			{
//				// ������ ���ڶ�
//				GCNPCResponse response;
//				response.setCode( NPC_RESPONSE_CLAN_REGIST_FAIL_FAME );
//				pPlayer->sendPacket( &response );
//
//				return;
//			}
			// ��� ��� â�� ��쵵�� �޽����� ������.
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_GUILD_SHOW_REGIST );

			//cout << "��尡�Ժ� : " << REQUIRE_VAMPIRE_MASTER_GOLD << endl;
			response.setParameter( REQUIRE_VAMPIRE_MASTER_GOLD );
			//cout << "��尡�Ժ�(param) : " << response.getParameter() << endl;
			pPlayer->sendPacket( &response );
		}
		else if ( pCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			Assert( pOusters != NULL );

			// ��� ���� ���� üũ
			if ( pOusters->getLevel() < REQUIRE_OUSTERS_MASTER_LEVEL )				// ���� 50�̻�
			{
				// ������ ����
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_GUILD_REGIST_FAIL_LEVEL );
				pPlayer->sendPacket( &response );

				return;
			}
			if ( pOusters->getGold() < REQUIRE_OUSTERS_MASTER_GOLD )			// ��Ϻ� 1��
			{
				// ��Ϻ� ���ڶ�
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_GUILD_REGIST_FAIL_MONEY );
				pPlayer->sendPacket( &response );

				return;
			}

			// ��� ��� â�� ��쵵�� �޽����� ������.
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_GUILD_SHOW_REGIST );

			//cout << "��尡�Ժ� : " << REQUIRE_VAMPIRE_MASTER_GOLD << endl;
			response.setParameter( REQUIRE_OUSTERS_MASTER_GOLD );
			//cout << "��尡�Ժ�(param) : " << response.getParameter() << endl;
			pPlayer->sendPacket( &response );
		}
	}
	else if ( m_Type == GUILD_DIALOG_WAIT_LIST )
	{
		//cout << "��� ��� ����Ʈ" << endl;
		GCWaitGuildList gcWaitGuildList;

//		const HashMapGuild& Guilds = g_pGuildManager->getGuilds_const();
//		HashMapGuildConstItor itr = Guilds.begin();

		GuildRace_t race;
		if ( pCreature->isSlayer() )
			race = Guild::GUILD_RACE_SLAYER;
		else if ( pCreature->isVampire() )
			race = Guild::GUILD_RACE_VAMPIRE;
		else if ( pCreature->isOusters() )
			race = Guild::GUILD_RACE_OUSTERS;
		else
			return;

		g_pGuildManager->makeWaitGuildList( gcWaitGuildList, race );

		pPlayer->sendPacket( &gcWaitGuildList );
	}
	else if ( m_Type == GUILD_DIALOG_LIST )
	{
		//cout << "��� ����Ʈ" << endl;
		GCActiveGuildList gcActiveGuildList;

		GuildRace_t race;
		if ( pCreature->isSlayer() )
			race = Guild::GUILD_RACE_SLAYER;
		else if ( pCreature->isVampire() )
			race = Guild::GUILD_RACE_VAMPIRE;
		else if ( pCreature->isOusters() )
			race = Guild::GUILD_RACE_OUSTERS;
		else
			return;

		g_pGuildManager->makeActiveGuildList( gcActiveGuildList, race );

		pPlayer->sendPacket( &gcActiveGuildList );
	}
	else if ( m_Type == GUILD_DIALOG_QUIT )
	{
		// ��� Ż�� â�� ����� �ʰ� ��� Ż�� Ȯ���� ������ �����Ѵ�.
		Guild* pGuild = NULL;

		Statement* pStmt = NULL;
		Result* pResult = NULL;
		
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
			pResult = pStmt->executeQuery( "SELECT GuildID FROM GuildMember WHERE Name = '%s'", pCreature->getName().c_str() );

			if ( pResult->next() )
			{
				pGuild = g_pGuildManager->getGuild( pResult->getInt(1) );
			}

			SAFE_DELETE( pStmt );
		}
		END_DB(pStmt)
				
		// ��� ���°� Ȱ�����̰ų� ������̾�� �Ѵ�.
		if ( pGuild == NULL || 
			 ( pGuild->getState() != Guild::GUILD_STATE_ACTIVE && pGuild->getState() != Guild::GUILD_STATE_WAIT ) )
		{ 
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
			pPlayer->sendPacket( &response );

			return; 
		}

		// �÷��̾ ����� ������� Ȯ���Ѵ�.
		GuildMember* pGuildMember = pGuild->getMember( pCreature->getName() );
		if ( pGuildMember == NULL )
		{ 
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
			pPlayer->sendPacket( &response );

			return; 
		}

		// Ȱ�� ���� ����� �����Ͷ�� Ż�� �����Ѵ�.
		if ( pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER && pGuild->getState() == Guild::GUILD_STATE_ACTIVE )
		{
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
			pPlayer->sendPacket( &response );

			return;
		}

		GSQuitGuild gsQuitGuild;
		gsQuitGuild.setGuildID( pGuild->getID() );
		gsQuitGuild.setName( pCreature->getName() );

		g_pSharedServerManager->sendPacket( &gsQuitGuild );

		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionShowGuildDialog::toString () const
{
	__BEGIN_TRY

	string str = "ActionShowGuildDialog";

	return str;

	__END_CATCH
}
