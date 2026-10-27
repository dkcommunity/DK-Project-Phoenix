#include <fstream>
#include <iostream>
#include "IFileIO.h"
class VirtualFileSystem;
//////////////////////////////////////////////////////////////////////////////
/// @file iovfs_base.h
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VirtualFileSystem.h"
#include "IFileIO.h"

class IFileIO;

//////////////////////////////////////////////////////////////////////////////
/// @class iovfs_base
/// @brief I/O Package File Stream Base
///
/// 일반 파일과 패키지 파일을 동일한 인터페이스로 접근 가능하게 하는 클래스
/// iostream과 같은 인터페이스로 사용이 가능하도록 디자인했다.
/// 아직 iostream의 모든 기능이 지원되지는 않지만
/// 기본적인 읽기/쓰기 기능은 구현된 상태이며 나중에 iostream의 특정 함수가
/// 필요하다면 iostream을 사용하지 말고 여기에 직접 구현해서 사용하길 권장한다. 
//////////////////////////////////////////////////////////////////////////////
#define EXCEPT_FILE_MAX 6

class iovfs_base
{
public:
	/// @brief 기본 생성자
	/// @sa Init()
	iovfs_base();

	/// @brief open 생성자
	/// @param szName 파일 경로
	/// @param nMode 파일 오픈 모드
	/// @sa Init(), open()
	iovfs_base(LPCTSTR szName, int nMode);

	/// @brief 파괴자
	/// @sa Destory()
	virtual ~iovfs_base();

public:
	/// @brief 멤버변수를 초기화한다.
	void			Init();

	/// @brief 멤버변수를 파괴한다.
	void			Destory();

public:
	/// @brief 파일을 연다.
	/// @param szName 파일 경로
	/// @param nMode 파일 오픈 모드
	virtual void	open(LPCTSTR szName, int nMode);

	/// @brief 파일을 닫는다.
	void			close();

	/// @brief 파일이 정상적으로 열려있는지 확인 한다.
	/// @return 파일이 정상적으로 열려있으면 true
	bool			is_open() const;

	/// @brief Get Count. 즉 방금 읽은 바이트 수를 반환한다.
	/// @return 방금 읽은 바이트 수
	/// @sa ivfstream::read()
	int				gcount() const;

	/// @brief 현재 열려있는 파일이 가상 파일인지 확인 한다.
	/// @return 열려있는 파일이 가상 파일이면 true
	bool			is_virtualfile() const;

	/// @brief 전역 VirtualFileSystem을 연다.
	/// @param filename VFS파일 경로
	/// @param flags 오픈 모드
	/// @warning 클라이언트에서 사용할 때는 읽기 전용으로 하는 것이 좋다. (flags를 FS_READ)
	///          VFS에 데이터를 기록 중에 비정상 종료될 경우 파일의 무결성을 보장 할 수 없기 때문이다.
	/// @sa s_VFSystem
	static void			start_vfs(LPCTSTR filename, int flags);

	/// @brief 전역 VirtualFileSystem을 닫는다.
	/// @sa s_VFSystem
	static void			end_vfs();	

	/// @brief 전역 VirtualFileSystem의 포인터를 얻는다.
	/// @return 전역 VFS의 포인터
	/// @sa s_VFSystem
	static LPVFSYSTEM	get_vfs()	{ return &s_VFSystem;	}

protected:
	IFileIO*		m_pFileIO;			///< 파일 읽기 인터페이스
	static VFSYSTEM s_VFSystem;			///< 전역 VirtualFileSystem
	std::string		m_strExceptList[EXCEPT_FILE_MAX];
};


//////////////////////////////////////////////////////////////////////////////
/// @class ivfstream
/// @brief Virtual File System Input Stream
///
/// 일반 파일 또는 가상 파일에서 데이터를 읽어오는 스트림
//////////////////////////////////////////////////////////////////////////////
class ivfstream : public iovfs_base
{
public:
	/// @brief 기본 생성자
	ivfstream();

	/// @brief 읽기 모드로 파일을 여는 생성자
	/// @param szName 파일 경로
	/// @param nMode 파일 오픈 모드, ios_base::in은 자동으로 추가된다.
	/// @sa open()
	ivfstream(LPCTSTR szName, int nMode);

	/// @brief 파괴자
	~ivfstream();

public:
	/// @brief 일반 파일 또는 가상 파일을 읽기 모드로 연다.
	/// @param szName 파일 경로
	/// @param nMode 오픈 모드, ios_base::in은 자동으로 추가된다.
	void			open(LPCTSTR szName, int nMode);

	/// \brief 데이터를 읽는다.
	/// \param buf 읽기 버퍼
	/// \param len 읽을 길이 (BYTE단위)
	void			read(char* buf, unsigned len);

	/// @brief 데이터를 탐색한다.
	/// @param off 탐색 범위
	/// @param dir 탐색 방향
	/// - ios_base::beg 파일의 시작 위치부터 탐색 (default)
	/// - ios_base::cur 파일의 현재 위치부터 탐색
	/// - ios_base::end 파일의 끝 위치부터 탐색
	void			seekg(long off, int dir = std::ios_base::beg);

	/// @brief 데이터를 읽기 위치를 얻는다.
	/// @return 파일에서의 읽기 위치
	int				tellg();

	/// @brief 파일의 끝인지 확인한다.
	/// @return 파일의 끝이면 true
	bool			eof() const;

	/// @brief 파일에서 한글자를 읽는다.
	/// @param ch 읽어온 글자를 외부로 변수
	void			get(char& ch);

	/// @brief 파일에서 한줄을 읽는다.
	/// @param psch 읽어온 글자를 저장할 버퍼
	/// @param nCount 버퍼 크기
	/// @param delim 줄바꿈 문자
	void*			getline(char* psch, int nCount, char delim = '\n');
};


//////////////////////////////////////////////////////////////////////////////
/// @class ovfstream
/// @brief Virtual File System Output Stream
///
/// 일반 파일 또는 가상 파일에 데이터를 기록하는 스트림
//////////////////////////////////////////////////////////////////////////////
class ovfstream : public iovfs_base
{
public:
	/// @brief 기본 생성자
	ovfstream();

	/// @brief 쓰기 모드로 파일을 여는 생성자
	/// @param szName 파일 경로
	/// @param nMode 파일 오픈 모드, ios_base::out은 자동으로 추가된다.
	/// @sa open()
	ovfstream(LPCTSTR szName, int nMode);

	/// @brief 파괴자
	~ovfstream();

public:
	/// @brief 일반 파일 또는 가상 파일을 쓰기 모드로 연다.
	/// @param szName 파일 경로
	/// @param nMode 오픈 모드, ios+_base::out은 자동으로 추가된다.
	void			open(LPCTSTR szName, int nMode);

	/// @brief 버퍼를 파일에 기록한다.
	/// @param src 소스 버퍼
	/// @param len 버퍼 크기
	void write(const char* src, int len);
	void seekg(long off, int dir = std::ios_base::beg);
};


#ifndef _DEBUG
#define PACKAGEFILE_INLINE inline
#include "vfstream.inl"
#endif
