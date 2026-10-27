//#define NETMARBLE_DEBUG
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include <windows.h>
#include "DarkAssert.h"
#include "NMCrypt.h"
#include "MString.h"

//wlzzi - ÀÏº» ¹öÀü ÀÏÄ¡ÀÛ¾÷..
#ifdef __DESIGNED_JAPAN
#include "NMCryptAPI.h"
#endif

extern	HWND				g_hWnd;

namespace
{
	MString g_NetmarbleCommandLine = "";
};

struct NETMARBLE_INFO
{
	MString ID;
	MString Password;
	int WorldID;
	int ServerID;
	bool bGore;
};

// 2004, 7, 14, sobeit add start
struct REALSERVER_INFO
{
	bool bMode;		// 0: ¼öµ¿Á¢¼Ó, 1:Á¤»óÁ¢¼Ó
	int WorldID;	// 0: ºê¶õ, 1: Æç·¹½º
	MString ID;		// id
	MString Key;	// Key
};

// 2004, 7, 14, sobeit add end
std::string g_ServIP;//     = arg[0];					// ¼­¹ö IP
DWORD g_ServPort;//   = (UINT)::atoi(arg[1]);	// ¼­¹ö Port
std::string g_AuthCookie;// = arg[2];					// ÀÎÁõ ÄíÅ°	- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
std::string g_DataCookie;// = arg[3];					// µ¥ÀÌÅÍ ÄíÅ°	- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
std::string g_CpCookie;//   = arg[4];					// CPÄíÅ°		- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
std::string g_SpareParam;///*= arg[5];					// ¿©ºÐÀÇ ÀÎÀÚ(CP°ÔÀÓµî¿¡¼­ »ç¿ë)*/


//wlzzi - ÀÏº» ³Ý¸¶ºí°ú ±¹³» ³Ý¸¶ºíÀÇ Ã³¸®°¡ ºÐ±âµÈ´Ù.
#ifdef __DESIGNED_JAPAN
std::string g_CpPassword;	//ÀÏº» ³Ý¸¶ºíÀº À¯Àúºñ¹øÀÌ Àü´ÞµÇ°í, ÀÌ°ÍÀ» ·Î±×ÀÎ ¼­¹ö¿¡ º¸³½´Ù.
bool
ParsingNetmarble(const char* pCommandLine, NETMARBLE_INFO &info)
{
	bool	bNetMarbleInit  = CallWinMain() ;
	if(!bNetMarbleInit)
	{
		CT_Done("±ÌvOÍlbg}[uÌz[y[WãÅ\n³µ­®ìµÜ·B\n¡·®z[y[WÖÚ®µÜ·©?");
		exit(0);
	}
	info.WorldID = 1;//GetWorldID() ; // GetPort()/10;
	info.ServerID = GetServerID() ; // GetPort()%10;

	info.ID =  GetLoginID() ;  //g_CpCookie.c_str();
	info.Password = GetLoginPW() ;

	//Game_UI.cpp¿¡¼­ g_CpCookie¸¦ »ç¿ëÇÏ°í ÀÖ´Ù. ÀÏº» ¼­ºñ½º¿ëÀÌ´õ¶óµµ~~ g_CpCookie¸¦ .. ¼³Á¤ÇÏÀÚ.
	g_CpCookie = GetLoginID();
	g_CpPassword = GetLoginPW();
	return true;
}

#else  // ±¹³» ³Ý¸¶ºí Ã³¸® ÄÚµåµé..

char* _StrTok(const char* str, const char sep)
{
	// strtok()¿Í °°Àº ±â´ÉÀÇ ÇÔ¼öÀÌÁö¸¸ separator¸¦ 1°³¸¸ ¹Þ°í
	// ÀÎÀÚ°¡ ºñ¾îÀÖ´Â °æ¿ìµµ ÀÎÀÚ·Î ÀÎÁ¤ÇÔ

	static const int TOK_BUFSIZE = 1024*40;

	static char* pnow = NULL;
	static char buf[TOK_BUFSIZE];
	static char token[TOK_BUFSIZE];

	if(str) {
		int slen = strlen(str);
//		assert(slen < TOK_BUFSIZE);		// ASSERT
		strcpy(buf, str);
		pnow = buf;
	}

	if(!pnow || *pnow == 0)
		return NULL;

	int tlen = 0;
	char* porg = pnow;
	while(*pnow != 0)
	{
		if(*pnow == sep) {
			++pnow;
			break;
		}

		++tlen;
		++pnow;
	}

	if(tlen > 0) {
		strncpy(token, porg, tlen);
		token[tlen] = 0;
	}
	else {
		token[0] = 0;
	}

	return token;
}

