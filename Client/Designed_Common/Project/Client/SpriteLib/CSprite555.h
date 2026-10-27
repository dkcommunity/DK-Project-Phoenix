//----------------------------------------------------------------------
// CSprite555.h
//----------------------------------------------------------------------
// 5:5:5 video card에 대한 sprite class
//----------------------------------------------------------------------

#ifndef	__CSPRITE555_H__
#define	__CSPRITE555_H__

#include <Windows.h>

#include "CSprite.h"


class CSprite555 : public CSprite {
	public :
		//---------------------------------------------------------
		// fstream에서 save/load를 한다.
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(ivfstream& file);
		//bool		LoadFromFileToBuffer(std::ifstream& file);

};


#endif



