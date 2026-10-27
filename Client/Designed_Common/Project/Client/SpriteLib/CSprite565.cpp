//----------------------------------------------------------------------
// CSprite565.cpp
//----------------------------------------------------------------------

#include "CSprite.h"
#include "CSprite565.h"
#include "crash_trace_tool.h"
#include <fstream>
#include <new>
#include <string.h>

namespace
{
	const WORD PHOENIX_MAX_SAFE_SPRITE_DIM = 8192;

	bool PhoenixReadExact565(ivfstream& file, char* data, unsigned bytes)
	{
		file.read(data, bytes);
		return file.gcount() == (int)bytes;
	}

	bool PhoenixValidateSpriteRow565(const WORD* row, WORD len, WORD width, int y)
	{
		if (row == NULL || len == 0)
			return false;

		const int count = row[0];
		int index = 1;
		int x = 0;

		if (count < 0 || count > (int)len)
		{
			CT_Done("CSprite565 invalid row count y=%d len=%d count=%d width=%d", y, len, count, width);
			return false;
		}

		for (int i = 0; i < count; ++i)
		{
			if (index + 1 >= (int)len)
			{
				CT_Done("CSprite565 invalid row header y=%d len=%d index=%d count=%d width=%d", y, len, index, count, width);
				return false;
			}

			const int transparentCount = row[index++];
			const int colorCount = row[index++];

			if (index + colorCount > (int)len)
			{
				CT_Done("CSprite565 invalid row payload y=%d len=%d index=%d colors=%d width=%d", y, len, index, colorCount, width);
				return false;
			}

			x += transparentCount + colorCount;
			if (x > (int)width)
			{
				CT_Done("CSprite565 row over width y=%d len=%d x=%d width=%d", y, len, x, width);
				return false;
			}

			index += colorCount;
		}

		return true;
	}
}

//extern BYTE	LOADING_STATUS_NONE;
//extern BYTE	LOADING_STATUS_NOW;
//extern BYTE	LOADING_STATUS_LOADING;

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// fstream에 save 한다.    ( file에는 5:6:5로 저장한다. )
//----------------------------------------------------------------------
bool
CSprite565::SaveToFile(std::ofstream& file)
{
	// width와 height를 저장한다.
	file.write((const char*)&m_Width , 2);
	file.write((const char*)&m_Height, 2);

	// NULL이면 저장하지 않는다. 길이만 저장되는 것이다.
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return false;

	// 압축 된 것 저장
	WORD index;

	int i;
	int j;

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i =0; i<m_Height; i++)
	{
		// 반복 회수의 2 byte
		int	count = m_Pixels[i][0],
				colorCount;
		index	= 1;

		// 각 line마다 byte수를 세어서 저장해야한다.
		for (int j =0; j<count; j++)
		{
			//transCount = m_Pixels[i][index];
			colorCount = m_Pixels[i][index+1];

			index+=2;	// 두 count 만큼

			index += colorCount;	// 투명색 아닌것만큼 +
		}

		// byte수와 실제 data를 저장한다.
		file.write((const char*)&index, 2);
		file.write((const char*)m_Pixels[i], index<<1);
	}


	return true;
}

