#include "Client_PCH.h"
#include "Defender.h"
#include "IOCTL.h"
#include "ThemidaSDK.h"

Defender::Defender()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	m_pid = 0;
	m_hProcess = INVALID_HANDLE_VALUE;
	m_hDriver = INVALID_HANDLE_VALUE;
	checkLive = TRUE;
#ifndef _DEBUG
	VM_END
#endif
}

Defender::~Defender()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	//	DWORD retnum;
	//	if(m_hDriver!=INVALID_HANDLE_VALUE)
	//		DeviceIoControl(m_hDriver, GM_STOP, NULL, 0, NULL, 0, &retnum, NULL);
	
	//	if(m_hProcess!=INVALID_HANDLE_VALUE)
	//		::TerminateProcess(m_hProcess, 0);
#ifndef _DEBUG
	VM_END
#endif
}

DWORD WINAPI CheckChildProcess(LPVOID lpParameter)
{
	Defender* pDefender = (Defender*)lpParameter;
	HANDLE hProcess = pDefender->getProcessHandle();

	::WaitForSingleObject(hProcess, INFINITE);
	
	pDefender->setCheckLive(FALSE);

// 	if(pDefender->openVVProtect()!=INVALID_HANDLE_VALUE)
// 		pDefender->UnLoadDriver();

	GM_PID_PARAM gm_pid;
	gm_pid.ulProcessId = ::GetCurrentProcessId();
	DWORD ret;
	DWORD retnum;
	int Ret = ::DeviceIoControl(pDefender->getDriverHandle(), GM_STOP, &gm_pid, sizeof(GM_PID_PARAM), &ret, sizeof(DWORD), &retnum, NULL);

	::ExitProcess(0);
	return 0;
}

BOOL Defender::init()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	char filename[MAX_PATH], filepath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, filepath);
	strcpy(filename, filepath);
	strcat(filename, "\\");

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = FALSE;
	HANDLE hEvent = ::CreateEvent(&sa, TRUE, FALSE, "<<<>>>");
	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		CreateProcess(NULL, filename, NULL, NULL, NULL, NULL, NULL, filepath, &si, &pi);
		m_pid = pi.dwProcessId;
		m_hProcess = pi.hProcess;
		DWORD ThreadId;
		HANDLE hWaitHandle[2];
		hWaitHandle[0] = ::CreateThread(NULL, 0, CheckChildProcess, this, 0, &ThreadId);
		hWaitHandle[1] = hEvent;
		if(m_pid == 0 || m_hProcess == INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hEvent);
			return FALSE;
		}
		
		//::WaitForSingleObject(hEvent, INFINITE);
		::WaitForMultipleObjects(2, hWaitHandle, FALSE, INFINITE);
		
		//char openname[MAX_PATH];
		//sprintf(openname, "\\\\.\\%s", EXE_DRIVER_NAME);
		//m_hDriver = CreateFileA(openname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		
		m_hDriver = openVVProtect();
		if(m_hDriver == INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hEvent);
			return FALSE;
		}
	}

	::CloseHandle(hEvent);
// 	else
// 	{
// 		HWND hWnd = ::FindWindow(NULL, "DKLegend Helper");
// 		if(hWnd != NULL)
// 		{
// 			DWORD ProcessId=0;
// 			::GetWindowThreadProcessId(hWnd, &ProcessId);
// 			m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
// 			if(m_hProcess != INVALID_HANDLE_VALUE)
// 			{
// 				DWORD ThreadId;
// 				::CreateThread(NULL, 0, CheckChildProcess, this, 0, &ThreadId);
// 			}
// 			m_hDriver = openVVProtect();
// 			if(m_hDriver == INVALID_HANDLE_VALUE) return FALSE;
// 			::SendMessage(hWnd, 0, ::GetCurrentProcessId(), 0);
// 		}
// 		else
// 			return FALSE;
// 	}

#ifndef _DEBUG
	VM_END
#endif
	return TRUE;
}

BOOL Defender::setProtectProcessPid()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	GM_PID_PARAM pid;
	pid.ulProcessId = (ULONG)::GetCurrentProcessId();
	DWORD ret;
	DWORD retnum;
	int Ret = DeviceIoControl(m_hDriver, GM_SET_PID, &pid, sizeof(GM_PID_PARAM), &ret, sizeof(DWORD), &retnum, NULL);
#ifndef _DEBUG
	VM_END
#endif
	return TRUE;
}

BOOL Defender::startProtect()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD ret;
	DWORD retnum;
	int Ret = DeviceIoControl(m_hDriver, GM_START, NULL, 0, &ret, sizeof(DWORD), &retnum, NULL);
	
#ifndef _DEBUG 
	VM_END
#endif
	return TRUE;
}

BOOL Defender::UnLoadDriver()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(hSCManager != NULL)
	{
		SC_HANDLE hService = OpenService(hSCManager, EXE_DRIVER_NAME, DELETE | SERVICE_STOP);
		if(hService != NULL)
		{
			SERVICE_STATUS ss;
			ControlService(hService, SERVICE_CONTROL_STOP, &ss);
			DeleteService(hService);
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hService);
	}
#ifndef _DEBUG
	VM_END
#endif
	return TRUE;
}

DWORD Defender::getCheck()
{
#ifndef _DEBUG
	VM_START_WITHLEVEL(20)
#endif
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return 0;
	DWORD pid, retnum;
	int ret = DeviceIoControl(m_hDriver, GM_CHECK, NULL, 0, &pid, sizeof(DWORD), &retnum, NULL);
	if(retnum != sizeof(DWORD))
		return 0;
#ifndef _DEBUG
	VM_END
#endif
	return pid;
}

HANDLE Defender::openVVProtect()
{
	HANDLE hDriver = INVALID_HANDLE_VALUE;
	char openname[MAX_PATH];
	sprintf(openname, "\\\\.\\%s", EXE_DRIVER_NAME);
	hDriver = CreateFileA(openname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	return hDriver;
}
