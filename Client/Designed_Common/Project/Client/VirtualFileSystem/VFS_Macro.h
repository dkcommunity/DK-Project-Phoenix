//////////////////////////////////////////////////////////////////////////////
/// @file VFS_Macro.h
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstring>
#include <string>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////
/// @fn make_unique_path
///
/// ��θ� ����ũ�ϰ� ��ȯ�Ѵ�.
//////////////////////////////////////////////////////////////////////////////
inline void make_unique_path(char *s) {
  char *d = s;
  char c;
  while (c = *s) {
    while ((c == '/' || c == '\\') && c == s[1])
      c = *(++s);
    c = tolower(c);
    if (c == '\\')
      c = '/';
    *d = c;
    ++s, ++d;
  }
  *d = '\0';
}

//////////////////////////////////////////////////////////////////////////////
/// @fn make_temp_filepath
///
/// �ӽ� ��θ� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
inline void make_temp_filepath(LPCTSTR path, LPTSTR tempPath) {
  const char *tok = "\\/";
  const size_t MAX_TEMP_PATH = MAX_PATH;  // 260

  // 1. INPUT PATH CHECK - ≥ 260 ise güvenli fail
  if (strlen(path) >= MAX_TEMP_PATH) {
    tempPath[0] = '\0';
    return;
  }

  // 2. Safe prefix copy
  const char* prefix = "temp/$";
  size_t len = strlen(prefix);
  if (len >= MAX_TEMP_PATH) return;
  strcpy_s(tempPath, MAX_TEMP_PATH, prefix);

  // 3. Safe input copy (artık check geçtiği için fail etmez)
  char szPathCopy[MAX_PATH];
  strcpy_s(szPathCopy, MAX_PATH, path);

  char *pFilePath = strtok(szPathCopy, tok);
  if (pFilePath) {
    size_t componentLen = strlen(pFilePath);
    if (len + componentLen >= MAX_TEMP_PATH) return;
    strcat_s(tempPath, MAX_TEMP_PATH, pFilePath);
    len += componentLen;
  }

  while ((pFilePath = strtok(NULL, tok)) != NULL) {
    if (len + 1 >= MAX_TEMP_PATH) return;
    strcat_s(tempPath, MAX_TEMP_PATH, "$");
    len += 1;

    size_t componentLen = strlen(pFilePath);
    if (len + componentLen >= MAX_TEMP_PATH) return;
    strcat_s(tempPath, MAX_TEMP_PATH, pFilePath);
    len += componentLen;
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @struct compare_path_string
///
/// �ΰ��� string�� ��η� �����ϰ� ��
/// - std::map�� template 3��° ���ڷ� ��밡��
/// - std::set�� template 2��° ���ڷ� ��밡��
//////////////////////////////////////////////////////////////////////////////
struct compare_path_string {
  bool operator()(const std::string &s1, const std::string &s2) const {
    const char *p1 = s1.c_str();
    const char *p2 = s2.c_str();

    char c1, c2;
    while ((c1 = *p1) & (c2 = *p2)) {
      while ((c1 == '/' || c1 == '\\') && c1 == p1[1])
        c1 = *(++p1);
      while ((c2 == '/' || c2 == '\\') && c2 == p2[1])
        c2 = *(++p2);

      c1 = tolower(c1);
      if (c1 == '\\')
        c1 = '/';

      c2 = tolower(c2);
      if (c2 == '\\')
        c2 = '/';

      if (c1 != c2)
        return c1 < c2;

      ++p1;
      ++p2;
    }

    if (!c1 && !c2)
      return false;
    else
      return c1 < c2;
  }
};

//////////////////////////////////////////////////////////////////////////////
/// @struct hash_func_path_string_stlport
///
/// �ΰ��� string�� ��η� �����ϴ� hash function
/// - hash_map(stlport)�� template 3��° ���ڷ� ��밡��
//////////////////////////////////////////////////////////////////////////////
struct hash_func_path_string_stlport {
  size_t operator()(const std::string &s) const {
    unsigned long hash = 5381;
    int c;
    const char *p = s.c_str();

    while (c = *p) {
      while ((c == '/' || c == '\\') && c == p[1])
        c = *(++p);
      c = tolower(c);
      if (c == '\\')
        c = '/';
      hash = ((hash << 5) + hash) + c;
      ++p;
    }

    return hash;
  };
};

//////////////////////////////////////////////////////////////////////////////
/// @struct equal_path_string
///
/// �ΰ��� string�� ��η� �����ϰ� ���� ������ üũ
/// - hash_map(stlport)�� template 4��° ���ڷ� ��밡��
//////////////////////////////////////////////////////////////////////////////
struct equal_path_string {
  bool operator()(const std::string &s1, const std::string &s2) const {
    const char *p1 = s1.c_str();
    const char *p2 = s2.c_str();

    char c1, c2;
    while ((c1 = *p1) & (c2 = *p2)) {
      while ((c1 == '/' || c1 == '\\') && c1 == p1[1])
        c1 = *(++p1);
      while ((c2 == '/' || c2 == '\\') && c2 == p2[1])
        c2 = *(++p2);

      c1 = tolower(c1);
      if (c1 == '\\')
        c1 = '/';
      c2 = tolower(c2);
      if (c2 == '\\')
        c2 = '/';

      if (c1 != c2)
        return false;

      ++p1;
      ++p2;
    }

    if (!c1 && !c2)
      return true;
    return false;
  }
};

//////////////////////////////////////////////////////////////////////////////
/// @fn equal_path_string
///
/// XOR ����ŷ �� ���ش�. -__-
//////////////////////////////////////////////////////////////////////////////
inline void encrypt_data(char *data, int size, int key) {
  ////const char*	XOR_MASKING_VALUE	= "This's Korea";
  // const char* XOR_MASKING_VALUE = "chyaya";
  // const int	XOR_MASKING_SIZE	= 6;
  const char *XOR_MASKING_VALUE = "zhegeshixiexuede";
  const int XOR_MASKING_SIZE = 16;

  if (key < 0)
    key = -key;

  if (size > XOR_MASKING_SIZE) {
    key %= (size - XOR_MASKING_SIZE);

    for (int i = 0; i < XOR_MASKING_SIZE; ++i, ++key)
      data[key] ^= XOR_MASKING_VALUE[i];
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @fn decode_data
///
/// ��ȣ Ǯ���ش�. ���� ����
//////////////////////////////////////////////////////////////////////////////
inline void decode_data(char *data, int size, int key) {
  encrypt_data(data, size, key);
}

//////////////////////////////////////////////////////////////////////////////
/// @fn calc_compress_buffer_size
///
/// ���� ���� ���鶧 �ణ ����ũ�⸦ ũ�� �ؾ��Ѵ�.
//////////////////////////////////////////////////////////////////////////////
inline int calc_compress_buffer_size(int size) {
  return size + (size >> 4) + 64 + 3;
}
