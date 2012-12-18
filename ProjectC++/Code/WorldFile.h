#pragma once

#include <string>
#include <fstream>
#include "Sector.h"
#include "Observer.h"
#include "WorldEvents.h"


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

class WorldFileHeader
{
public:
	unsigned int width;
	unsigned int height;

	WorldFileHeader() : width(10), height(10) {}
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


	/*
	Open The File, Reads the header
	*/
	void Open();

	/*
	Writes blend map to file
	*/
	void WriteHeightMap( const float* const data, unsigned int mapIndex );

	/*
	Reads Height map, Returns false if file does not contain a height map for this map
	*/
	bool ReadHeightMap( float* data, unsigned int mapIndex );


	/*
	Writes blend map to file
	*/
	void WriteBlendMap( const float* const data, unsigned int mapIndex );

	/*
	Reads blend map
	*/
	bool ReadBlendMap( float* data, unsigned int mapIndex );

	/*
	Read the world header
	*/
	void ReadHeader();

	/*
	Returns World Header
	*/
	const WorldFileHeader& GetHeader() const { return zHeader; }

	inline const std::string& GetFileName() const { return zFileName; }

private:
	unsigned int GetBlendsBegin() const;
	unsigned int GetHeightsBegin() const;
	unsigned int GetSectorsBegin() const;
	
};