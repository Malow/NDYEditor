#pragma once

#include "Sector.h"
#include "Observer.h"
#include <string>
#include <fstream>

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
	std::string zFileName;
	std::fstream zFile;

	Sector*** zSectors;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
public:
	World( Observer* observer, const std::string& fileName="" ) throw(const char*);
	World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();

	// Save World To File
	bool SaveFile( const std::string& fileName="" ) throw(const char*);

	//Modify functions
	bool ModifyPoint(Vector3 pos, float value);
	bool ModifyPoint(Vector2 pos, float value);
	bool ModifyPoint(Vector3 pos, Vector3 value);
	bool ModifyPoint(Vector2 pos, Vector3 value);

private:
	// Get Sector X Y
	Sector* GetSector( const Vector2& pos ) throw(const char*);
	Sector* GetSectorAtWorldPos( const Vector2& pos ) throw(const char*);
};