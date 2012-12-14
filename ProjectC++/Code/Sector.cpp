#include "Sector.h"


Sector::Sector() : zEditedFlag(false)
{
}


Sector::~Sector()
{
}


void Sector::Reset()
{
	for( unsigned int x=0; x<(SECTOR_LENGTH+1)*(SECTOR_LENGTH+1); ++x )
	{
		zHeightMap[x] = 0.0f;
	}


	for( unsigned int x=0; x<SECTOR_LENGTH*SECTOR_LENGTH; ++x )
	{
		zBlendMap[x*4] = 1.0f;
		zBlendMap[x*4+1] = 0.0f;
		zBlendMap[x*4+2] = 0.0f;
		zBlendMap[x*4+3] = 0.0f;
	}

	SetEdited(true);
}


void Sector::ModifyHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*)
{
	if ( x >= SECTOR_LENGTH+1 || y >= SECTOR_LENGTH+1 )
		throw("Out Of Bounds!");

	this->zHeightMap[y*(SECTOR_LENGTH+1)+x] += value;

	SetEdited(true);
}


float Sector::GetHeightAt( unsigned int x, unsigned int y ) const throw(const char*)
{
	if ( x >= SECTOR_LENGTH+1 || y >= SECTOR_LENGTH+1 )
		throw("Out Of Bounds!");

	return this->zHeightMap[y*(SECTOR_LENGTH+1)+x];
}


void Sector::SetHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*)
{
	if ( x >= SECTOR_LENGTH+1 || y >= SECTOR_LENGTH+1 )
		throw("Out Of Bounds!");

	this->zHeightMap[y*(SECTOR_LENGTH+1)+x] = value;

	SetEdited(true);
}