BOOL AnalyzeArgument(char *key)
{
	//
	// Å¬¸³º¸µå ÀÎÀÚ Àü´Þ ±ÔÄ¢(°¢°¢ÀÇ ÀÎÀÚ´Â ÄÄ¸¶(,)·Î ±¸ºÐµÇ¸ç ÀÎÀÚ »çÀÌÀÇ °ø¹éÀº ¾ø´Ù)
	// Àü´Þ ÀÎÀÚÀÇ ¼ø¼­ : ServerIP,Port,AuthCookie,DataCookie,CpCookie,SpareParam (ÀÎÀÚ´Â ¾ÏÈ£È­µÇ¾î ÀÖ´Ù)
	// - ³Ý¸¶ºí °ÔÀÓ¿¡¼­´Â CpCookie¸¦ Àü´ÞÇÏÁö ¾Ê´Â´Ù
	// - CP °ÔÀÓ¿¡¼­´Â AuthCookie, DataCookie¸¦ Àü´ÞÇÏÁö ¾Ê´Â´Ù(¸î¸î CP°ÔÀÓÀº ¿¹¿Ü)
	// - Extra ÀÎÀÚ´Â »ý·«µÇ±âµµ ÇÑ´Ù
	//

	char buf[1024*20] = {0,};	// ¹öÆÛ´Â ÃæºÐÈ÷ Å©°Ô Àâ´Â´Ù

	// Å¬¸³º¸µå¿¡¼­ ÀÎÀÚ µ¥ÀÌÅÍ¸¦ ÀÐ¾î¿Â´Ù(ÇÑ¹ø ¾ò¾î¿À¸é »èÁ¦µÊ)
	if(!GetNMClipData(buf, sizeof(buf)-1, key, true))
		return FALSE;


	//test ZZi
	
//	if(!GetNMClipData(buf, sizeof(buf)-1, &key[9], true))
//		return FALSE;

//	std::ofstream file("NetmarbleInfo.txt", 0);//std::ios::binary);
//
//	if (file.is_open())
//	{
//		file.write((const char*)key, strlen (key));
//		file.write((const char*)buf, strlen (buf));
//		file.close();
//	}
	//-

	if(strlen(buf)==0)
		return FALSE;

	char arg[6][1024*2] = {0,};

	int argcnt = 0;
	char* token = _StrTok(buf, ',');
	while(token && argcnt < 6)
	{
		strcpy(arg[argcnt], token);
		argcnt++;
		token = _StrTok(NULL, ',');
	}

/*	//test ZZi
	std::ofstream file2("NetmarbleInfo2.txt", std::ios::binary);

	if (file2.is_open())
	{
		char strInfo[2048];
		wsprintf (strInfo, "%s\r\n, %s\r\n, %s\r\n, %s\r\n, %s\r\n, %s\r\nArgCount=%d", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], argcnt);
		file2.write((const char*)strInfo, strlen (strInfo));
		file2.close();
	}
	//-
*/

	// ÀÎÀÚ°¡ ÃÖ¼ÒÇÑ 5°³´Â µÇ¾î¾ß ÇÑ´Ù
	if(argcnt < 5)
		return FALSE;

	// IPÁÖ¼Ò°¡ ¿Ã¹Ù¸¥Áö °Ë»ç
	if(strlen(arg[0]) > 16)
		return FALSE;

	// CP °ÔÀÓÀº ¾Æ·¡ºÎºÐÀ» °íÃÄ ¾µ°Í
	g_ServIP     = arg[0];					// ¼­¹ö IP
	g_ServPort   = (UINT)::atoi(arg[1]);	// ¼­¹ö Port
	g_AuthCookie = arg[2];					// ÀÎÁõ ÄíÅ°	- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
	g_DataCookie = arg[3];					// µ¥ÀÌÅÍ ÄíÅ°	- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
	g_CpCookie   = arg[4];					// CPÄíÅ°		- ¹öÆÛ Å©±â¸¦1024*2 Á¤µµ·Î Àâ¾Æ¾ß ÇÑ´Ù
	g_SpareParam = arg[5];					// ¿©ºÐÀÇ ÀÎÀÚ(CP°ÔÀÓµî¿¡¼­ »ç¿ë)
	
	return TRUE;
}

