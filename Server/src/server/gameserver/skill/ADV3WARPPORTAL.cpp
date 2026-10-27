//////////////////////////////////////////////////////////////////////////////
// Filename    : WARPPORTAL.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV3WARPPORTAL.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK5.h"

//////////////////////////////////////////////////////////////////////////////
// 轨颇捞绢 鸥老 勤甸矾 
//////////////////////////////////////////////////////////////////////////////
SkillResultType WARPPORTAL::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		SkillType_t       SkillType  = pSkillSlot->getSkillType();

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK5 _GCSkillToTileOK5;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if ( !rect.ptInRect(X, Y) )
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		if( pZone->getZoneID() == 8001 )
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		SkillInput input(pSlayer, pSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, X, Y, output.Range);
		bool bEffected	 = !isAbleToUseSelfSkill(pSlayer) // 20071228
							|| pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) 
							|| pSlayer->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected )
		{
			if (pZone->moveFastPC(pSlayer, pSlayer->getX(), pSlayer->getY(), X, Y, getSkillType())) 
			{
				decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

				_GCSkillToTileOK1.setSkillType(SkillType);
				_GCSkillToTileOK1.setCEffectID(0);
				_GCSkillToTileOK1.setX(X);
				_GCSkillToTileOK1.setY(Y);
				_GCSkillToTileOK1.setRange(0);
				_GCSkillToTileOK1.setDuration(output.Delay);

				_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
				_GCSkillToTileOK5.setSkillType(SkillType);
				_GCSkillToTileOK5.setX(X);
				_GCSkillToTileOK5.setY(Y);
				_GCSkillToTileOK5.setRange(0);
				_GCSkillToTileOK5.setDuration(output.Delay);

				postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );

				pPlayer->sendPacket( &_GCSkillToTileOK1 );
				pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &_GCSkillToTileOK5, pSlayer);

				pSkillSlot->setRunTime(output.Delay);
				
				return SKILL_RESULT_SUCCESS;
			} 
			else 
			{
				executeSkillFailException(pSlayer, getSkillType());
			}
		} 
		else 
		{
			executeSkillFailException(pSlayer, getSkillType());
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////////////
// 轨颇捞绢 坷宏璃飘 勤甸矾
//////////////////////////////////////////////////////////////////////////////
SkillResultType WARPPORTAL::execute(Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY


	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(ObjectID);

		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pSlayer, getSkillType());

			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

WARPPORTAL g_WARPPORTAL;

