#include "WorldFile.h"

struct WorldHeader
{
	unsigned int width, height;
};

struct HeightMap
{
	float height[SECTOR_LENGTH*SECTOR_LENGTH];
};

struct BlendMap
{
	float blend[SECTOR_LENGTH*SECTOR_LENGTH*3];
};

WorldFile::WorldFile( const std::string& fileName ) : 
	zFileName(fileName)
{
	zFile.open(fileName, std::ios::binary);
	
	if ( !zFile.good() )
		throw("Failed Opening File!");

	// Check Size
	zFile.seekg(0);
	unsigned int beg = zFile.tellg();
	zFile.seekg(std::ios::end);
	unsigned int end = zFile.tellg();
	unsigned int size = end-beg;

	if ( !size )
	{
		// Empty File
	}
	else
	{
		// Read Header
		zFile.seekg(std::ios::beg);
		WorldHeader header;
		zFile.read( (char*)&header, sizeof(WorldHeader) );

		// Extract Data
		zWidth = header.width;
		zHeight = header.height;
	}
}


WorldFile::~WorldFile()
{
	if ( zFile.good() )
		zFile.close();
}


void WorldFile::writeHeightMap( const float const* data, unsigned int mapx, unsigned int mapy )
{
	unsigned int mapIndex = mapy * zWidth + mapx;
	zFile.seekp( getSectorsBegin() + mapIndex * sizeof(HeightMap) );
	zFile.write((const char*)data,sizeof(HeightMap));
}


void WorldFile::readHeightMap( float* data, unsigned int mapx, unsigned int mapy )
{
	unsigned int mapIndex = mapy * zWidth + mapx;
	zFile.seekg( getSectorsBegin() + mapIndex * sizeof(HeightMap) );
	zFile.read((char*)data,sizeof(HeightMap));
}


unsigned int WorldFile::getSectorsBegin() const
{
	return sizeof(WorldHeader);
}

