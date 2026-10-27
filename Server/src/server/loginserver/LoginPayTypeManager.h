//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPayTypeManager.cpp
// Written by  : rappi76
// Description : 
//
//////////////////////////////////////////////////////////////////////

#include "LoginPlayer.h"

class LoginPayTypeManager
{
public :
	enum PayType
	{
		LOGIN_PAYTYPE_ALL_ALLLOW	= 0,	// default - 모든 로그인 허용
		LOGIN_PAYTYPE_PREMIUM		= 1		// 프리미엄/패밀리 요금제 사용자 로그인 허용
		
	};
	
	bool IsAllow(WorldID_t WorldID, LoginPlayer* pLoginPlayer);
	
};

extern LoginPayTypeManager * g_pLoginPayTypeManager;
