#include "WorldRenderer.h"
#include "EntityList.h"



WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics)
{
	zWorld->AddObserver(this);

	zGraphics->SetSunLightProperties(
		zWorld->GetSunDir(),
		zWorld->GetSunColor(),
		zWorld->GetSunIntensity() );

	// Render Loaded SEctors
	auto loadedSectors = zWorld->GetLoadedSectors();
	if ( !loadedSectors.empty() )
	{
		zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );
		for( auto i = loadedSectors.cbegin(); i != loadedSectors.cend(); ++i )
		{
			UpdateSector(i->x, i->y);
		}
	}
}


WorldRenderer::~WorldRenderer()
{
	// Stop Observing
	zWorld->RemoveObserver(this);

	// Clean Terrain
	for( unsigned int x=0; x<zTerrain.size(); ++x )
	{
		if ( zTerrain[x] )
			zGraphics->DeleteTerrain(zTerrain[x]);
	}
	zTerrain.clear();

	// Clean Entities
	for( auto i = zEntities.cbegin(); i != zEntities.cend(); ++i )
	{
		zGraphics->DeleteMesh( i->second );
	}

	zEntities.clear();
}


void WorldRenderer::onEvent( Event* e )
{
	if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
	{
		if ( WLE->world == zWorld )
		{
			zGraphics->SetSunLightProperties(
				WLE->world->GetSunDir(),
				WLE->world->GetSunColor(),
				WLE->world->GetSunIntensity() );
		}
	}
	else if ( SectorUnloadedEvent* SUE = dynamic_cast<SectorUnloadedEvent*>(e) )
	{
		if ( SUE->world == zWorld )
		{
			unsigned int tIndex = SUE->sectorY * SUE->world->GetNumSectorsWidth() + SUE->sectorX;
			zGraphics->DeleteTerrain(zTerrain[tIndex]);
			zTerrain[tIndex] = 0;
		}
	}
	else if ( WorldSunChanged* WSC = dynamic_cast<WorldSunChanged*>(e) )
	{
		zGraphics->SetSunLightProperties(
			WSC->world->GetSunDir(),
			WSC->world->GetSunColor(),
			WSC->world->GetSunIntensity() );
	}
	else if ( SectorLoadedEvent* SLE = dynamic_cast<SectorLoadedEvent*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			if ( !zTerrain.size() )
			{
				zTerrain.resize( SLE->world->GetNumSectorsWidth() * SLE->world->GetNumSectorsHeight() );
			}
			UpdateSector(SLE->x, SLE->y);
		}
	}
	else if ( SectorHeightMapChanged* SHMC = dynamic_cast<SectorHeightMapChanged*>(e) )
	{
		if ( SHMC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
			u = (UPDATEENUM)(u | UPDATE_HEIGHTMAP);
		}
	}
	else if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		const std::string& model = GetEntModel(ELE->entity->GetType());

		if ( model.substr(model.size()-4,4) == ".ani" )
		{
			zEntities[ELE->entity] = GetGraphics()->CreateAnimatedMesh(model.c_str(), ELE->entity->GetPosition());
		}
		else
		{
			zEntities[ELE->entity] = GetGraphics()->CreateMesh(model.c_str(), ELE->entity->GetPosition());
		}

		zEntities[ELE->entity]->Scale(ELE->entity->GetScale());
		ELE->entity->AddObserver(this);
	}
	else if ( EntityUpdatedEvent* EUE = dynamic_cast<EntityUpdatedEvent*>(e) )
	{
		Vector3 temp = EUE->entity->GetRotation();
		zEntities[EUE->entity]->SetPosition(EUE->entity->GetPosition());

		zEntities[EUE->entity]->SetQuaternion(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		zEntities[EUE->entity]->RotateAxis(Vector3(1.0f, 0.0f, 0.0f), EUE->entity->GetRotation().x * (3.1415f / 180.0f));
		zEntities[EUE->entity]->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), EUE->entity->GetRotation().y * (3.1415f / 180.0f));
		zEntities[EUE->entity]->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), EUE->entity->GetRotation().z * (3.1415f / 180.0f));
		
		if(EUE->entity->GetSelected())
			zEntities[EUE->entity]->SetSpecialColor(COLOR::RED_COLOR);
		else
			zEntities[EUE->entity]->SetSpecialColor(COLOR::NULL_COLOR);

		zEntities[EUE->entity]->SetScale(EUE->entity->GetScale()*0.05f);
	}
	else if ( EntityRemovedEvent* ERE = dynamic_cast<EntityRemovedEvent*>(e) )
	{
		if ( ERE->world == zWorld )
		{
			GetGraphics()->DeleteMesh(zEntities[ERE->entity]);
			zEntities.erase(ERE->entity);
		}
	}
	else if ( SectorBlendMapChanged* SHMC = dynamic_cast<SectorBlendMapChanged*>(e) )
	{
		if ( SHMC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
			u = (UPDATEENUM)(u | UPDATE_BLENDMAP);
		}
	}
	else if ( SectorBlendTexturesChanged* SBTC = dynamic_cast<SectorBlendTexturesChanged*>(e) )
	{
		if ( SBTC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
			u = (UPDATEENUM)(u | UPDATE_TEXTURES);
		}
	}
}


