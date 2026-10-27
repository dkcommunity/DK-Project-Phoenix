////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionChangePlayerColor.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionChangePlayerColor.h"
#include "NPC.h"
#include "Zone.h"
#include "ZonePlayerManager.h"
#include "PriceManager.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "ItemUtil.h"
#include "ItemFactoryManager.h"
#include "PacketUtil.h"
#include "CastleInfoManager.h"
#include "DB.h"
#include "Effect.h"
#include "EffectManager.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCAddSlayer.h"
#include "Gpackets/GCAddVampire.h"
#include "Gpackets/GCAddOusters.h"
#include "NPCInfo.h"
#include "ZoneUtil.h"
#include "GuildManager.h"
#include "Tile.h"
#include "LogClient.h"
#include "Utility.h"
#include "GQuestManager.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionChangePlayerColor::read (PropertyBuffer & propertyBuffer)
{
	__BEGIN_TRY

	try
	{
		m_Flag		= propertyBuffer.getPropertyInt("Flag");
		m_ColorValue= propertyBuffer.getPropertyInt("Value");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionChangePlayerColor::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	try {
		Assert( pCreature1 != NULL );
		Assert( pCreature2 != NULL );
		Assert( pCreature1->isNPC() );
		Assert( pCreature2->isPC() );

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
		Assert( pPC != NULL );

		Player* pPlayer = pPC->getPlayer();
		Assert( pPlayer != NULL );

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		Assert( pGamePlayer != NULL );

		NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
		Assert( pNPC != NULL );

		bool	bSlayer = pPC->isSlayer();
		bool	bVampire = pPC->isVampire();
		bool	bOusters = pPC->isOusters();

		Gold_t		PlayerMoney = pPC->getGold();
		Price_t		ItemPrice = 10000000;
		Gold_t		itemTax = 0;

		if( pNPC->getTaxingCastleZoneID() != 0 )
		{
			int itemTaxRatio = pNPC->getTaxRatio( pPC );
			if( itemTaxRatio > 100 )
			{
				int NewItemPrice = int(ItemPrice * (itemTaxRatio/100.0)) ;

				// 이제 30%는 서버가 데먹기로 했음. by bezz. 2006.08.23
				itemTax = (int)((NewItemPrice - ItemPrice) * 0.7);
				ItemPrice = NewItemPrice;
			}
		}
		if( PlayerMoney < ItemPrice )
		{
			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode(NPC_RESPONSE_NOT_ENOUGH_MONEY_CAHNGE_COLOR);
			pPlayer->sendPacket(&gcNPCResponse);
			return;
		}
		if( bSlayer )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			Assert( pSlayer != NULL );
			char query[25];

			switch( m_Flag )
			{
				case 0:
					if( pSlayer->getHairColor() == m_ColorValue ) 
					{
						GCNPCResponse gcNPCResponse;
						gcNPCResponse.setCode(NPC_RESPONSE_NOT_CAHNGE_COLOR);
						pPlayer->sendPacket(&gcNPCResponse);
						return;
					}
					pSlayer->setHairColor( m_ColorValue );

					sprintf( query, "HairColor=%u", m_ColorValue );
					pSlayer->tinysave( query );
					break;
				case 1:
					if( pSlayer->getSkinColor() == m_ColorValue )
					{
						GCNPCResponse gcNPCResponse;
						gcNPCResponse.setCode(NPC_RESPONSE_NOT_CAHNGE_COLOR);
						pPlayer->sendPacket(&gcNPCResponse);
						return;
					}
					pSlayer->setSkinColor( m_ColorValue );

					sprintf( query, "SkinColor=%u", m_ColorValue );
					pSlayer->tinysave( query );
					break;
				default:
				{
					return;
				}
			}
		}
		else if( bVampire )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );
			char query[25];
			switch( m_Flag )
			{
				case 1:
					if( pVampire->getSkinColor() == m_ColorValue )
					{
						GCNPCResponse gcNPCResponse;
						gcNPCResponse.setCode(NPC_RESPONSE_NOT_CAHNGE_COLOR);
						pPlayer->sendPacket(&gcNPCResponse);
						return;
					}
					pVampire->setSkinColor( m_ColorValue );

					sprintf( query, "SkinColor=%u", m_ColorValue );
					pVampire->tinysave( query );
					break;
				case 0:
				default:
				{
					return;
				}
			}
		}
		else if( bOusters )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			Assert( pOusters != NULL );
			char query[25];

			switch( m_Flag )
			{
				case 0:
					if( pOusters->getHairColor() == m_ColorValue )
					{
						GCNPCResponse gcNPCResponse;
						gcNPCResponse.setCode(NPC_RESPONSE_NOT_CAHNGE_COLOR);
						pPlayer->sendPacket(&gcNPCResponse);
						return;
					}

					pOusters->setHairColor( m_ColorValue );
					
					sprintf( query, "HairColor=%u", m_ColorValue);
					pOusters->tinysave( query );
					break;
				case 1:
				default:
				{
					return;
				}
			}
		}
		else
			return;
		// 사용한 시스템(아이템)의 가격을 빼준다.
		pPC->decreaseGoldEx(ItemPrice);
		filelog("Tax.log", "%s 가 %s에게 %u 만큼을 세금으로 냈습니다.", pPC->getName().c_str(), pNPC->getName().c_str(), itemTax);
		// 성 세율을 보내준다.
		g_pCastleInfoManager->increaseTaxBalance( pNPC->getTaxingCastleZoneID(), itemTax );
		
		// 염색이 완료 된걸 알려준다.
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_CHANGE_COLOR_OK );
		pPlayer->sendPacket( &response );

		GCNPCResponse quit;
		quit.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &quit );

		Zone* pZone = pCreature2->getZone();

		pPC->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
		transportCreature( pPC, pPC->getZoneID(), pPC->getX(), pPC->getY(), false );

		if( bSlayer )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			Assert( pSlayer != NULL );


			GCAddSlayer gcAddSlayer;
			makeGCAddSlayer( &gcAddSlayer, pSlayer );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddSlayer, pPC );
		}
		else if( bVampire )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
			Assert( pVampire != NULL );
			
			GCAddVampire gcAddVampire;
			makeGCAddVampire( &gcAddVampire, pVampire );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddVampire, pPC );
		}
		else if( bOusters )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
			Assert( pOusters != NULL );

			GCAddOusters gcAddOusters;
			makeGCAddOusters( &gcAddOusters, pOusters );
			pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcAddOusters, pPC );

		}
		else
		{
			Assert( false );
		}

	} catch (Throwable &t) {
		cout << t.toString() << endl;
		throw; // -_-;
	}
	
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionChangePlayerColor::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionChangePlayerColor("
		<< "Flag:" << (int)m_Flag
		<< ",ColorValue:"     << (int)m_ColorValue
		<< ")";
	return msg.toString();

	__END_CATCH
}
