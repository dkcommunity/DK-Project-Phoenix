//----------------------------------------------------------------------
// CTypeTable.h
//----------------------------------------------------------------------

#ifndef __CTYPETABLE_H__
#define __CTYPETABLE_H__

#pragma warning(disable : 4786)

#include <stdio.h>

// #include "DebugInfo.h"
// #define	 new DEBUG_NEW
#include "vfstream.h"

//----------------------------------------------------------------------
//
// Info?뿉 ????븳 ?젙蹂? Table
//
//----------------------------------------------------------------------
template <class Type> class CTypeTable {
public:
  CTypeTable();
  ~CTypeTable();

  //-------------------------------------------------------
  // Init / Release
  //-------------------------------------------------------
  void Init(int size);
  void Release();

  //-------------------------------------------------------
  // Size
  //-------------------------------------------------------
  int GetSize() const { return m_Size; }

  //-------------------------------------------------------
  // Reference
  //-------------------------------------------------------
  const Type &operator[](int type) const { return m_pTypeInfo[type]; }
  Type &operator[](int type) { return m_pTypeInfo[type]; }
  Type &Get(int type) { return m_pTypeInfo[type]; }

  //-------------------------------------------------------
  // File I/O
  //-------------------------------------------------------
  void SaveToFile(std::ofstream &file);
  void LoadFromFile(ivfstream &file);
  // Phoenix: vs_ui_gamecommon2.cpp içindeki helper sınıflar (EventGiftInfo vb.)
  // CTypeTable<T>::LoadFromFile'ı std::ifstream ile çağırıyor.
  void LoadFromFile(std::ifstream &file);
  void SaveToFile(const char *filename);
  void LoadFromFile(const char *filename);
  bool LoadFromFile_NickNameString(ivfstream &file);

protected:
  int m_Size;        // Type 醫낅쪟 ?닔
  Type *m_pTypeInfo; // Type ?젙蹂?
};

//----------------------------------------------------------------------
//
//    constructor/destructor
//
//----------------------------------------------------------------------
template <class Type> CTypeTable<Type>::CTypeTable() {
  m_pTypeInfo = NULL;
  m_Size = 0;
}

template <class Type> CTypeTable<Type>::~CTypeTable() { Release(); }

//----------------------------------------------------------------------
//
//  member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
template <class Type> void CTypeTable<Type>::Init(int size) {
  // 媛쒖닔媛? ?뾾?쓣 寃쎌슦
  // Sanity Check: Prevent bad_alloc on invalid or excessive sizes
  if (size <= 0 || size > 100000)
    return;

  // ?씪?떒 ?빐?젣
  Release();

  // 硫붾え由? ?옟湲?
  m_Size = size;

  m_pTypeInfo = new Type[m_Size];
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class Type> void CTypeTable<Type>::Release() {
  if (m_pTypeInfo != NULL) {
    // 紐⑤뱺 CSprite瑜? 吏??슫?떎.
    delete[] m_pTypeInfo;
    m_pTypeInfo = NULL;

    m_Size = 0;
  }
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
template <class Type> void CTypeTable<Type>::SaveToFile(std::ofstream &file) {
  // size ????옣
  file.write((const char *)&m_Size, 4);

  // ?븘臾? 寃껊룄 ?뾾?뒗 寃쎌슦
  if (m_pTypeInfo == NULL)
    return;

  // 媛곴컖?쓽 ?젙蹂? ????옣
  for (int i = 0; i < m_Size; i++) {
    m_pTypeInfo[i].SaveToFile(file);
  }
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class Type> void CTypeTable<Type>::LoadFromFile(ivfstream &file) {
  int numSize;

  // size ?씫?뼱?삤湲?
  file.read((char *)&numSize, 4);

  // Sanity Check: Prevent bad_alloc on invalid or excessive sizes
  if (numSize < 0 || numSize > 100000)
    return;

  // ?쁽?옱 ?옟????엳?뒗 硫붾え由ъ?? ?떎瑜대㈃ ?떎?떆 硫붾え由щ?? ?옟?뒗?떎.
  if (m_Size != numSize) {
    // 硫붾え由? ?빐?젣
    Release();

    // 硫붾え由? ?옟湲?
    Init(numSize);
  }

  // FAZ 4 (2026-05-04): EOF early-break kaldırıldı — partial-load tüm action.inf'i
  // bozuyor (m_Action=0, animasyon yok). Orijinal davranış restore.
  for (int i = 0; i < m_Size; i++) {
    m_pTypeInfo[i].LoadFromFile(file);
  }
}

// Phoenix: std::ifstream overload'ı — ivfstream versiyonuyla aynı mantık.
template <class Type> void CTypeTable<Type>::LoadFromFile(std::ifstream &file) {
  int numSize;
  file.read((char *)&numSize, 4);

  if (numSize < 0 || numSize > 100000)
    return;

  if (m_Size != numSize) {
    Release();
    Init(numSize);
  }

  // FAZ 4 (2026-05-04): EOF early-break kaldırıldı — partial-load tüm action.inf'i
  // bozuyor (m_Action=0, animasyon yok). Orijinal davranış restore.
  for (int i = 0; i < m_Size; i++) {
    m_pTypeInfo[i].LoadFromFile(file);
  }
}

template <class Type>
void CTypeTable<Type>::LoadFromFile(const char *lpszFilename) {
  ivfstream file(lpszFilename, std::ios::binary);
  if (file.is_open()) {
    LoadFromFile(file);
    file.close();
  }
}

template <class Type>
void CTypeTable<Type>::SaveToFile(const char *lpszFilename) {
  std::ofstream file(lpszFilename, std::ios::binary);
  SaveToFile(file);
  file.close();
}
// 2004, 6, 18 sobeit add start - nick name - ?뙆?씪 援ъ“媛? 姨? ????젮?꽌 ?쟾?슜?쑝濡?
// 留뚮벉..^^:
//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class Type>
bool CTypeTable<Type>::LoadFromFile_NickNameString(ivfstream &file) {
  int numSize;
  WORD wIndex;
  // size ?씫?뼱?삤湲?
  file.read((char *)&numSize, 4);

  // Sanity Check: Prevent bad_alloc on invalid or excessive sizes
  if (numSize < 0 || numSize > 100000)
    return false;

  // ?쁽?옱 ?옟????엳?뒗 硫붾え由ъ?? ?떎瑜대㈃ ?떎?떆 硫붾え由щ?? ?옟?뒗?떎.
  if (m_Size != numSize) {
    // 硫붾え由? ?빐?젣
    Release();

    // 硫붾え由? ?옟湲?
    Init(numSize);
  }

  for (int i = 0; i < m_Size; i++) {
    file.read((char *)&wIndex, 2);
    if (wIndex >= numSize)
      return false;
    m_pTypeInfo[wIndex].LoadFromFile(file);
  }
  return true;
}
// 2004, 6, 18 sobeit add start - nick name - ?뙆?씪 援ъ“媛? 姨? ????젮?꽌 ?쟾?슜?쑝濡?
// 留뚮벉..^^:
#endif