int HexStringToDec(char *str)
{
	int dec = 0;
	const int strLen = strlen(str);

	for(int i = strLen-1, multi = 1; i >= 0; i--, multi *= 16)
	{
		int num;
		if(str[i] >= '0' && str[i] <= '9')
			num = str[i] - '0';
		else
		if(str[i] >= 'A' && str[i] <= 'F')
			num = str[i] - 'A' + 10;
		else
			return -1;

		dec += num*multi;
	}
	return dec;
}

// ³Ý¸¶ºí¿ë
//-----------------------------------------------------------------------
// ParsingNetmarble
//-----------------------------------------------------------------------
// DarkEden.exe ID&PASS&WORLDID&SERVERID
//-----------------------------------------------------------------------
bool
ParsingNetmarble(const char* pCommandLine, NETMARBLE_INFO &info)
{
	char szTemp[4096];

#ifdef OUTPUT_DEBUG
	char szTemp1[4096];
	wsprintf(szTemp1 , "NEWSTART %s",pCommandLine) ; 
	const char *pString = strchr(szTemp1, ' ');
#else
	const char *pString = strchr(pCommandLine, ' ');
#endif
		

	if(pString == NULL)
	{
		return false;
	}


	strcpy(szTemp, pString+1);
		
	BOOL bResult = AnalyzeArgument( szTemp );

	if( bResult == FALSE )
	{
		CT_Done("³Ý¸¶ºí È¨ÆäÀÌÁö¸¦ ÅëÇØ¼­ ´Ù½Ã ½ÇÇàÇØÁÖ¼¼¿ä.");
		exit(0);
		Assert( bResult );
	}

	info.WorldID = g_ServPort/10;
	info.ServerID = g_ServPort%10;

//	sprintf( szTemp, "%d %d", info.WorldID, info.ServerID );
//	MessageBox( NULL, szTemp, szTemp, MB_OK );
	
	info.ID = g_CpCookie.c_str();
	info.Password = "test";

//zzi - ³ªÀÌÃßÃâ : ·Î±×ÀÎÃ¢¿¡¼­ Ç¥ÇöµÇ´Â 'ÀÌ¿ëÁ¦ÇÑUI'ÀÇ ¿Ã¹Ù¸¥ Ç¥ÇöÀ» À§ÇØ, ³ªÀÌ°ªÀ» SpareParamÀ¸·Î Àü´Þ¹Þ±â·Î ÇÏ¿´´Ù. <07³â 10¿ù 15ÀÏ °æ>
//				: ÇöÀç´Â, Å×½ºÆ® url¿¡¼­ ½ÇÇàÇÒ ¶§¸¸ SpareParamÀÌ Àü´ÞµÈ´Ù. http://game3.netmarble.net/cp_site/dark/
//				: SpareParam : "Age=35" ½ÄÀ¸·Î Àü´ÞµÈ´Ù. 
//				: ¿©±â¼­, ³ªÀÌ°ª¸¸ ÃßÃâÇÏ¿©, info.bGore°ªÀ» ¼³Á¤ÇÏµµ·Ï ÇÑ´Ù.
/*	int strAgeLen = g_SpareParam.length ();
	if (strAgeLen > 4)
	{
		char *pstrAge = new char [strAgeLen+1];
		strcpy (pstrAge, g_SpareParam.c_str ());
		pstrAge[strAgeLen] = '\0';
		bool bfind = false;
		for (int i=0; i<strAgeLen; i++)
		{
			if (pstrAge[i] == '=')
			{
				bfind = true;
				break;
			}
		}
		if (bfind)
		{
//			MessageBox (g_hWnd, &pstrAge[i+1], "ÇÏÇÏ", MB_OK);
			int iAge = atoi (&pstrAge[i+1]);
			if (iAge > 18) info.bGore = true;
			else info.bGore =false;
		}
		else
		{
			info.bGore = false;
		}
	}
	else	//°ªÀÌ Àß¸øµÇ¾ú´Ù. ÀÌ·²¸®´Â ¾øÁö¸¸, ¤Ñ¤Ñ; µðÆúÆ® Ã³¸®.
		info.bGore = false;*/


//--------------------------------------------------------------------------------------------------------------------------------------------

//	strcpy( szTemp, g_NetmarbleCommandLine.GetString() );
////	strcpy( szTemp, "2,20,netmetro3,netmetro3NM00011EB7BA45");
//
//	char *pszTemp, *pszTemp2;
////	char* pStr;
//	
//	pszTemp = szTemp;
//
//	//-------------------------------------------------
//	// Teen ÃàÃâ
//	//-------------------------------------------------
////	pszTemp = pszTemp2+1;
//	pszTemp2 = strchr(pszTemp, ',');
//
//	// ÆÄ½Ì¿ë &¸¦ Ã£Áö ¸øÇÏ¸é ¿¡·¯!
//	if(pszTemp2 == NULL)
//		return false;
//
//	*pszTemp2 = '\0';
//
//	if(strlen(pszTemp) != 1)
//		return false;
//
//	switch(atoi(pszTemp))
//	{
//	case 0:
//	case 1:
////		g_pUserInformation->GoreLevel = false;
//		info.bGore = false;
//		break;
//	
//	case 2:
////		g_pUserInformation->GoreLevel = true;
//		info.bGore = true;
//		break;
//	}
//
//	//-------------------------------------------------
//	// WorldID ÃàÃâ
//	//-------------------------------------------------
//	pszTemp = pszTemp2+1;
//	pszTemp2 = strchr(pszTemp, ',');
//
//	// ÆÄ½Ì¿ë &¸¦ Ã£Áö ¸øÇÏ¸é ¿¡·¯!
//	if(pszTemp2 == NULL)
//	{
//		return false;
//	}
//
//	*pszTemp2 = '\0';
//
//	if(strlen(pszTemp) != 2)
//	{
//		return false;
//	}
//	
//	int num = atoi(pszTemp);
////	g_pUserInformation->NetmarbleWorldID = num/10;
////	g_pUserInformation->NetmarbleServerID = num%10;
//	info.WorldID = num/10;
//	info.ServerID = num%10;
//
//	//-------------------------------------------------
//	// ID ÃàÃâ
//	//-------------------------------------------------
//	// 2,10,larosellarosel,larosellaroselNM000076B17852
//	pszTemp = pszTemp2+1;
//	pszTemp2 = strchr(pszTemp, ',');
//
//	// ÆÄ½Ì¿ë &¸¦ Ã£Áö ¸øÇÏ¸é ¿¡·¯!
//	if(pszTemp2 == NULL)
//	{
//		return false;
//	}
//
//	*pszTemp2 = '\0';
//	// ID°¡ 12byte ÀÌ»óÀÌ¸é ¿¡·¯!
//	if(strlen(pszTemp) > 24)
//	{
//		return false;
//	}
//
////	g_pUserInformation->NetmarbleID = szTemp;

//	g_pUserInformation->NetmarbleID = g_CpCookie.c_str();
//	
//	//-------------------------------------------------
//	// Password ÃàÃâ
//	//-------------------------------------------------
//	pszTemp = pszTemp2+1;
//	pszTemp2 = pszTemp+strlen(pszTemp);
//
//	// ID¸¸Å­ Á¦³¤´Ù.
//	pszTemp += info.ID.GetLength();
//
////	pszTemp2 = strchr(pszTemp, ',');
//
//	// ÆÄ½Ì¿ë &¸¦ Ã£Áö ¸øÇÏ¸é ¿¡·¯!
////	if(pszTemp2 == NULL)
////		return false;
//
//	*pszTemp2 = '\0';
//
//	// ID°¡ 5byte ÀÌÇÏÀÌ¸é ¿¡·¯!
//	if(strlen(pszTemp) != 14)
//	{
//		return false;
//	}
//
//	switch(HexStringToDec(pszTemp+12))
//	{
//	case 'M':
//	case 'E':
//	case 'T':
//	case 'R':
//	case 'O':
//	case 'C':
//	case 'H':
//		break;
//
//	default:
//		return false;
//	}
//
//	*(pszTemp2-2) = '\0';
//
//	if(pszTemp[0] != 'N' || pszTemp[1] != 'M')
//	{
//		return false;
//	}
//
//	int uniID = HexStringToDec(pszTemp+2);
//	if(uniID == -1)
//	{
//		return false;
//	}
//
//	sprintf(szTemp, "%d", uniID);
//	int skip = 12 - strlen(szTemp);
//	
//	sprintf(szTemp, "NM0000000000");
//	sprintf(szTemp+skip, "%d", uniID);
//	info.Password = szTemp;
//
////	g_pUserInformation->NetmarblePassword = pszTemp;
//	g_pUserInformation->NetmarblePassword = "test";

	return true;
}

