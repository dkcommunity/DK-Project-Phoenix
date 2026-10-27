#include "GQuestGiveDomainExpElement.h"
#include "PlayerCreature.h"
#include "Slayer.h"
#include "skill/SkillUtil.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSystemMessage.h"
#include "Player.h"
#include "StringPool.h"

GQuestElement::ResultType GQuestGiveDomainExpElement::checkCondition( PlayerCreature* pPC ) const
{
	if ( !pPC->isSlayer() ) return FAIL;

	GCModifyInformation gcMI;
	Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
	increaseDomainExp(pSlayer, pSlayer->getHighestSkillDomain(), m_Amount, gcMI );
	pSlayer->getPlayer()->sendPacket( &gcMI );

	GCSystemMessage gcSM;
	gcSM.setMessage( g_pStringPool->c_str(STRID_GET_DOMAIN_EXP) ); // 20070814
	pSlayer->getPlayer()->sendPacket( &gcSM );

	return OK;
}

GQuestGiveDomainExpElement* GQuestGiveDomainExpElement::makeElement(XMLTree* pTree)
{
	GQuestGiveDomainExpElement* pRet = new GQuestGiveDomainExpElement;

	pTree->GetAttribute("amount", pRet->m_Amount);

	return pRet;
}

GQuestGiveDomainExpElement g_GiveDomainExpElement;
