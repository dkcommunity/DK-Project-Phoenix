#include "GQuestGiveOustersExpElement.h"
#include "PlayerCreature.h"
#include "Ousters.h"
#include "skill/SkillUtil.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSystemMessage.h"
#include "Player.h"
#include "StringPool.h"

GQuestElement::ResultType GQuestGiveOustersExpElement::checkCondition( PlayerCreature* pPC ) const
{
	if ( !pPC->isOusters() ) return FAIL;

	GCModifyInformation gcMI;
	Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
	increaseOustersExp( pOusters, m_Amount, gcMI );

	pOusters->getPlayer()->sendPacket( &gcMI );

	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_GET_EXP) ); // 20070814
	pOusters->getPlayer()->sendPacket( &gcSM );

	return OK;
}

GQuestGiveOustersExpElement* GQuestGiveOustersExpElement::makeElement(XMLTree* pTree)
{
	GQuestGiveOustersExpElement* pRet = new GQuestGiveOustersExpElement;

	pTree->GetAttribute("amount", pRet->m_Amount);

	return pRet;
}

GQuestGiveOustersExpElement g_GiveOustersExpElement;