//----------------------------------------------------------------------
// fstream에서 load한다.
//----------------------------------------------------------------------
bool
CSprite565::LoadFromFile(ivfstream& file)
{
	// 이거를 하나로 묶어야 하는데..
	if (m_bLoading)
	{
		return false;
	}
	m_bLoading = true;

	// F --> T
	/*
	bool*	pCheck = &m_bLoading;

	bool	T = true;
	bool	F = false;

	// Loading하고 있지 않은 상태인 경우
	// Loading할려는 상태로 만든다.
	void* pOld = InterlockedCompareExchange(
						(PVOID *)&pCheck,  // pointer to the destination pointer
						&T,      // the exchange value
						&F		// the value to compare
					);

	bool old = *(bool*)pOld;

	// 지금 loading할것이 아니면 return
	*/

	// loadind중이라고 표시
	m_bLoading = true;


	// 이미 잡혀있는 memory를 release한다.
	Release();

	//long start = file.tellg();

	// width와 height를 Load한다.
	if (!PhoenixReadExact565(file, (char*)&m_Width, 2) ||
		!PhoenixReadExact565(file, (char*)&m_Height, 2))
	{
		CT_Done("CSprite565::LoadFromFile header read failed");
		m_bLoading = false;
		return false;
	}


	// 길이가 0이면 더 Load할게 없겠지..
	if (m_Width==0 || m_Height==0)
	{
		m_bInit = true;

		m_bLoading = false;

		return true;
	}

	if (m_Width > PHOENIX_MAX_SAFE_SPRITE_DIM || m_Height > PHOENIX_MAX_SAFE_SPRITE_DIM)
	{
		CT_Done("CSprite565::LoadFromFile invalid dimension w=%d h=%d", m_Width, m_Height);
		m_Width = 0;
		m_Height = 0;
		m_bLoading = false;
		return false;
	}

	//---------------------------------
	// for OLD version of CSprite
	//---------------------------------
	//BOOL dummy;
	//file.read((char*)&dummy, 1);
	//---------------------------------

	m_Pixels = NULL;
	m_Pixels = new (std::nothrow) WORD* [m_Height];
	//m_Pixels = (WORD**)malloc(sizeof(WORD*)*m_Height);
	if (m_Pixels == NULL)
	{
		CT_Done("CSprite565::LoadFromFile pointer allocation failed w=%d h=%d", m_Width, m_Height);
		m_Width = 0;
		m_Height = 0;
		m_bLoading = false;
		return false;
	}
	memset(m_Pixels, 0, sizeof(WORD*) * m_Height);

	WORD len;
	auto failLoad = [this](const char* reason, int row, int rowLen) -> bool
	{
		CT_Done("CSprite565::LoadFromFile invalid sprite reason=%s w=%d h=%d row=%d len=%d", reason, m_Width, m_Height, row, rowLen);
		Release();
		m_Width = 0;
		m_Height = 0;
		m_bLoading = false;
		return false;
	};

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		// byte수와 실제 data를 Load한다.
		if (!PhoenixReadExact565(file, (char*)&len, 2))
			return failLoad("row length read failed", i, 0);

		if (len == 0)
		{
			m_Pixels[i] = new (std::nothrow) WORD [1];
			if (m_Pixels[i] == NULL)
				return failLoad("empty row allocation failed", i, len);
			m_Pixels[i][0] = 0;
			continue;
		}

		m_Pixels[i] = NULL;
		m_Pixels[i] = new (std::nothrow) WORD [len];
		//m_Pixels[i] = (WORD*)malloc(sizeof(WORD)*len);
		if (m_Pixels[i] == NULL)
			return failLoad("row allocation failed", i, len);

		if (!PhoenixReadExact565(file, (char*)m_Pixels[i], len<<1))
			return failLoad("row payload read failed", i, len);

		if (!PhoenixValidateSpriteRow565(m_Pixels[i], len, m_Width, i))
			return failLoad("row validation failed", i, len);
	}

	m_bInit = true;

	m_bLoading = false;

	return true;
}


//----------------------------------------------------------------------
// fstream에서 Buffer로 load한다.
//----------------------------------------------------------------------
/*
bool
CSprite565::LoadFromFileToBuffer(std::ifstream& file)
{
	// width와 height를 Load한다.
	file.read((char*)&s_Width , 2);
	file.read((char*)&s_Height, 2);

	// 길이가 0이면 더 Load할게 없겠지..
	if (s_Width==0 || s_Height==0)
		return false;

	//---------------------------------
	// for OLD version of CSprite
	//---------------------------------
	//BOOL dummy;
	//file.read((char*)&dummy, 1);
	//---------------------------------

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<s_Height; i++)
	{
		// byte수와 실제 data를 Load한다.
		file.read((char*)&s_BufferLen[i], 2);

		file.read((char*)s_Buffer[i], s_BufferLen[i]<<1);
	}

	return true;
}
*/
