#include "Sector.h"


Sector::Sector() : zEditedFlag(false)
{
	Reset();
}


Sector::~Sector()
{
}


void Sector::Reset()
{
	for( unsigned int x=0; x<SECTOR_LENGTH*SECTOR_LENGTH; ++x )
	{
		zHeightMap[x] = 0.0f;
		zBlendMap[x*3] = 0.0f;
		zBlendMap[x*3+1] = 0.0f;
		zBlendMap[x*3+2] = 0.0f;
	}
	zEditedFlag = true;
}

bool Sector::ModifyPoint( unsigned int x, unsigned int y, float value )
{
	// Optimization For Release Mode
#ifdef _DEBUG
	if ( x < 0 || x >= SECTOR_LENGTH || y < 0 || y >= SECTOR_LENGTH)
		throw("Out Of Bounds!");
#endif 

	this->zHeightMap[y*SECTOR_LENGTH+x] += value;

	zEditedFlag = true;

	return true;
}