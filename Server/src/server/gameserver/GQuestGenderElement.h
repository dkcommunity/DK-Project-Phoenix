#ifndef __GQUEST_GENDER_ELEMENT_H__
#define __GQUEST_GENDER_ELEMENT_H__

#include "GQuestElement.h"
#include "GQuestStatus.h"
#include <vector>

class GQuestGenderElement : public GQuestElement
{
public:
	GQuestGenderElement() { }
	string		 				getElementName() const { return "Gender"; }

	ResultType	 		checkCondition( PlayerCreature* pPC ) const;

	GQuestGenderElement*	makeElement(XMLTree* pTree);

private:
	vector<GQuestElement*>	m_Elements;
	string m_strGenderType;
};

extern GQuestGenderElement g_GenderElement;

#endif
