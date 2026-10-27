//-----------------------------------------------------------------------------
// AcceleratorManager.h
//-----------------------------------------------------------------------------

#ifndef __ACCELERATORMANAGER_H__
#define __ACCELERATORMANAGER_H__

// Phoenix: __CONTENTS() makrosu için gerekli — koşullu Function_ACCEL_*
// tanımları bu include olmadan false'ta kalıyor.
#include "ContentsFilter.h"

#include <Windows.h>
#include "MFunctionManager.h"

class AcceleratorManager : public MFunctionManager {
	public :
		AcceleratorManager();
		~AcceleratorManager();

	public :
		static void		Function_ACCEL_F1(void* pVoid);
		static void		Function_ACCEL_F2(void* pVoid);
		static void		Function_ACCEL_F3(void* pVoid);
		static void		Function_ACCEL_F4(void* pVoid);
		static void		Function_ACCEL_F5(void* pVoid);
		static void		Function_ACCEL_F6(void* pVoid);
		static void		Function_ACCEL_F7(void* pVoid);
		static void		Function_ACCEL_F8(void* pVoid);
		static void		Function_ACCEL_F9(void* pVoid);
		static void		Function_ACCEL_F10(void* pVoid);
		static void		Function_ACCEL_F11(void* pVoid);
		static void		Function_ACCEL_F12(void* pVoid);
		static void		Function_ACCEL_ESC(void* pVoid);
		static void		Function_ACCEL_SKILL(void* pVoid);
		static void		Function_ACCEL_INVENTORY(void* pVoid);
		static void		Function_ACCEL_GEAR(void* pVoid);
		static void		Function_ACCEL_CHARINFO(void* pVoid);
		static void		Function_ACCEL_SKILLINFO(void* pVoid);
		static void		Function_ACCEL_GRADE1INFO(void* pVoid);
		static void		Function_ACCEL_WORLDMAP(void* pVoid);
		static void		Function_ACCEL_PARTY(void* pVoid);
		static void		Function_ACCEL_MARK(void* pVoid);
		static void		Function_ACCEL_HELP(void* pVoid);
		static void		Function_ACCEL_QUICKITEM_SLOT(void* pVoid);
		static void		Function_ACCEL_EXTEND_CHAT(void* pVoid);
		static void		Function_ACCEL_CHAT_UP(void* pVoid);
		static void		Function_ACCEL_CHAT_DN(void* pVoid);
		static void		Function_ACCEL_NORMALCHAT(void* pVoid);
		static void		Function_ACCEL_ZONECHAT(void* pVoid);
		static void		Function_ACCEL_WHISPER(void* pVoid);
		static void		Function_ACCEL_PARTYCHAT(void* pVoid);
		static void		Function_ACCEL_GUILD(void* pVoid);
		static void		Function_ACCEL_UNION(void* pVoid);
//		static void		Function_ACCEL_GRADE2INFO(void* pVoid);
//		static void		Function_ACCEL_GRADE3INFO(void* pVoid);
		static void		Function_ACCEL_QUEST(void* pVoid);
		static void		Function_ACCEL_MAILBOX(void* pVoid);
		static void		Function_ACCEL_PETINFO(void* pVoid);
		static void		Function_ACCEL_SUMMON_PET(void* pVoid);
		static void		Function_ACCEL_TRIBE_PRIOR(void* pVoid);
		static void		Function_ACCEL_TRIBE_NEXT(void* pVoid);
//		static void		Function_ACCEL_GUILD_INFO(void* pVoid);
//		static void		Function_ACCEL_GUILD_MEMBER_LIST(void* pVoid);
		static void		Function_ACCEL_COER_ATTACK(void* pVoid);
// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
		static void		Function_ACCEL_AUTO_ATTACK(void* pVoid);
#endif  //__AUTO_ATTACT
// end
		static void		Function_ACCEL_SYSRQ(void* pVoid);
		static void		Function_ACCEL_MINIMAP(void* pVoid);
//		static void		Function_ACCEL_SLASH(void* pVoid);
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// AcceleratorManager ����â ��üŰ ȣ�� �Լ� ����
		static void		Function_ACCEL_GEAR_CHANGE(void *pVoid);
		static void		Function_ACCEL_GEAR_CHANGE_SHOW(void *pVoid);
#endif // __GEAR_SWAP_CHANGE
};

extern AcceleratorManager*		g_pAcceleratorManager;

#endif

