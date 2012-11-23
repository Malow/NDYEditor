#include "World.h"

World::World()
{
	this->zSectors = 0;
	this->zNrOfSectorsWidth = 0;
	this->zNrOfSectorsHeight = 0;
}

World::World( unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight )
{
	this->zNrOfSectorsWidth = nrOfSectorWidth;
	this->zNrOfSectorsHeight = nrOfSectorHeight;
	this->zSectors = new Sector*[this->zNrOfSectorsWidth];
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		this->zSectors[i] = new Sector[this->zNrOfSectorsHeight];
	}
}

World::~World()
{
	//Delete the zSectors pointers.
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		if(this->zSectors[i])
		{
			delete this->zSectors[i];
			this->zSectors[i] = 0;
		}
	}
	//Delete the zSector pointer.
	delete [] this->zSectors;
	this->zSectors = 0;
}
//Edits the heightMap with 3D vector
bool World::ModifyPoint( Vector3 pos, float value )
{
	if((this->zNrOfSectorsWidth * SECTOR_LENGTH) <= pos.x || (this->zNrOfSectorsHeight * SECTOR_LENGTH) <= pos.y)
	{
		return false;
	}
	Vector3 localPos = Vector3(fmod(pos.x, (float)SECTOR_LENGTH), fmod(pos.y, (float)SECTOR_LENGTH), pos.z);
	return this->zSectors[(int)(pos.x / SECTOR_LENGTH)][(int)(pos.y / SECTOR_LENGTH)].ModifyPoint(localPos, value);
}
//Edits the heightMap with 2D vector
bool World::ModifyPoint( Vector2 pos, float value )
{
	if((this->zNrOfSectorsWidth * SECTOR_LENGTH) <= pos.x || (this->zNrOfSectorsHeight * SECTOR_LENGTH) <= pos.y)
	{
		return false;
	}
	Vector2 localPos = Vector2(fmod(pos.x, (float)SECTOR_LENGTH), fmod(pos.y, (float)SECTOR_LENGTH));
	return this->zSectors[(int)pos.x / SECTOR_LENGTH][(int)pos.y / SECTOR_LENGTH].ModifyPoint(localPos, value);
}

//Edits the blendMap with 2D vector
bool World::ModifyPoint( Vector2 pos, Vector3 value )
{
	if((this->zNrOfSectorsWidth * SECTOR_LENGTH) <= pos.x || (this->zNrOfSectorsHeight * SECTOR_LENGTH) <= pos.y)
	{
		return false;
	}
	Vector2 localPos = Vector2(fmod(pos.x, (float)SECTOR_LENGTH), fmod(pos.y, (float)SECTOR_LENGTH));
	return this->zSectors[(int)(pos.x / SECTOR_LENGTH)][(int)(pos.y / SECTOR_LENGTH)].ModifyPoint(localPos, value);
}
//Edits the blendMap with 3D vector
bool World::ModifyPoint( Vector3 pos, Vector3 value )
{
	if((this->zNrOfSectorsWidth * SECTOR_LENGTH) <= pos.x || (this->zNrOfSectorsHeight * SECTOR_LENGTH) <= pos.y)
	{
		return false;
	}
	Vector3 localPos = Vector3(fmod(pos.x, (float)SECTOR_LENGTH), fmod(pos.y, (float)SECTOR_LENGTH), pos.z);
	return this->zSectors[(int)(pos.x / SECTOR_LENGTH)][(int)(pos.y / SECTOR_LENGTH)].ModifyPoint(localPos, value);
}
// Create a new world
bool World::CreateWorld( unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight )
{
	if(this->zNrOfSectorsWidth == 0 && this->zNrOfSectorsHeight == 0)
	{
		//Delete the zSectors pointers.
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			if(this->zSectors[i])
			{
				delete this->zSectors[i];
				this->zSectors[i] = 0;
			}
		}
		//Delete the zSector pointer.
		delete [] this->zSectors;
		this->zSectors = 0;
	}
	this->zNrOfSectorsWidth = nrOfSectorWidth;
	this->zNrOfSectorsHeight = nrOfSectorHeight;
	this->zSectors = new Sector*[this->zNrOfSectorsWidth];
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		this->zSectors[i] = new Sector[this->zNrOfSectorsHeight];
	}

	return true; //Maybe add something that return false later.
}
