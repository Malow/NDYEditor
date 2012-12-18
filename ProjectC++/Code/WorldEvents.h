#pragma once

#include "Observer.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 )

class World;
class Entity;
class WorldFile;
class WorldFileHeader;

class WorldHeaderLoadedEvent : public Event
{
public:
	WorldFile* file;
	const WorldFileHeader& header;

	WorldHeaderLoadedEvent( WorldFile* file, const WorldFileHeader& header ) :
		file(file),
		header(header)
	{
	}
};

class WorldHeaderCreateEvent : public Event
{
public:
	WorldFile* file;
	WorldFileHeader& header;

	WorldHeaderCreateEvent( WorldFile* file, WorldFileHeader& header ) : 
		file(file),
		header(header)
	{
	}
};

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

class SectorBlendMapChanged : public Event
{
public:
	World* world;
	unsigned int sectorx, sectory, localx, localy;
	SectorBlendMapChanged( World* world, unsigned int sectorx, unsigned int sectory, unsigned int localx, unsigned int localy ) :
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
	EntityLoadedEvent( World* world, Entity* entity, const std::string fileName) : world(world), entity(entity), fileName(fileName){}
};

class EntityRemovedEvent : public Event
{
public:
	World* world;
	Entity* entity;
	EntityRemovedEvent( World* world, Entity* entity ) : world(world), entity(entity) {}
};

#pragma warning (pop)