//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectADV2TORRENTIALLIFE.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectADV2TORRENTIALLIFE.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "Player.h"
#include "PacketUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCOtherModifyInfo.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectADV2TORRENTIALLIFE::EffectADV2TORRENTIALLIFE(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer());

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2TORRENTIALLIFE::unaffect() 
	throw(Error)
{
	__BEGIN_TRY	

	//cout << "ADV2TORRENTIALLIFE" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	//cout << "ADV2TORRENTIALLIFE" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectADV2TORRENTIALLIFE::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "ADV2TORRENTIALLIFE" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	Assert(pCreature->isSlayer()); // œœ·¹ÀÌŸîž»°íŽÂ °Éž®Áö ŸÊŽÂŽÙ.

	pCreature->removeFlag(Effect::EFFECT_CLASS_ADV2TORRENTIALLIFE);

	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

	pSlayer->initAllStatAndSend();

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ADV2TORRENTIALLIFE);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "ADV2TORRENTIALLIFE" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectADV2TORRENTIALLIFE::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectADV2TORRENTIALLIFE("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}
