#ifndef __GQUEST_SLAYERHIGHESTSKILLDOMAIN_ELEMENT_H__
#define __GQUEST_SLAYERHIGHESTSKILLDOMAIN_ELEMENT_H__

#include "GQuestElement.h"
#include "GQuestStatus.h"
#include <vector>

class GQuestSlayerHighestSkillDomainElement : public GQuestElement
{
public:
	GQuestSlayerHighestSkillDomainElement() { }
	string		 				getElementName() const { return "SlayerHighestSkillDomain"; }

	ResultType	 		checkCondition( PlayerCreature* pPC ) const;

	GQuestSlayerHighestSkillDomainElement*	makeElement(XMLTree* pTree);

private:
	vector<GQuestElement*>	m_Elements;
	string m_strDomain;
};

extern GQuestSlayerHighestSkillDomainElement g_SlayerHighestSkillDomainElement;

#endif
