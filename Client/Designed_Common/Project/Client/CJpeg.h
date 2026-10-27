#ifndef __C_JPEG_H__
#define __C_JPEG_H__

#include "VirtualTempFileIO.h"

extern bool LoadJPG(LPCTSTR lpszFileName, int &width, int &height, int &bpp, unsigned char** p_data);

class CJpeg
{
public:
	CJpeg()								{ m_iWidth = m_iHeight = m_iBpp = -1; m_pImage = NULL; }
	~CJpeg()							{ Release(); }

	void Release()						{ m_iWidth = m_iHeight = m_iBpp = -1; if(m_pImage != NULL){ delete []m_pImage; m_pImage = NULL; } }
	bool Open(LPCTSTR lpszFileName)
	{
		Release();
		VirtualTempFileIO tempFile(iovfs_base::get_vfs());
		LPCSTR szRealFileName = lpszFileName;
		// 실제 파일이 없을 경우 임시 파일을 연다.
		if(_access(lpszFileName, 0)) {
			tempFile.open(lpszFileName, std::ios_base::binary);
			if(!tempFile.is_open()) return false;
			szRealFileName = tempFile.get_temppath();
		}
		return LoadJPG(szRealFileName, m_iWidth, m_iHeight, m_iBpp, &m_pImage);
	}

	bool IsInit() const					{ return (m_pImage != NULL); }

	unsigned char* GetImage() const		{ return m_pImage; }
	int GetWidth() const				{ return m_iWidth; }
	int GetHeight() const				{ return m_iHeight; }
	int GetBpp() const					{ return m_iBpp; }
	
	void operator = (const CJpeg& jpg)
	{
		m_iWidth = jpg.m_iWidth;
		m_iHeight = jpg.m_iHeight;
		m_iBpp = jpg.m_iBpp;
		m_pImage = new unsigned char [m_iWidth*m_iHeight*m_iBpp];
		memcpy(m_pImage, jpg.m_pImage, m_iWidth*m_iHeight*m_iBpp); 
	}

private:
	int m_iWidth, m_iHeight, m_iBpp;
	unsigned char* m_pImage;
	

};

#endif