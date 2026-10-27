//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterSummonInfo.cpp
// Written By  : ��
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Types.h"
#include "MonsterSummonInfo.h"
#include "MonsterInfo.h"
#include <list>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
//
// MonsterCollectionInfo :: parseString( infoString )
//
//////////////////////////////////////////////////////////////////////////////
//	SpriteType_t	SpriteType;
//	int             Num;
//
//        a        b   c
// ex1>  "(����ٵ�, 20)"		// ����ٵ� 20����
//
//        0 2   6
//        a b   c
// ex2>  "(5, 10)"				// SpriteType 5��(����ٵ�) 10����
//
// ex3>  "({34}, 20)"		// �� ���{}�� MonsterType�� ����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void MonsterCollectionInfo::parseString(const string& text)
{
	SpriteType = 0;
	MonsterType = 0;
	Num = 0;

	if (text.size() < 5)
		return;

	// FAZ 4 - 64-bit migration fix: uint string::npos'u truncate ediyor (0xFFFFFFFF),
	// `d != string::npos` her zaman true verince yanlis dala giriyor. size_t kullan.
	size_t a = text.find_first_of('(');
	size_t b = text.find_first_of(',');
	size_t c = text.find_first_of(')');

	size_t d = text.find_first_of('{', a);
	size_t e = text.find_first_of('}', d);

	bool bMonsterType = (d!=string::npos && e!=string::npos);

	if (a<b && b<c)
	{
		string name;
		
		if (bMonsterType)
		{
			name = trim(text.substr(d+1, e-d-1));
		}
		else
		{
			name = trim(text.substr(a+1, b-a-1));
		}

		Num  = atoi( trim(text.substr(b+1, c-b-1)).c_str() );

		//cout << "[MonsterCollectionInfo] " << name.c_str() << ", " << (int)Num << endl;

		if (bMonsterType)
		{
			MonsterType = atoi(name.c_str());

			// FAZ 4 - Veri butunlugu: bilinmeyen MonsterType referansi varsa
			// server'i kapatma, sadece warning bas + bu collection'i bos birak.
			try {
				g_pMonsterInfoManager->getMonsterInfo(MonsterType);
			} catch (Throwable& t) {
				cout << "[Warn] MonsterSummonInfo unknown MonsterType={" << MonsterType
					 << "}: " << t.toString().c_str() << endl;
				MonsterType = 0;
			}
		}
		else
		{
			SpriteType = g_pMonsterInfoManager->getSpriteTypeByName( name );

			if (SpriteType==0)
				SpriteType = atoi(name.c_str());
		}

		if (SpriteType==0 && MonsterType==0)
		{
			// FAZ 4 - Tanimlanamayan ifade: warning bas, devam et.
			cout << "[Warn] MonsterSummonInfo unparseable token: '"
				 << name.c_str() << "' — skip" << endl;
		}
	}
}

string MonsterCollectionInfo::toString() const
{
	StringStream msg;

	if (SpriteType!=0)
	{
		msg << "(" << (int)SpriteType << ", " << (int)Num << ")";
	}
	else if (MonsterType!=0)
	{
		msg << "({" << (int)MonsterType << "}, " << (int)Num << ")";
	}

	return msg.toString();
}

//////////////////////////////////////////////////////////////////////////////
//
// parseString( collectionString )
//
//////////////////////////////////////////////////////////////////////////////
// list<MonsterCollectionInfo> Infos;
//
//      a          bc a          b
// ex> "(����ٵ�,5), (�ʹ׵���,3)"      // ����ٵ� 5���� + �ʹ׵��� 3����
//////////////////////////////////////////////////////////////////////////////
void MonsterCollection::parseString(const string& text)
{
	Infos.clear();

	// FAZ 4 - 64-bit migration fix: uint string::npos'u truncate ediyor.
	size_t a = 0, b = 0, c = 0;

	while (1)
	{
		a = text.find_first_of('(', c);
		b = text.find_first_of(')', a);

		if (a==string::npos || b==string::npos)
			break;

		MonsterCollectionInfo mc;
		mc.parseString( text.substr(a, b-a+1) );

		//cout << "[MonsterCollection] " << text.substr(a,b-a+1).c_str() << endl;

		Infos.push_back( mc );

		c = text.find_first_of(',', b);

		if (c==string::npos)
			break;

		c++;	// ū ����� ������..
	}
}

