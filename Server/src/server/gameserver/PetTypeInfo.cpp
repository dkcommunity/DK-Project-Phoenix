
#include "PetTypeInfo.h"
#include "DB.h"

MonsterType_t PetTypeInfo::getPetCreatureType(PetLevel_t petLevel) const
{
	uint index = 0;
	if ( petLevel >= 10 ) index = petLevel / 10 - 1;
	return getPetCreatureTypeByIndex( index );
}

void PetTypeInfoManager::clear()
{
	vector<PetTypeInfo*>::iterator itr = m_PetTypeInfos.begin();
	vector<PetTypeInfo*>::iterator endItr = m_PetTypeInfos.end();

	for (; itr != endItr; ++itr)
	{
		SAFE_DELETE( (*itr) );
	}

	m_PetTypeInfos.clear();
}

void PetTypeInfoManager::load()
{
	clear();

	Statement* pStmt;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT MAX(PetType) FROM PetTypeInfo" );

		if ( !pResult->next() ) throw Error("PetTypeInfo 가 없습니다.");

		PetType_t MaxPetType = pResult->getInt(1);

		m_PetTypeInfos.resize(MaxPetType+1, NULL);

		pResult = pStmt->executeQuery(	"SELECT PetType, OriginalMonsterType, CreatureType1, CreatureType2, CreatureType3, CreatureType4, CreatureType5, CreatureType6, FoodType "
										"FROM PetTypeInfo" );

		while ( pResult->next() )
		{
			PetTypeInfo* pPetTypeInfo = new PetTypeInfo( pResult->getInt(1) );
			pPetTypeInfo->m_OriginalMonsterType = pResult->getInt(2);
			pPetTypeInfo->m_PetCreatureType[0] = pResult->getInt(3);
			pPetTypeInfo->m_PetCreatureType[1] = pResult->getInt(4);
			pPetTypeInfo->m_PetCreatureType[2] = pResult->getInt(5);
			pPetTypeInfo->m_PetCreatureType[3] = pResult->getInt(6);
			pPetTypeInfo->m_PetCreatureType[4] = pResult->getInt(7);
			pPetTypeInfo->m_PetCreatureType[5] = pResult->getInt(8);
			pPetTypeInfo->m_FoodType = pResult->getInt(9);

			addPetTypeInfo( pPetTypeInfo );
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt)
}

void PetTypeInfoManager::addPetTypeInfo( PetTypeInfo* pPetTypeInfo )
{
	if ( pPetTypeInfo->m_PetType >= m_PetTypeInfos.size() ) throw Error("Pet Type이 최대값을 초과했네용");

	m_PetTypeInfos[pPetTypeInfo->m_PetType] = pPetTypeInfo;
}

PetTypeInfo* PetTypeInfoManager::getPetTypeInfo( PetType_t PetType )
{
	if ( PetType >= m_PetTypeInfos.size() ) return NULL;

	return m_PetTypeInfos[PetType];
}
