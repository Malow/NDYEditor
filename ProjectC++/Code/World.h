#pragma once

#include "Sector.h"
#include "Observer.h"
#include "WorldFile.h"
#include <string>

class World;

class WorldLoadedEvent : public Event
{
public:
	World* world;
	WorldLoadedEvent( World* world ) : world(world) {}
};

class SectorLoadedEvent : public Event
{
public:
	World* world;
	Vector2 sector;
	SectorLoadedEvent( World* world, const Vector2& pos ) : world(world), sector(pos) {}
};

class World : public Observed
{
private:
	WorldFile* zFile;

	Sector*** zSectors;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
public:
	World( Observer* observer, const std::string& fileName="" ) throw(const char*);
	World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();

	// Save World To File
	void SaveFile();

	//Modify functions
	bool ModifyPoint(Vector2 pos, float value);

private:
	// Get Sector X Y
	Sector* GetSector( const Vector2& pos ) throw(const char*);
	Sector* GetSectorAtWorldPos( const Vector2& pos ) throw(const char*);
};