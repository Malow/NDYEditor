#pragma once

#include <string>
#include <fstream>
#include "Sector.h"
#include "Observer.h"

/*
File Order:
WorldHeader
HeightMaps
*/

enum WORLDFILE_OPENMODE
{
	OPEN_LOAD = 1,
	OPEN_SAVE = 2,
	OPEN_EDIT = 3
};

class WorldFile;

struct WorldHeader
{
	unsigned int width;
	unsigned int height;

	WorldHeader() : width(10), height(10) {}
};

class WorldHeaderLoadedEvent : public Event
{
public:
	WorldFile* file;
	WorldHeader header;
	WorldHeaderLoadedEvent( WorldFile* file ) : file(file) {}
};

class WorldHeaderCreateEvent : public Event
{
public:
	WorldFile* file;
	WorldHeader header;

	WorldHeaderCreateEvent( WorldFile* file ) : file(file)
	{
	}
};

class WorldFile : public Observed
{
	std::string zFileName;
	std::fstream *zFile;
	WORLDFILE_OPENMODE zMode;
	unsigned int zNumSectors;

public:
	WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode );
	virtual ~WorldFile();


	/*
	Open The File, Reads the header
	*/
	void Open();

	/*
	Writes Heightmap to file
	*/
	void WriteHeightMap( const float* const data, unsigned int mapIndex );

	/*
	Reads Heightmap, Returns false if file does not contain a heightmap for this map
	*/
	bool ReadHeightMap( float* data, unsigned int mapIndex );

	/*
	Read the world header
	*/
	void ReadHeader();

	inline const std::string& GetFileName() const { return zFileName; }

private:
	unsigned int GetHeightsBegin() const;
	unsigned int GetSectorsBegin() const;
};