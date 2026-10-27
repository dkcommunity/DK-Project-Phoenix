//////////////////////////////////////////////////////////////////////////////
// Filename    : CLCreatePCHandler.cc
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLCreatePC.h"

#ifdef __LOGIN_SERVER__
	#include "LoginPlayer.h"
	#include "PCSlayerInfo.h"
	#include "Assert.h"
	#include "GameServerInfoManager.h"
	#include "DB.h"
	#include <list>
	#include "Lpackets/LCCreatePCOK.h"
	#include "Lpackets/LCCreatePCError.h"
	#include <string.h>

	#include "chinabilling/CBillingInfo.h"
	#include "SystemAvailabilitiesManager.h"
	#include "Utility.h"
#endif

#include <vector>

bool isAvailableID(const char* pID);

#ifdef __THAILAND_SERVER__
// tis620 charset filter functions
int extNumberic(string srcStr);
int extEnglish(string srcStr);
int extAsciiSpecial(string srcStr);
int extTis620Normal(string srcStr);
bool isAllowString(string str);
#endif

#ifdef __CHINA_SERVER__
// GB2312(simple chinese) charset filter functions 
int extNumberic(string srcStr);
int extEnglish(string srcStr);
int extGb2312Normal(string srcStr);
int extGb2312Special(string srcStr);
int extAsciiSpecial(string srcStr);
bool isAllowString(string str);
#endif


class GiveItem	
{
public :
	GiveItem(int iGoodID, int iGoodCount)
	{
		m_iGoodID = iGoodID;
		m_iGoodCount = iGoodCount;
	}

