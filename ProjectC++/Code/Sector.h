#pragma once
// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska Högskola)

#include "Vector.h"
#include <vector>

static const int SECTOR_LENGTH = 32;

class Sector
{
private:
	float zHeightMap[SECTOR_LENGTH*SECTOR_LENGTH];
	float zBlendMap[SECTOR_LENGTH*SECTOR_LENGTH*3];

	// Save height and width just because it could be needed.
public:
	/*Default functions.*/
	Sector();
	virtual ~Sector();

	// Nullifies all the data in this sector
	void Reset();

	// Returns the blendmap as a double pointer (Vector3**).
	inline float* GetBlendMap() { return &zBlendMap[0]; }

	// Returns the heightmap as a double pointer (Int**).
	inline float* GetHeightMap() { return &zHeightMap[0]; }

	/*Modify points with 3d vectors*/
	// Modify blendMap with float
	bool ModifyPoint( unsigned int x, unsigned int y, float value );

	int GetSectorLength() { return SECTOR_LENGTH; }
};