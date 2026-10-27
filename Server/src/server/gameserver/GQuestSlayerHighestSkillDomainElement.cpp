#include "GQuestSlayerHighestSkillDomainElement.h"
#include "PlayerCreature.h"
#include "Slayer.h"

GQuestElement::ResultType GQuestSlayerHighestSkillDomainElement::checkCondition( PlayerCreature* pPC ) const
{
	Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
	
	GQuestElement::ResultType Result;

	if ( m_strDomain.compare("blade") == 0 )
	{
		if ( pSlayer->getHighestSkillDomain() != SKILL_DOMAIN_BLADE )
		{
			return WAIT;
		}
	}
	else if ( m_strDomain.compare("sword") == 0 )
	{
		if ( pSlayer->getHighestSkillDomain() != SKILL_DOMAIN_SWORD )
		{
			return WAIT;
		}
	}
	else if ( m_strDomain.compare("gun") == 0 )
	{
		if ( pSlayer->getHighestSkillDomain() != SKILL_DOMAIN_GUN )
		{
			return WAIT;
		}
	}
	else if ( m_strDomain.compare("heal") == 0 )
	{
		if ( pSlayer->getHighestSkillDomain() != SKILL_DOMAIN_HEAL )
		{
			return WAIT;
		}
	}
	else if ( m_strDomain.compare("enchant") == 0 )
	{
		if ( pSlayer->getHighestSkillDomain() != SKILL_DOMAIN_ENCHANT )
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

GQuestSlayerHighestSkillDomainElement* GQuestSlayerHighestSkillDomainElement::makeElement(XMLTree* pTree)
{
	GQuestSlayerHighestSkillDomainElement* pRet = new GQuestSlayerHighestSkillDomainElement;

	Assert( pTree->GetAttribute("domain", pRet->m_strDomain ) );

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

GQuestSlayerHighestSkillDomainElement g_SlayerHighestSkillDomainElement;
