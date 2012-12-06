#include "Sector.h"


Sector::Sector() : zEditedFlag(false)
{
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

	SetEdited(true);
}


void Sector::ModifyHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*)
{
	if ( x < 0 || x >= SECTOR_LENGTH || y < 0 || y >= SECTOR_LENGTH)
		throw("Out Of Bounds!");

	this->zHeightMap[y*SECTOR_LENGTH+x] += value;

	SetEdited(true);
}


float Sector::GetHeightAt( unsigned int x, unsigned int y ) const throw(const char*)
{
	if ( x < 0 || x >= SECTOR_LENGTH || y < 0 || y >= SECTOR_LENGTH)
		throw("Out Of Bounds!");

	return this->zHeightMap[y*SECTOR_LENGTH+x];
}


void Sector::SetHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*)
{
	if ( x < 0 || x >= SECTOR_LENGTH || y < 0 || y >= SECTOR_LENGTH)
		throw("Out Of Bounds!");

	this->zHeightMap[y*SECTOR_LENGTH+x] = value;

	SetEdited(true);
}
