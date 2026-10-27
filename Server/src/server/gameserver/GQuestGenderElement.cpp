#include "GQuestGenderElement.h"
#include "PlayerCreature.h"

GQuestElement::ResultType GQuestGenderElement::checkCondition( PlayerCreature* pPC ) const
{
	GQuestElement::ResultType Result;

	if ( m_strGenderType.compare("male") == 0 )
	{
		if ( pPC->getSex() != MALE )
		{
			return WAIT;
		}
	}
	else if ( m_strGenderType.compare("female") == 0 )
	{
		if ( pPC->getSex() != FEMALE )
		{
			return WAIT;
		}
	}
	else if ( m_strGenderType.compare("both") == 0 )
	{
		if ( !(pPC->getSex() == MALE || pPC->getSex() == FEMALE) )
		{
			return WAIT;
		}
	}
	else
	{
		return WAIT;
	}

	vector<GQuestElement*>::const_iterator itr = m_Elements.begin();
	for ( ; itr != m_Elements.end() ; ++itr )
	{
		Result = (*itr)->checkCondition(pPC);

		if ( Result != OK ) return Result;
	}

	return OK;
}

GQuestGenderElement* GQuestGenderElement::makeElement(XMLTree* pTree)
{
	GQuestGenderElement* pRet = new GQuestGenderElement;

	Assert( pTree->GetAttribute("restriction", pRet->m_strGenderType ) );

	for ( size_t i=0; i<pTree->GetChildCount() ; ++i )
	{
		XMLTree* pChild = pTree->GetChild(i);
		GQuestElement* pElement = GQuestElementFactory::Instance().makeElement(pChild);
		if ( pElement != NULL )
		{
			pRet->m_Elements.push_back( pElement );
		}
		else
		{
			cout << "엘리먼트를 못 만들겠음 : " << pChild->GetName() << endl;
			Assert( false );
		}
	}

	return pRet;
}

GQuestGenderElement g_GenderElement;
