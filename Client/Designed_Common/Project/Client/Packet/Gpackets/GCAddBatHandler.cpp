//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddBatHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "ClientDef.h"
#include "Client_PCH.h"
#include "EffectSpriteTypeDef.h"
#include "GCAddBat.h"
#include "MEffectSpriteTypeTable.h"
#include "SkillDef.h"


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddBatHandler::execute(GCAddBat *pPacket, Player *pPlayer)

{
  __BEGIN_TRY

#ifdef __GAME_CLIENT__

  int batCreatureType = 0;
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
  int batType = pPacket->getBatType();
  // 만일 자신에게 날아온 GCAddBat이고, 신규이동수단일 경우..... 그 컬러값만
  // 취한다.
  if (g_pPlayer->GetID() == pPacket->getObjectID()) {
    if (batType == 2) {
#if __CONTENTS(__SECOND_TRANSFORTER)
      if (pPacket->getItemType() == 0) {
#endif //__SECOND_TRANSFORTER
        if (pPacket->getWingColor1() != 0)
          g_pPlayer->SetWingColor(pPacket->getWingColor1());
        else
          g_pPlayer->SetWingColor(403);
        g_pPlayer->SetWingEffectColor(pPacket->getWingColor2());

#if __CONTENTS(__SECOND_TRANSFORTER)
      } else if (pPacket->getItemType() == 1) {
        if (pPacket->getWingColor1() != 0)
          g_pPlayer->SetWingColor(pPacket->getWingColor1());
        else
          g_pPlayer->SetWingColor(403);
        g_pPlayer->SetWingEffectColor(pPacket->getWingColor2());
      }
#endif //__SECOND_TRANSFORTER
       // 그리고는.. 바로 리턴.
       // 왜? GCSkillToInventoryOk1Handler에서 컬러값만 처리하지 못하기 때문에..
       // 자신에게도 GCAddBat을 보내도록 하여.. Color값을 취하기 때문.
      g_pPlayer->SetWingType(batType);
      return;
    }
  }
  if (batType == 2) // battype이 2인 경우가 신규이동수단 ..
  {
    if (pPacket->getItemType() == 0) // 플리러 마우스(의 아이템타입번호)
      batCreatureType = CREATURETYPE_FLITTERMOUSE;
#if __CONTENTS(__SECOND_TRANSFORTER)
    else if (pPacket->getItemType() == 1) // 데몬 (의 아이템타입번호)
      batCreatureType = CREATURETYPE_SHAPE_OF_DEMON;
#endif                                    //__SECOND_TRANSFORTER
    else if (pPacket->getItemType() == 2) // 데몬 (의 아이템타입번호)
      batCreatureType = CREATURETYPE_NEDE;
    else if (pPacket->getItemType() == 3) // 데몬 (의 아이템타입번호)
      batCreatureType = CREATURETYPE_KLTL;
    else // itemtype이  추가되지 않은 신규 이동수단? 그럴 순 없다.
      batCreatureType = CREATURETYPE_BAT;
  } else
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
    batCreatureType = CREATURETYPE_BAT;

  _MinTrace("%d\n", pPacket->getBatColor());

  //------------------------------------------------------
  // Zone이 아직 생성되지 않은 경우
  //------------------------------------------------------
  if (g_pZone == NULL) {
    // message
    DEBUG_ADD("[Error] Zone is Not Init.. yet.");

  }
  //------------------------------------------------------
  // 정상..
  //------------------------------------------------------
  else {
    // AfxMessageBox( pPacket->toString().c_str() );
    MCreature *pCreature = g_pZone->GetCreature(pPacket->getObjectID());

    //--------------------------------------------------
    // 새로운 Creature이면 추가
    //--------------------------------------------------
    if (pCreature == NULL) {
      pCreature = new MCreatureWear;

      pCreature->SetZone(g_pZone);

      // pCreature->SetCreatureType( 0 );
      pCreature->SetCreatureType(batCreatureType);

      pCreature->SetName(pPacket->getName().c_str());

      // 임시로
      pCreature->SetGuildNumber(pPacket->getGuildID());

      pCreature->SetFlyingCreature();

      pCreature->SetID(pPacket->getObjectID());
      // pCreature->SetAction(ACTION_MOVE);
      pCreature->SetPosition(pPacket->getX(), pPacket->getY());
      pCreature->SetServerPosition(pPacket->getX(), pPacket->getY());
      pCreature->SetDirection(pPacket->getDir());
      pCreature->SetCurrentDirection(pPacket->getDir());
      pCreature->SetAction(ACTION_STAND);

      pCreature->SetStatus(MODIFY_MAX_HP, pPacket->getMaxHP());
      pCreature->SetStatus(MODIFY_CURRENT_HP, pPacket->getCurrentHP());

      // 2026-05-19 DEATH STATE FIX: Re-entry HP=0 -> SetCorpse() (bkz
      // GCAddMonsterHandler.cpp ayni pattern).
      if (pPacket->getCurrentHP() == 0)
      {
        pCreature->SetCorpse();
      }

      // pPacket->getName()
      //  색상 정보

      if (pPacket->getBatColor() != 0)
        pCreature->SetBatColor(pPacket->getBatColor());
      else
        pCreature->SetBatColor(0xFFFF);

      pCreature->SetAdvanceBatColor(pPacket->getAdvanceBatColor());

      if (!g_pZone->AddCreature(pCreature)) {
        delete pCreature;
      }
      // pCreature->SetWingType(pPacket->getBatType());
    }
    //--------------------------------------------------
    // 이미 있는 Creature인 경우
    //--------------------------------------------------
    else {
      // pCreature->SetCreatureType( batCreatureType );

      // 임시로
      pCreature->SetGuildNumber(pPacket->getGuildID());

      // pCreature->SetAction(ACTION_MOVE);
      pCreature->MovePosition(pPacket->getX(), pPacket->getY());
      pCreature->SetServerPosition(pPacket->getX(), pPacket->getY());
      pCreature->SetDirection(pPacket->getDir());
      pCreature->SetCurrentDirection(pPacket->getDir());
      // pCreature->SetAction( ACTION_STAND );

      pCreature->SetStatus(MODIFY_MAX_HP, pPacket->getMaxHP());
      pCreature->SetStatus(MODIFY_CURRENT_HP, pPacket->getCurrentHP());

      if (pPacket->getBatColor() != 0)
        pCreature->SetBatColor(pPacket->getBatColor());
      else
        pCreature->SetBatColor(0xFFFF);

      pCreature->SetAdvanceBatColor(pPacket->getAdvanceBatColor());

#if __CONTENTS(__FAST_TRANSFORTER) || __CONTENTS(__SECOND_TRANSFORTER)
      if (batType == 2
#if __CONTENTS(__SECOND_TRANSFORTER)
          && pCreature->GetWingItemType() == 0
#endif //__SECOND_TRANSFORTER
      ) {
        // 신규 이동수단 Wingcolor도 세팅하자.
        if (pPacket->getWingColor1() != 0)
          pCreature->SetWingColor(pPacket->getWingColor1());
        else
          pCreature->SetWingColor(403);
        pCreature->SetWingEffectColor(pPacket->getWingColor2());

        pCreature->SetWingEffect1();
      } else if (batType == 2 && pCreature->GetWingItemType() == 2) {
        pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2328, 0xFFFF, 0,
                                      FALSE, MEffect::EFFECT_ATTACH, false);
      }
      // else
      //	if(batType == 2 && pCreature->GetWingItemType()==3)
      //	{
      //		pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2695,
      //0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
      //	}
      //  승직 뱀파이어의 경우 박쥐가 아니라 고스트다.
      else
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
          if (batType == 2 && pCreature->GetWingItemType() == 1) {
        if (pPacket->getWingColor1() != 0)
          pCreature->SetWingColor(pPacket->getWingColor1());
        else
          pCreature->SetWingColor(403);
        pCreature->SetWingEffectColor(pPacket->getWingColor2());

        pCreature->SetWingEffect2();
      }
      // 승직 뱀파이어의 경우 박쥐가 아니라 고스트다.
      else
#endif //__SECOND_TRANSFORTER
        if (pCreature->IsVampire() && pCreature->IsAdvancementClass()) {
          batCreatureType = CREATURETYPE_VAMPIRE_GHOST;
          pCreature->SetAdvanceBatEffect();
        } else {
          pCreature->SetFlyingCreature();
        }
      // pCreature->SetWingType(pPacket->getBatType());

      //--------------------------------------------------
      // 박쥐로 변신하는 결과
      //--------------------------------------------------
      MActionResult *pResult = new MActionResult;

      pResult->Add(new MActionResultNodeChangeCreatureType(pCreature->GetID(),
                                                           batCreatureType));

      int skillType = 0;
#if __CONTENTS(__FAST_TRANSFORTER)
      if (batType == 2 && pPacket->getItemType() == 0)
        skillType = SKILL_CLIENT_FLITTERMOUSE;
      else if (batType == 2 && pPacket->getItemType() == 2)
        skillType = SKILL_CLIENT_FLITTERMOUSE;
      else
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
          if (batType == 2 && pPacket->getItemType() == 1)
        skillType = SKILL_CLIENT_SHAPE_OF_DEMON;
      else
#endif //__SECOND_TRANSFORTER
        skillType = RESULT_MAGIC_TRANSFORM_TO_BAT;

      //--------------------------------------------------
      // 박쥐 변신
      //--------------------------------------------------
      ExecuteActionInfoFromMainNode(skillType, // 사용 기술 번호

                                    pCreature->GetX(), pCreature->GetY(), 0,
                                    pCreature->GetDirection(), // 사용 방향

                                    OBJECTID_NULL, // 목표에 대한 정보
                                    pCreature->GetX(), pCreature->GetY(), 0,

                                    0, // 기술의 (남은) 지속 시간

                                    pResult, // NULL,

                                    false); // 기술 첨부터 시작한다.

      // pCreature->SetDelay( 1000 );
    }
  }

  // [도움말] Vampire가 나타날때
  //	__BEGIN_HELP_EVENT
  //		//ExecuteHelpEvent( HE_CREATURE_APPEAR_VAMPIRE );
  //	__END_HELP_EVENT

#endif

  __END_CATCH
}
