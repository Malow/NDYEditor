#include "World.h"
#include "CircleAndRect.h"
#include <windows.h>
#include "MaloWFileDebug.h"
#include <sstream>



World::World( Observer* observer, const std::string& fileName) throw(...) : 
	Observed(observer),
	zSectors(NULL), 
	zNrOfSectorsWidth(0), 
	zNrOfSectorsHeight(0)
{
	zFile = new WorldFile(this, fileName, OPEN_EDIT);
	zFile->ReadHeader();
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

	zStartCamPos.x = zNrOfSectorsWidth * SECTOR_WORLD_SIZE * 0.5f;
	zStartCamPos.z = zNrOfSectorsHeight * SECTOR_WORLD_SIZE * 0.5f;
	zStartCamPos.y = 1.7f;
	
	zStartCamRot.x = 1.0f;
	zStartCamRot.y = 0.0f;
	zStartCamRot.z = 0.0f;

	zAmbient.x = 0.1f;
	zAmbient.y = 0.1f;
	zAmbient.z = 0.1f;

	zSunDir = Vector3(0.5f, -1.0f, 0.0f);
	zSunColor = Vector3(1.0f, 1.0f, 1.0f);
	zSunIntensity = 1.0f;

	NotifyObservers( &WorldLoadedEvent(this) );
}


World::~World()
{
	// Notify Observers
	NotifyObservers( &WorldDeletedEvent(this) );

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

Entity* World::CreateEntity( unsigned int entityType )
{
	Entity* temp = new Entity(entityType);
	zEntities.push_back(temp);
	NotifyObservers( &EntityLoadedEvent(this, temp) );
	temp->SetEdited(true);
	return temp;
}

void World::ModifyHeightAt( float x, float y, float delta )
{
	SetHeightAt( x, y, GetHeightAt( x, y ) + delta );
}

void World::SetHeightAt( float x, float y, float val )
{
	unsigned int sectorX = (unsigned int)x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)y / SECTOR_WORLD_SIZE;

	float localX = fmod(x, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;
	float localY = fmod(y, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	float snapX = floor(localX * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);
	float snapY = floor(localY * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);

	GetSector(sectorX, sectorY)->SetHeightAt(snapX, snapY, val);

	// Notify Sector Change
	NotifyObservers( &SectorHeightMapChanged(this, sectorX, sectorY, localX, localY) );

	// Overlap Left
	if ( sectorX > 0 && snapX == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX-1, sectorY)->SetHeightAt(border, snapY, val);
		NotifyObservers( &SectorHeightMapChanged(this, sectorX-1, sectorY, border, snapY) );
	}

	// Overlap Up
	if ( sectorY > 0 && snapY == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX, sectorY-1)->SetHeightAt(snapX, border, val);
		NotifyObservers( &SectorHeightMapChanged(this, sectorX, sectorY-1, snapX, border) );
	}

	// Overlap Left Up Corner
	if ( sectorY > 0 && snapY == 0.0f && sectorX > 0 && snapX == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX-1, sectorY-1)->SetHeightAt(border, border, val);
		NotifyObservers( &SectorHeightMapChanged(this, sectorX-1, sectorY-1, border, border) );
	}
}


