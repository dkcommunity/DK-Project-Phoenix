//--------------------------------------------------------------------------
// MString.cpp
//--------------------------------------------------------------------------

/*	using namespace std 를 쓰면 좋지 않다는 한가지 교훈 --;;
#include <windows.h>
#include "MString.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <mmsystem.h>
#include <iostream.h>
#include <fstream>
#include "DebugInfo.h"
/*/
#include "MString.h"
#include "Client_PCH.h"
#include <stdarg.h>

//*/

// #include "DebugInfo.h"
// #define	new			DEBUG_NEW
// #define	delete		DEBUG_DELETE

//--------------------------------------------------------------------------
// static
//--------------------------------------------------------------------------
char MString::s_pBuffer[MAX_BUFFER_LENGTH];

//--------------------------------------------------------------------------
//
// constructor / destructor
//
//--------------------------------------------------------------------------
MString::MString() {
  m_Length = 0;
  m_pString = NULL;
}

MString::MString(const MString &str) {
  m_Length = 0;
  m_pString = NULL;
  *this = str;
}

MString::MString(const char *str) {
  m_Length = 0;
  m_pString = NULL;
  *this = str;
}

MString::~MString() {
  if (m_pString != NULL) {
    delete[] m_pString;
  }
}

//--------------------------------------------------------------------------
//
// member functions
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Init( len )
//--------------------------------------------------------------------------
// size만큼 memory확보
//--------------------------------------------------------------------------
void MString::Init(int len) {
  Release();

  m_Length = 0;
  m_pString = new char[len + 1];
  m_pString[0] = NULL;
}

//--------------------------------------------------------------------------
// Relase
//--------------------------------------------------------------------------
// memory에서 제거
//--------------------------------------------------------------------------
void MString::Release() {
  if (m_pString != NULL) {
    delete[] m_pString;
    m_pString = NULL;
    m_Length = 0;
  }
}

//--------------------------------------------------------------------------
// Assign operator =
//--------------------------------------------------------------------------
void MString::operator=(const char *str) {
  if (m_pString != NULL) {
    delete[] m_pString;
    m_pString = NULL;
  }

  if (str == NULL) {
    m_Length = 0;
  } else {
    m_Length = strlen(str);

    m_pString = new char[m_Length + 1];
    strcpy(m_pString, str);
  }
}

//--------------------------------------------------------------------------
// Assign operator =
//--------------------------------------------------------------------------
void MString::operator=(const MString &str) {
  //--------------------------------
  // 길이가 0인 경우..
  //--------------------------------
  if (str.m_Length == 0) {
    if (m_pString != NULL) {
      delete[] m_pString;
      m_pString = NULL;
    }

    m_Length = 0;

    if (str.m_pString != NULL) {
      m_pString = new char[1];
      m_pString[0] = '\0';
    }
  }
  //--------------------------------
  // 길이가 0 이상인 경우...
  //--------------------------------
  else {
    if (m_pString != NULL) {
      delete[] m_pString;
    }

    m_Length = str.m_Length;
    m_pString = new char[m_Length + 1];

    strcpy(m_pString, str.m_pString);
  }
}

//--------------------------------------------------------------------------
// Format
//--------------------------------------------------------------------------
// 적절한 형식으로 string을 만든다.
//--------------------------------------------------------------------------
void MString::Format(const char *format, ...) {
  va_list vl;

  va_start(vl, format);
  vsprintf(s_pBuffer, format, vl);
  va_end(vl);

  *this = s_pBuffer;
}

//--------------------------------------------------------------------------
// Save To File
//--------------------------------------------------------------------------
void MString::SaveToFile(std::ofstream &file) {
  file.write((const char *)&m_Length, 4);

  // length가 0이 아닌 경우에만..
  if (m_Length != 0) {
    file.write((const char *)m_pString, static_cast<int>(m_Length));
  }
}

//--------------------------------------------------------------------------
// Load From File (std::ifstream overload — Phoenix)
// vs_ui_gamecommon2.cpp içindeki yardımcı sınıflar VFS yerine doğrudan
// std::ifstream geçiyor (mail, event gift vb. LoadFromFile(ifstream&) zincirleri).
// Eski darkeden kodunda bu overload vardı, modernizasyonda kaldırılmıştı.
// ivfstream versiyonuyla birebir aynı mantık; sadece stream tipi farklı.
//--------------------------------------------------------------------------
void MString::LoadFromFile(std::ifstream &file) {
  if (m_pString != NULL) {
    delete[] m_pString;
    m_pString = NULL;
  }

  file.read((char *)&m_Length, 4);

  // Sanity check: if length is unreasonably large or negative, treat as empty
  if (m_Length < 0 || m_Length > 65535) {
    m_Length = 0;
  }

  bool bNull = (m_Length != 0);

  if (bNull) {
    m_pString = new char[m_Length + 1];
    if (m_Length != 0) {
      file.read((char *)m_pString, static_cast<int>(m_Length));
    }
    m_pString[m_Length] = '\0';
  } else {
    // FAZ 4 (2026-05-03): "<null std::string>" debug placeholder kaldırıldı
    // (UI'da literal görünüyordu). Boş string sakla.
    m_pString = new char[1];
    m_pString[0] = '\0';
  }
}

//--------------------------------------------------------------------------
// Load From File
//--------------------------------------------------------------------------
void MString::LoadFromFile(ivfstream &file) {
  if (m_pString != NULL) {
    delete[] m_pString;
    m_pString = NULL;
  }

  file.read((char *)&m_Length, 4);

  // Sanity check: if length is unreasonably large or negative, treat as empty
  // Prevents std::bad_alloc -> unhandled exception -> STATUS_FAIL_FAST
  // (0xC0000409)
  if (m_Length < 0 || m_Length > 65535) {
    m_Length = 0;
  }

  bool bNull = (m_Length != 0);

  if (bNull) {
    // len이 0이 아닌 경우에만...
    m_pString = new char[m_Length + 1];

    if (m_Length != 0) {
      file.read((char *)m_pString, static_cast<int>(m_Length));
    }

    m_pString[m_Length] = '\0';
  } else {
    // FAZ 4 (2026-05-03): Önceden burada "<null std::string>" debug placeholder
    // string'i saklanıyordu — boş option name'leri (item.inf'te bazı entry'lerin
    // HName/EName boş olabilir) UI'da literal "<null std::string>" olarak
    // gösteriyordu. Doğrusu: boş string olarak sakla.
    m_pString = new char[1];
    m_pString[0] = '\0';
  }
}
