#include "World.h"
#include <windows.h>
#include "CircleAndRect.h"


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

	// Delete Entities
	for( auto i = zEntities.begin(); i != zEntities.end(); )
	{
		NotifyObservers( &EntityRemovedEvent(this,*i));
		delete *i;
		i = zEntities.erase(i);
	}

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
					this->zSectors[i][o] = NULL;
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
	if ( value != 0.0f )
	{
		SetHeightAt(x,y,GetHeightAt(x,y)+value);
	}
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

	// Overlap Left
	if ( sectorx > 0 && localx == 0 )
	{
		GetSector(sectorx-1,sectory)->SetHeightAt(SECTOR_LENGTH,localy,value);
		NotifyObservers( &SectorHeightMapChanged(this, sectorx-1, sectory, SECTOR_LENGTH, localy) );
	}

	// Overlap Up
	if ( sectory > 0 && localy == 0 )
	{
		GetSector(sectorx,sectory-1)->SetHeightAt(localx,SECTOR_LENGTH,value);
		NotifyObservers( &SectorHeightMapChanged(this, sectorx, sectory-1, localx, SECTOR_LENGTH) );
	}

	// Overlap Left Up Corner
	if ( sectorx > 0 && localy == 0 && sectorx > 0 && localx == 0 )
	{
		GetSector(sectorx-1,sectory-1)->SetHeightAt(SECTOR_LENGTH,SECTOR_LENGTH,value);
		NotifyObservers( &SectorHeightMapChanged(this, sectorx-1, sectory-1, SECTOR_LENGTH, SECTOR_LENGTH) );
	}
}


float World::GetHeightAt( unsigned int x, unsigned int y )
{
	Sector* sector = GetSector(x/SECTOR_LENGTH,y/SECTOR_LENGTH);
	return sector->GetHeightAt(x%SECTOR_LENGTH,y%SECTOR_LENGTH);
}


void World::SaveFile()
{
	if ( zFile && this->zSectors )
	{
		for(unsigned int x=0; x<GetNumSectorsWidth(); ++x)
		{
			for(unsigned int y=0; y<GetNumSectorsHeight(); ++y)
			{
				if ( this->zSectors[x][y] && this->zSectors[x][y]->IsEdited() )
				{
					zFile->WriteHeightMap(this->zSectors[x][y]->GetHeightMap(), y * zNrOfSectorsWidth + x);
					zFile->WriteBlendMap(this->zSectors[x][y]->GetBlendMap(), y * zNrOfSectorsWidth + x);
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
	if( x >= this->GetNumSectorsWidth() || y >= this->GetNumSectorsHeight() )
		throw("Index Out Of Bounds");

	Sector* s = this->zSectors[x][y];

	if ( !s )
	{
		s = new Sector();

		this->zSectors[x][y] = s;

		if ( zFile )
		{
			// Load From File
			if ( !zFile->ReadHeightMap(s->GetHeightMap(), y * GetNumSectorsWidth() + x) )
			{
				// Load Failed, reset
				s->Reset();
			}

			if ( !zFile->ReadBlendMap(s->GetBlendMap(), y * GetNumSectorsWidth() + x ) )
			{
				s->Reset();
			}
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
	for( unsigned int x=0; x<GetNumSectorsWidth(); ++x )
	{
		for( unsigned int y=0; y<GetNumSectorsHeight(); ++y )
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


// TODO: Test It
void World::GetSectorsInCicle( const Vector2& center, float radius, std::vector<Sector*>& out )
{
	int centerX = center.x / SECTOR_LENGTH;
	int centerY = center.y / SECTOR_LENGTH;
	int radiusDiv = radius / SECTOR_LENGTH;

	for( unsigned int x=center.x - radius; x < center.x + radius; ++x )
	{
		if ( x < 0 || x > GetNumSectorsWidth() ) continue;
		for( unsigned int y=center.y - radius; y < center.y + radius; ++y)
		{
			if ( y < 0 || y > GetNumSectorsHeight() ) continue;
			if ( DoesIntersect( Rect(Vector2(x*SECTOR_LENGTH,y*SECTOR_LENGTH),Vector2(SECTOR_LENGTH,SECTOR_LENGTH)), Circle(center,radius) ) )
			{
				out.push_back( GetSector(x,y) );
			}
		}
	}
}

// TODO: Make Function
void World::GetHeightNodesInCircle( const Vector2& center, float radius, std::vector<Vector2>& out )
{
	
}


bool World::IsSectorLoaded( unsigned int x, unsigned int y ) const
{
	if ( x > zNrOfSectorsWidth ) return false;
	if ( y > zNrOfSectorsHeight ) return false;
	return zSectors[x][y] != 0;
}


unsigned int World::GetNumSectorsWidth() const
{
	if ( !zNrOfSectorsWidth ) zFile->ReadHeader();
	return zNrOfSectorsWidth;
}


unsigned int World::GetNumSectorsHeight() const
{
	if ( !zNrOfSectorsHeight ) zFile->ReadHeader();
	return zNrOfSectorsHeight;
}


void World::RemoveEntity( Entity* entity )
{
	NotifyObservers( &EntityRemovedEvent(this,entity));
	auto i = std::find(zEntities.begin(), zEntities.end(), entity);
	delete *i;
	zEntities.erase(i);
}
