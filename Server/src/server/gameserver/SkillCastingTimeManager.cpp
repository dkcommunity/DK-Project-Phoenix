#include "SkillCastingTimeManager.h"
#include "Item.h"

SkillCastingTimeManager g_SkillCastingTimeManager;

SkillCastingTimeList::SkillCastingTimeList()
{
	m_bHasDefaultCastingTime = false;
}

void SkillCastingTimeList::SetDefault(SkillCastingTime& castingTime)
{
	m_DefaultCastingTime = castingTime;
	m_bHasDefaultCastingTime = true;
}

void SkillCastingTimeList::Set(Item::ItemClass IClass, SkillCastingTime& castingTime)
{
	m_SkillCastingTimeMap[IClass] = castingTime;
}

bool SkillCastingTimeList::GetSkillTime(Item::ItemClass IClass, SkillCastingTime& castingTime)
{
	TSkillCastingTimeMap::iterator iter;
	
	iter = m_SkillCastingTimeMap.find(IClass);
	if ( iter != m_SkillCastingTimeMap.end() )
	{
		castingTime = iter->second;
	
		return true;
	}
	else
	{
		if ( m_bHasDefaultCastingTime )
		{
			castingTime = m_DefaultCastingTime;
			
			return true;
		}
		else
		{
			iter = m_SkillCastingTimeMap.begin();
			if ( iter != m_SkillCastingTimeMap.end() )
			{
				castingTime = iter->second;
				
				return true;
			}
		}
	}
	
	return false;
	
}

SkillCastingTimeManager::SkillCastingTimeManager()
{
	
}

SkillCastingTimeManager::~SkillCastingTimeManager()
{
	SkillCastingTimeList *pSkillCastingTimeList = NULL;
	TSkillCastingTimeList::iterator iter;
	
	iter = m_SkillCastingTimeList.begin();
	for( ; iter != m_SkillCastingTimeList.end() ; ++iter )
	{
		pSkillCastingTimeList = iter->second;
		SAFE_DELETE(pSkillCastingTimeList);
	}
}

void SkillCastingTimeManager::ParseSkillCastingTime(string& strCastingTimeParam, TParamList& ParamList)
{
	char ch;
	size_t iStartPos = string::npos;
	size_t iEndPos = string::npos;

	ParamList.clear();

	for(size_t i = 0; i < strCastingTimeParam.size() ; ++i)
	{
		ch = strCastingTimeParam.at(i);

		if ( ch == '(' )
		{
			iStartPos = i;
		}

		if ( ch == ')' )
		{
			iEndPos = i;
		}

		if ( iStartPos != string::npos && iEndPos != string::npos )
		{
			string strParams;

			TParams Params;
			string strParam;

			size_t iCurCommaIndex = 0;
			size_t iPrevCommaIndex = 0;

			strParams = strCastingTimeParam.substr(iStartPos + 1, iEndPos - (iStartPos + 1));

			iCurCommaIndex = strParams.find(",");
			while ( iCurCommaIndex != string::npos )
			{
				strParam = strParams.substr(iPrevCommaIndex, iCurCommaIndex - iPrevCommaIndex);
				trim(strParam);

				Params.push_back( strParam );

				iPrevCommaIndex = iCurCommaIndex + 1;
				iCurCommaIndex = strParams.find(",", iCurCommaIndex + 1);
			}

			Params.push_back( strParams.substr(iPrevCommaIndex) );

			iStartPos = string::npos;
			iEndPos = string::npos;

			ParamList.push_back( Params );
		}
	}
}

SkillCastingTimeList *SkillCastingTimeManager::GetSkillCastingTimeList(SkillType_t SkillType)
{
	TSkillCastingTimeList::iterator iter;
	SkillCastingTimeList *pSkillCastingTimeList = NULL;
	
	iter = m_SkillCastingTimeList.find(SkillType);
	if ( iter != m_SkillCastingTimeList.end() )
	{
		pSkillCastingTimeList = iter->second;
	}
	else
	{
		pSkillCastingTimeList = new SkillCastingTimeList;
					
		m_SkillCastingTimeList[SkillType] = pSkillCastingTimeList;			 
	}
	
	return pSkillCastingTimeList;
}

void SkillCastingTimeManager::SetDefault(SkillType_t SkillType, SkillCastingTime& castingTime)
{
	GetSkillCastingTimeList(SkillType)->SetDefault(castingTime);
}

void SkillCastingTimeManager::SetDefault(SkillType_t SkillType, TParamList& ParamList)
{
	if ( ParamList.size() == 0 )
	{
		return;
	}
	
	TParamList::iterator iter;
	SkillCastingTime castingTime;
	
	iter = ParamList.begin();
	if ( iter != ParamList.end() )
	{
		if ( (*iter).size() >= 2 ) 
		{
			castingTime.iCastingTimeForNonAdv = atoi((*iter)[0].c_str());
			castingTime.iCastingTimeForAdv = atoi((*iter)[1].c_str());
			
			SetDefault(SkillType, castingTime);
		}
	}
}

void SkillCastingTimeManager::Set(SkillType_t SkillType, Item::ItemClass IClass, SkillCastingTime& castingTime)
{
	GetSkillCastingTimeList(SkillType)->Set(IClass, castingTime);
}

void SkillCastingTimeManager::Set(SkillType_t SkillType, TParamList& ParamList)
{
	if ( ParamList.size() == 0 )
	{
		return;
	}
	
	SkillCastingTimeManager::TParamList::iterator iter;
	Item::ItemClass IClass;
	SkillCastingTime castingTime;
	
	iter = ParamList.begin();
	for( ; iter != ParamList.end() ; ++iter )
	{
		if ( (*iter).size() >= 3 ) 
		{
			IClass = Item::getItemClass((*iter)[0]);
			
			if ( IClass != Item::ITEM_CLASS_MAX )
			{
				castingTime.iCastingTimeForNonAdv = atoi((*iter)[1].c_str());
				castingTime.iCastingTimeForAdv = atoi((*iter)[2].c_str());
				
				Set(SkillType, IClass, castingTime);
			}
		}
	}
}

bool SkillCastingTimeManager::GetSkillTime(SkillType_t SkillType, Item::ItemClass IClass, SkillCastingTime& castingTime)
{
	if ( GetSkillCastingTimeList(SkillType)->GetSkillTime(IClass, castingTime) )
	{
		return true;
	}
	
	return false;
}
