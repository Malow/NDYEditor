#include "WorldFile.h"
#include "MaloW.h"


struct HeightMap
{
	float height[(SECTOR_LENGTH+1)*(SECTOR_LENGTH+1)];
};


struct BlendMap
{
	float blend[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
};


struct SectorHeader
{
	bool generated;
};


struct TextureNamesStruct
{
	char data[TEXTURE_NAME_LENGTH*4];
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

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;

		// Expand File
		zFile->seekp(0, std::ios::end);
		unsigned int fileSize = zFile->tellp();
		if ( fileSize < GetEnding() )
		{
			unsigned int missingSpace = GetEnding() - fileSize;
			std::vector<unsigned char> v;
			v.resize(missingSpace);
			zFile->write(reinterpret_cast<char*>(&v[0]), missingSpace);
		}

		// Save header
		zFile->seekp(0, std::ios::beg);
		zFile->write(reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));
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


unsigned int WorldFile::GetSectorHeadersBegin() const
{
	return sizeof(WorldFileHeader);
}


void WorldFile::WriteSectorHeader( unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		SectorHeader header;
		header.generated = true;
		zFile->seekp( GetSectorHeadersBegin() + sectorIndex * sizeof(SectorHeader), std::ios::beg );
		zFile->write((const char*)&header, sizeof(SectorHeader));
	}
}


bool WorldFile::ReadSectorHeader( unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetSectorHeadersBegin() + sectorIndex * sizeof(SectorHeader), std::ios::beg );
	if ( zFile->eof() ) return false;

	SectorHeader header;
	if ( !zFile->read((char*)&header, sizeof(SectorHeader)) ) return false;
	if ( !header.generated ) return false;

	return true;
}


unsigned int WorldFile::GetSectorTexturesBegin() const
{
	return GetSectorHeadersBegin() + zNumSectors * sizeof(SectorHeader);
}


void WorldFile::WriteTextureNames( const char* data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetSectorTexturesBegin() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
		zFile->write(data, sizeof(TextureNamesStruct));
	}
}


bool WorldFile::ReadTextureNames( char* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetSectorTexturesBegin() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read(data, sizeof(TextureNamesStruct)) ) return false;
	if ( std::string(data) != "TerrainTexture.png")
	{
		int i = 0;
	}
	return true;
}


unsigned int WorldFile::GetHeightsBegin() const
{
	return GetSectorTexturesBegin() + zNumSectors * sizeof(TextureNamesStruct);
}


void WorldFile::WriteHeightMap( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetHeightsBegin() + sectorIndex * sizeof(HeightMap), std::ios::beg );
		zFile->write((const char*)data,sizeof(HeightMap));
	}
}


bool WorldFile::ReadHeightMap( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetHeightsBegin() + sectorIndex * sizeof(HeightMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(HeightMap)) ) return false;
	return true;
}


unsigned int WorldFile::GetBlendsBegin() const
{
	return GetHeightsBegin() + zNumSectors * sizeof(HeightMap);
}


void WorldFile::WriteBlendMap( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetBlendsBegin() + sectorIndex * sizeof(BlendMap), std::ios::beg );
		zFile->write((const char*)data, sizeof(BlendMap));
	}
}


bool WorldFile::ReadBlendMap( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetBlendsBegin() + sectorIndex * sizeof(BlendMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(BlendMap)) ) return false;;
	return true;
}


unsigned int WorldFile::GetEntitiesBegin() const
{
	return GetBlendsBegin() + zNumSectors * sizeof(BlendMap);
}


void WorldFile::WriteEntities( const std::array<EntityStruct, 256>& entities, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetEntitiesBegin() + sectorIndex * sizeof(EntityStruct) * 256, std::ios::beg );
		zFile->write(reinterpret_cast<const char*>(entities.data()), sizeof(EntityStruct) * 256);
	}
}


bool WorldFile::ReadEntities( unsigned int sectorIndex, std::array<EntityStruct, 256>& out )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetEntitiesBegin() + sectorIndex * sizeof(EntityStruct) * 256, std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read(reinterpret_cast<char*>(out.data()), sizeof(EntityStruct) * 256) ) return false;
	return true;
}


unsigned int WorldFile::GetEnding() const
{
	return GetEntitiesBegin() + zNumSectors * sizeof(EntityStruct) * 256;
}
