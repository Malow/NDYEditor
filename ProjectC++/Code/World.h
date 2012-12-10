#pragma once

#include "Sector.h"
#include "Observer.h"
#include "WorldFile.h"
#include "Entity.h"
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
	unsigned int x,y;
	SectorLoadedEvent( World* world, unsigned int x, unsigned int y ) : world(world), x(x), y(y) {}
};

class SectorHeightMapChanged : public Event
{
public:
	World* world;
	unsigned int sectorx, sectory, localx, localy;
	SectorHeightMapChanged( World* world, unsigned int sectorx, unsigned int sectory, unsigned int localx, unsigned int localy ) :
		world(world),
		sectorx(sectorx),
		sectory(sectory),
		localx(localx),
		localy(localy)
	{

	}
};


class SectorUnloadedEvent : public Event
{
public:
	World* world;
	Vector2 sector;
	SectorUnloadedEvent( World* world, const Vector2& pos ) : world(world), sector(pos) {}
};


class EntityLoadedEvent : public Event
{
public:
	World* world;
	Entity* entity;
	std::string fileName;
	EntityLoadedEvent( World* world, Entity* entity, std::string fileName) : world(world), entity(entity), fileName(fileName){}
};


class World : public Observed, public Observer
{
private:
	WorldFile* zFile;

	Sector*** zSectors;

	std::vector <Entity*> zEntities;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
public:
	World( Observer* observer, const std::string& fileName="" ) throw(const char*);
	World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();

	// Save World To File
	void SaveFile();
	void SaveFileAs( const std::string& fileName );

	// Load All Sectors
	void LoadAllSectors();

	// Modify functions
	void ModifyHeightAt( unsigned int x, unsigned int y, float value );
	float GetHeightAt( unsigned int x, unsigned int y );
	void SetHeightAt( unsigned int x, unsigned int y, float value );

	bool CreateEntity(Vector3 pos, ENTITYTYPE entityType, std::string filePath);

	// Inline Functions
	inline unsigned int GetNumSectorsWidth() const { return zNrOfSectorsWidth; }
	inline unsigned int GetNumSectorsHeight() const { return zNrOfSectorsHeight; }

	// Sector Functions
	Sector* GetSector( unsigned int x, unsigned int y ) throw(const char*);
	Sector* GetSectorAtWorldPos( const Vector2& pos ) throw(const char*);

protected:
	virtual void onEvent( Event* e );
};