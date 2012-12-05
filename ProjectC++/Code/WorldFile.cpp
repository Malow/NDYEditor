#include "WorldFile.h"

struct WorldHeader
{
	unsigned int width;
	unsigned int height;

	WorldHeader() : width(10), height(10) {}
};

struct SectorHeader
{
	char textureOne[100];
	char textureTwo[100];
	char textureThree[100];
};

struct HeightMap
{
	float height[SECTOR_LENGTH*SECTOR_LENGTH];
};

struct BlendMap
{
	float blend[SECTOR_LENGTH*SECTOR_LENGTH*3];
};

WorldFile::WorldFile( Observer* observer, const std::string& fileName, bool readOnly ) : 
	zFileName(fileName),
	zReadOnly(readOnly)
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

	if ( size == 0 && !zReadOnly )
	{
		// Empty File, Write Default Header
		WorldHeader defaultHeader;
		zWidth = defaultHeader.width;
		zHeight = defaultHeader.height;

		// Write Header
		if ( !readOnly )
		{
			zFile.seekp(std::ios::beg);
			zFile.write( reinterpret_cast<const char*>(&defaultHeader), sizeof(WorldHeader) );
		}
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

	// Header Loaded
	notifyObservers( &WorldHeaderLoadedEvent(this) );
}


WorldFile::~WorldFile()
{
	if ( zFile.good() )
		zFile.close();
}


void WorldFile::writeHeightMap( const float const* data, unsigned int mapx, unsigned int mapy )
{
	if ( !zReadOnly )
	{
		unsigned int mapIndex = mapy * zWidth + mapx;
		zFile.seekp( getSectorsBegin() + mapIndex * sizeof(HeightMap) );
		zFile.write((const char*)data,sizeof(HeightMap));
	}
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

