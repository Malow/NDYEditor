#pragma once

// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska Högskola)

#include "Vector.h"
#include <vector>
#include <bitset>

static const unsigned int SECTOR_WORLD_SIZE = 32;
static const unsigned int SECTOR_LENGTH = 64;
static const unsigned int SECTOR_BLEND_SIZE = 64;
static const unsigned int SECTOR_HEIGHT_SIZE = 64;
static const unsigned int TEXTURE_NAME_LENGTH = 60;

// AI Grid
static const unsigned int SECTOR_AI_GRID_SIZE = 63;
typedef std::bitset< SECTOR_AI_GRID_SIZE * SECTOR_AI_GRID_SIZE > AIGrid;



class Sector
{
private:
	float zHeightMap[SECTOR_HEIGHT_SIZE*SECTOR_HEIGHT_SIZE];
	float zBlendMap[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
	char zTextureNames[TEXTURE_NAME_LENGTH*4];
	float zAmbient[3];
	AIGrid zAiGrid;

	bool zEditedFlag;

public:
	/*Default functions.*/
	Sector();
	virtual ~Sector();

	// Sets the edited flag
	void Reset();

	// Returns the blend map as a float array where each pixel is 4 floats.
	inline float* GetBlendMap() { return &zBlendMap[0]; }

	// Returns the height map as a float array.
	inline float* GetHeightMap() { return &zHeightMap[0]; }

	// Sets texture blending at point
	void SetBlendingAt( float x, float y, const Vector4& val );

	// Returns the texture blending at point
	Vector4 GetBlendingAt( float x, float y ) const;

	// Ambient
	inline Vector3 GetAmbient() const { return Vector3(zAmbient[0], zAmbient[1], zAmbient[2]); }
	void SetAmbient( const Vector3& ambient );

	// AI Grid
	AIGrid& GetAIGrid() { return zAiGrid; }
	void SetBlocking( const Vector2& pos, bool flag );
	bool GetBlocking( const Vector2& pos ) const;

	/*
	Get the value at point
	Throws when out of bounds 
	*/
	float GetHeightAt( float x, float y ) const throw(...);

	/*
	Sets the height at a point local to the sector
	Sets the edited flag
	Throws when out of bounds
	*/
	void SetHeightAt( float x, float y, float val ) throw(...);

	// Returns The Texture Name (0<=index<4)
	const char* const GetTextureName( unsigned int index ) const;
	void SetTextureName( unsigned int index, const std::string& name );
	inline char* GetTextureNames() { return &zTextureNames[0]; }

	// Inline Functions
	inline int GetSectorLength() { return SECTOR_LENGTH; }
	inline void SetEdited( bool state ) { zEditedFlag = state; }
	inline bool IsEdited() const { return zEditedFlag; }	
};