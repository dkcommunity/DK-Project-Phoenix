//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddSlayerHandler.cc
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAddSlayer.h"
#include "ClientDef.h"
#include "AddonDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"
#include "MGameStringTable.h"

extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddSlayerHandler::execute ( GCAddSlayer * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
		
	}	
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		const PCSlayerInfo3 & si = pPacket->getSlayerInfo();
			
		DEBUG_ADD_FORMAT("[AddSlayer] id=%d, xy=(%d,%d), dir=%d", si.getObjectID(), si.getX(), si.getY(), si.getDir());				
		
		MCreature* pCreature = g_pZone->GetCreature(si.getObjectID());

		//--------------------------------------------------
		// 새로운 Creature이면 추가
		// 복장이 있는 Creature이다.
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			CT_Done("DIAG: GCAddSlayer pre new MCreatureWear, objID=%d", (int)si.getObjectID());
			MCreatureWear* pCreatureWear = new MCreatureWear;
			CT_Done("DIAG: GCAddSlayer post new MCreatureWear=%p", (void*)pCreatureWear);

			pCreatureWear->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );
			pCreatureWear->SetZone( g_pZone );
			CT_Done("DIAG: GCAddSlayer post SetStatus+SetZone");

			pCreature = (MCreature*)pCreatureWear;

			if (si.getCompetence()==0)
			{
				pCreatureWear->SetCompetence( 0 );
				pCreatureWear->SetCreatureType( CREATURETYPE_SLAYER_OPERATOR );
				pCreatureWear->SetMale( si.getSex()==MALE );
			}
			else
			{
				pCreatureWear->SetCreatureType( (si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
			}

			pCreatureWear->SetGuildNumber( si.getGuildID() );
			pCreatureWear->SetOriginServerNum( si.getBorn() );
			pCreatureWear->SetUnionGuildID( si.getUnionID() );

			pCreatureWear->SetGroundCreature();
			pCreatureWear->SetID(si.getObjectID());
			//pCreatureWear->SetAction(ACTION_MOVE);
			pCreatureWear->SetPosition( si.getX(), si.getY() );
			pCreatureWear->SetServerPosition( si.getX(), si.getY() );
			pCreatureWear->SetDirection( si.getDir() );
			pCreatureWear->SetCurrentDirection( si.getDir() );
			pCreatureWear->SetAction( ACTION_STAND );
			CT_Done("DIAG: GCAddSlayer post position/dir/action");

			// 피부색
			pCreatureWear->SetBodyColor1( si.getSkinColor() );

#if __CONTENTS(__SECOND_TRANSFORTER)
			pCreatureWear->SetWingItemType(si.getMotorItemType());
#endif //__SECOND_TRANSFORTER

			pCreature->SetMasterEffectType(si.getMasterEffectColor());

			pCreatureWear->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
			pCreatureWear->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );
			pCreatureWear->SetStatus( MODIFY_ALIGNMENT, si.getAlignment() );
			pCreatureWear->SetStatus( MODIFY_RANK, si.getRank() );
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			pCreatureWear->SetStatus( MODIFY_CONTRIBUTE_POINT, si.getContributePoint());
#endif //__CONTRIBUTE_SYSTEM
			// 이름
			pCreatureWear->SetName( si.getName().c_str() );

			// 색깔 정보

			// 복장 정보 --> 일단 기본 옷을 입고 있게 한다.
			//----------------------------------------
			// 복장을 착용한다.
			//----------------------------------------
			CT_Done("DIAG: GCAddSlayer pre SetAddonToSlayer");
			SetAddonToSlayer( pCreatureWear, &si );
			CT_Done("DIAG: GCAddSlayer post SetAddonToSlayer");

			// 2026-05-19 DEATH STATE FIX (bkz GCAddSlayerCorpseHandler same pattern)
			// Re-entry sirasinda HP=0 ile GCAddSlayer gelirse default ACTION_STAND
			// ayakta ama olu gosteriyor. SetCorpse() ile m_bAlive=false + ACTION_DIE.
			if (si.getCurrentHP() == 0)
			{
				pCreatureWear->SetCorpse();
			}
			if(pCreature->IsNPC() == false) {
				CT_Done("DIAG: GCAddSlayer pre SetPersnalShop");
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
				CT_Done("DIAG: GCAddSlayer post SetPersnalShop");
			}

			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsSlayer()== true && pCreature->IsNPC() == false)
			{
				// 2006.04.25 chyaya
				WORD colorIndex = pPacket->getStoreOutlook().getPaintColor();
				pCreature->SetPersnalShopColor( MCreature::s_PersnalShopColor[colorIndex] );

				/* 남의 상점까지도 채팅 색상으로 열고 있다. -_-;;
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
				*/

				// 요청에 따라 디폴트 색으로 열리도록 수정
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString( (char*)pPacket->getStoreOutlook().getSign().c_str() );
				else
					pCreature->SetPersnalString( (*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString() );
				// by chyaya 2006.04.19
			}

			CT_Done("DIAG: GCAddSlayer pre g_pZone->AddCreature");
			if (!g_pZone->AddCreature( pCreatureWear ))
			{
				CT_Done("DIAG: GCAddSlayer AddCreature returned false, deleting");
				delete pCreatureWear;
				pCreatureWear = NULL;
				pCreature = NULL;
			}
			CT_Done("DIAG: GCAddSlayer post AddCreature");
		}
		//--------------------------------------------------
		// 이미 있는 Creature인 경우
		//--------------------------------------------------
		else
		{
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );
			pCreature->SetGuildNumber( si.getGuildID() );
			pCreature->SetOriginServerNum( si.getBorn() );
			pCreature->SetUnionGuildID( si.getUnionID() );

			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( si.getX(), si.getY() );
			pCreature->SetServerPosition( si.getX(), si.getY() );
			pCreature->SetDirection( si.getDir() );
			pCreature->SetCurrentDirection( si.getDir() );
			pCreature->SetAction( ACTION_STAND );

			// 피부색
			pCreature->SetBodyColor1( si.getSkinColor() );
			pCreature->SetMasterEffectType(si.getMasterEffectColor());

			pCreature->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, si.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, si.getRank() );
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			pCreature->SetStatus( MODIFY_CONTRIBUTE_POINT, si.getContributePoint());
#endif //__CONTRIBUTE_SYSTEM

			// 2026-05-19 DEATH STATE FIX (bkz GCAddSlayerCorpseHandler same pattern)
			if (si.getCurrentHP() == 0 && !pCreature->IsDead())
			{
				pCreature->SetStop();
				pCreature->SetDead();
			}

			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsSlayer()== true && pCreature->IsNPC() == false)
			{
				// 2006.04.25 chyaya
				WORD colorIndex = pPacket->getStoreOutlook().getPaintColor();
				pCreature->SetPersnalShopColor( MCreature::s_PersnalShopColor[colorIndex] );

				/*
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
				*/

				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString( (char*)pPacket->getStoreOutlook().getSign().c_str() );
				else
					pCreature->SetPersnalString( (*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString() );
				// by chyaya 2006.04.19

			}

		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			// Effect 붙이기..
			//--------------------------------------------------
			CT_Done("DIAG: GCAddSlayer pre SetEffectInfo");
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );
			CT_Done("DIAG: GCAddSlayer post SetEffectInfo");

			// 펫 처리
			if(pPacket->getPetInfo() != NULL) {
				CT_Done("DIAG: GCAddSlayer pre SetPetInfo");
				SetPetInfo(pPacket->getPetInfo(), pCreature->GetID());
				CT_Done("DIAG: GCAddSlayer post SetPetInfo");
			}
			NicknameInfo* _tempNick = pPacket->getNicknameInfo();
			if(_tempNick != NULL)
			{
				CT_Done("DIAG: GCAddSlayer pre Nickname processing type=%d",
					(int)_tempNick->getNicknameType());
				// 커스텀 닉네임 일때
				if(_tempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM_FORCED ||
				   _tempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM)
				{
					pCreature->SetNickName(_tempNick->getNicknameType(), (char*)_tempNick->getNickname().c_str());
					
				}
				else // 닉네임 인덱스가 있을 때
				{
					int TempIndex = _tempNick->getNicknameIndex();
					if(TempIndex >= g_pNickNameStringTable->GetSize())
						TempIndex = 0;
					pCreature->SetNickName(_tempNick->getNicknameType(), (char*)(*g_pNickNameStringTable)[TempIndex].GetString());
				}
				CT_Done("DIAG: GCAddSlayer post Nickname");
			}
			CT_Done("DIAG: GCAddSlayer block-end (pCreature non-NULL path)");
		}
		CT_Done("DIAG: GCAddSlayer execute exit");
	}

	// [도움말] Slayer가 나타날때
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_SLAYER );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