	int m_iGoodID;
	int m_iGoodCount;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CLCreatePCHandler::execute (CLCreatePC* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __LOGIN_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

#ifdef __GLOBAL_SERVER__
	SYSTEM_BEGIN_IF_NOT(SYSTEM_CREATE_OUSTERS)
	{
		if ( pPacket->getRace() == RACE_OUSTERS )
			return;
	}
	SYSTEM_END
#endif

	LoginPlayer*    pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
	Statement*      pStmt        = NULL;
	Statement*      pLoginStmt = NULL;
	Result*         pResult      = NULL;
	LCCreatePCError lcCreatePCError;
	WorldID_t		WorldID      = pLoginPlayer->getWorldID();

	string connectIP = pLoginPlayer->getSocket()->getHost();
	string strName = pPacket->getName();
	int iSlot = pPacket->getSlot() + 1;

    // SQL Injection ���
    if ( CheckSQLInjectionString(strName) )
    {
        lcCreatePCError.setErrorID(ETC_ERROR);

        filelog("Attack_SQLInjection.log", "%s CLCreatePC : [PlayerID=%s Name=%s Slot=%d]",
       		connectIP.c_str(),
			pPlayer->getID().c_str(), 
			strName.c_str(),
			iSlot
		);
        
        return;
    }
    
    if ( (iSlot < 1) ||(iSlot > SLOT_MAX) )
    {
    	lcCreatePCError.setErrorID(ETC_ERROR);
    	
    	filelog("Attack_DataManipulation.log", "%s CLCreatePC : [PlayerID=%s Name=%s Slot=%d]",
   			connectIP.c_str(),
			pPlayer->getID().c_str(), 
			strName.c_str(),
			iSlot
		);
    	
    	return;
    }
    
    
#ifdef __METRO_SERVER__
	// ���� ���տ� �ӽ� ���� �ڵ�
	// Ʈ���Ǻ��Ͼư� �ƴ� ��� �ű� ����(2005��12��26�� ���� ����)�� �ɸ� ������ �����ϴ�.
	/*if ( WorldID != 2 )
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery( "SELECT PlayerID FROM Player WHERE PlayerID = '%s' AND creation_date > '2005-12-14'", pLoginPlayer->getID().c_str() );

			if ( pResult->getRowCount() == 0 )
			{
				lcCreatePCError.setErrorID( NOT_AVAILABLE );
				pLoginPlayer->sendPacket( &lcCreatePCError );

				SAFE_DELETE(pStmt);
				return;
			}
		}
		END_DB(pStmt)
	}*/
#endif

#ifdef __CONNECT_CBILLING_SYSTEM__
	if ( pLoginPlayer->isCBillingVerified() )
	{
		if ( !pLoginPlayer->isPayPlayer() )
		{
			lcCreatePCError.setErrorID( CANNOT_CREATE_PC_BILLING );
			pLoginPlayer->sendPacket( &lcCreatePCError );
			return;
		}
	}
	else
	{
		lcCreatePCError.setErrorID( CANNOT_AUTHORIZE_BILLING );
		pLoginPlayer->sendPacket( &lcCreatePCError );
		return;
	}
#endif

	try 
	{
		pStmt = g_pDatabaseManager->getConnection( WorldID )->createStatement();
		pLoginStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		// �ý��ۿ��� ����ϰų�, ������ �̸��� �ƴѱ� �����Ѵ�.
		// NONE, ZONE***, INV***, QUICK...
		//string text = pPacket->getName();

		if (!isAvailableID( pPacket->getName().c_str() ))
		{
			lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
		}

		if ( pPacket->getName().find(" ") != string::npos ||
			pPacket->getName().find((char)0xFF) != string::npos )
		{
			lcCreatePCError.setErrorID(ETC_ERROR);
	        throw DuplicatedException("������� �ʴ� ���ڰ� ���ԵǾ� ����");
		}
		
		/*
		list<string>::const_iterator itr = InvalidTokenList.begin();
		for (; itr != InvalidTokenList.end(); itr++)
		{
			if (text.find(*itr) != string::npos)
			{
				lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
				throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
			}
		}
		*/
#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

        /*
         * �±��� ���ڼ� tis620 thailand charset ���� ����ϴ� ���ڸ��� �ɸ��� �̸�����
         * �� �� �ֵ��� �����Ѵ�.
         *
         * */

         /*
         * �߱��� �����ڵ�� gb2312-simple chinese ���� ����ϴ� ���ڸ��� �ɸ��� �̸�����
         * �� �� �ֵ��� �����Ѵ�.
         *
         * */

       string tmpStr = pPacket->getName();
        bool isAllowStr = isAllowString(tmpStr);

        if(isAllowStr == false)
        {
            lcCreatePCError.setErrorID(ETC_ERROR);
            throw DuplicatedException("������� �ʴ� ���ڰ� ���ԵǾ� ����");

        }

#endif

		pResult = pLoginStmt->executeQuery("SELECT Name FROM Slayer WHERE Name = '%s'", pPacket->getName().c_str());
		if (pResult->getRowCount() != 0) 
		{
			lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
		}
		// �̹� �����ϴ� ĳ���� �̸��� �ƴ��� �����Ѵ�.
		///*
		pResult = pStmt->executeQuery("SELECT Name FROM Slayer WHERE Name = '%s'", pPacket->getName().c_str());
		if (pResult->getRowCount() != 0) 
		{
			lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
		}

		// �ش� ������ ĳ���Ͱ� �̹� ������ ������ �����Ѵ�.
		pResult = pStmt->executeQuery("SELECT Name FROM Slayer WHERE PlayerID ='%s' and Slot ='%s' AND Active='ACTIVE'", pLoginPlayer->getID().c_str(), Slot2String[pPacket->getSlot()].c_str() );
		if (pResult->getRowCount() != 0) 
		{
			lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
		}
		//*/
		// �� ������ �ϳ���. 2002. 7. 13 by sigi. �̰� ������. - -;
		/*
		pResult = pStmt->executeQuery("SELECT Name FROM Slayer WHERE Name='%s' OR PlayerID='%s' AND Slot='%s'", 
											pPacket->getName().c_str(), 
											pLoginPlayer->getID().c_str(), 
											Slot2String[pPacket->getSlot()].c_str() );

		if (pResult->getRowCount() != 0) 
		{
			lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
		}
		*/

#ifdef __METRO_SERVER__
		// ���� ���տ� �ӽ� ���� �ڵ�
		// ���� ���� ��û ���̺����� ��û�� ĳ������ �ִ��� �˻��Ѵ�.
		if ( WorldID != 2 )
		{
			pResult = pStmt->executeQuery( "SELECT FixName FROM NameChange WHERE FixName = '%s'", pPacket->getName().c_str() );

			if ( pResult->getRowCount() != 0 )
			{
				lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
				throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
			}

/*			pResult = pStmt->executeQuery( "SELECT NewName FROM ChangeWorld WHERE NewName = '%s'", pPacket->getName().c_str() );

			if ( pResult->getRowCount() != 0 )
			{
				lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
				throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
			}

			pResult = pStmt->executeQuery( "SELECT NewName FROM WebTemp WHERE NewName = '%s'", pPacket->getName().c_str() );
			
			if ( pResult->getRowCount() != 0 )
			{
				lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
				throw DuplicatedException("�̹� �����ϴ� ���̵��Դϴ�.");
			}*/
		}
#endif

		// �߸��� �ɷ�ġ�� ������ ĳ���͸� �����Ϸ� �ϴ� ���� �ƴ��� �����Ѵ�.
		

		bool bInvalidAttr = false;
		int  nSTR         = pPacket->getSTR();
		int  nSTRExp      = 0;
		int  nSTRGoalExp  = 0;
		int  nDEX         = pPacket->getDEX();
		int  nDEXExp      = 0;
		int  nDEXGoalExp  = 0;
		int  nINT         = pPacket->getINT();
		int  nINTExp      = 0;
		int  nINTGoalExp  = 0;

		//int  Rank         = 1;
		//int  RankExp      = 0;

		// 
		static int  RankGoalExpSlayer  = -1;
		static int  GoalExpVampire  = -1;	// by sigi. 2002.12.20
		static int  RankGoalExpVampire  = -1;
		static int  GoalExpOusters = -1;
		static int  RankGoalExpOusters = -1;

		if (RankGoalExpSlayer==-1)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=0");
			if (pResult->next()) RankGoalExpSlayer = pResult->getInt(1);
		}

		if (GoalExpVampire==-1)	// by sigi. 2002.12.20
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM VampEXPBalanceInfo WHERE Level=1");
			if (pResult->next()) GoalExpVampire = pResult->getInt(1);
		}

