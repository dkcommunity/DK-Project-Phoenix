//////////////////////////////////////////////////////////////////////////////
/// @file CrashReport.cpp
/// @author chyaya
/// @date 2007.10.25
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CrashReport.h"
#include "Properties.h"
#include "BugTrapInclude.h"
#include <tchar.h>


void CrashReport::Install()
{
#ifdef	__BUGTRAP
	BT_SetTerminate(); 
#endif//__BUGTRAP
}

bool CrashReport::LoadOptionFile(const char* fileName)
{
	Properties crashReportInfo;

	// 중요 옵션
	try
	{
		crashReportInfo.load(fileName);
		
		const std::string appName = crashReportInfo.getProperty("AppName");
		const std::string supportURL = crashReportInfo.getProperty("SupportURL");
		const std::string supportServerURL = crashReportInfo.getProperty("SupportServerURL");
		const int supportServerPort = crashReportInfo.getPropertyInt("SupportServerPort");
#ifdef	__BUGTRAP	
		BT_InstallSehFilter();
		
		// Setup exception handler
		BT_SetAppName(appName.c_str());
		BT_SetSupportURL(supportURL.c_str());

		// = BugTrapServer ===========================================
		BT_SetSupportServer(supportServerURL.c_str(), supportServerPort);
#endif //__BUGTRAP
	}
	catch(...)
	{
		return false;
	}
	
	// 추가 옵션
	try
	{
		const std::string dialogMessageIntro1 = crashReportInfo.getProperty("DialogMessageIntro1");
		const std::string dialogMessageIntro2 = crashReportInfo.getProperty("DialogMessageIntro2");
#ifdef	__BUGTRAP
		BT_SetDialogMessage(BTDM_INTRO1, dialogMessageIntro1.c_str());
		BT_SetDialogMessage(BTDM_INTRO2, dialogMessageIntro2.c_str());
#endif //__BUGTRAP
	}
	catch(...) {}


	// 각종 Flag 없어도 큰 지장 없음
	const int flagsSize = 6;

	const char* stringFlags[flagsSize] = {
		"DetailedMode", "EditMail", "AttachReport",
		"ListProcesses", "ShowAdvanceDUI", "ScreenCapture",
	};

	const DWORD numberFlags[flagsSize] = {
		BTF_DETAILEDMODE, BTF_EDITMAIL, BTF_ATTACHREPORT,
		BTF_LISTPROCESSES, BTF_SHOWADVANCEDUI, BTF_SCREENCAPTURE
	};

	DWORD flags = BTF_NONE;

	for(int i = 0; i < flagsSize; ++i)
	{
		try
		{
			const int flagValue = crashReportInfo.getPropertyInt(stringFlags[i]);
			if(flagValue) flags |= numberFlags[i];
		}
		catch (...) {}
	}
#ifdef	__BUGTRAP
	BT_SetFlags(flags);
#endif //__BUGTRAP
	return true;
}


void CrashReport::SetAppVersion(int version)
{
	char buffer[32];
	itoa(version, buffer, 10);
#ifdef	__BUGTRAP
	BT_SetAppVersion(buffer);
#endif //__BUGTRAP
}
