#include "Sector.h"


Sector::Sector() : zEditedFlag(false)
{
}


Sector::~Sector()
{
}


void Sector::Reset()
{
	for( unsigned int x=0; x<SECTOR_HEIGHT_SIZE*SECTOR_HEIGHT_SIZE; ++x )
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

	for( unsigned int x=0; x<4; ++x )
	{
		memset(zTextureNames[x], 0, TEXTURE_NAME_LENGTH);
	}

	SetEdited(true);
}


float Sector::GetHeightAt( unsigned int x, unsigned int y ) const throw(const char*)
{
	if ( x >= SECTOR_HEIGHT_SIZE || y >= SECTOR_HEIGHT_SIZE )
		throw("Out Of Bounds!");

	return this->zHeightMap[y*(SECTOR_HEIGHT_SIZE)+x];
}


void Sector::SetHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*)
{
	if ( x >= SECTOR_HEIGHT_SIZE || y >= SECTOR_HEIGHT_SIZE )
		throw("Out Of Bounds!");

	this->zHeightMap[y*(SECTOR_HEIGHT_SIZE)+x] = value;

	SetEdited(true);
}


void Sector::SetBlendingAt( unsigned int x, unsigned int y, const Vector4& val )
{
	if ( x >= SECTOR_BLEND_SIZE || y >= SECTOR_BLEND_SIZE )
		throw("Out Of Bounds!");

	for( unsigned int x=0; x<4; ++x )
	{
		zBlendMap[ (y * (SECTOR_BLEND_SIZE) + x) * 4 + 0 ] = val[x];
	}

	SetEdited(true);
}


Vector4 Sector::GetBlendingAt( unsigned int x, unsigned int y ) const
{
	if ( x >= SECTOR_BLEND_SIZE || y >= SECTOR_BLEND_SIZE )
		throw("Out Of Bounds!");

	Vector4 vec;

	for( unsigned int x=0; x<4; ++x )
	{
		vec[x] = zBlendMap[ (y * (SECTOR_BLEND_SIZE) + x) * 4 + x];
	}

	return vec;
}

const std::string& Sector::GetTextureName( unsigned int index ) const
{
	if ( index > 3 ) throw("Index Out Of Range");

	return std::string(&zTextureNames[index][0]);
}

void Sector::SetTextureName( unsigned int index, const std::string& name )
{
	if ( index > 3 ) throw("Index Out Of Range!");
	if ( name.length() >= TEXTURE_NAME_LENGTH ) throw("Texture Name Too Long!");

	memset( &zTextureNames[index][0], 0, TEXTURE_NAME_LENGTH );
	memcpy( &zTextureNames[index][0], &name[0], name.length() );
}
