#pragma once
// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska H�gskola)

#include "Vector.h"
#include <vector>

static const int SECTOR_LENGTH = 32;
static const int SECTOR_BLEND_SIZE = SECTOR_LENGTH*2;

class Sector
{
private:
	float zHeightMap[(SECTOR_LENGTH+1)*(SECTOR_LENGTH+1)];
	float zBlendMap[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
	bool zEditedFlag;

public:
	/*Default functions.*/
	Sector();
	virtual ~Sector();

	/*
	Sets the edited flag
	*/
	void Reset();

	// Returns the blend map as a float array where each pixel is 4 floats.
	inline float* GetBlendMap() { return &zBlendMap[0]; }

	// Returns the height map as a float array.
	inline float* GetHeightMap() { return &zHeightMap[0]; }

	// Sets texture blending at point
	void SetBlendingAt( unsigned int x, unsigned int y, const Vector4& val );

	// Returns the texture blending at point
	Vector4 GetBlendingAt( unsigned int x, unsigned int y ) const;

	/*
	Get the value at point
	Throws when out of bounds 
	*/
	float GetHeightAt( unsigned int x, unsigned int y ) const throw(const char*);

	/*
	Sets the height at a point local to the sector
	Sets the edited flag
	Throws when out of bounds
	*/
	void SetHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*);

	// Inline Functions
	inline int GetSectorLength() { return SECTOR_LENGTH; }
	inline void SetEdited( bool state ) { zEditedFlag = state; }
	inline bool IsEdited() const { return zEditedFlag; }
};