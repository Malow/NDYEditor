#pragma once

#include <string>
#include <fstream>
#include "Sector.h"

/*
Header
HeightMaps
BlendMaps
*/

class WorldFile
{
	std::string zFileName;
	std::fstream zFile;
	unsigned int zWidth,zHeight;
public:
	WorldFile( const std::string& fileName );
	virtual ~WorldFile();

	void writeHeightMap( const float const* data, unsigned int mapx, unsigned int mapy );
	void readHeightMap( float* data, unsigned int mapx, unsigned int mapy );
private:
	unsigned int getSectorsBegin() const;
};