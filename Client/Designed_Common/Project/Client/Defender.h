#ifndef __DEFENDER_H__
#define __DEFENDER_H__

#define EXE_DRIVER_NAME	"VVProtect"

class Defender  
{
public:
	Defender();
	virtual ~Defender();
	BOOL init();

	HANDLE	getDriverHandle() const { return m_hDriver; }
	DWORD	getPid() const {return m_pid; }
	HANDLE	getProcessHandle() const { return m_hProcess; }
	BOOL	setProtectProcessPid();
	BOOL	startProtect();
	BOOL	UnLoadDriver();
	DWORD	getCheck();
	HANDLE	openVVProtect();
	BOOL	getCheckLive() const { return checkLive; }
	void	setCheckLive(const BOOL isLive) { checkLive = isLive; }
private:
	DWORD m_pid;
	HANDLE m_hProcess;
	HANDLE m_hDriver;
	BOOL checkLive;
};

#endif 
