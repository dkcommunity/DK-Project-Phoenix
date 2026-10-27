//////////////////////////////////////////////////////////////////////////////
// Filename    : RapidGliding.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "RapidGliding.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK5.h"

#include "Gpackets/GCAddEffectToTile.h"
#include "EffectDarkness.h"
#include "Gpackets/GCAddEffect.h"
//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 타일 핸들러 
//////////////////////////////////////////////////////////////////////////////
SkillResultType RapidGliding::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		// NPC는 공격할 수가 없다.
		//		bool bIncreaseDomainExp = pVampire->isRealWearingEx(Vampire::WEAR_RIGHTHAND);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK5 _GCSkillToTileOK5;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if ( !rect.ptInRect(X, Y) )
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		if( pZone->getZoneID() == 8001 )
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}

		SkillInput input(pVampire);
		SkillOutput output;
		computeOutput(input, output);

		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, X, Y, output.Range);
		bool bEffected	 = !isAbleToUseSelfSkill(pVampire) // 20071228
							|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_FLAG) 
							|| pVampire->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER);
		bool bEffected2	 = !pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF);
		bool bPassLine	 = isPassLine( pZone, pVampire->getX(), pVampire->getY(), X, Y );

		if (bManaCheck && bTimeCheck && bRangeCheck && !bEffected && bPassLine && bEffected2 )
		{

			// 빠르게 PC를 움직여준다.
			if (pZone->moveFastPC(pVampire, pVampire->getX(), pVampire->getY(), X, Y, getSkillType())) 
			{
				decreaseMana(pVampire, RequiredMP, _GCSkillToTileOK1);

				_GCSkillToTileOK1.setSkillType(SkillType);
				_GCSkillToTileOK1.setCEffectID(0);
				_GCSkillToTileOK1.setX(X);
				_GCSkillToTileOK1.setY(Y);
				_GCSkillToTileOK1.setRange(0);
				_GCSkillToTileOK1.setDuration(0);

				_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
				_GCSkillToTileOK5.setSkillType(SkillType);
				_GCSkillToTileOK5.setX(X);
				_GCSkillToTileOK5.setY(Y);
				_GCSkillToTileOK5.setRange(0);
				_GCSkillToTileOK5.setDuration(0);

				// 스킬 후 처리
				postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1 );

				// 자신에게 바뀐 HP를 알려준다.
				pPlayer->sendPacket( &_GCSkillToTileOK1);

				// 주위에 HP가 바꼈다고 알린다.
				GCStatusCurrentHP gcStatusCurrentHP;
				gcStatusCurrentHP.setObjectID(pVampire->getObjectID());
				gcStatusCurrentHP.setCurrentHP(pVampire->getHP());
				pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcStatusCurrentHP, pVampire);
				pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &_GCSkillToTileOK5, pVampire);

				pVampireSkillSlot->setRunTime(output.Delay);
				
				if(pVampire->getCompetence()==0)
				{
					/*SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_DARKNESS);
					Assert(pSkillHandler != NULL);
					cout<<"use darkness 1111111"<<endl;
					SkillInfo* pInfo = g_pSkillInfoManager->getSkillInfo(SKILL_DARKNESS);
					Assert(pInfo != NULL);
					cout<<"use darkness 22222222222222"<<endl;
					VampireSkillSlot* pSlot = pVampire->hasSkill(SKILL_DARKNESS);
					Assert(pSlot != NULL);
					cout<<"use darkness 33333333333"<<endl;
					pSkillHandler->execute(pVampire, pVampire->getX(), pVampire->getY(),pSlot,(CEffectID_t)29);
					cout<<"use darkness success"<<endl;

					_GCSkillToTileOK1.setSkillType(SKILL_DARKNESS);
					_GCSkillToTileOK1.setCEffectID((CEffectID_t)29);
					_GCSkillToTileOK1.setX(pVampire->getX());
					_GCSkillToTileOK1.setY(pVampire->getY());
					_GCSkillToTileOK1.setDuration(50);
					_GCSkillToTileOK1.setRange(1);
					pPlayer->sendPacket( &_GCSkillToTileOK1);*/
					GCAddEffectToTile gcAddEffectToTile;
					int edge =2;
					int oX, oY;
					int pX= pVampire->getX();
					int pY= pVampire->getY();
					for(oY = -edge; oY <= edge; oY++)
					for(oX = -edge; oX <= edge; oX++)
					{
						int tileX = pX+oX;
						int tileY = pY+oY;
						if (rect.ptInRect(tileX, tileY))
						{
							Tile& tile = pZone->getTile(tileX, tileY);

								if ( tile.canAddEffect() )
								{
									EffectDarkness* pEffect = new EffectDarkness( pZone, tileX, tileY );
									pEffect->setDeadline(30);
									pEffect->setLevel(300);
									pEffect->setDuration(30);
									pEffect->setStartTime();

									pZone->registerObject( pEffect );
									pZone->addEffect( pEffect );
									tile.addEffect( pEffect );


									gcAddEffectToTile.setObjectID(pEffect->getObjectID());
									gcAddEffectToTile.setXY(tileX, tileY); 
									gcAddEffectToTile.setEffectID((CEffectID_t)29);
									gcAddEffectToTile.setDuration(30);
									pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffectToTile);
								}
						}
					}

				}

				return SKILL_RESULT_SUCCESS;
			} 
			else 
			{
				executeSkillFailException(pVampire, getSkillType());
			}
		} 
		else 
		{
			executeSkillFailException(pVampire, getSkillType());
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType RapidGliding::execute(Vampire * pVampire, ObjectID_t TargetObjectID, VampireSkillSlot * pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		// NPC는 공격할 수가 없다.
		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		execute( pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID );
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

RapidGliding g_RapidGliding;

