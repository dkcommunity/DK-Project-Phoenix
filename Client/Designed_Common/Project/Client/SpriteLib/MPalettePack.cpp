//#include "MPalettePack.h"
//#include <fstream>
//
//void MPalettePack::Init(WORD size, bool b565)
//{
//	if(size == 0)
//		return;
//	
//	Release();
//	
//	m_b565 = b565;
//	m_Size = size;
//	
//	if(m_b565)
//		m_pData = new MPalette565[size];
//	else
//		m_pData = new MPalette555[size];
//}
//
//bool MPalettePack::LoadFromFile(LPCTSTR lpszFilename, bool b565)
//{
//	std::ifstream file(lpszFilename, std::ios::binary);
//	bool re = LoadFromFile(file, b565);
//	file.close();
//	
//	return re;
//}
//
//bool MPalettePack::LoadFromFile(ivfstream &file, bool b565)
//{
////	Release();
//	
//	file.read((char *)&m_Size, 2);
//	
//	Init(m_Size, b565);
//	
//	for(int i = 0; i < m_Size; i++)
//	{
//		m_pData[i].LoadFromFile(file);
//	}
//	
//	return true;
//}
