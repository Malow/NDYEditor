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


	for( unsigned int x=0; x<SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE; ++x )
	{
		zBlendMap[x*4] = 1.0f;
		zBlendMap[x*4+1] = 0.0f;
		zBlendMap[x*4+2] = 0.0f;
		zBlendMap[x*4+3] = 0.0f;
	}

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


void Sector::SetBlendingAt( unsigned int x, unsigned int y, const Vector4& val )
{
	if ( x >= SECTOR_LENGTH+1 || y >= SECTOR_LENGTH+1 )
		throw("Out Of Bounds!");

	for( unsigned int x=0; x<4; ++x )
	{
		zBlendMap[ (y * (SECTOR_BLEND_SIZE) + x) * 4 + 0 ] = val[x];
	}
}


Vector4 Sector::GetBlendingAt( unsigned int x, unsigned int y ) const
{
	if ( x >= SECTOR_LENGTH+1 || y >= SECTOR_LENGTH+1 )
		throw("Out Of Bounds!");

	Vector4 vec;

	for( unsigned int x=0; x<4; ++x )
	{
		vec[x] = zBlendMap[ (y * (SECTOR_BLEND_SIZE) + x) * 4 + x];
	}

	return vec;
}