		if (GoalExpOusters==-1)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM OustersEXPBalanceInfo WHERE Level=1");
			if (pResult->next()) GoalExpOusters = pResult->getInt(1);
		}

		if (RankGoalExpVampire==-1)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=1");
			if (pResult->next()) RankGoalExpVampire = pResult->getInt(1);
		}

		if (RankGoalExpOusters==-1)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=2");
			if (pResult->next()) RankGoalExpOusters = pResult->getInt(1);
		}

		if (pPacket->getRace() == RACE_SLAYER)
		{
			if (nSTR < 5 || nSTR > 20)  bInvalidAttr = true;
			if (nDEX < 5 || nDEX > 20)  bInvalidAttr = true;
			if (nINT < 5 || nINT > 20)  bInvalidAttr = true;
			if (nSTR + nDEX + nINT > 30) bInvalidAttr = true;

			//cout << "Slayer: " << nSTR << ", " << nDEX << ", " << nINT << endl;
		}
		else if ( pPacket->getRace() == RACE_VAMPIRE )	// vampire�� ���. ������ 20. by sigi. 2002.10.31
		{
			if (nSTR != 20
				|| nDEX != 20
				|| nINT != 20) 
			{
				bInvalidAttr = true;
			}
			else
			{
				// �������� vampire�� ���
				// Slayer�� �ɷ�ġ�� �ٽ� ��������� �Ѵ�. -_-;
				// by sigi. 2002.11.7
				nSTR = 5 + rand()%16;	// 5~20
				nDEX = 5 + rand()%(21-nSTR);
				nINT = 30 - nSTR - nDEX;

				pPacket->setSTR(nSTR);
				pPacket->setDEX(nDEX);
				pPacket->setINT(nINT);
			}
			// 

			//cout << "Vampire: " << nSTR << ", " << nDEX << ", " << nINT << endl;
		}
		else if ( pPacket->getRace() == RACE_OUSTERS )
		{
/*			if ( nSTR < 10 ) bInvalidAttr = true;
			if ( nDEX < 10 ) bInvalidAttr = true;
			if ( nINT < 10 ) bInvalidAttr = true;*/

			if ( nSTR < 10 || nDEX < 10 || nINT < 10 )
			{
				filelog("CreatePC.log", "Illegal PC Create [%s:%s] : %u/%u/%u", pPlayer->getID().c_str(), pPacket->getName().c_str(), nSTR, nDEX, nINT);
			}

			if ( nSTR + nDEX + nINT != 45 )
				bInvalidAttr = true;
		}

		if (bInvalidAttr)
		{
			SAFE_DELETE(pStmt);
			throw InvalidProtocolException("CLCreatePCHandler::too large character attribute");
		}

		// ��� �˻縦 �����ߴٸ� ���� ĳ���͸� �����Ѵ�.
		ServerGroupID_t CurrentServerGroupID = pPlayer->getServerGroupID();

		// ������.. �ϴ� ������ ���Ϸ��� static���� ������ �ߴ�.
		// ���߿� �ƿ� �α��� ���� �㶧�� ����ġ table���� loading�� �ε����ؾ��� ���̴�. 2002.7.13 by sigi
		static int STRGoalExp[100] = { 0, }; 
		static int STRAccumExp[100] = { 0, };
		static int DEXGoalExp[100] = { 0, }; 
		static int DEXAccumExp[100] = { 0, };
		static int INTGoalExp[100] = { 0, };
		static int INTAccumExp[100] = { 0, };

		nSTRGoalExp = STRGoalExp[nSTR];
		if (nSTRGoalExp==0)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM STRBalanceInfo WHERE Level = %d", nSTR);
			if (pResult->next()) nSTRGoalExp = STRGoalExp[nSTR] = pResult->getInt(1);
		}

		nSTRExp = STRAccumExp[nSTR-1];
		if (nSTRExp==0)
		{
			pResult = pStmt->executeQuery("SELECT AccumExp FROM STRBalanceInfo WHERE Level = %d", nSTR - 1);
			if (pResult->next()) nSTRExp = STRAccumExp[nSTR-1] = pResult->getInt(1);
		}

		nDEXGoalExp = DEXGoalExp[nDEX];
		if (nDEXGoalExp==0)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM DEXBalanceInfo WHERE Level = %d", nDEX);
			if (pResult->next()) nDEXGoalExp = DEXGoalExp[nDEX] = pResult->getInt(1);
		}

		nDEXExp = DEXAccumExp[nDEX-1];
		if (nDEXExp==0)
		{
			pResult = pStmt->executeQuery("SELECT AccumExp FROM DEXBalanceInfo WHERE Level = %d", nDEX - 1);
			if (pResult->next()) nDEXExp = DEXAccumExp[nDEX-1] = pResult->getInt(1);
		}

		nINTGoalExp = INTGoalExp[nINT];
		if (nINTGoalExp==0)
		{
			pResult = pStmt->executeQuery("SELECT GoalExp FROM INTBalanceInfo WHERE Level = %d", nINT);
			if (pResult->next()) nINTGoalExp = INTGoalExp[nINT] = pResult->getInt(1);
		}

		nINTExp = INTAccumExp[nINT-1];
		if (nINTExp==0)
		{
			pResult = pStmt->executeQuery("SELECT AccumExp FROM INTBalanceInfo WHERE Level = %d", nINT - 1);
			if (pResult->next()) nINTExp = INTAccumExp[nINT-1] = pResult->getInt(1);
		}

		// �ϴ� ������ ����.. ��/�� ���и�..
		DWORD slayerShape = (pPacket->getSex()==1? 1 : 0);
		DWORD vampireShape = slayerShape;

		slayerShape |= (pPacket->getHairStyle() << PCSlayerInfo::SLAYER_BIT_HAIRSTYLE1);

		Color_t	HelmetColor = 0;
		Color_t JacketColor = 0;
		Color_t	PantsColor  = 0;
		Color_t	WeaponColor = 0;
		Color_t	ShieldColor = 0;

		/*
		StringStream slayerSQL;
		slayerSQL << "INSERT INTO Slayer ("
			<< " Race, Name, PlayerID, Slot, ServerGroupID, Active,"
			<< " Sex, HairStyle, HairColor, SkinColor, Phone, "
			<< " STR, STRExp, STRGoalExp, DEX, DEXExp, DEXGoalExp, INTE, INTExp, INTGoalExp, HP, CurrentHP, MP, CurrentMP,"
			<< " ZoneID, XCoord, YCoord, Sight, Gold, Alignment,"
			<< " Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor,"
			<< " creation_date) VALUES ('"
			<< "SLAYER" << "', '"
			<< pPacket->getName() << "', '"
			<< pLoginPlayer->getID() << "', '"
			<< Slot2String[pPacket->getSlot()] << "', "
			<< (int)CurrentServerGroupID << " , "
			<< "'ACTIVE', '"
			<< Sex2String[pPacket->getSex()] << "', '"
			<< HairStyle2String[pPacket->getHairStyle()] << "', "
			<< (int)pPacket->getHairColor() << ", "
			<< (int)pPacket->getSkinColor() << ", '"
			<< (int)0 << "', "
			<< (int)pPacket->getSTR() << ", "
			<< nSTRExp << ", "
			<< nSTRGoalExp << ", "
			<< (int)pPacket->getDEX() << ", "
			<< nDEXExp << ", "
			<< nDEXGoalExp << ", "
			<< (int)pPacket->getINT() << ", "
			<< nINTExp << ", "
			<< nINTGoalExp << ", "
			<< (int)pPacket->getSTR()*2 << ","
			<< (int)pPacket->getSTR()*2 << ","
			<< (int)pPacket->getINT()*2 << ","
			<< (int)pPacket->getINT()*2 << ","
			<< "2101, 65, 45, 13, 0, 7500, "
			<< slayerShape << ", "
			<< (int)HelmetColor << ", "
			<< (int)JacketColor << ", "
			<< (int)PantsColor << ", "
			<< (int)WeaponColor << ", "
			<< (int)ShieldColor << ", "
			<< "now() "
			<< ")";

		StringStream vampireSQL;
		vampireSQL << "INSERT INTO Vampire ("
			<< " Name, PlayerID, Slot, ServerGroupID, Active,"
			<< " Sex, HairColor, SkinColor,"
			<< " STR, DEX, INTE, HP, CurrentHP,"
			<< " ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, Shape) VALUES ('"
			<< pPacket->getName() << "', '"
			<< pLoginPlayer->getID() << "', '"
			<< Slot2String[pPacket->getSlot()] << "', "
			<< (int)CurrentServerGroupID << " , "
			<< "'ACTIVE', '"
			<< Sex2String[pPacket->getSex()] << "', "
			<< (int)pPacket->getHairColor() << ", "
			<< (int)pPacket->getSkinColor() << ", "
			<< "20, 20, 20, 50, 50, "
			<< "2020, 233, 55, 13, 7500, 0, 125, "
			<< vampireShape
			<< ")";

		pStmt->executeQuery(slayerSQL.toString());
		pStmt->executeQuery(vampireSQL.toString());
		*/

		// ĳ���� �����ÿ� �����̾ ������ �� �ִ�.
		// by sigi. 2002.10.31
		string race;
		BYTE Job = (int)pPacket->getJob();
		BYTE Job2 = 0;
		int nowstr = 0;
		int nowdex = 0;
		int nowinte = 0;
		int blaze = 0;
		int sword = 0;
		int gun = 0;
		int heal = 0;
		int enchant = 0;
		int AdvancementClass =0;
		int Bonus =0;
		if(Job >4 || Job <0)
		{
			Job = 0;
		}
		switch ( pPacket->getRace() )
		{
			case RACE_SLAYER:
				race = "SLAYER";
				Job2 = Job;
				// 2026-05-10: AdvancementClass=0 (Domain Level/JC oncesi mode)
				// AC=1 = JC Class (oyunun ikinci asamasi). Yeni karakter Domain
				// Level'den baslar (Level 1-150), 150'de quest tetiklenir, NPC'den
				// AC=1'e gecis yapilir. Hardcoded 1 = oyuncuyu yanlis class'ta
				// baslatiyordu.
				AdvancementClass = 0;
				Bonus = 0;
				break;
			case RACE_VAMPIRE:
				race = "VAMPIRE";
				Job2 = 100;
				break;
			case RACE_OUSTERS:
				race = "OUSTERS";
				Job2 = 100;
				break;
			default:
				cout << "send CLCreatePCError : " << "���� ���� ����" << endl;

				lcCreatePCError.setErrorID(ETC_ERROR);
				pLoginPlayer->sendPacket(&lcCreatePCError); // Ŭ���̾�Ʈ���� PC ���� ���� ��Ŷ�� ������.
				return;
		}

		// 2006.2.22 Slayer�� ���� ��ġ X=237, Y=138���� X=209, Y=104�� ���� �Ǿ���.
		pStmt->executeQuery(
				"INSERT INTO Slayer (Race, Name, PlayerID, Slot, ServerGroupID, Active, Sex, HairStyle, HairColor, SkinColor, Phone, STR, STRExp, STRGoalExp, DEX, DEXExp, DEXGoalExp, INTE, INTExp, INTGoalExp, `Rank`, RankExp, RankGoalExp, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Gold, Alignment, Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor,Job,AdvancementClass,Bonus,BladeLevel,SwordLevel,GunLevel,HealLevel,EnchantLevel, creation_date) VALUES ('%s', '%s', '%s', '%s', %d, 'ACTIVE', '%s', '%s', %d, %d, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 64, 29, 222, 13, 0, 7500, %d, %d, %d, %d, %d, %d,%d,%d,%d,%d,%d,%d,%d,%d,now())",
				race.c_str(),
				pPacket->getName().c_str(), 
				pLoginPlayer->getID().c_str(), 
				Slot2String[pPacket->getSlot()].c_str(), 
				(int)CurrentServerGroupID,
				Sex2String[pPacket->getSex()].c_str(), 
				HairStyle2String[pPacket->getHairStyle()].c_str(), 
				(int)pPacket->getHairColor(), 
				(int)pPacket->getSkinColor(),
				(int)pPacket->getSTR()+nowstr, nSTRExp, nSTRGoalExp, 
				(int)pPacket->getDEX()+nowdex, nDEXExp, nDEXGoalExp, 
				(int)pPacket->getINT()+nowinte, nINTExp, nINTGoalExp,
				1, 0, RankGoalExpSlayer,
				(int)pPacket->getSTR()*2,
				(int)pPacket->getSTR()*2,
				(int)pPacket->getINT()*2,
				(int)pPacket->getINT()*2,
				slayerShape,
				(int)HelmetColor,
				(int)JacketColor,
				(int)PantsColor,
				(int)WeaponColor,
				(int)ShieldColor,
				Job2,
				AdvancementClass,
				Bonus,
				blaze,
				sword,
				gun,
				heal,
				enchant);

		// ���� ��ġ ����. by sigi. 2002.10.31
		// �ƿ콺�ͽ����� ������ ������ �����Ƿ� ���߿� �ϳ��� �����.
		// �ٵ� ���� ������ ������ ������ �𸥴ٴ� �ұ��� ������ ���
		// �׻� �׷� �������� �¾� �Ա⶧���� ������ �� �ּ��� ���� �Ѵ� Ǯ���ִ°�....����~~
		if (  pPacket->getRace() != RACE_OUSTERS )
		{
			// 2026-05-10: AdvancementClass=0 (Domain Level mode), Level=1 default.
			// Trailing `377,1,0,1` (CoatColor=377, AdvancementClass=1, Bonus=0, Level=1)
			// → `377,0,0,1` (AC=0). Vampire JC oncesi normal Domain Level sistemiyle
			// baslayacak; AC=0 default (advancement happen via NPC quest at Level 150).
			pStmt->executeQuery(
					"INSERT INTO Vampire ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, SkinColor, STR, DEX, INTE, HP, CurrentHP, ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, `Rank`, RankExp, RankGoalExp, Shape, CoatColor,AdvancementClass,Bonus,Level) VALUES ( '%s', '%s', '%s', %d, 'ACTIVE', '%s', %d, 20, 20, 20, 50, 50, 1003, 62, 64, 13, 7500, 0, %d, 1, 0, %d, %d, 377,0,0,1 )",
					pPacket->getName().c_str(),
					pLoginPlayer->getID().c_str(),
					Slot2String[pPacket->getSlot()].c_str(),
					(int)CurrentServerGroupID,
					Sex2String[pPacket->getSex()].c_str(),
					(int)pPacket->getSkinColor(),
					GoalExpVampire,	// by sigi. 2002.12.20
					RankGoalExpVampire,
					vampireShape);
		} else
		{
			// 2026-05-10: AdvancementClass=0 (Domain Level mode), Level=1 default.
			// Trailing `377,1,1,0` (BootsColor=377, AdvancementClass=1, Level=1,
			// SkillBonus=0) → `377,0,1,0` (AC=0). Ousters JC oncesi Domain Level
			// sistemiyle baslayacak.
			pStmt->executeQuery(
					"INSERT INTO Ousters ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, STR, DEX, INTE, BONUS, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, `Rank`, RankExp, RankGoalExp, CoatColor, HairColor, ArmColor, BootsColor,AdvancementClass,Level,SkillBonus ) Values ( '%s', '%s', '%s', %d, 'ACTIVE', 'FEMALE', %d, %d, %d, 0, 50, 50, 50, 50, 1311, 24, 73, 13, 7500, 0, %d, 1, 0, %d, 377, %d, 377, 377,0,1,0 )",
					pPacket->getName().c_str(),
					pLoginPlayer->getID().c_str(),
					Slot2String[pPacket->getSlot()].c_str(),
					(int)CurrentServerGroupID,
					(int)pPacket->getSTR(),
					(int)pPacket->getDEX(),
					(int)pPacket->getINT(),
					GoalExpOusters,
					RankGoalExpOusters,
					(int)pPacket->getHairColor() );
		}

		if (pPacket->getRace()==RACE_SLAYER)
		{
			pStmt->executeQuery("INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES ('%s','11110010001')", pPacket->getName().c_str());
		}
		else if(pPacket->getRace() == RACE_OUSTERS) //by viva  newbie item
		{
			pStmt->executeQuery("INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES ('%s','00000010001')", pPacket->getName().c_str());
		}
		else
		{
			pStmt->executeQuery("INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES ('%s','00000010001')", pPacket->getName().c_str());
		}

		// Ŭ���̾�Ʈ���� PC ���� ���� ��Ŷ�� ������.
		LCCreatePCOK lcCreatePCOK;
		pLoginPlayer->sendPacket(&lcCreatePCOK);
		pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

	} 
	catch (DuplicatedException & de) 
	{
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pLoginStmt);
		pLoginPlayer->sendPacket(&lcCreatePCError); // Ŭ���̾�Ʈ���� PC ���� ���� ��Ŷ�� ������.
	} 
	catch (SQLQueryException & sqe) 
	{
		SAFE_DELETE(pStmt);
		SAFE_DELETE(pLoginStmt);
		cout << "send CLCreatePCError : " << "Exception �߻�" << endl;

		lcCreatePCError.setErrorID(ETC_ERROR);
		pLoginPlayer->sendPacket(&lcCreatePCError); // Ŭ���̾�Ʈ���� PC ���� ���� ��Ŷ�� ������.
	}

	SAFE_DELETE(pStmt);
	SAFE_DELETE(pLoginStmt);
