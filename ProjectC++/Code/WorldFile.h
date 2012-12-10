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
	OPEN_LOAD = 0,
	OPEN_SAVE = 1,
	OPEN_EDIT = 2
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
	std::fstream zFile;
	WORLDFILE_OPENMODE zMode;

public:
	WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode );
	virtual ~WorldFile();

	void WriteHeightMap( const float* const data, unsigned int mapIndex );
	void ReadHeightMap( float* data, unsigned int mapIndex );

	inline const std::string& GetFileName() const { return zFileName; }

private:
	unsigned int GetSectorsBegin() const;
};