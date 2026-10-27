// APICheck.h: interface for the APICheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APICHECK_H__9A3E0710_3AC7_4BC7_955A_323AAC3F0D3A__INCLUDED_)
#define AFX_APICHECK_H__9A3E0710_3AC7_4BC7_955A_323AAC3F0D3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class APICheck  
{
public:
	APICheck();
	virtual ~APICheck();

	BOOL CheckApi();
	BOOL init();
private:
	BOOL GetTimeAddr();

	DWORD (__stdcall *m_hGetTickCount)();
	DWORD (__stdcall *m_htimeGetTime)();
	
private:
	BYTE m_buffGetTickCount[100];
	BYTE m_bufftimeGetTime[100];
	int m_HookCount;
};

#endif // !defined(AFX_APICHECK_H__9A3E0710_3AC7_4BC7_955A_323AAC3F0D3A__INCLUDED_)