#endif

	__END_DEBUG_EX __END_CATCH
}

bool
isAvailableID(const char* pID)
{
	const int maxInvalidID = 11;
	static const char* invalidID[maxInvalidID] =
	{
		"NONE",
		"������",
		"�����",
		"�����",
		"�",
		"��ȹ��",
		"������",
		"�׽���",
		"����",
		"GM",
		"NMCH"
	};
	
	// �� ������. - -; 2002.7.13 by sigi.
	for (int i=0; i<maxInvalidID; i++)
	{
		if (strstr(pID, invalidID[i])!=NULL) 
		{
			return false;
		}
	}

	return true;
}

#ifdef __THAILAND_SERVER__
int extNumberic(string srcStr)
{
    unsigned char   ch;
    int nNumChar    = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for ( unsigned int i=0; i < srcStr.size(); i++ )
        {
            ch = srcStr[i];
            if(ch >= '0' && ch <='9')
            {
                if(i == 0) { return -1; }
                else { nNumChar++; }
            }
        }
    }

    return nNumChar;
}
int extEnglish(string srcStr)
{
    unsigned char   ch;
    int nEngChar    = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for (  unsigned int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];
            if(ch >=97 && ch <=122)
            {
                nEngChar++;
            }
            else if(ch >=65 && ch <=90)
            {
                nEngChar++;
            }
        }
    }

    return nEngChar;
}
int extAsciiSpecial(string srcStr)
{
    unsigned char   ch;
    int nASPCChar   = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for ( int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];

            if(ch >=0x00 && ch <=47)
            {
                nASPCChar++;
            }
            else if(ch >=58 && ch <=64)
            {
                nASPCChar++;
            }
            else if(ch >=91 && ch <=96)
            {
                nASPCChar++;
            }
            else if(ch >=123 && ch <=160)
            {
                nASPCChar++;
            }
            else if(ch >=219 && ch <=222)
            {
                nASPCChar++;
            }
            else if(ch >=252)
            {
                nASPCChar++;
            }

        }
    }

    return nASPCChar;
}
int extTis620Normal(string srcStr)
{
    unsigned char   ch;
    int nNorChar   = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for ( int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];

            if(ch >=161  && ch <=218 )
            {
                nNorChar++;
            }
            else if(ch >=223 && ch <=251)
            {
                nNorChar++;
            }
        }
    }

    return nNorChar;

}

