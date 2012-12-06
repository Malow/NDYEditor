#include "WorldFile.h"
#include "MaloW.h"

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
	float blend[SECTOR_LENGTH*SECTOR_LENGTH*4];
};


WorldFile::WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode ) :
	Observed(observer),
	zFileName(fileName),
	zMode(mode)
{
	auto format = std::ios::in | std::ios::binary;
	if ( (zMode & OPEN_EDIT) == OPEN_EDIT ) format = format | std::ios::ate;
	if ( (zMode & OPEN_SAVE) == OPEN_SAVE ) format = format | std::ios::out | std::ios::trunc;

	// Open File
	zFile.open(fileName, format);

	// Did it fail?
	if ( !zFile.is_open() )
	{
		// Create File
		std::ofstream s(fileName);
		if ( !s.good() )
			throw("Failed Creating File!");
		s.close();

		// Reopen
		zFile.open(fileName, format);
		if ( !zFile.is_open() )
			throw("Failed Opening File!");
	}

	if ( (zMode & OPEN_SAVE) == OPEN_SAVE )
	{
		// New Header
		WorldHeaderCreateEvent WHCE(this);
		NotifyObservers(&WHCE);

		// Save Header
		zFile.seekp(0, std::ios::beg);
		zFile.write( reinterpret_cast<const char*>(&WHCE.header), sizeof(WorldHeader));
	}
	
	if ( (zMode & OPEN_EDIT) == OPEN_EDIT )
	{
		// Check Size
		std::streampos end = zFile.tellg();
		zFile.seekg(0, std::ios::beg);
		unsigned int size = (unsigned int)(end - zFile.tellg());

		if ( size == 0 )
			throw("Empty File!");

		if ( size < sizeof(WorldHeader) )
			throw("File Doesn't Have Header!");

		// Read File Header
		WorldHeaderLoadedEvent headerEvent(this);
		zFile.read( reinterpret_cast<char*>(&headerEvent.header), sizeof(WorldHeader) );

		// Header Loaded
		NotifyObservers( &headerEvent );
	}
}


WorldFile::~WorldFile()
{
	if ( zFile )
	{
		zFile.close();
	}
}


void WorldFile::WriteHeightMap( const float* const data, unsigned int mapIndex )
{
	if ( ( zMode & OPEN_LOAD ) != OPEN_LOAD )
	{
		zFile.seekp( GetSectorsBegin() + mapIndex * sizeof(HeightMap) );
		zFile.write((const char*)data,sizeof(HeightMap));
	}
}


void WorldFile::ReadHeightMap( float* data, unsigned int mapIndex )
{
	zFile.seekg( GetSectorsBegin() + mapIndex * sizeof(HeightMap) );
	zFile.read((char*)data,sizeof(HeightMap));
}


unsigned int WorldFile::GetSectorsBegin() const
{
	return sizeof(WorldHeader);
}

