#pragma once
// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska Högskola)

#include "Vector.h"
using namespace MaloW;

const int SECTOR_LENGTH = 32;

class Sector
{
private:
	float**			zHeightMap;
	Vector3**		zBlendMap;

	// Save height and width just because it could be needed.
public:
	/*Default functions.*/

	Sector();
	//Sector(int width, int height); // Not needed sense we have SECTOR_LENGTH
	virtual ~Sector();

	/*Useful functions.*/
	// Returns the blendmap as a double pointer (Vector3**).
	inline Vector3** GetBlendMap(){ return this->zBlendMap; }
	// Returns the heightmap as a double pointer (Int**).
	inline float** GetHeightMap(){ return this->zHeightMap; }

	/*Modify points with 3d vectors*/
	//Modify blendMap with 3d vector
	bool ModifyPoint(Vector3 point, Vector3 value);
	//Modify heightMap with 3d vector
	bool ModifyPoint(Vector3 point, float value);
	/* 2d vector if one like to use that*/
	//Modify blendMap with 2d vector
	bool ModifyPoint( Vector2 point, Vector3 value );
	//Modify heihtMap with 2d vector
	bool ModifyPoint( Vector2 point, float value );

	int GetSectorLength(){ return SECTOR_LENGTH; }
};