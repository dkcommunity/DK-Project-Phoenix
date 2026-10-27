#include "ItemGradeManager.h"
#include "DB.h"
#include "Assert.h"

void ItemGradeManager::load() throw(Error)
{
	__BEGIN_TRY

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT Grade, Ratio, GambleRatio, BeadRatio FROM ItemGradeRatioInfo" );

		Assert( pResult->getRowCount() == 10 );
		m_GradeGambleRatios.resize(10, 0);
		m_GradeBeadRatios.resize(10, 0);
		m_GradeRatios.resize(10, 0);

		while ( pResult->next() )
		{
			int Grade = pResult->getInt(1);
			int Ratio = pResult->getInt(2);
			int GambleRatio = pResult->getInt(3);
			int BeadRatio = pResult->getInt(4);

			m_GradeRatios[Grade-1] = Ratio;
			m_GradeGambleRatios[Grade-1] = GambleRatio;
			m_GradeBeadRatios[Grade-1] = BeadRatio;
			cout << Grade << "Бо : " << Ratio << ", " << GambleRatio << ", " << BeadRatio << endl;
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

Grade_t ItemGradeManager::getRandomGrade() const
{
	int value = rand()%100000;
	int i=0;

	for ( ; i<10; ++i )
	{
		value -= m_GradeRatios[i];
		if ( value < 0 ) return i+1;
	}

	return i;
}

Grade_t ItemGradeManager::getRandomGambleGrade() const
{
	int value = rand()%100000;
	int i=0;

	for ( ; i<10; ++i )
	{
		value -= m_GradeGambleRatios[i];
		if ( value < 0 ) return i+1;
	}

	return i;
}

Grade_t ItemGradeManager::getRandomBeadGrade() const
{
	int value = rand()%100000;
	int i=0;

	for ( ; i<10; ++i )
	{
		value -= m_GradeBeadRatios[i];
		if ( value < 0 ) return i+1;
	}

	return i;
}

Grade_t ItemGradeManager::getRandomCueOfAdamGrade() const
{
	int value = rand()%100;

	if ( value < 45 )
		return 1;
	else if ( value < 80 )
		return 2;
	else if ( value < 90 )
		return 3;
	else if ( value < 97 )
		return 4;
	else if ( value < 100 )
		return 5;

	return 0;
}

ItemGradeManager& ItemGradeManager::Instance()
{
	static ItemGradeManager theInstance;
	return theInstance;
}
