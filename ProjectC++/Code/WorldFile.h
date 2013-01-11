#pragma once

#include <string>
#include <fstream>
#include "Sector.h"
#include "Observer.h"
#include "WorldEvents.h"
#include <array>

/*
File Order:
WorldHeader
SectorHeaders
TextureNames
HeightMaps
BlendMaps
Entities
*/

enum WORLDFILE_OPENMODE
{
	OPEN_LOAD = 1,
	OPEN_SAVE = 2,
	OPEN_EDIT = 3
};

class WorldFileHeader
{
public:
	unsigned int width;
	unsigned int height;

	WorldFileHeader() : width(10), height(10) {}
};

struct EntityStruct
{
	float pos[3];
	float rot[3];
	float scale[3];
	unsigned int type;
};

class WorldFile : public Observed
{
	WorldFileHeader zHeader;
	std::string zFileName;
	std::fstream *zFile;
	WORLDFILE_OPENMODE zMode;
	unsigned int zNumSectors;

public:
	WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode );
	virtual ~WorldFile();

	// Save Entities
	void WriteEntities( const std::array<EntityStruct,256>& entities, unsigned int sectorIndex );
	bool ReadEntities( unsigned int sectorIndex, std::array<EntityStruct,256>& out );

	// Open The World File, Reads the world header
	void Open();

	// Writes height map to file
	void WriteHeightMap( const float* const data, unsigned int mapIndex );
	bool ReadHeightMap( float* data, unsigned int mapIndex );

	// Sector header
	void WriteSectorHeader( unsigned int mapIndex );
	bool ReadSectorHeader( unsigned int sectorIndex );
	
	// Texture names
	void WriteTextureNames( const char* data, unsigned int x );
	bool ReadTextureNames( char* data, unsigned int sectorIndex );

	// Writes blend map to file
	void WriteBlendMap( const float* const data, unsigned int mapIndex );
	bool ReadBlendMap( float* data, unsigned int mapIndex );

	// Read the world header
	void ReadHeader();

	// Returns World Header
	const WorldFileHeader& GetHeader() const { return zHeader; }

	inline const std::string& GetFileName() const { return zFileName; }

private:
	unsigned int GetBlendNamesBegin() const;
	unsigned int GetBlendsBegin() const;
	unsigned int GetHeightsBegin() const;
	unsigned int GetSectorHeadersBegin() const;
	unsigned int GetSectorTexturesBegin() const;
	unsigned int GetEntitiesBegin() const;
	unsigned int GetEnding() const;
};