string MonsterCollection::toString() const
{
	StringStream msg;

	list<MonsterCollectionInfo>::const_iterator itr = Infos.begin();

	while (itr!=Infos.end())
	{
		const MonsterCollectionInfo& mc = *itr;

		msg << mc.toString();

		itr++;

		if (itr!=Infos.end())
		{
			msg << ", ";
		}
	}

	return msg.toString();
}

//////////////////////////////////////////////////////////////////////////////
//
// getRandomMonsterCollection
//
//////////////////////////////////////////////////////////////////////////////
const MonsterCollection* MonsterSummonStep::getRandomMonsterCollection() const
{
	if (Collections.size()==0)
		return NULL;

	int pos = rand()%Collections.capacity();

	return &(Collections[pos]);
}

//////////////////////////////////////////////////////////////////////////////
//
// parseString( summonStepString )
//
//////////////////////////////////////////////////////////////////////////////
//	vector<MonsterCollection>  Collections;
//
// ex>  // ����ٵ�5+�ʹ׵���3 or ����ٵ�10+�ʹ׵���1
//
//       a                        b c a                         b
//     "[(����ٵ�,5), (�ʹ׵���,3) / (����ٵ�,10), (�ʹ׵���,1)]"
//
//////////////////////////////////////////////////////////////////////////////
void MonsterSummonStep::parseString(const string& text)
{
	Collections.clear();

	// FAZ 4 - 64-bit migration fix: int/uint string::npos truncate oluyor.
	size_t l = text.find_first_of('[');
	size_t r = text.find_first_of(']');

	if (l == string::npos || r == string::npos || l >= r)
		return;

	size_t a, b, c = 0;
	while (1)
	{
		a = text.find_first_of('(', c);
		if (a == string::npos) break;
		c = text.find_first_of('/', a);
		b = text.find_last_of(')', c);

		if (b == string::npos || b < a)
			break;

		MonsterCollection mc;
		mc.parseString( text.substr(a, b-a+1) );

		//cout << "[MonsterSummonStep] " << text.substr(a,b-a+1).c_str() << endl;

		Collections.push_back( mc );
	}
}

string MonsterSummonStep::toString() const
{
	StringStream msg;

	vector<MonsterCollection>::const_iterator itr = Collections.begin();

	msg << "[ ";

	while (itr!=Collections.end())
	{
		const MonsterCollection& mc = *itr;

		msg << mc.toString();

		itr++;

		if (itr!=Collections.end())
		{
			msg << " / ";
		}
	}

	msg << " ]";

	return msg.toString();
}


//////////////////////////////////////////////////////////////////////////////
//
// getRandomMonsterCollection
//
//////////////////////////////////////////////////////////////////////////////
const MonsterCollection* MonsterSummonInfo::getRandomMonsterCollection(int step) const
{
	if (Steps.empty() || step>=Steps.size())
		return NULL;

	return Steps[step].getRandomMonsterCollection();
}

bool MonsterSummonInfo::hasNextMonsterCollection(int step) const
{
	if (Steps.empty() || step>=Steps.size())
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////////
//
// parseString( summonInfoString )
//
//////////////////////////////////////////////////////////////////////////////
// vector<MonsterSummonStep>  Steps;
//
// ex>  // 1�ܰ�: ����ٵ�5+�ʹ׵���3 or ����ٵ�10+�ʹ׵���1
//      // 2�ܰ�: �ʹ׵���8+Ű��2 or �ʹ׵���3+����3

//      a                                                        b
//     "[(����ٵ�,5), (�ʹ׵���,3) / (����ٵ�,10), (�ʹ׵���,1)]
//      a                                               b
//      [(�ʹ׵���,8), (Ű��,2) / (�ʹ׵���,3), (����,3)]"
//
//////////////////////////////////////////////////////////////////////////////
void MonsterSummonInfo::parseString(const string& text)
{
	// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
	size_t a, b = 0;
	while (1)
	{
		a = text.find_first_of('[', b);
		if (a == string::npos) break;
		b = text.find_first_of(']', a);
		if (b == string::npos) break;

		MonsterSummonStep ms;
		ms.parseString( text.substr(a,b-a+1) );

		//cout << "[MonsterSummonInfo] " << text.substr(a,b-a+1).c_str() << endl;

		Steps.push_back( ms );
	}
}

string MonsterSummonInfo::toString() const
{
	StringStream msg;

	vector<MonsterSummonStep>::const_iterator itr = Steps.begin();

	while (itr!=Steps.end())
	{
		const MonsterSummonStep& mss = *itr;

		msg << mss.toString();

		itr++;
	}

	return msg.toString();
}


