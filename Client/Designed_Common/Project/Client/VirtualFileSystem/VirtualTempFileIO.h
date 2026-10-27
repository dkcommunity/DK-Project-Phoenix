//////////////////////////////////////////////////////////////////////
//
// Filename    : FileIO.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CNormalFileIO Class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IFileIO.h"

class VirtualFileSystem;
typedef VirtualFileSystem *LPVFSYSTEM;

//////////////////////////////////////////////////////////////////////
//
// VirtualTempFileIO
//
//////////////////////////////////////////////////////////// chyaya //
class VirtualTempFileIO : public IFileIO {
public:
  VirtualTempFileIO(VirtualFileSystem *pVFS);
  virtual ~VirtualTempFileIO();

public:
  void open(LPCTSTR szName, int nMode);
  void close();

  bool is_open() const;

  void read(char *buf, int len);

  int gcount() const;

  void seekg(long off, int dir = std::ios_base::beg);
  int tellg();

  void write(const char *src, int len);

  void seekp(long off, int dir = std::ios_base::beg);
  int tellp();

  bool eof() const;

  void get(char &ch);
  void *getline(char *psch, int nCount, char delim = '\n');

public:
  LPCTSTR get_temppath() { return m_strTempPath.c_str(); }

private:
  std::string m_strTempPath;
  bool m_bOpen;

  LPVFSYSTEM m_pVFS;
};

#ifndef _DEBUG
#define VT_FILEIO_INLINE inline
#include "VirtualTempFileIO.inl"
#endif