bool    isAllowString(string str)
{
    bool isAllow = true;

    if(str.size() <= 0) isAllow = false;
    else
    {
        int nEng = extEnglish(str);
        int nNum = extNumberic(str);
        int nASpc = extAsciiSpecial(str);
        int nNor = extTis620Normal(str);

        if(nNum == -1)  isAllow = false;      // ���ڿ��� ó���� ���ڷ� �����ϸ�
        if(nEng && nNor)isAllow = false;      // �Ϲݹ��ڿ� ������ ���� �ִٸ�
        if(nASpc)       isAllow = false;      // Ascii�������� Ư�����ڰ� �߰ߵǸ�
    }

    return isAllow;
}

#endif


#ifdef __CHINA_SERVER__
// ���ڿ��� ���ڰ� � �ִ��� ã��
// return : -1 ���ڿ��� ó���� ���ڰ� �ִ�.
// return : 0  ���ڿ����� ���ڸ� ã�� ���ߴ�.
// return : x > 0 ���ڿ����� 1�� �̻��� ���ڸ� ã�Ҵ�.
int extNumberic(string srcStr)
{
    unsigned char   ch;
    int nNumChar    = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for ( unsigned int i=0; i < srcStr.size(); i++ )
        {
            ch = srcStr[i];
            if(ch >= '0' && ch <='9')
            {
                if(i == 0) { return -1; }
                else { nNumChar++; }
            }
        }
    }

    return nNumChar;
}


