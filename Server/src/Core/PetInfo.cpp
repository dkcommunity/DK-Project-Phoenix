#include "PetInfo.h"
#include "VSDateTime.h"

PetInfo::PetInfo()
{
	m_PetType=PET_NONE;
	m_PetLevel=0;
	m_PetAttrLevel=0;
	m_PetExp=0;
	m_PetHP=0;
	m_PetAttr=0;
	m_PetOption=0;
	m_PetOption2 = 0;	//1204 wlzzi - PetOption2 �߰�
	m_PetOption3 = 0;
	m_IsSummonInfo = 0;

	m_CanCutHead = 0;
	m_CanAttack = 0;
	m_CanGamble = 0;

//#ifdef __GAME_SERVER__
	m_FeedTurn = 1;
	m_CurrentFeedTurn = 0;
//#endif

	m_MagicRemainSec = 0;
	// 20070719
	m_EnchantSkillType = 0;
	m_ExpertPetChangerRemainSec = 0;	//1202 wlzzi �� ���� ����

	m_Function_Petfood_Type = FOOD_FUNCTION_NONE; // 20090109 wlzzi

	// FAZ 4 (2026-05-04 oturum 3): VSDateTime member'lar default constructor
	// uninitialized garbage birakir. PetInfo create ile DB INSERT'te
	// m_MagicExpireTime ve m_ExpertPetChangerExpireTime garbage year (orn
	// 2935093) MySQL "Incorrect datetime value" reddediyor → UNHANDLED
	// EXCEPTION → gameserver crash. Init currentDateTime ile.
	m_MagicExpireTime = VSDateTime::currentDateTime();
	m_ExpertPetChangerExpireTime = VSDateTime::currentDateTime();
	m_LastFeedTime = VSDateTime::currentDateTime();
}

PacketSize_t PetInfo::getSize() const
{
	if ( m_PetType == PET_NONE ) return szPetType;
	
	PacketSize_t szPetInfo = szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + m_Nickname.size() + szint + szCEffectID;
	szPetInfo += szint;
	szPetInfo += szBYTE;
	szPetInfo += m_OptionType.size() * szOptionType;
	szPetInfo += szOptionType;				//1204 wlzzi - PetOption2 �߰�
	szPetInfo += szOptionType;//pet option3 by rallser
	szPetInfo += szBYTE;					//20090108 wlzzi - Ǫ����� �߰�	
	szPetInfo += szBYTE + m_MixOptionType.size() * szOptionType;
	return szPetInfo;
}

PacketSize_t PetInfo::getMaxSize()
{
	PacketSize_t szMaxPetInfo = szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + 22 + szint + szCEffectID;
	szMaxPetInfo += szint;
	szMaxPetInfo += szBYTE;
	szMaxPetInfo += 10 * szOptionType;	//OptionType�� �ִ� 10��������.. �� �� �ִ�.
	szMaxPetInfo += szOptionType;			//PetOption2
	szMaxPetInfo += szOptionType;	//petoption 3 by rallser
	szMaxPetInfo += szBYTE;				//20090109 wlzzi - Ǫ����� �߰�
	szMaxPetInfo += szBYTE + 10 * szOptionType;	//OptionType�� �ִ� 10��������.. �� �� �ִ�. 
	return szMaxPetInfo;
//	return szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + 22 + szint + szint + szCEffectID + 10*szint;
}

void PetInfo::read(SocketInputStream& iStream) throw(ProtocolException, Error)
{
	__BEGIN_TRY

	iStream.read( m_PetType );

	if ( m_PetType == PET_NONE ) return;

	iStream.read( m_PetCreatureType );
	iStream.read( m_PetLevel );
	iStream.read( m_PetExp );

	iStream.read( m_PetHP );
	iStream.read( m_PetAttr );
	iStream.read( m_PetAttrLevel );
	iStream.read( m_PetOption );
	//1204 wlzzi
	iStream.read( m_PetOption2 );
	// -
	iStream.read( m_PetOption3 );// by rallser ,pet option 3
	iStream.read( m_PetFoodType );

	iStream.read( m_CanGamble );
	iStream.read( m_CanCutHead );
	iStream.read( m_CanAttack );

	iStream.read( m_IsSummonInfo );
	
	BYTE szSTR;
	iStream.read( szSTR );
	if ( szSTR != 0 ) iStream.read( m_Nickname, szSTR );

	ObjectID_t ItemObjectID;
	iStream.read( ItemObjectID );

	iStream.read( m_MagicRemainSec );
//	iStream.read( m_EnchantSkillType );
	iStream.read( m_ExpertPetChangerRemainSec );	//1202 wlzzi �� ���� ����
	iStream.read( m_EnchantSkillType );

//1203 wlzzi - OptionType �߰�
	BYTE optionSize;
	iStream.read( optionSize );
	for (int i=0; i<optionSize; i++)
	{
		OptionType_t optionType;
		iStream.read( optionType );
		addOptionType( optionType );
	}
//-
	iStream.read(m_Function_Petfood_Type);	//20090109 wlzzi Ǫ������߰�	
	
	//20090526 ksym555
	BYTE MixoptionSize;
	iStream.read( MixoptionSize );
	for (int i=0; i<MixoptionSize; i++)
	{
		OptionType_t MixoptionType;
		iStream.read( MixoptionType );
		addMixOptionType( MixoptionType );
	}
	
	__END_CATCH
}

