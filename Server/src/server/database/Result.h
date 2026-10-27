//////////////////////////////////////////////////////////////////////////////
// File Name	: Result.h
// Written By	: Gday29@ewestsoft.com
// Description	: Definition of Result Class
//////////////////////////////////////////////////////////////////////////////

#ifndef __RESULT_H__
#define __RESULT_H__

#include "Types.h"
#include "Exception.h"
#include <sys/time.h>
#include <mysql/mysql.h>

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Connection;
class Statement;

#define T_RESULT MYSQL_RES

//////////////////////////////////////////////////////////////////////////////
// class Result;
// Result �� ������ �ʿ䰡 ����. ����ڴ� Statement �� �����ϸ� �ȴ�.
//////////////////////////////////////////////////////////////////////////////

class Result 
{
public:
	Result(T_RESULT *, const string& statement) throw(Error);
	~Result() throw();

public:
	// ���� row�� �Ѿ��.
	bool next() throw(Error);
	
	// Ư�� �ʵ�(�÷�) ���� ������ �´�.
	char * getField(uint index) throw(OutOfBoundException, Error);
	// FAZ 4 (2026-05-02): NULL guard eklendi. Eski hâl atoi(NULL)/strtoul(NULL)
	// segfault ediyordu (libc.so.6 +0x57e04). Kanıt: GDB stack trace
	// Slayer::hasTaoZhuang() Slayer.cpp:2790 → getInt(2) → atoi(NULL).
	// MySQL computed column (TzWearPart-1) NULL döndüğünde crash. Sistematik
	// fix: NULL'da default 0. Davranış kaybı: NULL data sessizce 0 olur.
	char getChar(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? p[0] : '\0'; }
	int getInt(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? atoi(p) : 0; }
	uint getUInt(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? (uint)atoi(p) : 0u; }
	BYTE getBYTE(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? (BYTE)atoi(p) : (BYTE)0; }
	WORD getWORD(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? (WORD)atoi(p) : (WORD)0; }
	DWORD getDWORD(uint index) throw(OutOfBoundException, Error) { char* p = getField(index); return p ? strtoul(p, (char**)NULL, 10) : 0UL; }
	const char* getString(uint index) throw(OutOfBoundException, Error);

	// ���� ������� �����ϴ� row/column�� ���ڸ� �����Ѵ�.
	uint getRowCount() const throw() { return m_RowCount; }
	uint getFieldCount() const throw() { return m_FieldCount; }

	string getStatement(void) const { return m_Statement; }

private:
	T_RESULT*  m_pResult;    // ������� ��Ÿ���� MYSQL structure
	MYSQL_ROW  m_pRow;       // ���� ó���ϰ� �ִ� row
	uint       m_RowCount;   // ���� ����� �� row�� ����
	uint       m_FieldCount;
	string     m_Statement;  // � query���� ���� ����ΰ�...?
};

#endif // __RESULT_H__