// ���ڿ����� ���ڰ� � �ִ��� ã�´�
// return : 0 ���ڿ����� ���ڸ� ã�� ���ߴ�.
// return : x > 0 ���ڿ����� 1�� �̻��� ���ڸ� ã�Ҵ�. x�� ã�� ����
int extEnglish(string srcStr)
{
    unsigned char   ch;
    int nEngChar    = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for (  unsigned int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];
            if(ch >=97 && ch <=122)
            {
                nEngChar++;
            }
            else if(ch >=65 && ch <=90)
            {
                nEngChar++;
            }
        }
    }

    return nEngChar;
}
// GB2312 �ڵ���� ���ڰ� � �ִ��� �ִ��� ã�´�.
int extGb2312Normal(string srcStr)
{
    unsigned char   ch;
    int nNormalChar = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for (  unsigned int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];
            if(ch >= 0xB0 && ch <= 0xF7)
            {
                unsigned char ch2 = srcStr[i+1];
                if(ch2 > 0xA0 && ch < 0xFF)
                {
                    nNormalChar++;
                    i++;
                }
            }
            else if((ch >= 0xA1 && ch <= 0xAF) || (ch >=0xF8 && ch <= 0xFE))
            {
                unsigned char ch2 = srcStr[i+1];
                if(ch2 >= 0xA0 && ch <= 0xFF)
                {
                    i++;
                }
            }

        }
    }
    return nNormalChar;
}
int extGb2312Special(string srcStr)
{
    unsigned char   ch;
    int nSpecialChar    = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for (  unsigned int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];
            if(ch >= 0xB0 && ch <= 0xF7)
            {
                unsigned char ch2 = srcStr[i+1];
                if(ch2 > 0xA0 && ch < 0xFF)
                {
                    i++;
                }
            }
            else if((ch >= 0xA1 && ch <= 0xAF) || (ch >=0xF8 && ch <= 0xFE))
            {
                unsigned char ch2 = srcStr[i+1];
                if(ch2 >= 0xA0 && ch <= 0xFF)
                {
                    nSpecialChar++;
                    i++;
                }
            }
        }
    }
    return nSpecialChar;
}
int extAsciiSpecial(string srcStr)
{
    unsigned char   ch;
    int nASPCChar   = 0;

    if(srcStr.size() <= 0 ) return 0;
    else
    {
        for ( int i=0; i < srcStr.size(); i++)
        {
            ch = srcStr[i];

            if(ch >=0x00 && ch <=47)
            {
                nASPCChar++;
            }
            else if(ch >=58 && ch <=64)
            {
                nASPCChar++;
            }
            else if(ch >=91 && ch <=96)
            {
                nASPCChar++;
            }
            else if(ch >=123 && ch <=127)
            {
                nASPCChar++;
            }

        }
    }

    return nASPCChar;
}
bool    isAllowString(string str)
{
    bool isAllow = true;

    if(str.size() <= 0) isAllow = false;
    else
    {
        int nEng = extEnglish(str);
        int nNum = extNumberic(str);
        int nNor = extGb2312Normal(str);
        int nSpc = extGb2312Special(str);
        int nASpc = extAsciiSpecial(str);

        if(nNum == -1)  isAllow = false;      // ���ڿ��� ó���� ���ڷ� �����ϸ�
        if(nSpc)        isAllow = false;      // Ư�����ڰ� ���ԵǾ� �ִٸ� (gb2312�ȿ����� üũ��
        if(nEng && nNor)isAllow = false;      // �Ϲݹ��ڿ� ������ ���� �ִٸ�
        if(nASpc)       isAllow = false;      // Ascii�������� Ư�����ڰ� �߰ߵǸ�
    }

    return isAllow;
}

#endif
