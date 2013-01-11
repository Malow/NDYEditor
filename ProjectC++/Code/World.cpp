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
		SetHeightAt(x, y, GetHeightAt(x,y)+value);
	}
}


Entity* World::CreateEntity( unsigned int entityType )
{
	Entity* temp = new Entity(entityType);
	zEntities.push_back(temp);
	NotifyObservers( &EntityLoadedEvent(this, temp) );
	return temp;
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
				if ( this->zSectors[x][y] )
				{
					if ( this->zSectors[x][y]->IsEdited() )
					{
						zFile->WriteHeightMap(this->zSectors[x][y]->GetHeightMap(), y * zNrOfSectorsWidth + x);
						zFile->WriteBlendMap(this->zSectors[x][y]->GetBlendMap(), y * zNrOfSectorsWidth + x);
						zFile->WriteTextureNames(this->zSectors[x][y]->GetTextureNames(), y * zNrOfSectorsWidth + x);
						zFile->WriteSectorHeader(y * zNrOfSectorsWidth + x);
						this->zSectors[x][y]->SetEdited(false);
					}

					// Save Entities
					Rect sectorRect(Vector2(x * SECTOR_WORLD_SIZE, y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE));
					std::set< Entity* > entitiesInArea;
					bool save = false;

					if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[x][y]] )
					{
						save = true;
					}

					std::array<EntityStruct,256> eArray;
					memset(&eArray[0], 0, sizeof(EntityStruct)*256);
					unsigned int cur = 0;
					for( auto e = entitiesInArea.begin(); e != entitiesInArea.end(); ++e )
					{
						if( (*e)->IsEdited() )
						{
							save = true;
							(*e)->SetEdited(false);
						}

						// Position
						Vector3 pos = (*e)->GetPosition();
						eArray[cur].pos[0] = pos.x - sectorRect.topLeft.x;
						eArray[cur].pos[1] = pos.y;
						eArray[cur].pos[2] = pos.z - sectorRect.topLeft.y;

						// Rotation
						Vector3 rot = (*e)->GetRotation();
						eArray[cur].rot[0] = rot.x;
						eArray[cur].rot[1] = rot.y;
						eArray[cur].rot[2] = rot.z;

						// Scale
						Vector3 scale = (*e)->GetScale();
						eArray[cur].scale[0] = scale.x;
						eArray[cur].scale[1] = scale.y;
						eArray[cur].scale[2] = scale.z;

						// Type
						eArray[cur].type = (*e)->GetType();

						cur++;
					}

					if ( save )
					{
						zFile->WriteEntities(eArray, y * zNrOfSectorsWidth + x);
						zLoadedEntityCount[zSectors[x][y]] = cur;
					}
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


Sector* World::GetSectorAtWorldPos( const Vector2& pos )
{
	Vector2UINT sectorPos = WorldPosToSector(pos);
	return GetSector( sectorPos.x, sectorPos.y );
}


Vector2UINT World::WorldPosToSector( const Vector2& pos ) const
{
	return Vector2UINT((unsigned int)pos.x / SECTOR_WORLD_SIZE, (unsigned int)pos.y / SECTOR_WORLD_SIZE);
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
			if ( !zFile->ReadSectorHeader(y * GetNumSectorsWidth() + x) )
			{
				s->Reset();
			}
			else if ( !zFile->ReadHeightMap(s->GetHeightMap(), y * GetNumSectorsWidth() + x) )
			{
				s->Reset();
			}
			else if ( !zFile->ReadBlendMap(s->GetBlendMap(), y * GetNumSectorsWidth() + x ) )
			{
				s->Reset();
			}
			else if ( !zFile->ReadTextureNames(s->GetTextureNames(), y * GetNumSectorsWidth() + x ) )
			{
				s->Reset();
			}

			// Load Entities
			std::array<EntityStruct,256> eArray;
			if ( zFile->ReadEntities(y * GetNumSectorsWidth() + x, eArray) )
			{
				unsigned int counter=0;
				for( auto e = eArray.cbegin(); e != eArray.cend(); ++e )
				{
					unsigned int eType = e->type;
					if ( eType != 0 )
					{
						Vector3 pos(e->pos[0]+(x*SECTOR_WORLD_SIZE), e->pos[1], e->pos[2]+(y*SECTOR_WORLD_SIZE));
						Vector3 rot(e->rot[0], e->rot[1], e->rot[2]);
						Vector3 scale(e->scale[0], e->scale[1], e->scale[2]);

						Entity* ent = new Entity(eType, pos, rot, scale);
						ent->SetEdited(false);
						zEntities.push_back(ent);
						NotifyObservers( &EntityLoadedEvent(this,ent) );
						counter++;
					}
				}
				zLoadedEntityCount[zSectors[x][y]] = counter;
			}
			else
			{
				zLoadedEntityCount[zSectors[x][y]] = 0;
			}
		}
		else
		{
			s->Reset();
		}

		zLoadedSectors.insert( Vector2UINT(x, y) );
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


