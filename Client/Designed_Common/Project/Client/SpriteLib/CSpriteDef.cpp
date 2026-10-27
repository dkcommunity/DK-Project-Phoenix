//----------------------------------------------------------------------
// CSpriteDef.cpp
//----------------------------------------------------------------------

#include <fstream>
#include "CSpriteDef.h"

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void	
SPRITE_FILEPOSITION_NODE::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&SpriteID, SIZE_SPRITEID);
	file.write((const char*)&FilePosition, 4);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void	
SPRITE_FILEPOSITION_NODE::LoadFromFile(ivfstream& file)
{
	file.read((char*)&SpriteID, SIZE_SPRITEID);
	file.read((char*)&FilePosition, 4);
}
