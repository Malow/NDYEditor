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

	SetTextureName(0, "TerrainTexture.png");
	SetTextureName(1, "Green.png");
	SetTextureName(2, "Blue.png");
	SetTextureName(3, "Red.png");

	zAmbient[0] = 0.0f;
	zAmbient[1] = 0.0f;
	zAmbient[2] = 0.0f;

	SetEdited(false);
}


float Sector::GetHeightAt( float x, float y ) const throw(...)
{
	if ( x < 0.0f || x >= 1.0f || y < 0.0f || y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(x * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;
	float snapY = floor(y * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * SECTOR_HEIGHT_SIZE);
	unsigned int scaledY = (unsigned int)(snapY * SECTOR_HEIGHT_SIZE);

	// Set Values
	return zHeightMap[ scaledY * SECTOR_HEIGHT_SIZE + scaledX ];
}


void Sector::SetHeightAt( float x, float y, float val ) throw(...)
{
	if ( x < 0.0f || x >= 1.0f || y < 0.0f || y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(x * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;
	float snapY = floor(y * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * SECTOR_HEIGHT_SIZE);
	unsigned int scaledY = (unsigned int)(snapY * SECTOR_HEIGHT_SIZE);

	// Set Values
	zHeightMap[ scaledY * SECTOR_HEIGHT_SIZE + scaledX ] = val;

	SetEdited(true);
}


void Sector::SetBlendingAt( float x, float y, const Vector4& val )
{
	if ( x < 0.0f || x > (float)SECTOR_BLEND_SIZE || y < 0.0f || y > (float)SECTOR_BLEND_SIZE )
		throw("Out Of Bounds!");

	float density = (float)SECTOR_WORLD_SIZE/(float)SECTOR_BLEND_SIZE;
	float snapX = floor( x / density ) * density;
	float snapY = floor( y / density ) * density;

	unsigned int scaledX = (unsigned int)((snapX / (float)SECTOR_WORLD_SIZE)*(SECTOR_BLEND_SIZE));
	unsigned int scaledY = (unsigned int)((snapY / (float)SECTOR_WORLD_SIZE)*(SECTOR_BLEND_SIZE));

	// Clamp Val
	Vector4 normalizedVal = val;
	normalizedVal.Normalize();

	// Set Values
	for( unsigned int i=0; i<4; ++i )
	{
		zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ] = normalizedVal[i];
	}

	SetEdited(true);
}


Vector4 Sector::GetBlendingAt( float x, float y ) const
{
	if ( x < 0.0f || x >= SECTOR_BLEND_SIZE || y < 0.0f || y >= SECTOR_BLEND_SIZE )
		throw("Out Of Bounds!");

	float density = (float)SECTOR_WORLD_SIZE/(float)SECTOR_BLEND_SIZE;
	float snapX = floor( x / density ) * density;
	float snapY = floor( y / density ) * density;

	unsigned int scaledX = (unsigned int)((snapX / (float)SECTOR_WORLD_SIZE)*(SECTOR_BLEND_SIZE));
	unsigned int scaledY = (unsigned int)((snapY / (float)SECTOR_WORLD_SIZE)*(SECTOR_BLEND_SIZE));

	Vector4 vec;

	for( unsigned int i=0; i<4; ++i )
	{
		vec[i] = zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ];
	}

	return vec;
}


const char* const Sector::GetTextureName( unsigned int index ) const
{
	if ( index > 3 ) throw("Index Out Of Range");

	return &zTextureNames[index*TEXTURE_NAME_LENGTH];
}


void Sector::SetTextureName( unsigned int index, const std::string& name )
{
	if ( index > 3 ) throw("Index Out Of Range!");
	if ( name.length() >= TEXTURE_NAME_LENGTH ) throw("Texture Name Too Long!");

	memset( &zTextureNames[index*TEXTURE_NAME_LENGTH], 0, TEXTURE_NAME_LENGTH );
	memcpy( &zTextureNames[index*TEXTURE_NAME_LENGTH], &name[0], name.length() );

	SetEdited(true);
}


void Sector::SetAmbient( const Vector3& ambient )
{
	for( unsigned int i=0; i != 3; ++i )
	{
		if ( zAmbient[i] != ambient[i] )
		{
			zAmbient[i] = ambient[i];
			SetEdited(true);
		}
	}
}


void Sector::SetBlocking( const Vector2& pos, bool flag )
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_AI_GRID_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_AI_GRID_SIZE));

	// Set Values
	zAiGrid[ scaledY * SECTOR_AI_GRID_SIZE + scaledX ] = flag;

	// Changed
	SetEdited(true);
}


bool Sector::GetBlocking( const Vector2& pos ) const
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_AI_GRID_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_AI_GRID_SIZE));

	// Set Values
	return zAiGrid[ scaledY * SECTOR_AI_GRID_SIZE + scaledX ];
}


Vector3 Sector::GetNormalAt( float x, float y ) throw(...)
{
	float minX = floor(x * SECTOR_HEIGHT_SIZE);
	float minY = floor(y * SECTOR_HEIGHT_SIZE);

	float maxX = minX + 1.0f;
	float maxY = minY + 1.0f;

	minX = minX / SECTOR_HEIGHT_SIZE;
	minY = minY / SECTOR_HEIGHT_SIZE;
	maxX = maxX / SECTOR_HEIGHT_SIZE;
	maxY = maxY / SECTOR_HEIGHT_SIZE;
	
	Vector3 a(minX, GetHeightAt(minX, minY), minY);
	Vector3 b(minX, GetHeightAt(minY, maxY), maxY);
	Vector3 c(maxX, GetHeightAt(maxY, minY), minY);
	Vector3 d(maxX, GetHeightAt(maxY, maxY), maxY);

	//a.Normalize();
	//b.Normalize();
	//c.Normalize();
	//d.Normalize();

	Vector3 normal = (d-a).GetCrossProduct(c-b);
	normal.Normalize();
	return normal;
}
