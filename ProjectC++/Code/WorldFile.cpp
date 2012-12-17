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
	float height[(SECTOR_LENGTH+1)*(SECTOR_LENGTH+1)];
};

struct BlendMap
{
	float blend[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
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


void WorldFile::WriteBlendMap( const float* const data, unsigned int mapIndex )
{
	if ( !zFile ) Open();
	if ( zMode != OPEN_LOAD )
	{
		zFile->seekp( GetBlendsBegin() + mapIndex * sizeof(BlendMap), std::ios::beg );
		zFile->write((const char*)data,sizeof(BlendMap));
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


bool WorldFile::ReadBlendMap( float* data, unsigned int mapIndex )
{
	if ( !zFile ) Open();
	zFile->seekg( GetBlendsBegin() + mapIndex * sizeof(BlendMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	zFile->read((char*)data,sizeof(BlendMap));
	return true;
}


unsigned int WorldFile::GetSectorsBegin() const
{
	return sizeof(WorldFileHeader);
}


unsigned int WorldFile::GetHeightsBegin() const
{
	return GetSectorsBegin() + zNumSectors * sizeof(SectorHeader);
}


unsigned int WorldFile::GetBlendsBegin() const
{
	return GetHeightsBegin() + sizeof( HeightMap ) * zNumSectors;
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

	if ( zMode == OPEN_SAVE )
	{
		// New header
		NotifyObservers(&WorldHeaderCreateEvent(this,zHeader));

		// Save header
		zFile->seekp(0, std::ios::beg);
		zFile->write( reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;
	}
	else if ( zMode == OPEN_EDIT )
	{
		// Check Size
		std::streampos end = zFile->tellg();
		zFile->seekg(0, std::ios::beg);
		unsigned int size = (unsigned int)(end - zFile->tellg());

		if ( size == 0 )
			throw("Empty File!");

		if ( size < sizeof(WorldFileHeader) )
			throw("File Doesn't Have Header!");

		// Read File Header
		zFile->read( reinterpret_cast<char*>(&zHeader), sizeof(WorldFileHeader) );

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;

		// Header Loaded
		NotifyObservers( &WorldHeaderLoadedEvent(this,zHeader) );
	}
}


void WorldFile::ReadHeader()
{
	if( !zFile ) Open();
}
