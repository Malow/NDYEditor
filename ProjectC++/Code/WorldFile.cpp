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
	zMode(mode),
	zNumSectors(0),
	zFile(0)
{
}


WorldFile::~WorldFile()
{
	if ( zFile ) delete zFile, zFile=0;
}


void WorldFile::WriteHeightMap( const float* const data, unsigned int mapIndex )
{
	if ( !zFile ) Open();
	if ( zMode != OPEN_LOAD )
	{
		zFile->seekp( GetHeightsBegin() + mapIndex * sizeof(HeightMap), std::ios::beg );
		zFile->write((const char*)data,sizeof(HeightMap));
	}
}


bool WorldFile::ReadHeightMap( float* data, unsigned int mapIndex )
{
	if ( !zFile ) Open();
	zFile->seekg( GetHeightsBegin() + mapIndex * sizeof(HeightMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	zFile->read((char*)data,sizeof(HeightMap));
	return true;
}


unsigned int WorldFile::GetSectorsBegin() const
{
	return sizeof(WorldHeader);
}


unsigned int WorldFile::GetHeightsBegin() const
{
	return GetSectorsBegin() + zNumSectors * sizeof(SectorHeader);
}


void WorldFile::Open()
{
	if ( zFile ) return;

	auto format = std::ios::in | std::ios::binary;
	if ( zMode == OPEN_EDIT ) format = format | std::ios::out | std::ios::ate;
	if ( zMode == OPEN_SAVE ) format = format | std::ios::out | std::ios::trunc;

	// Open File
	zFile = new fstream(zFileName, format);

	// Did it fail?
	if ( !zFile->is_open() )
	{
		// Create File
		std::ofstream s(zFileName);
		if ( !s.good() )
			throw("Failed Creating File!");
		s.close();

		// Reopen
		zFile->open(zFileName, format);
		if ( !zFile->is_open() )
			throw("Failed Opening File!");
	}

	if ( OPEN_SAVE == OPEN_SAVE )
	{
		// New Header
		WorldHeaderCreateEvent WHCE(this);
		NotifyObservers(&WHCE);

		// Save Header
		zFile->seekp(0, std::ios::beg);
		zFile->write( reinterpret_cast<const char*>(&WHCE.header), sizeof(WorldHeader));

		// Save Num Sectors
		zNumSectors = WHCE.header.width * WHCE.header.height;
	}
	else if ( zMode == OPEN_EDIT )
	{
		// Check Size
		std::streampos end = zFile->tellg();
		zFile->seekg(0, std::ios::beg);
		unsigned int size = (unsigned int)(end - zFile->tellg());

		if ( size == 0 )
			throw("Empty File!");

		if ( size < sizeof(WorldHeader) )
			throw("File Doesn't Have Header!");

		// Read File Header
		WorldHeaderLoadedEvent headerEvent(this);
		zFile->read( reinterpret_cast<char*>(&headerEvent.header), sizeof(WorldHeader) );

		// Save Num Sectors
		zNumSectors = headerEvent.header.width * headerEvent.header.height;

		// Header Loaded
		NotifyObservers( &headerEvent );
	}
}


void WorldFile::ReadHeader()
{
	if( !zFile ) Open();
}

