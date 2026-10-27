//----------------------------------------------------------------------
// CAlphaSprite555.h
//----------------------------------------------------------------------
// 5:5:5 video card에 대한 sprite class
//----------------------------------------------------------------------

#ifndef	__CALPHASPRITE555_H__
#define	__CALPHASPRITE555_H__

#include <Windows.h>
std::ofstream;
std::ifstream;


#include "CAlphaSprite.h"


class CAlphaSprite555 : public CAlphaSprite {
	public :
	
		//---------------------------------------------------------
		// fstream에서 save/load를 한다.
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(ivfstream& file);
};


#endif



