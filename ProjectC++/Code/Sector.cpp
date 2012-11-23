#include "Sector.h"

Sector::Sector()
{
	// Init zHeightMap with and an 2D array that is zWidth x zHeight.
	this->zHeightMap = new float*[SECTOR_LENGTH]();
	for(unsigned int i = 0; i < SECTOR_LENGTH; i++)
	{
		this->zHeightMap[i] = new float[SECTOR_LENGTH];
	}
	// Sets default values
	for(unsigned int x = 0; x < SECTOR_LENGTH; x++)
	{
		for(unsigned int y = 0; y < SECTOR_LENGTH; y++)
		{
			this->zHeightMap[x][y] = int(0);
		}
	}
	// Init zBlendMap with and an 2D array that is zWidth x zHeight.
	this->zBlendMap = new Vector3*[SECTOR_LENGTH]();
	for(unsigned int i = 0; i < SECTOR_LENGTH; i++)
	{
		this->zBlendMap[i] = new Vector3[SECTOR_LENGTH];
	}
	// Sets default values
	for(unsigned int x = 0; x < SECTOR_LENGTH; x++)
	{
		for(unsigned int y = 0; y < SECTOR_LENGTH; y++)
		{
			this->zBlendMap[x][y] = Vector3(255,0,0);
		}
	}
}
Sector::~Sector()
{
	//Delete the zHeightMap pointers.
	for(unsigned int i = 0; i < SECTOR_LENGTH; i++)
	{
		if(this->zHeightMap)
		{
			delete this->zHeightMap[i];
			this->zHeightMap[i] = 0;
		}
	}
	//Delete the zHeightMap pointer.
	delete [] this->zHeightMap;
	this->zHeightMap = 0;

	//Delete all the zBlendMap pointers.
	for(unsigned int i = 0; i < SECTOR_LENGTH; i++)
	{
		if(this->zHeightMap)
		{
			delete this->zBlendMap[i];
			this->zBlendMap[i] = 0;
		}
	}
	//Delete the zBlendMap pointer.
	delete [] this->zBlendMap;
	this->zBlendMap = 0;
}
//Modify blendMap
bool Sector::ModifyPoint( Vector3 point, Vector3 value )
{
	if (point.x >= SECTOR_LENGTH || point.y >= SECTOR_LENGTH)
	{
		return false;
	}
	this->zBlendMap[(int)point.x][(int)point.y] += value;

	return true;
}

//Modify heihtMap
bool Sector::ModifyPoint( Vector3 point, float value )
{
	if (point.x >= SECTOR_LENGTH || point.y >= SECTOR_LENGTH)
	{
		return false;
	}
	this->zHeightMap[(int)point.x][(int)point.y] += value;

	return true;
}

//Modify blendMap
bool Sector::ModifyPoint( Vector2 point, Vector3 value )
{
	if (point.x >= SECTOR_LENGTH || point.y >= SECTOR_LENGTH)
	{
		return false;
	}
	this->zBlendMap[(int)point.x][(int)point.y] += value;

	return true;
}

//Modify heihtMap
bool Sector::ModifyPoint( Vector2 point, float value )
{
	if (point.x >= SECTOR_LENGTH || point.y >= SECTOR_LENGTH)
	{
		return false;
	}
	this->zHeightMap[(int)point.x][(int)point.y] += value;

	return true;
}