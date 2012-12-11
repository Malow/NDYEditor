#include "World.h"
#include <windows.h>


World::World( Observer* observer, const std::string& fileName) throw(const char*) : 
	Observed(observer),
	zSectors(NULL), 
	zNrOfSectorsWidth(0), 
	zNrOfSectorsHeight(0)
{
	zFile = new WorldFile(this, fileName, OPEN_EDIT);
}


World::World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight ) :
	Observed(observer),
	zNrOfSectorsWidth(nrOfSectorWidth),
	zNrOfSectorsHeight(nrOfSectorHeight),
	zFile(0)
{
	this->zSectors = new Sector**[this->zNrOfSectorsWidth];
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		this->zSectors[i] = new Sector*[this->zNrOfSectorsHeight];
		for( unsigned int o=0; o < this->zNrOfSectorsHeight; ++o )
		{
			this->zSectors[i][o] = NULL;
		}
	}

	NotifyObservers( &WorldLoadedEvent(this) );
}


World::~World()
{
	// Close File
	if ( zFile ) delete zFile, zFile=0;

	// Delete the zSectors pointers.
	if ( this->zSectors )
	{
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			for(unsigned int o = 0; o < this->zNrOfSectorsHeight; o++)
			{
				if ( this->zSectors[i][o] )
				{
					delete this->zSectors[i][o];
					this->zSectors[i] = NULL;
				}
			}
			delete [] this->zSectors[i];
			this->zSectors[i] = 0;
		}

		// Delete the zSector pointer.
		delete [] this->zSectors;
		this->zSectors = NULL;
	}
}


void World::ModifyHeightAt( unsigned int x, unsigned int y, float value )
{
	unsigned int sectorx = x / SECTOR_LENGTH;
	unsigned int sectory = y / SECTOR_LENGTH;
	unsigned int localx = x % SECTOR_LENGTH;
	unsigned int localy = y % SECTOR_LENGTH;

	Sector* sector = GetSector(sectorx, sectory);
	sector->ModifyHeightAt(localx, localy, value);
	
	// Notify Sector Change
	NotifyObservers( &SectorHeightMapChanged(this, sectorx, sectory, localx, localy) );
}


bool World::CreateEntity( Vector3 pos, ENTITYTYPE entityType, std::string filePath)
{
	if(entityType == ENTITYTYPE::TREE)
	{
		
		Entity* temp = new Entity(pos);
		zEntities.push_back(temp);
		NotifyObservers( &EntityLoadedEvent(this, temp, filePath) );
		return true;
	}
	return false;
}


void World::SetHeightAt( unsigned int x, unsigned int y, float value )
{
	unsigned int sectorx = x / SECTOR_LENGTH;
	unsigned int sectory = y / SECTOR_LENGTH;
	unsigned int localx = x % SECTOR_LENGTH;
	unsigned int localy = y % SECTOR_LENGTH;

	Sector* sector = GetSector(sectorx, sectory);
	sector->SetHeightAt(localx, localy, value);

	// Notify Sector Change
	NotifyObservers( &SectorHeightMapChanged(this, sectorx, sectory, localx, localy) );
}


float World::GetHeightAt( unsigned int x, unsigned int y )
{
	Sector* sector = GetSector(x/SECTOR_LENGTH,y/SECTOR_LENGTH);
	sector->GetHeightAt(x%SECTOR_LENGTH,y%SECTOR_LENGTH);
	return true;
}


void World::SaveFile()
{
	if ( zFile )
	{
		for(unsigned int x=0; x<zNrOfSectorsWidth; ++x)
		{
			for(unsigned int y=0; y<zNrOfSectorsHeight; ++y)
			{
				if ( this->zSectors && this->zSectors[x][y] && this->zSectors[x][y]->IsEdited() )
				{
					zFile->WriteHeightMap(this->zSectors[x][y]->GetHeightMap(), y *zNrOfSectorsWidth + x);
					this->zSectors[x][y]->SetEdited(false);
				}
			}
		}
	}
}


void World::SaveFileAs( const std::string& fileName )
{
	if ( fileName.empty() )
		throw("Filename is empty!");

	if ( zFile )
	{
		if ( fileName == zFile->GetFileName() )	// Saving to the currently open file
		{
			SaveFile();
			return;
		}
		else
		{
			// Handle Save As
			std::string filePath = zFile->GetFileName();
			delete zFile;
			CopyFile( filePath.c_str(), fileName.c_str(), false );
			zFile = new WorldFile(this, fileName, OPEN_SAVE);
			SaveFile();
		}
	}
	else
	{
		DeleteFile( fileName.c_str() );
		zFile = new WorldFile(this, fileName, OPEN_SAVE);
		SaveFile();
	}
}


Sector* World::GetSectorAtWorldPos( const Vector2& pos ) throw(const char*)
{
	return GetSector( (unsigned int)(pos.x / (float)SECTOR_LENGTH), (unsigned int)(pos.y / (float)SECTOR_LENGTH) );
}


Sector* World::GetSector( unsigned int x, unsigned int y ) throw(const char*)
{
	if( (this->zNrOfSectorsWidth) <= x || (this->zNrOfSectorsHeight) <= y )
		throw("Index Out Of Bounds");

	Sector* s = this->zSectors[x][y];

	if ( !s )
	{
		s = new Sector();

		this->zSectors[x][y] = s;

		if ( zFile )
		{
			// Load From File
			zFile->ReadHeightMap(s->GetHeightMap(), y * zNrOfSectorsWidth + x);
		}
		else
		{
			s->Reset();
		}

		NotifyObservers( &SectorLoadedEvent(this,x,y) );
	}

	return s;
}


void World::onEvent( Event* e )
{
	if ( WorldHeaderLoadedEvent* WHL = dynamic_cast<WorldHeaderLoadedEvent*>(e) )
	{
		zNrOfSectorsWidth = WHL->header.width;
		zNrOfSectorsHeight = WHL->header.height;

		this->zSectors = new Sector**[this->zNrOfSectorsWidth];
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			this->zSectors[i] = new Sector*[this->zNrOfSectorsHeight];
			for( unsigned int o=0; o < this->zNrOfSectorsHeight; ++o )
			{
				this->zSectors[i][o] = NULL;
			}
		}

		// World Has Been Loaded
		NotifyObservers( &WorldLoadedEvent(this) );
	}
	else if ( WorldHeaderCreateEvent* WHCE = dynamic_cast<WorldHeaderCreateEvent*>(e) )
	{
		WHCE->header.width = zNrOfSectorsWidth;
		WHCE->header.height = zNrOfSectorsHeight;
	}
}


void World::LoadAllSectors()
{
	for( unsigned int x=0; x<zNrOfSectorsWidth; ++x )
	{
		for( unsigned int y=0; y<zNrOfSectorsHeight; ++y )
		{
			GetSector( x, y );
		}
	}
}

void World::GetEntitiesInCircle( Vector3 pos, float radius, std::vector<Entity*>& entityVector )
{
	radius = pow(radius, 2);
	for(int i = 0; i < zEntities.size(); i++)
	{
		if((zEntities.at(i)->GetPosition() - pos).GetLengthSquared() < radius)
		{
			entityVector.push_back(zEntities.at(i));
		}
	}
	return;
}
