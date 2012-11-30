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
	bool zEditedFlag;

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

	// Modify point by value, set the edited flag
	bool ModifyPoint( unsigned int x, unsigned int y, float value );

	inline int GetSectorLength() { return SECTOR_LENGTH; }
	inline void setEdited( bool state ) { zEditedFlag = state; }
	inline bool isEdited() const { return zEditedFlag; }
};