void PetInfo::write(SocketOutputStream& oStream) const throw(ProtocolException, Error)
{
	__BEGIN_TRY

	oStream.write( m_PetType );

	if ( m_PetType == PET_NONE ) return;

	oStream.write( m_PetCreatureType );
	oStream.write( m_PetLevel );
	oStream.write( m_PetExp );

	oStream.write( m_PetHP );
	oStream.write( m_PetAttr );
	oStream.write( m_PetAttrLevel );
	oStream.write( m_PetOption );
//1204 wlzzi
	oStream.write( m_PetOption2 );
//	cout << "PetInfo::write - m_PetOption2 = " << (int)m_PetOption2 << endl;
// -
	oStream.write( m_PetOption3 );
//pet option3 by rallser,write it.
	oStream.write( m_PetFoodType );

	oStream.write( m_CanGamble );
	oStream.write( m_CanCutHead );
	oStream.write( m_CanAttack );

	oStream.write( m_IsSummonInfo );

	BYTE szSTR = m_Nickname.size();
	oStream.write( szSTR );
	if ( szSTR != 0 ) oStream.write( m_Nickname );

	ObjectID_t ItemObjectID = getItemObjectID();
	oStream.write( ItemObjectID );

	oStream.write( m_MagicRemainSec );
	oStream.write( m_ExpertPetChangerRemainSec );	//1202 wlzzi �� ���� ����
	oStream.write( m_EnchantSkillType );
//081203 wlzzi - OptionType�߰�
	BYTE optionSize = m_OptionType.size();
	oStream.write( optionSize );
//	cout << "PetInfo::write - m_OptionType.size() = " << (int)optionSize << endl;
	
	list<OptionType_t>::const_iterator itr = m_OptionType.begin();
	for (; itr!=m_OptionType.end(); itr++)
	{
		OptionType_t optionType = *itr;
		oStream.write( optionType );
//		cout << "PetInfo::write - m_OptionType = " << (int)optionType << endl;
	}
//-	
	oStream.write(m_Function_Petfood_Type);		//20090109 wlzzi Ǫ����� �߰�
	
	//20090526 ksym555
	BYTE MixoptionSize = m_MixOptionType.size();
	oStream.write( MixoptionSize );
		
	list<OptionType_t>::const_iterator itr2 = m_MixOptionType.begin();
	for (; itr2!=m_MixOptionType.end(); itr2++)
	{
		OptionType_t MixoptionType = *itr2;
		oStream.write( MixoptionType );
	}
		
	__END_CATCH
}

void PetInfo::setMagicExpireTime(const VSDateTime& time)
{
	m_MagicExpireTime = time;

	VSDateTime currentTime = VSDateTime::currentDateTime();

	int diffSecs = currentTime.secsTo( m_MagicExpireTime );
	if ( diffSecs < 0 )
		diffSecs = 0;

	m_MagicRemainSec = diffSecs;
}

void PetInfo::setExpertPetChangerExpireTime( const VSDateTime& time )
{
	m_ExpertPetChangerExpireTime = time;

	VSDateTime currentTime = VSDateTime::currentDateTime();

	int diffSecs = currentTime.secsTo( m_ExpertPetChangerExpireTime );
	if ( diffSecs < 0 )
		diffSecs = 0;

	m_ExpertPetChangerRemainSec = diffSecs;
}

PetInfo *PetInfo::createClone()
{
	PetInfo* pNewPetInfo = new PetInfo;
	
	pNewPetInfo->setPetType( this->getPetType() );
	pNewPetInfo->setPetCreatureType( this->getPetCreatureType() );
	pNewPetInfo->setPetLevel( this->getPetLevel() );
	pNewPetInfo->setPetExp( this->getPetExp() );
	pNewPetInfo->setPetHP( this->getPetHP() );
	pNewPetInfo->setPetAttr( this->getPetAttr() );
	pNewPetInfo->setPetAttrLevel( this->getPetAttrLevel() );
	pNewPetInfo->setPetOption( this->getPetOption() );
//1204 wlzzi - PetOption2 �߰�
	pNewPetInfo->setPetOption2( this->getPetOption2() );
//-
	pNewPetInfo->setPetOption3( this->getPetOption3() );
//by rallser ,get 3
	pNewPetInfo->setFoodType( this->getFoodType() );
	pNewPetInfo->setGamble( this->canGamble() );
	pNewPetInfo->setAttack( this->canAttack() );
	pNewPetInfo->setFeedTime( this->getLastFeedTime() );
	pNewPetInfo->setNickname( this->getNickname() );
	pNewPetInfo->setCutHead( this->canCutHead() );
	pNewPetInfo->setEnchantSkill( this->getEnchantSkill() );
	//2008.1202 wlzzi - Expert Pet Changer �� ���� ����
	//	�������� - m_ExpertPetChangerRemainSec, m_ExpertPetChangerExpireTime �� ����ؾ� �Ѵ�.
	pNewPetInfo->setExpertPetChangerExpireTime (this->getExpertPetChangerExpireTime() );
	pNewPetInfo->setExpertPetChangerRemainSec (this->getExpertPetChangerRemainSec() );
	
	//2008.1203 wlzzi - OptionType�߰�
	pNewPetInfo->setOptionType( this->getOptionType() );
	//20090109 Ǫ����� �߰�
	pNewPetInfo->setFunctionPetfoodType (this->getFunctionPetfoodType ());
	//20090526 ksym555
	pNewPetInfo->setMixOptionType( this->getMixOptionType() );
	
	return pNewPetInfo; 
}

string PetInfo::toString() const
{
	StringStream msg;
	
	msg << "PetInfo("
		<< "PetType : " << (int)m_PetType
		<< ", PetLevel : " << (int)m_PetLevel
		<< ", PetExp : " << (int)m_PetExp
		<< ", PetHP : " << (int)m_PetHP
		<< ", PetAttr : " << (int)m_PetAttr
		<< ", PetOption : " << (int)m_PetOption
		<< ", PetFoodType : " << (int)m_PetFoodType
		<< ", EnchantSkillType : " << (int)m_EnchantSkillType
		<< ")";

	return msg.toString();
}