#endif

/* old version
char* _StrTok(const char* str, const char sep)
{
	// strtok()¿Í °°Àº ±â´ÉÀÇ ÇÔ¼öÀÌÁö¸¸ separator¸¦ 1°³¸¸ ¹Þ°í
	// ÀÎÀÚ°¡ ºñ¾îÀÖ´Â °æ¿ìµµ ÀÎÀÚ·Î ÀÎÁ¤ÇÔ

	static const int TOK_BUFSIZE = 4096;

	static char* pnow = NULL;
	static char buf[TOK_BUFSIZE];
	static char token[TOK_BUFSIZE];

	if(str) {
		int slen = strlen(str);
		Assert(slen < TOK_BUFSIZE);		// ASSERT

		strcpy(buf, str);
		pnow = buf;
	}

	if(!pnow || *pnow == 0)
		return NULL;

	int tlen = 0;
	char* porg = pnow;
	while(*pnow != 0)
	{
		if(*pnow == sep) {
			++pnow;
			break;
		}

		++tlen;
		++pnow;
	}

	if(tlen > 0) {
		strncpy(token, porg, tlen);
		token[tlen] = 0;
	}
	else {
		token[0] = 0;
	}

	return token;
}
 


BOOL AnalizeArgument(const char *strarg)
{
#ifdef NETMARBLE_DEBUG
	MessageBox(NULL,strarg,strarg,MB_OK);
#endif
	//
	// ÀÎÀÚ Àü´Þ ±ÔÄ¢(°¢°¢ÀÇ ÀÎÀÚ´Â ÄÄ¸¶(,)·Î ±¸ºÐµÇ¸ç ÀÎÀÚ »çÀÌÀÇ °ø¹éÀº ¾ø´Ù)
	// Àü´Þ ÀÎÀÚÀÇ ¼ø¼­ : ServerIP,Port,UserID,Pass,Extra (ÀÎÀÚ´Â ¾ÏÈ£È­µÇ¾î ÀÖ´Ù)
	// - Extra ÀÎÀÚ´Â »ý·«µÇ±âµµ ÇÑ´Ù
	//

	// arg[0],arg[1],arg[2],arg[2]arg[3]arg[4]
	// 2,10,larosellarosel,larosellaroselNM000076B17852

	// ÀÎÀÚ°¡ ³Ê¹« ±æ¸é ¾ÈµÈ´Ù
	if(strlen(strarg) >= 4096)
		return FALSE;

	char key[1024] = {0,};
	char buf[4096] = {0,};

	// ¾ÏÈ£ ÇØµ¶ Å°¸¦ ¾ò¾î¿Â´Ù(ÇÑ¹ø ¾ò¾î¿À¸é »èÁ¦µÊ)
	if(!GetCryptKey(key, 1024-1, false)) 
	{
		return FALSE;
	}

	// º¹È£È­ ¼º°ø¿©ºÎ °Ë»ç
	if(CRYPT_SUCCESS != DecryptString(strarg, key, buf))
	{
		return FALSE;
	}

	// -_- netmarble ¿¡¼­ º¸³»ÁÖ´Â extra ÄÚµå´Â ¾È¾´´Ù.
	char *endchar = strstr(buf,",NM");

	if( endchar != NULL )
		*endchar = NULL;

	buf[ strlen(buf) - 1 ] = '\0';
	
	g_NetmarbleCommandLine = buf;

//	char arg[5][1024] = {0,};
//
//	int argcnt = 0;
//	char* token = _StrTok(buf, ',');
//	while(token && argcnt<5)
//	{
//		strcpy(arg[argcnt], token);
//		argcnt++;
//		token = _StrTok(NULL, ',');
//	}

	// ÀÎÀÚ°¡ ÃÖ¼ÒÇÑ 4°³´Â µÇ¾î¾ß ÇÑ´Ù
//	if(argcnt < 4)
//		return FALSE;

	// IPÁÖ¼Ò°¡ ¿Ã¹Ù¸¥Áö °Ë»ç
//	if(strlen(arg[0]) > 16)
//		return FALSE;

	// CP °ÔÀÓÀº ¾Æ·¡ºÎºÐÀ» °íÃÄ ¾µ°Í
//	g_ServIP    = arg[0];		// ¼­¹ö IP
//	g_ServPort  = (UINT)::atoi(arg[1]);	// ¼­¹ö Port
//	g_LoginID   = arg[2];		// ·Î±×ÀÎ ID
//	g_LoginPass = arg[3];		// ·Î±×ÀÎ ÆÐ½º¿öµå(CP°ÔÀÓÀÌ¶ó¸é À¯´ÏÅ© ³Ñ¹ö)
//	g_Extra     = arg[4];		// ¿©ºÐÀÇ ÀÎÀÚ(CP°ÔÀÓµî¿¡¼­ »ç¿ë)
	
	return TRUE;
}
*/