float World::GetHeightAt( float x, float y )
{
	unsigned int sectorX = (unsigned int)x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)y / SECTOR_WORLD_SIZE;

	float localX = fmod(x, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;
	float localY = fmod(y, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	float snapX = floor(localX * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);
	float snapY = floor(localY * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);

	return GetSector(sectorX, sectorY)->GetHeightAt(snapX, snapY);
}


void World::SaveFile()
{
	if ( zFile )
	{
		zFile->SetStartCamera(zStartCamPos, zStartCamRot);
		zFile->SetWorldAmbient(zAmbient);
		zFile->SetSunProperties(zSunDir,zSunColor,zSunIntensity);

		if ( zSectors )
		{
			for(unsigned int x=0; x<GetNumSectorsWidth(); ++x)
			{
				for(unsigned int y=0; y<GetNumSectorsHeight(); ++y)
				{
					if ( this->zSectors[x][y] )
					{
						// Optimization because it was used a lot
						unsigned int sectorIndex = y * zNrOfSectorsWidth + x;

						if ( this->zSectors[x][y]->IsEdited() )
						{
							// Write Sector Data
							zFile->WriteHeightMap(zSectors[x][y]->GetHeightMap(), sectorIndex);
							zFile->WriteBlendMap(zSectors[x][y]->GetBlendMap(), sectorIndex);
							zFile->WriteTextureNames(zSectors[x][y]->GetTextureNames(), sectorIndex);
							zFile->WriteAIGrid(zSectors[x][y]->GetAIGrid(), sectorIndex);
							zFile->WriteBlendMap2(zSectors[x][y]->GetBlendMap2(), sectorIndex);
							zFile->WriteTextureNames2(zSectors[x][y]->GetTextureNames2(), sectorIndex);

							// Write Sector Header
							WorldFileSectorHeader header;
							header.generated = true;
							for( unsigned int i = 0; i < 3; ++i )
							{
								header.ambientColor[i] = zSectors[x][y]->GetAmbient()[i];
							}
							
							zFile->WriteSectorHeader(header, sectorIndex);
							zSectors[x][y]->SetEdited(false);
						}

						// Save Entities
						Rect sectorRect(Vector2(x * SECTOR_WORLD_SIZE, y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE));
						std::set< Entity* > entitiesInArea;
						bool save = false;

						// Find Regional Entities
						if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[x][y]] )
						{
							save = true;
						}

						// Go Through List
						std::array<EntityStruct, 256> eArray;
						for( unsigned int i=0; i<256; ++i )
						{
							eArray[i].type = 0;
						}

						unsigned int cur = 0;
						for( auto e = entitiesInArea.begin(); e != entitiesInArea.end(); ++e )
						{
							if( (*e)->IsEdited() )
							{
								(*e)->SetEdited(false);
								save = true;
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
							zFile->WriteEntities(eArray, y * GetNumSectorsWidth() + x);
							zLoadedEntityCount[zSectors[x][y]] = cur;
						}
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

			if ( !CopyFile( filePath.c_str(), fileName.c_str(), false ) )
			{
				int err = GetLastError();
				throw("Failed Copy!");
			}

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


Sector* World::GetSector( unsigned int x, unsigned int y ) throw(...)
{
	if( x >= this->GetNumSectorsWidth() || y >= this->GetNumSectorsHeight() )
		throw("Index Out Of Bounds");

	Sector* s = this->zSectors[x][y];

	if ( !s )
	{
		s = new Sector();

		zSectors[x][y] = s;

		if ( zFile )
		{
			// Read Header
			WorldFileSectorHeader header;
			if ( zFile->ReadSectorHeader(header, y * GetNumSectorsWidth() + x) && header.generated )
			{
				// Set Header
				s->SetAmbient(Vector3(header.ambientColor[0], header.ambientColor[1], header.ambientColor[2]));

				if ( !zFile->ReadHeightMap(s->GetHeightMap(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Loading AI For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );
				}

				if ( !zFile->ReadBlendMap(s->GetBlendMap(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Loading AI For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );
				}

				if ( !zFile->ReadTextureNames(s->GetTextureNames(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Loading AI For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );
				}
				
				if ( !zFile->ReadAIGrid(s->GetAIGrid(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Loading AI For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );
				}

				if ( !zFile->ReadBlendMap2(s->GetBlendMap2(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Reading Second Blendmap For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );

					s->ResetBlendMap2();
				}

				if ( !zFile->ReadTextureNames2(s->GetTextureNames2(), y * GetNumSectorsWidth() + x) )
				{
					std::stringstream ss;
					ss << "Failed Reading Second Blendmap Textures For Sector: (" << x << ", " << y << ")";
					MaloW::Debug( ss.str() );

					s->SetTextureName(4, s->GetTextureName(0));
					s->SetTextureName(5, s->GetTextureName(1));
					s->SetTextureName(6, s->GetTextureName(2));
					s->SetTextureName(7, s->GetTextureName(3));
				}
			}
			else
			{
				std::stringstream ss;
				ss << "Failed Loading Header For Sector: (" << x << ", " << y << ")";
				MaloW::Debug( ss.str() );
				s->Reset();
			}

			// Load Entities
			std::array<EntityStruct,256> eArray;
			if ( zFile->ReadEntities(y * GetNumSectorsWidth() + x, eArray) )
			{
				unsigned int counter=0;
				for( auto e = eArray.cbegin(); e != eArray.cend() && e->type != 0; ++e )
				{
					Vector3 pos(e->pos[0]+(x*SECTOR_WORLD_SIZE), e->pos[1], e->pos[2]+(y*SECTOR_WORLD_SIZE));
					Vector3 rot(e->rot[0], e->rot[1], e->rot[2]);
					Vector3 scale(e->scale[0], e->scale[1], e->scale[2]);

					Entity* ent = new Entity(e->type, pos, rot, scale);
					ent->SetEdited(false);
					zEntities.push_back(ent);
					NotifyObservers( &EntityLoadedEvent(this,ent) );
					NotifyObservers( &EntityUpdatedEvent(ent) );
					counter++;
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

		zStartCamPos = zFile->GetStartCamPos();
		zStartCamRot = zFile->GetStartCamRot();
		zAmbient = zFile->GetWorldAmbient();
		zSunDir = zFile->GetSunDirection();
		zSunColor = zFile->GetSunColor();
		zSunIntensity = zFile->GetSunIntensity();

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

unsigned int World::GetEntitiesInCircle( const Vector2& center, float radius, std::set<Entity*>& out) const
{
	unsigned int counter=0;

	for(auto i = zEntities.cbegin(); i != zEntities.cend(); i++)
	{
		Vector2 pos( (*i)->GetPosition().x, (*i)->GetPosition().z );
		if( Vector2(center-pos).GetLength() < radius)
		{
			out.insert(*i);
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

unsigned int World::GetHeightNodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate Height Node Density
	float density = (float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1);

	for( float x = center.x - radius; x < center.x + radius; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x >= GetWorldSize().x )
			continue;

		for( float y = center.y - radius; y < center.y + radius; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y >= GetWorldSize().y )
				continue;

			// Snap it
			Vector2 snap;
			snap.x = floor(x / density) * density;
			snap.y = floor(y / density) * density;


			if ( Circle(Vector2(center.x, center.y), radius).IsInside(Vector2(snap.x, snap.y) ) )
			{
				out.insert(Vector2(snap.x, snap.y));
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
	NotifyObservers(&EntityRemovedEvent(this,entity));
	auto i = std::find(zEntities.begin(), zEntities.end(), entity);
	zEntities.erase(i);
	delete entity;
}

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

BlendValues World::GetBlendingAt( const Vector2& worldPos )
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;

	return GetSector(sectorX, sectorY)->GetBlendingAt( localPos );
}

void World::ModifyBlendingAt( const Vector2& worldPos, const BlendValues& val )
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;

	GetSector(sectorX, sectorY)->ModifyBlendingAt( localPos, val );

	NotifyObservers( &SectorBlendMapChanged(this, 
		worldPos.x/SECTOR_WORLD_SIZE,
		worldPos.y/SECTOR_WORLD_SIZE,
		fmod(worldPos.x,SECTOR_WORLD_SIZE),
		fmod(worldPos.y,SECTOR_WORLD_SIZE)) );
}

void World::SetBlendingAt( const Vector2& worldPos, const BlendValues& val )
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;

	GetSector(sectorX, sectorY)->SetBlendingAt(localPos, val);

	NotifyObservers(&SectorBlendMapChanged(this, 
		worldPos.x/SECTOR_WORLD_SIZE,
		worldPos.y/SECTOR_WORLD_SIZE,
		localPos.x,
		localPos.y ));
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
		// Is Sector Anchored?
		if ( anchoredSectors.find(*i) == anchoredSectors.end() )
		{
			// Has the sector been edited?
			if ( !GetSector( i->x, i->y )->IsEdited() )
			{
				// Does Sector have edited entities?
				Rect sectorRect(Vector2(i->x * SECTOR_WORLD_SIZE, i->y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE));
				std::set< Entity* > entitiesInArea;
				bool unsavedEntities = false;

				if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[i->x][i->y]] )
				{
					unsavedEntities = true;
				}

				for( auto e = entitiesInArea.begin(); !unsavedEntities && e != entitiesInArea.end(); ++e )
				{
					if ( (*e)->IsEdited() )
					{
						unsavedEntities = true;
					}
				}

				if ( !unsavedEntities )
					sectorsToUnload.insert(*i);
			}
		}
	}

	// Unload Sectors
	for( auto i = sectorsToUnload.begin(); i != sectorsToUnload.end(); ++i )
	{
		// Delete Entities
		Rect sectorRect(Vector2(i->x * SECTOR_WORLD_SIZE, i->y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE));
		std::set< Entity* > entitiesInArea;
		GetEntitiesInRect(sectorRect, entitiesInArea);
		for( auto e = entitiesInArea.begin(); e != entitiesInArea.end(); ++e )
		{
			RemoveEntity(*e);
		}
		zLoadedEntityCount.erase(zSectors[i->x][i->y]);

		// Delete Map
		NotifyObservers(&SectorUnloadedEvent(this,i->x,i->y));
		zLoadedSectors.erase(Vector2UINT(i->x, i->y));
		delete zSectors[i->x][i->y];
		zSectors[i->x][i->y] = 0;
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

float World::CalcHeightAtWorldPos( const Vector2& worldPos ) throw(...)
{
	// Outside world
	if ( worldPos.x >= GetWorldSize().x ||
		worldPos.y >= GetWorldSize().y ||
		worldPos.x < 0 ||
		worldPos.y < 0 ) throw("Out Of Bounds!");

	// Height Nodes Density
	float density = (float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1);

	// Snap To Lower
	float minX = floor(worldPos.x * density) / density;
	float minY = floor(worldPos.y * density) / density;

	// Snap To Lower
	float maxX = minX + density;
	float maxY = minY + density;

	Vector3 a(minX, GetHeightAt(minX, minY), minY);
	Vector3 b(minX, GetHeightAt(minX, maxY), maxY);
	Vector3 c(maxX, GetHeightAt(maxX, minY), minY);
	Vector3 d(maxX, GetHeightAt(maxX, maxY), maxY);

	Vector4 proportions;
	proportions.x = (worldPos - Vector2(a.x, a.y)).GetLength();
	proportions.y = (worldPos - Vector2(b.x, b.y)).GetLength();
	proportions.z = (worldPos - Vector2(c.x, c.y)).GetLength();
	proportions.w = (worldPos - Vector2(d.x, d.y)).GetLength();

	float total = proportions[0] + proportions[1] + proportions[2] + proportions[3];
	for( unsigned int x=0; x<4; ++x )
	{
		proportions[x] /= total;
	}

	return ( 
		a.y * proportions.x +
		b.y * proportions.y +
		c.y * proportions.z +
		d.y * proportions.w );
}

const Vector3& World::GetStartCamPos() const
{
	return zStartCamPos;
}

const Vector3& World::GetStartCamRot() const
{
	return zStartCamRot;
}

void World::SetStartCamera( const Vector3& pos, const Vector3& rot )
{
	zStartCamPos = pos;
	zStartCamRot = rot;
}

Vector3 World::GetAmbientAtWorldPos( const Vector2& worldPos )
{
	if ( worldPos.x < 0 ) return zAmbient;
	if ( worldPos.y < 0 ) return zAmbient;
	if ( worldPos.x >= GetNumSectorsWidth() * SECTOR_WORLD_SIZE ) return zAmbient;
	if ( worldPos.y >= GetNumSectorsHeight() * SECTOR_WORLD_SIZE ) return zAmbient;

	Sector* s = GetSectorAtWorldPos(worldPos);
	return zAmbient + s->GetAmbient();
}

void World::SetSunProperties( const Vector3& dir, const Vector3& color, float intensity )
{
	zSunDir = dir;
	zSunColor = color;
	zSunIntensity = intensity;

	if ( zFile )
		zFile->SetSunProperties(dir, color, intensity);

	NotifyObservers( &WorldSunChanged(this) );
}

void World::SetWorldAmbient( const Vector3& ambient )
{
	zAmbient = ambient;

	if ( zFile )
		zFile->SetWorldAmbient(ambient);
}

bool World::IsBlockingAt( const Vector2& pos )
{
	unsigned int sectorX = (unsigned int)pos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)pos.y / SECTOR_WORLD_SIZE;
	float localX = fmod(pos.x, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;
	float localY = fmod(pos.y, SECTOR_WORLD_SIZE)/SECTOR_WORLD_SIZE;

	return GetSector(sectorX, sectorY)->GetBlocking( Vector2(localX, localY) );
}

void World::SetBlockingAt( const Vector2& pos, const bool& flag )
{
	unsigned int sectorX = (unsigned int)pos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)pos.y / SECTOR_WORLD_SIZE;
	float localX = fmod(pos.x, SECTOR_WORLD_SIZE)/(float)SECTOR_WORLD_SIZE;
	float localY = fmod(pos.y, SECTOR_WORLD_SIZE)/(float)SECTOR_WORLD_SIZE;

	GetSector(sectorX, sectorY)->SetBlocking( Vector2(localX, localY), flag );

	NotifyObservers(&SectorAIGridChanged(this, sectorX, sectorY));
}

Vector3 World::CalcNormalAt( const Vector2& worldPos ) throw(...)
{
	// Outside World
	if ( worldPos.x >= GetWorldSize().x ||
		worldPos.y >= GetWorldSize().y ||
		worldPos.x < 0 ||
		worldPos.y < 0 ) throw("Out Of Bounds!");

	// Density
	float density = (float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1);

	Vector3 a(worldPos.x, GetHeightAt(worldPos.x, worldPos.y), worldPos.y);
	Vector3 b(worldPos.x+density, GetHeightAt(worldPos.x+density, worldPos.y), worldPos.y);
	Vector3 c(worldPos.x, GetHeightAt(worldPos.x, worldPos.y+density), worldPos.y+density);
	Vector3 d(worldPos.x+density, GetHeightAt(worldPos.x+density, worldPos.y+density), worldPos.y+density);

	Vector3 normal = (c-b).GetCrossProduct(d-a);
	normal.Normalize();
	return normal;
}

unsigned int World::GetAINodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate Height Node Density
	float density = ( (float)SECTOR_WORLD_SIZE / (float)SECTOR_AI_GRID_SIZE );

	for( float x = center.x-radius-density; x <= center.x+radius+density; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x > GetWorldSize().x )
			continue;

		for( float y = center.y-radius-density; y <= center.y+radius+density; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y > GetWorldSize().y )
				continue;

			Vector2 snap;
			snap.x = floor(x / density) * density;
			snap.y = floor(y / density) * density;

			Rect r(snap, Vector2(density,density));

			if (DoesIntersect(r, Circle(center,radius)))
			{
				out.insert( Vector2(snap.x, snap.y) );
				counter++;
			}
		}
	}

	return counter;
}

Vector2 World::GetWorldSize() const
{
	return Vector2( GetNumSectorsWidth() * SECTOR_WORLD_SIZE, GetNumSectorsHeight() * SECTOR_WORLD_SIZE );
}
