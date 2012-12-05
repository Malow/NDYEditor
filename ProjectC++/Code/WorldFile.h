#pragma once

#include <string>
#include <fstream>
#include "Sector.h"
#include "Observer.h"

/*
File Order:
WorldHeader
SectorHeaders
HeightMaps
BlendMaps
*/

class WorldFile;

class WorldHeaderLoadedEvent : public Event
{
public:
	WorldFile* file;
	WorldHeaderLoadedEvent( WorldFile* File ) : file(file) {}
};

class WorldFile : public Observed
{
	std::string zFileName;
	std::fstream zFile;
	unsigned int zWidth,zHeight;
	bool zReadOnly;

public:
	WorldFile( Observer* observer, const std::string& fileName, bool readOnly=true );
	virtual ~WorldFile();

	void writeHeightMap( const float const* data, unsigned int mapx, unsigned int mapy );
	void readHeightMap( float* data, unsigned int mapx, unsigned int mapy );

	inline unsigned int getWorldWidth() const { return zWidth; }
	inline unsigned int getWorldHeight() const { return zHeight; }
private:
	unsigned int getSectorsBegin() const;
};