float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = (unsigned int)(y/SECTOR_WORLD_SIZE) * zWorld->GetNumSectorsWidth() + (unsigned int)(x/SECTOR_WORLD_SIZE);
	
	if ( zTerrain[tIndex] )
	{
		return zTerrain[tIndex]->GetYPositionAt(fmod(x, (float)SECTOR_WORLD_SIZE), fmod(y, (float)SECTOR_WORLD_SIZE));
	}
	
	return std::numeric_limits<float>::infinity();
}


CollisionData WorldRenderer::Get3DRayCollisionDataWithGround()
{
	// Get Applicable Sectors
	Vector2 camPos( GetGraphics()->GetCamera()->GetPosition().x, GetGraphics()->GetCamera()->GetPosition().z );
	std::set< Vector2UINT > sectors;
	zWorld->GetSectorsInCicle( camPos, 100.0f, sectors );

	// Check For Collision
	for( auto i = sectors.begin(); i != sectors.end(); ++i )
	{
		unsigned int sectorIndex = i->y * zWorld->GetNumSectorsWidth() + i->x;

		CollisionData cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayTerrain(
			GetGraphics()->GetCamera()->GetPosition(), 
			GetGraphics()->GetCamera()->Get3DPickingRay(), 
			zTerrain[sectorIndex]);

		if(cd.collision)
		{
			return cd;
		}
	}

	// Return default collision
	return CollisionData();
}


Entity* WorldRenderer::Get3DRayCollisionWithMesh()
{
	unsigned int counter = 0;
	bool found = false;

	Entity* returnPointer = NULL;

	iCamera* cam = GetGraphics()->GetCamera();
	Vector3 camPos = cam->GetPosition();
	iPhysicsEngine* pe = GetGraphics()->GetPhysicsEngine();
	
	std::vector<Entity*> closeEntities;
	zWorld->GetEntitiesInCircle(Vector2(cam->GetPosition().x, cam->GetPosition().z), 200.0f, closeEntities);
	if( closeEntities.size() == 0 )
		return NULL;

	float curDistance = 100000.0f;
	returnPointer = closeEntities[0];

	CollisionData cd;
	cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(cam->GetPosition(), cam->Get3DPickingRay(), zEntities[closeEntities[0]]);
	if(cd.collision)
	{
		found = true;
		curDistance = (Vector3(cd.posx,cd.posy,cd.posz) - camPos).GetLength();
	}

	while(counter < zEntities.size())
	{
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
			cam->GetPosition(), 
			cam->Get3DPickingRay(), 
			zEntities[closeEntities[counter]]);

		if(cd.collision)
		{
			found = true;
			float thisDistance = (Vector3(cd.posx,cd.posy,cd.posz) - camPos).GetLength();
			if(thisDistance < curDistance)
			{
				returnPointer = closeEntities[counter];
				curDistance = thisDistance;
			}
		}
		counter++;
	}
	cam = NULL;
	pe = NULL;

	if(found)
		return returnPointer;
	else
		return NULL;
}


