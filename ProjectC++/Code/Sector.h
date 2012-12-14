#pragma once
// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska Högskola)

#include "Vector.h"
#include <vector>

static const int SECTOR_LENGTH = 32;

class Sector
{
private:
	float zHeightMap[(SECTOR_LENGTH+1)*(SECTOR_LENGTH+1)];
	float zBlendMap[SECTOR_LENGTH*SECTOR_LENGTH*4];
	bool zEditedFlag;

public:
	/*Default functions.*/
	Sector();
	virtual ~Sector();

	/*
	Sets the edited flag
	*/
	void Reset();

	// Returns the blendmap as a float array where each pixel is 4 floats.
	inline float* GetBlendMap() { return &zBlendMap[0]; }

	// Returns the heightmap as a float array.
	inline float* GetHeightMap() { return &zHeightMap[0]; }

	/*
	Modify point by value, set the edited flag
	Sets the edited flag
	Throws when out of bounds
	*/
	void ModifyHeightAt( unsigned int x, unsigned int y, float value ) throw(const char*);

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