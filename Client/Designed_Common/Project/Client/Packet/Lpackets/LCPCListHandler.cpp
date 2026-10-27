//----------------------------------------------------------------------
//
// Filename    : LCPCListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "LCPCList.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MStorage.h"
#include "UserInformation.h"
#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"
	#include "UserInformation.h"
	#include "Cpackets/CLCreatePC.h"
	#include "Cpackets/CLDeletePC.h"
	#include "Cpackets/CLSelectPC.h"
#endif

//----------------------------------------------------------------------
void LCPCListHandler::execute ( LCPCList * pPacket , Player * pPlayer )
{
	CT_Done("DIAG: LCPCListHandler::execute() hit");
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

		/*
	#if __LINUX__ || __WIN_CONSOLE__

		ClientPlayer * pClientPlayer = (ClientPlayer*)pPlayer;

		std::cout << std::endl
			 << "+----------------------+" << std::endl
			 << "| CHARACTER MANAGEMENT |" << std::endl
			 << "+----------------------+" << std::endl
			 << std::endl;
	
		uint nPCs = 0;
	
		for ( uint i = 0 ; i < SLOT_MAX ; i ++ ) {
	
			try {
	
				PCInfo * pPCInfo = pPacket->getPCInfo( Slot(i) );
				nPCs ++;
	
				switch ( pPCInfo->getPCType() ) {
	
					case PC_SLAYER :
						{
							PCSlayerInfo * pPCSlayerInfo = dynamic_cast<PCSlayerInfo*>(pPCInfo);							
							std::cout << pPCSlayerInfo->toString() << std::endl;
						}
						break;
	
					case PC_VAMPIRE :
						{
							PCVampireInfo * pPCVampireInfo = dynamic_cast<PCVampireInfo*>(pPCInfo);		
							std::cout << pPCVampireInfo->toString() << std::endl;
						}
						break;
	
					default :	
						throw InvalidProtocolException("invalid pc type");
				}
	
			} catch ( NoSuchElementException ) {
			}

		}

		if ( nPCs == 0 ) {
			std::cout << "캐릭터가 하나도 없습니다." << std::endl;
		}

		char cmd[80+1];

		do {

			std::cout << std::endl
				 << "+--------------+" << std::endl
			  	 << "| 1. Create PC |" << std::endl
			  	 << "| 2. Delete PC |" << std::endl
			 	 << "| 3. Select PC |" << std::endl
			 	 << "+--------------|" << std::endl
			 	 << "select > ";

			std::cin.getline(cmd,80);

		} while ( strcmp(cmd,"1") != 0 && strcmp(cmd,"2") != 0 && strcmp(cmd,"3") != 0 );

		if ( strcmp(cmd,"1") == 0 ) {

			std::cout << std::endl
				 << "+--------------------+" << std::endl
				 << "| CHARACTER CREATION |" << std::endl
				 << "+--------------------+" << std::endl
				 << std::endl;

			CLCreatePC clCreatePC;

			std::cout << "Name : ";
			std::cin.getline(cmd,80);
			clCreatePC.setName(cmd);

			std::cout << "Slot (1-3) : ";
			std::cin.getline(cmd,80);
			Slot slot;
			if ( strcmp(cmd,"1") == 0 ) slot = SLOT1;
			else if ( strcmp(cmd,"2") == 0 ) slot = SLOT2;
			else if ( strcmp(cmd,"3") == 0 ) slot = SLOT3;
			else slot = SLOT1;
			clCreatePC.setSlot(slot);

			std::cout << "Sex (남/여) : ";
			std::cin.getline(cmd,80);
			Sex sex = ( strcmp(cmd,"남") == 0 ) ? sex = MALE : sex = FEMALE;
			clCreatePC.setSex(sex);
			std::cout << "Sex : " << Sex2String[sex] << std::endl;

			std::cout << "HairStyle (1-3) : ";
			std::cin.getline(cmd,80);
			HairStyle hairStyle;
			if ( strcmp(cmd,"1") == 0 ) hairStyle = HAIR_STYLE1;
			else if ( strcmp(cmd,"2") == 0 ) hairStyle = HAIR_STYLE2;
			else if ( strcmp(cmd,"3") == 0 ) hairStyle = HAIR_STYLE3;
			else hairStyle = HAIR_STYLE1;
			clCreatePC.setHairStyle(hairStyle);
			
			std::cout << "HairColor (0-255) : ";
			std::cin.getline(cmd,80);
			clCreatePC.setHairColor( atoi( cmd ) );
			
			std::cout << "SkinColor (0-255) : ";
			std::cin.getline(cmd,80);			
			clCreatePC.setSkinColor( atoi( cmd ) );

			std::cout << "ShirtColor (0-255) : ";
			std::cin.getline(cmd,80);
			clCreatePC.setSkinColor( atoi( cmd ) );

			std::cout << "JeansColor (0-255) : ";
			std::cin.getline(cmd,80);
			clCreatePC.setSkinColor( atoi( cmd ) );

			std::cout << "Try to creating PC : " << clCreatePC.toString() << std::endl;

			pClientPlayer->sendPacket( &clCreatePC );
			pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_CREATE_PC );

		} else if ( strcmp(cmd,"2") == 0 ) {

			std::cout << std::endl
				 << "+------------------+" << std::endl
				 << "| DELETE CHARACTER |" << std::endl
				 << "+------------------+" << std::endl
				 << std::endl;

			CLDeletePC clDeletePC;

			std::cout << "Name : ";
			std::cin.getline(cmd,80);
			clDeletePC.setName(cmd);

			std::cout << "Slot (1-3) : ";
			std::cin.getline(cmd,80);
			Slot slot;
			if ( strcmp(cmd,"1") == 0 ) slot = SLOT1;
			else if ( strcmp(cmd,"2") == 0 ) slot = SLOT2;
			else if ( strcmp(cmd,"3") == 0 ) slot = SLOT3;
			else slot = SLOT1;
			clDeletePC.setSlot(slot);

			pClientPlayer->sendPacket( &clDeletePC );
			pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_DELETE_PC );

		} else if ( strcmp(cmd,"3") == 0 ) {

			std::cout << std::endl
				 << "+------------------+" << std::endl
				 << "| SELECT CHARACTER |" << std::endl
				 << "+------------------+" << std::endl
				 << std::endl;

			CLSelectPC clSelectPC;

			std::cout << "Name : ";
			std::cin.getline(cmd,80);
			clSelectPC.setPCName(cmd);

			std::cout << "PCType (S/V) : ";
			std::cin.getline(cmd,80);
			if ( strcmp(cmd,"S") == 0 || strcmp(cmd,"s") == 0 ) 
				clSelectPC.setPCType(PC_SLAYER);
			else if ( strcmp(cmd,"V") == 0 || strcmp(cmd,"v") == 0 ) 
				clSelectPC.setPCType(PC_VAMPIRE);
			else
				clSelectPC.setPCType(PC_SLAYER);

			// CGConnect 때 사용하려면 여기서 저장해둬야 한다.
			pClientPlayer->setPCType(clSelectPC.getPCType());
			pClientPlayer->setPCName(clSelectPC.getPCName());

			std::cout << std::endl << clSelectPC.toString() << std::endl;
		
			pClientPlayer->sendPacket( &clSelectPC );
			pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_SELECT_PC );
		}	
	#endif
	*/

	////
	/*

	enum CHAR_MAN
	{
		M_FACE1,
		M_FACE2,
		M_FACE3,
		M_NEW_COAT,
		M_NEW_HELMET,
		M_NEW_TROUSER,
		M_OLD_COAT,
		M_OLD_HELMET,
		M_OLD_TROUSER,
	};

	enum CHAR_WOMAN
	{
		W_FACE1,
		W_FACE2,
		W_FACE3,
		W_HAIR1,
		W_HAIR2,
		W_HAIR3,
		W_NEW_COAT,
		W_NEW_HELMET,
		W_NEW_TROUSER,
		W_OLD_COAT,
		W_OLD_HELMET,
		W_OLD_TROUSER,
	};
	struct MAN_INFO
	{
		CHAR_MAN		face;
		CHAR_MAN		helmet;
		CHAR_MAN		coat;
		CHAR_MAN		trouser;
	};

	struct WOMAN_INFO // C_VS_UI_NEWCHAR::
	{
		CHAR_WOMAN	face;
		CHAR_WOMAN	hair;
		CHAR_WOMAN	helmet;
		CHAR_WOMAN	coat;
		CHAR_WOMAN	trouser;
	};
	struct S_SLOT
	{
		bool							bl_set; // slot이 설정되었는가?
		bool							bl_female;
		C_VS_UI_NEWCHAR::MAN_INFO		man_info;
		C_VS_UI_NEWCHAR::WOMAN_INFO		woman_info;
	};

	*/

	//------------------------------------------------------------
	// Characer 선택 창을 띄örmek için.
	//------------------------------------------------------------
	CT_Done("DIAG: PCList - pre UI_StartCharacterManager");
	UI_StartCharacterManager( !pPacket->isAgree() && g_pUserInformation->IsNetmarble) ; // && g_pUserInformation->IsNetmarbleLogin);
	CT_Done("DIAG: PCList - post UI_StartCharacterManager");
	//UI_StartCharacterManager( !pPacket->isAgree() );

	for (int i=0; i<SLOT_MAX; i++)
	{
		CT_Done("DIAG: PCList - slot %d loop iteration start", i);
		try {
			PCInfo * pInfo = pPacket->getPCInfo ( i==0? SLOT1 : i==1? SLOT2 : SLOT3 );
			CT_Done("DIAG: PCList - slot %d got pInfo=%p", i, pInfo);

			switch ( pInfo->getPCType() )
			{
				case PC_SLAYER :
				{
					CT_Done("DIAG: PCList - slot %d SLAYER case", i);
					PCSlayerInfo * pPCSlayerInfo = dynamic_cast<PCSlayerInfo*>(pInfo);
					CT_Done("DIAG: PCList - slot %d SLAYER cast=%p", i, pPCSlayerInfo);

					//char johapName[16];

					//------------------------------------------------------------
					// 완성형 --> 조합형
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCSlayerInfo->getName().c_str(), johapName );

					//g_pUserInformation->CharacterW[i] = pPCSlayerInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					CT_Done("DIAG: PCList - SLAYER pre Character set name=%s", pPCSlayerInfo->getName().c_str());
					g_pUserInformation->Character[i] = pPCSlayerInfo->getName().c_str();
					g_pUserInformation->Race[i]	= RACE_SLAYER;
					CT_Done("DIAG: PCList - SLAYER pre getHairStyle");
					g_pUserInformation->FaceStyleSlot[i] = pPCSlayerInfo->getHairStyle();
					CT_Done("DIAG: PCList - SLAYER pre UI_SetCharacter");
					UI_SetCharacter( i, pPCSlayerInfo );
					CT_Done("DIAG: PCList - SLAYER post UI_SetCharacter");
				}
				break;

				case PC_VAMPIRE :
				{
					PCVampireInfo * pPCVampireInfo = dynamic_cast<PCVampireInfo*>(pInfo);		
					
					//char johapName[16];

					//------------------------------------------------------------
					// 완성형 --> 조합형
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCVampireInfo->getName().c_str(), johapName );

					//g_pUserInformation->CharacterW[i] = pPCVampireInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					g_pUserInformation->Character[i] = pPCVampireInfo->getName().c_str();
					g_pUserInformation->Race[i]	= RACE_VAMPIRE;
					g_pUserInformation->FaceStyleSlot[i] = 0;//pPCVampireInfo->

					UI_SetCharacter( i, pPCVampireInfo );
				}
				break;

				case PC_OUSTERS :
				{
					PCOustersInfo * pPCOustersInfo = dynamic_cast<PCOustersInfo*>(pInfo);		
					
					//char johapName[16];
					
					//------------------------------------------------------------
					// 완성형 --> 조합형
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCVampireInfo->getName().c_str(), johapName );
					
					//g_pUserInformation->CharacterW[i] = pPCVampireInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					g_pUserInformation->Character[i] = pPCOustersInfo->getName().c_str();
					g_pUserInformation->Race[i]	= RACE_OUSTERS;
					g_pUserInformation->FaceStyleSlot[i] = 0;//pPCVampireInfo->
					
					UI_SetCharacter( i, pPCOustersInfo );
				}
				break;
			}
			
		} catch (NoSuchElementException) {			
		}
	}

	//----------------------------------------------
	// 보관함을 비워둔다.
	//----------------------------------------------
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;
	}

			

	// 다음은 
	CT_Done("DIAG: LCPCListHandler - transitioning to MODE_WAIT_SELECTPC");
	SetMode( MODE_WAIT_SELECTPC );
	
#endif
		
	__END_CATCH
}
