//--------------------------------------------------------------------------------
//
// Filename    : GCAddOustersHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAddOusters.h"
#include "ClientDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"
#include "MGameStringTable.h"

extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddOustersHandler::execute ( GCAddOusters * pPacket , Player * pPlayer )
	 
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
		const PCOustersInfo3 & oi = pPacket->getOustersInfo ();

		MCreature* pCreature = g_pZone->GetCreature(oi.getObjectID());

		//--------------------------------------------------
		// 새로운 Creature이면 추가
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			CT_Done("DIAG: GCAddOusters pre new MCreatureWear, objID=%d", (int)oi.getObjectID());
			pCreature = new MCreatureWear;
			CT_Done("DIAG: GCAddOusters post new MCreatureWear=%p", (void*)pCreature);

			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, oi.getAdvancementLevel() );

			pCreature->SetZone( g_pZone );
			CT_Done("DIAG: GCAddOusters post SetStatus+SetZone");

			pCreature->SetName( oi.getName().c_str() );

			//--------------------------------------------------
			// CreatureType 설정
			//--------------------------------------------------
			ItemType_t coatType = oi.getCoatType();

			if (oi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_OUSTERS_OPERATOR );
			}
			else
			{
				pCreature->SetCreatureType( GetOustersCreatureType( coatType ) );
			}

			pCreature->SetID(oi.getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( oi.getX(), oi.getY() );
			pCreature->SetServerPosition( oi.getX(), oi.getY() );
			pCreature->SetDirection( oi.getDir() );
			pCreature->SetCurrentDirection( oi.getDir() );
			pCreature->SetAction( ACTION_STAND );
			CT_Done("DIAG: GCAddOusters post position/dir/action");

			// 색깔
			pCreature->SetBodyColor1( oi.getHairColor() );
			pCreature->SetBodyColor2( oi.getCoatColor() );

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
			pCreature->SetWingType(oi.getWingSylphType());
			pCreature->SetWingColor(oi.getWingBodyColor());
			pCreature->SetWingEffectColor(oi.getWingEffectColor());
#if __CONTENTS(__SECOND_TRANSFORTER)
			pCreature->SetWingItemType(oi.getWingItemType());
#endif //__SECOND_TRANSFORTER
#endif//__FAST_TRANSFORTER||__SECOND_TRANSFORTER
			pCreature->SetMasterEffectType( oi.getMasterEffectColor() );
			//--------------------------------------------------
			// [ TEST CODE ]
			//--------------------------------------------------
			// 옷 색깔 설정하기
			//--------------------------------------------------
			/*
			if (pCreature->IsMale())
			{
				pCreature->SetBodyColor2( 91 );
			}
			else
			{
				pCreature->SetBodyColor2( 38 );
			}
			*/

			pCreature->SetStatus( MODIFY_MAX_HP, oi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, oi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, oi.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, oi.getRank() );
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			pCreature->SetStatus( MODIFY_CONTRIBUTE_POINT, oi.getContributePoint());
#endif //__CONTRIBUTE_SYSTEM

			// 2026-05-19 DEATH STATE FIX (bkz GCAddOustersCorpseHandler same pattern)
			// Re-entry sirasinda HP=0 ile GCAddOusters gelirse default ACTION_STAND
			// ayakta ama olu gosteriyor. SetCorpse() ile m_bAlive=false + ACTION_DIE.
			if (oi.getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			//oi.getName()
			// 색상 정보

			pCreature->SetWeaponSpeed( oi.getAttackSpeed() );
			
			// 임시로
			pCreature->SetGuildNumber( oi.getGuildID() );
			pCreature->SetOriginServerNum( oi.getBorn() );
			pCreature->SetUnionGuildID( oi.getUnionID() );
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsOusters()== true && pCreature->IsNPC() == false)
			{
				// 2006.04.25 chyaya
				WORD colorIndex = pPacket->getStoreOutlook().getPaintColor();
				pCreature->SetPersnalShopColor( MCreature::s_PersnalShopColor[colorIndex] );


				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString( (char*)pPacket->getStoreOutlook().getSign().c_str() );
				else 
					pCreature->SetPersnalString( (*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString() );
				// by chyaya 2006.04.19
			}


			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}
			
		}
		//--------------------------------------------------
		// 이미 있는 Creature인 경우
		//--------------------------------------------------
		else
		{
			//--------------------------------------------------
			// CreatureType 설정
			//--------------------------------------------------
			ItemType_t coatType = oi.getCoatType();

			if (oi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_OUSTERS_OPERATOR );
			}
			else
			{
				pCreature->SetCreatureType( GetOustersCreatureType( coatType ) );
			}

			// 임시로
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, oi.getAdvancementLevel() );
			pCreature->SetGuildNumber( oi.getGuildID() );
			pCreature->SetOriginServerNum( oi.getBorn() );
			pCreature->SetUnionGuildID( oi.getUnionID() );
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( oi.getX(), oi.getY() );
			pCreature->SetServerPosition( oi.getX(), oi.getY() );			
			pCreature->SetDirection( oi.getDir() );
			pCreature->SetCurrentDirection( oi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			// 색깔
			pCreature->SetBodyColor1( oi.getHairColor() );
			pCreature->SetBodyColor2( oi.getCoatColor() );
			pCreature->SetMasterEffectType( oi.getMasterEffectColor() );
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
			pCreature->SetWingType(oi.getWingSylphType());
			pCreature->SetWingColor(oi.getWingBodyColor());
			pCreature->SetWingEffectColor(oi.getWingEffectColor());
#if __CONTENTS(__SECOND_TRANSFORTER)
			pCreature->SetWingItemType(oi.getWingItemType());
#endif //__SECOND_TRANSFORTER
#endif//__FAST_TRANSFORTER||__SECOND_TRANSFORTER
			//--------------------------------------------------
			// [ TEST CODE ]
			//--------------------------------------------------
			// 옷 색깔 설정하기
			//--------------------------------------------------
			/*
			if (pCreature->IsMale())
			{
				pCreature->SetBodyColor2( 91 );
			}
			else
			{
				pCreature->SetBodyColor2( 38 );
			}
			*/

			pCreature->SetStatus( MODIFY_MAX_HP, oi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, oi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, oi.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, oi.getRank() );

			// 2026-05-19 DEATH STATE FIX (bkz GCAddOustersCorpseHandler same pattern)
			if (oi.getCurrentHP() == 0 && !pCreature->IsDead())
			{
				pCreature->SetStop();
				pCreature->SetDead();
			}

			pCreature->SetWeaponSpeed( oi.getAttackSpeed() );

			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());

			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsOusters()== true && pCreature->IsNPC() == false)
			{

			// 2006.04.25 chyaya
				WORD colorIndex = pPacket->getStoreOutlook().getPaintColor();
				pCreature->SetPersnalShopColor( MCreature::s_PersnalShopColor[colorIndex] );


				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString( (char*)pPacket->getStoreOutlook().getSign().c_str() );
				else
					pCreature->SetPersnalString( (*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString() );
				// by chyaya 2006.04.19
 
//				pCreature->SetPersnalString((char *)pPacket->getStoreOutlook().getSign().c_str());
//				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			}

			
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			// Effect 붙이기..
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );

			CT_Done("DIAG: GCAddOusters pre SetAddonToOusters pCreature=%p", (void*)pCreature);
			// FAZ 4 (2026-05-12): packet handler sweep — defansif dynamic_cast.
			// pCreature GetCreature ile bulunduysa MCreatureWear olmayabilir;
			// release build'de class hiyerarsi mismatch korumasi.
			{
				MCreatureWear* pOustersWear = dynamic_cast<MCreatureWear*>(pCreature);
				if (pOustersWear != NULL)
					SetAddonToOusters( pOustersWear, &oi );
				else
					DEBUG_ADD_FORMAT("[Error] dynamic_cast<MCreatureWear*> failed for Ousters id=%d", oi.getObjectID());
			}
			CT_Done("DIAG: GCAddOusters post SetAddonToOusters");

			// 펫 처리
			if(pPacket->getPetInfo() != NULL)
				SetPetInfo(pPacket->getPetInfo(), pCreature->GetID());

			DEBUG_ADD_FORMAT("[GCADDOUSTERS] ID = %d", pCreature->GetName());	
			
			NicknameInfo* _tempNick = pPacket->getNicknameInfo();
			if(_tempNick != NULL)
			{
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
			}

		}
	}

#endif

	__END_CATCH
}
