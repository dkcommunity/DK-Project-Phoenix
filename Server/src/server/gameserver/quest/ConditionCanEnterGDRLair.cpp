////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionCanEnterGDRLair.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionCanEnterGDRLair.h"
#include "GDRLairManager.h"
#include "Effect.h"
#include "Gpackets/GCSystemMessage.h"
#include "Player.h"
#include "StringPool.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionCanEnterGDRLair::isSatisfied (Creature * pCreature1 , Creature * pCreature2, void* pParam) const 
{ 
	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

//	return false;

/*	if ( GDRLairManager::Instance().getTotalPCs() > 36 )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( "제한 인원이 초과되어 들어갈 수 없습니다." );
		pCreature2->getPlayer()->sendPacket (&gcSystemMessage);
		return false;
	}*/

	if ( !GDRLairManager::Instance().canEnter() )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_GDR_LAIR_NOT_OPEN) ); // 20070814
		pCreature2->getPlayer()->sendPacket (&gcSystemMessage);
		return false;
	}

	if ( !pCreature2->isFlag( Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR ) )
	{
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_DESTROY_STONE_MANGIN_AT_LUST_TOWER2) ); // 20070814
		pCreature2->getPlayer()->sendPacket (&gcSystemMessage);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionCanEnterGDRLair::read (PropertyBuffer & propertyBuffer)
{
}

////////////////////////////////////////////////////////////////////////////////
	// get debug string
////////////////////////////////////////////////////////////////////////////////
string ConditionCanEnterGDRLair::toString () const 
{ 
	__BEGIN_TRY

	StringStream msg;
	msg << "ConditionCanEnterGDRLair("
		<< ")"; 
	return msg.toString();

	__END_CATCH
}
