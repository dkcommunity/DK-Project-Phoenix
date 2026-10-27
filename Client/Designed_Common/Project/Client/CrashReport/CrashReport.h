//////////////////////////////////////////////////////////////////////////////
/// @file CrashReport.h
/// @author chyaya
/// @date 2007.10.25
//////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////////
/// @class CrashReport
/// @brief Crash report manager for BugTrap library
///
/// BugTrap 라이브러리를 사용해서 CrashReport를 전송받아 BoA요~
//////////////////////////////////////////////////////////////////////////////
class CrashReport
{
public:	
	/// @brief 옵션 파일을 연다.
	/// @param fileName 파일 이름
	/// @return 성공 여부
	static bool LoadOptionFile(const char* fileName);

	/// @brief 버전을 세팅한다.
	/// @param version 버전
	///
	/// 버전 파일이 따로 빠져 있어서 부득이하게 - _-;
	static void SetAppVersion(int version);

	/// @brief 모든 프로시저에서 이 함수가 호출되어야한다.
	static void Install();
};
