#pragma once

#include "Sector.h"

class World
{
private:
	Sector** zSectors;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
public:
	World();
	World(unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();
	//Good functions
	/* Create a world with nrOfSectorWidth x nrOfSectorHeight sectors. 
	And they are each SECTOR_LENGTH x SECTOR_LENGTH big*/
	bool CreateWorld(unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);

	//Modify functions
	bool ModifyPoint(Vector3 pos, float value);
	bool ModifyPoint(Vector2 pos, float value);
	bool ModifyPoint(Vector3 pos, Vector3 value);
	bool ModifyPoint(Vector2 pos, Vector3 value);
};