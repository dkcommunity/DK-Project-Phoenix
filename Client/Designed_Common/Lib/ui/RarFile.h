// RarFile.h: interface for the CRarFile class.
// Made by 쑥갓 2001. 7. 24
//////////////////////////////////////////////////////////////////////
//
// FAZ 4 (2026-05-04) — DESC ABI PARADOX ROOT CAUSE FIX
// ----------------------------------------------------
// Bu dosya tarihen orphan idi: Designed_Common/Lib/ui altinda eslesen .cpp
// YOK. Gercek implementation Project/Client/RarLib/RarFile.cpp'de ve
// 6. private alan `VirtualTempFileIO* m_pTempFile` kullaniyor (4 yerde,
// new/delete + temp file open/close path'inde).
//
// Eski hal (5 alan) Client.exe target'i icin sizeof(CRarFile) = 248 + alpha
// yapiyordu, vs_ui_lib hedefi RarLib/RarFile.h'i (6 alan) gordugu icin
// sizeof = 252 + alpha. Tam 4 byte fark — VS_UI_DESC sinifinin `CRarFile
// m_pack_file` alani araciligi ile sizeof(C_VS_UI_DESC) Client=248 vs
// vs_ui_lib=252 asimetrisini uretiyordu (Brief F audit'in izleyemedigi
// 8 deneme paradox'u).
//
// Cozum: Bu header'i RarLib/RarFile.h ile bire-bir senkron tutmak. Public
// interface aynen korunur (sadece `DeleteTempFile()` method declaration'i
// eklendi); private layout RarLib versiyonu ile esit hale getirildi —
// `VirtualTempFileIO* m_pTempFile` alani forward-declared sinif pointer'i
// olarak eklendi (sizeof = 4 byte x86, ABI deterministik).
//////////////////////////////////////////////////////////////////////

#ifndef _RAR_FILE_HEADER_
#define _RAR_FILE_HEADER_

#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// urarlib.h
///////////////////////////////////////////////////////////////////////////////
#define _DO_CRC32_CHECK                     /* perform cyclical redundancy  */
                                            /* check (CRC32) - disable this */
                                            /* for a little speed-up        */
#define _USE_ASM                            /*
                                             * enable assembly extensions
                                             * x86 cpus.
                                             */

#define _WIN_32                             /* Win32 with VisualC           */

typedef unsigned char    UBYTE;             /* WIN32 definitions            */
typedef unsigned short   UWORD;
typedef unsigned long    UDWORD;

/* This structure is used for listing archive content                       */
struct RAR20_archive_entry                  /* These infos about files are  */
{                                           /* stored in RAR v2.0 archives  */
  char *Name;
  UWORD NameSize;
  UDWORD PackSize;
  UDWORD UnpSize;
  UBYTE HostOS;                             /* MSDOS=0,OS2=1,WIN32=2,UNIX=3 */
  UDWORD FileCRC;
  UDWORD FileTime;
  UBYTE UnpVer;
  UBYTE Method;
  UDWORD FileAttr;
};

struct ArchiveList_struct
{
  struct RAR20_archive_entry item;
  struct ArchiveList_struct *next;
};
/*typedef struct
{
  struct RAR20_archive_entry item;
  struct ArchiveList_struct *next;
} ArchiveList_struct;*/

// FAZ 4 (2026-05-04): RarLib/RarFile.h ile ABI sync — VirtualTempFileIO
// forward declaration. Implementation Project/Client/RarLib/RarFile.cpp'de;
// new/delete sadece o TU icinde gerceklesir, bu header pointer'i ileri
// declare etmek yeterli (incomplete type, sizeof gerekmez).
class VirtualTempFileIO;

////////////////////////////////////////////////////////////////////////////////////

class CRarFile
{
private:
	std::string m_rar_filename;
	std::string m_password;

	// FAZ 4 (2026-05-04): RarLib/RarFile.h ile ABI sync — bu alanin eksikligi
	// VS_UI_DESC sizeof asimetrisinin (Client=248 vs lib=252) sebebiydi.
	VirtualTempFileIO* m_pTempFile;

	char *m_data;
	char *m_file_pointer;
	int m_size;

public:
	// 생성자
	CRarFile();
	CRarFile(const char *rar_filename, const char *pass);

	// 소멸자
	~CRarFile();

	// 읽어온 rar을 해제한다.
	void Release();

	// FAZ 4 (2026-05-04): RarLib/RarFile.h ile sync — temp file cleanup metodu.
	// Implementation Project/Client/RarLib/RarFile.cpp:65-70.
	void DeleteTempFile();

	// rar파일 이름 & 패스 세팅
	void SetRAR(const char *rar_filename, const char *pass);

	// 파일 열기
	bool Open(const char *in_filename);

	// 파일에서 읽기
	char*	Read(char *buf, int size);
	char*	Read(int size);
	bool	GetString(char* buf, int size);

	// 파일이 열렸나?
	bool	IsSet()	{ return (m_rar_filename !=""); }

	//	파일의 끝이면 true else false
	bool	IsEOF(int plus = 0);

	std::vector<std::string> *GetList(char *filter = NULL);

	const char *GetFilePointer() const { return m_file_pointer; }
	const int GetSize() const { return m_size; }


};

// FAZ 4 (2026-05-04 oturum 2) ABI SENTINEL — Cross-TU sizeof(CRarFile) guard.
// CRarFile m_pack_file / m_rpk_file / m_Quest_XML_file etc. members live in
// VS_UI_DESC, VS_UI_Control, VS_UI_GameCommon2, VS_UI_Tutorial,
// MHelpMessageManager. If even one of those TUs sees a stale CRarFile layout
// (e.g. incremental build cache from before m_pTempFile was added), inline
// accessors like GetFilePointer() read the wrong offset and return garbage —
// which is exactly what crashed Client when EventGQuest.xml load returned
// from urarlib path. Build will fail loudly here instead of silently
// crashing in random callers.
//
// Expected: 60 bytes on x86 (2x std::string MSVC SSO=24, +4 ptr +4 ptr +4 ptr
// +4 int = 60). Adjust if your STL implementation produces a different
// std::string size (STLPort etc).
static_assert(sizeof(CRarFile) >= 60,
	"CRarFile ABI mismatch — sizeof too small. The m_pTempFile field is "
	"likely missing (stale 5-field layout from before 2026-05-04 sync). "
	"Clean rebuild Client to invalidate incremental cache, or re-sync "
	"Lib/ui/RarFile.h with Project/Client/RarLib/RarFile.h.");

#endif
