//////////////////////////////////////////////////////////////////////////////
// Filename    : InstallTurret.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2HALCYONMK1.h"
#include "EffectADV2HALCYONMK1.h"

#include "ItemUtil.h"
#include "Zone.h"
#include "Tile.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// œœ·¹ÀÌŸî Œ¿ÇÁ
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2HALCYONMK1::execute(Slayer* pSlayer, SlayerSkillSlot * pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
		if (pWeapon == NULL || !isArmsWeapon(pWeapon) || pSlayer->hasRelicItem())
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		bool bIncreaseDomainExp = pSlayer->isRealWearingEx(Slayer::WEAR_RIGHTHAND);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		ZoneCoord_t X = pSlayer->getX();
		ZoneCoord_t Y = pSlayer->getY();
		Tile& rTile = pZone->getTile(X,Y);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		//bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		bool bEffected   = pSlayer->isFlag( Effect::EFFECT_CLASS_ADV2HALCYONMK1 );

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected)//&& bHitRoll 
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToSelfOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			EffectADV2HALCYONMK1* pEffect = new EffectADV2HALCYONMK1( pSlayer );
			pEffect->setDeadline(output.Duration);
			pEffect->setDamage( output.Damage );
			//pEffect->setDefense( output.ToHit );

			cout << "Turret Duration : " << output.Duration << endl;
			cout << "Turret Delay : " << output.Delay << endl;

			pSlayer->setFlag( pEffect->getEffectClass() );
			pSlayer->addEffect( pEffect );
		        cout << "MK1++ : " << output.Delay << endl;	

			GCAddEffect gcAE;
			gcAE.setObjectID( pSlayer->getObjectID() );
			gcAE.setDuration( output.Duration );
			gcAE.setEffectID( pEffect->getSendEffectClass() );

			pZone->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAE );

			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord(prev);
			pSlayer->initAllStat();
			pSlayer->sendRealWearingInfo();
			pSlayer->addModifyInfo(prev, _GCSkillToSelfOK1);

			// ÆÐÅ¶À» žžµéŸî ºž³œŽÙ.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(output.Duration);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(output.Duration);

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			// Send Packet
			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(X, Y, &_GCSkillToSelfOK2, pSlayer);
		
			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
                      cout << "MK1 go now: " << output.Delay << endl;	
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
		cout << t.toString() << endl;
	}

	cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2HALCYONMK1 g_ADV2HALCYONMK1;