void WorldRenderer::UpdateSectorHeightMap( unsigned int x, unsigned int y )
{
	if ( zWorld->IsSectorLoaded(x,y) )
	{
		unsigned int tIndex = y * zWorld->GetNumSectorsWidth() + x;
		if ( zTerrain[tIndex] )
		{
			zTerrain[tIndex]->SetHeightMap( zWorld->GetSector(x, y)->GetHeightMap() );
		}
	}
}


void WorldRenderer::UpdateSectorBlendMap( unsigned int x, unsigned int y )
{
	if ( zWorld->IsSectorLoaded(x,y) )
	{
		unsigned int tIndex = y * zWorld->GetNumSectorsWidth() + x;
		zTerrain[tIndex]->SetBlendMap( SECTOR_BLEND_SIZE, zWorld->GetSector(x, y)->GetBlendMap() );
	}
}


void WorldRenderer::UpdateSector( unsigned int sectorX, unsigned int sectorY )
{
	if ( zWorld->IsSectorLoaded(sectorX,sectorY) )
	{
		// Create if it doesn't exist
		unsigned int tIndex = sectorY * zWorld->GetNumSectorsWidth() + sectorX;
		if ( !zTerrain[tIndex] )
		{
			Vector3 pos(sectorX * SECTOR_WORLD_SIZE + SECTOR_WORLD_SIZE/2, 0.0f, sectorY * SECTOR_WORLD_SIZE + SECTOR_WORLD_SIZE/2);
			zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(SECTOR_WORLD_SIZE, 1.0f, SECTOR_WORLD_SIZE), SECTOR_HEIGHT_SIZE);
			zTerrain[tIndex]->SetTextureScale(10.0f);
		}

		// Update Textures
		UpdateSectorTextures(sectorX, sectorY);

		// Update Blend Map
		UpdateSectorBlendMap(sectorX, sectorY);

		// Update Height Map
		UpdateSectorHeightMap(sectorX, sectorY);
	}
}


void WorldRenderer::UpdateSectorTextures( unsigned int sectorX, unsigned int sectorY )
{
	if ( zWorld->IsSectorLoaded(sectorX, sectorY) )
	{
		unsigned int tIndex = sectorY * zWorld->GetNumSectorsWidth() + sectorX;

		const char* terrainTextures[4];
		std::string files[4];
		for( unsigned int x=0; x<4; ++x )
		{
			files[x] = "Media/Textures/";
			files[x] += zWorld->GetSector(sectorX, sectorY)->GetTextureName(x);
			terrainTextures[x] = &files[x][0];
		}
		zTerrain[tIndex]->SetTextures(terrainTextures);
	}
}


void WorldRenderer::update()
{
	while ( !zUpdatesRequired.empty() )
	{
		auto i = zUpdatesRequired.begin();

		if ( ( i->second & UPDATE_BLENDMAP ) == UPDATE_BLENDMAP )
			UpdateSectorBlendMap(i->first.x, i->first.y);

		if ( ( i->second & UPDATE_HEIGHTMAP ) == UPDATE_HEIGHTMAP )
			UpdateSectorHeightMap(i->first.x, i->first.y);

		if ( ( i->second & UPDATE_TEXTURES ) == UPDATE_TEXTURES )
			UpdateSectorTextures(i->first.x, i->first.y);

		zUpdatesRequired.erase(i);
	}
}