unsigned int World::GetSectorsInCicle( const Vector2& center, float radius, std::set<Vector2UINT>& out ) const
{
	unsigned int counter=0;
	
	unsigned int xMin = ( ( center.x - radius ) < 0 ? 0 : center.x - radius  ) / SECTOR_WORLD_SIZE;
	unsigned int xMax = ( ( center.x + radius ) > zNrOfSectorsWidth*SECTOR_WORLD_SIZE? zNrOfSectorsWidth*SECTOR_WORLD_SIZE : ( center.x + radius ) ) / SECTOR_WORLD_SIZE;

	unsigned int yMin = ( ( center.y - radius ) < 0 ? 0 : center.y - radius ) / SECTOR_WORLD_SIZE;
	unsigned int yMax = ( ( center.y + radius ) > zNrOfSectorsHeight*SECTOR_WORLD_SIZE? zNrOfSectorsHeight*SECTOR_WORLD_SIZE : ( center.y + radius ) ) / SECTOR_WORLD_SIZE; 

	for( unsigned int x=xMin; x<xMax; ++x )
	{
		for( unsigned int y=yMin; y<yMax; ++y )
		{
			Rect sectorRect( Vector2(x * SECTOR_WORLD_SIZE, y * SECTOR_WORLD_SIZE ), Vector2(32,32) );
			if ( DoesIntersect(sectorRect, Circle(center,radius)) )
			{
				counter++;
				out.insert( Vector2UINT(x,y) );
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
	if ( x >= zNrOfSectorsWidth ) return false;
	if ( y >= zNrOfSectorsHeight ) return false;
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
	float fSize = (float)SECTOR_WORLD_SIZE;
	Sector* sector = GetSector(x/fSize,y/fSize);
	return sector->GetBlendingAt(fmod(x,fSize),fmod(y,fSize));
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


void World::Update()
{
	// Anchored sectors
	std::set< Vector2UINT > anchoredSectors;
	for( auto i = zAnchors.begin(); i != zAnchors.end(); ++i )
	{
		GetSectorsInCicle( (*i)->position, (*i)->radius, anchoredSectors );
	}

	// Loaded sectors
	std::set< Vector2UINT > loadedSectors = GetLoadedSectors();

	// Unload sectors
	std::set< Vector2UINT > sectorsToUnload;
	for( auto i = loadedSectors.begin(); i != loadedSectors.end(); ++i )
	{
		if ( !anchoredSectors.count(*i) )
		{
			Rect sectorRect(Vector2(i->x * SECTOR_WORLD_SIZE, i->y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE));
			std::set< Entity* > entitiesInArea;
			bool unsavedEntities = false;

			if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[i->x][i->y]] )
			{
				unsavedEntities = true;
			}

			for( auto e = entitiesInArea.begin(); e != entitiesInArea.end(); ++e )
			{
				if ( (*e)->IsEdited() )
				{
					unsavedEntities = true;
					break;
				}
			}

			if ( !unsavedEntities && !GetSector( i->x, i->y )->IsEdited() )
			{
				NotifyObservers(&SectorUnloadedEvent(this,i->x,i->y));
				zLoadedSectors.erase( Vector2UINT(i->x, i->y) );
				zLoadedEntityCount.erase(zSectors[i->x][i->y]);
				delete zSectors[i->x][i->y];
				zSectors[i->x][i->y] = 0;
			}
		}
	}

	// Load new sectors
	for( auto i = anchoredSectors.begin(); i != anchoredSectors.end(); ++i )
	{
		GetSector( i->x, i->y );
	}
}


void World::SetSectorTexture( unsigned int x, unsigned int y, const std::string& texture, unsigned int index )
{
	Sector* s = GetSector(x,y);
	s->SetTextureName(index, texture);
	NotifyObservers( &SectorBlendTexturesChanged(this,x,y) );
}


const char* const World::GetSectorTexture( unsigned int x, unsigned int y, unsigned int index )
{
	Sector *s = GetSector(x,y);
	return s->GetTextureName(index);
}


unsigned int World::GetEntitiesInRect( const Rect& rect, std::set<Entity*>& out ) const
{
	unsigned int counter=0;

	for( auto i = zEntities.begin(); i != zEntities.end(); ++i )
	{
		if ( rect.IsInside( (*i)->GetPosition().GetXZ() ) )
		{
			out.insert(*i);
			counter++;
		}
	}

	return counter;
}



