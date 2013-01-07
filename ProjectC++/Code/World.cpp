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
	unsigned int sectorx = x / SECTOR_WORLD_SIZE;
	unsigned int sectory = y / SECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	unsigned int localx = (x % SECTOR_WORLD_SIZE);
	unsigned int localy = (y % SECTOR_WORLD_SIZE);

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
	Sector* sector = GetSector(x/SECTOR_WORLD_SIZE,y/SECTOR_WORLD_SIZE);
	return sector->GetHeightAt(x%SECTOR_WORLD_SIZE,y%SECTOR_WORLD_SIZE);
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
					zFile->WriteBlendFiles(this->zSectors[x][y]->GetTextureNames(), y * zNrOfSectorsWidth + x);
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
	return GetSector( (unsigned int)(pos.x / (float)SECTOR_WORLD_SIZE), (unsigned int)(pos.y / (float)SECTOR_WORLD_SIZE) );
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
				s->Reset();
			}

			if ( !zFile->ReadBlendMap(s->GetBlendMap(), y * GetNumSectorsWidth() + x ) )
			{
				s->Reset();
			}

			if ( !zFile->ReadBlendFiles(s->GetTextureNames(), y * GetNumSectorsWidth() + x ) )
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


unsigned int World::GetEntitiesInCircle( const Vector2& center, float radius, std::vector<Entity*>& out) const
{
	unsigned int counter=0;

	for(auto i = zEntities.cbegin(); i != zEntities.cend(); i++)
	{
		Vector2 pos( (*i)->GetPosition().x, (*i)->GetPosition().z );
		if( Vector2(center-pos).GetLength() < radius)
		{
			out.push_back(*i);
			counter++;
		}
	}

	return counter;
}


unsigned int World::GetSectorsInCicle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	for( unsigned int x=0; x<zNrOfSectorsWidth; ++x )
	{
		for( unsigned int y=0; y<zNrOfSectorsHeight; ++y )
		{
			Rect sectorRect( Vector2(x * SECTOR_WORLD_SIZE, y * SECTOR_WORLD_SIZE ), Vector2(32,32) );
			if ( DoesIntersect(sectorRect, Circle(center,radius)) )
			{
				counter++;
				out.insert( Vector2(x,y) );
			}
		}
	}

	return counter;
}


unsigned int World::GetHeightNodesInCircle( const Vector2& center, float radius, std::vector<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate Height Node Density
	float density = ( (float)SECTOR_WORLD_SIZE / (float)SECTOR_LENGTH );

	// Snap Center To Closest Position
	float centerSnapX = floor( center.x / density ) * density;
	float centerSnapY = floor( center.y / density ) * density;

	for( float x = centerSnapX - radius; x < centerSnapX + radius; x+=density )
	{
		// Outside World
		if ( x < 0 || x > GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
			continue;

		for( float y = centerSnapY - radius; y < centerSnapY + radius; y+=density )
		{
			// Outside World
			if ( y < 0 || y > GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
				continue;

			if ( Circle(center,radius).IsInside(Vector2(x,y) ) )
			{
				out.push_back( Vector2(x,y) );
				counter++;
			}
		}
	}

	return counter;
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


// TODO: Make it work correctly, Alexivan
unsigned int World::GetTextureNodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate Height Node Density
	float density = ( (float)SECTOR_WORLD_SIZE / (float)SECTOR_BLEND_SIZE );

	// Snap Center To Closest Position
	float centerSnapX = floor( center.x / density ) * density;
	float centerSnapY = floor( center.y / density ) * density;

	for( float x = centerSnapX - radius; x < centerSnapX + radius; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x > GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
			continue;

		for( float y = centerSnapY - radius; y < centerSnapY + radius; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y > GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
				continue;

			if ( Circle(Vector2(centerSnapX,centerSnapY),radius).IsInside(Vector2(x,y) ) )
			{
				out.insert( Vector2(x,y) );
				counter++;
			}
		}
	}

	return counter;
}


Vector4 World::GetBlendingAt( float x, float y )
{
	Sector* sector = GetSector(x/SECTOR_WORLD_SIZE,y/SECTOR_WORLD_SIZE);
	return sector->GetBlendingAt(fmod(x,SECTOR_WORLD_SIZE),fmod(y,SECTOR_WORLD_SIZE));
}


void World::ModifyBlendingAt( float x, float y, const Vector4& val )
{
	if ( val.GetLength() > 0.0f )
	{
		SetBlendingAt( x,y, GetBlendingAt(x,y) + val );
	}
}


void World::SetBlendingAt( float x, float y, const Vector4& val )
{
	Sector* sector = GetSector(x/SECTOR_WORLD_SIZE,y/SECTOR_WORLD_SIZE);
	sector->SetBlendingAt(fmod(x,SECTOR_WORLD_SIZE),fmod(y,SECTOR_WORLD_SIZE),val);

	NotifyObservers( &SectorBlendMapChanged(this, 
		x/SECTOR_WORLD_SIZE,
		y/SECTOR_WORLD_SIZE,
		fmod(x,SECTOR_WORLD_SIZE),
		fmod(y,SECTOR_WORLD_SIZE)) );
}


WorldAnchor* World::CreateAnchor()
{
	WorldAnchor* newAnchor = new WorldAnchor();

	zAnchors.insert(newAnchor);

	return newAnchor;
}


void World::DeleteAnchor( WorldAnchor*& anchor )
{
	zAnchors.erase(anchor);
	if ( anchor ) delete anchor, anchor = 0;
}


void World::Update( float dt )
{
	// Anchored sectors
	std::set< Vector2 > anchoredSectors;
	for( auto i = zAnchors.begin(); i != zAnchors.end(); ++i )
	{
		GetSectorsInCicle( (*i)->position, (*i)->radius, anchoredSectors );
	}

	// Loaded sectors
	std::set< Vector2 > loadedSectors;
	GetLoadedSectors(loadedSectors);

	// Unload sectors
	std::set< Vector2 > sectorsToUnload;
	for( auto i = loadedSectors.begin(); i != loadedSectors.end(); ++i )
	{
		if ( !anchoredSectors.count(*i) )
		{
			Sector *sector = GetSector( (unsigned int)i->x, (unsigned int)i->y );
			if ( !sector->IsEdited() )
			{
				NotifyObservers(&SectorUnloadedEvent(this,*i));
				delete zSectors[(unsigned int)i->x][(unsigned int)i->y];
				zSectors[(unsigned int)i->x][(unsigned int)i->y] = 0;
			}
		}
	}

	// Load new sectors
	for( auto i = anchoredSectors.begin(); i != anchoredSectors.end(); ++i )
	{
		GetSector( i->x, i->y );
	}
}


unsigned int World::GetLoadedSectors( std::set<Vector2>& out ) const
{
	unsigned int counter = 0;
	
	if ( zSectors )
	{
		for( unsigned int x=0; x<zNrOfSectorsWidth; ++x )
		{
			for( unsigned int y=0; y<zNrOfSectorsHeight; ++y )
			{
				if ( zSectors[x][y] != 0 )
				{
					counter++;
					out.insert( Vector2(x,y) );
				}
			}
		}
	}

	return counter;
}
