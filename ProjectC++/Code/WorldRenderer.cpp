#include "WorldRenderer.h"
#include "EntityList.h"



WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics),
	zShowAIMap(false)
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
			CreateTerrain(*i);
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
	for( auto i = zEntities.begin(); i != zEntities.end(); ++i )
	{
		zGraphics->DeleteMesh( i->second );
	}

	zEntities.clear();
}


void WorldRenderer::OnEvent( Event* e )
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

			// Remove AI Grid
			auto grid = zAIGrids.find(zTerrain[tIndex]);
			if ( grid != zAIGrids.end() )
				zAIGrids.erase(grid);

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
			CreateTerrain(Vector2UINT(SLE->x, SLE->y));
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
		if ( model == "water" )
		{
			zEntities[ELE->entity] = zGraphics->CreateWaterPlane(ELE->entity->GetPosition(), "Media/WaterTexture.png");
		}
		else
		{
			float billboardDistance = GetEntBillboardDistance(ELE->entity->GetType());

			if ( billboardDistance > 0.0f )
			{
				zEntities[ELE->entity] = zGraphics->CreateMesh(
					model.c_str(), 
					ELE->entity->GetPosition(), 
					GetEntBillboard(ELE->entity->GetType()).c_str(),
					billboardDistance);

			}
			else
			{
				zEntities[ELE->entity] = zGraphics->CreateMesh(model.c_str(), ELE->entity->GetPosition());
			}
		}

		zEntities[ELE->entity]->SetQuaternion(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		zEntities[ELE->entity]->RotateAxis(Vector3(1.0f, 0.0f, 0.0f), ELE->entity->GetRotation().x * (3.1415f / 180.0f));
		zEntities[ELE->entity]->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), ELE->entity->GetRotation().y * (3.1415f / 180.0f));
		zEntities[ELE->entity]->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), ELE->entity->GetRotation().z * (3.1415f / 180.0f));

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
			zGraphics->DeleteMesh(zEntities[ERE->entity]);
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
	else if ( SectorAIGridChanged* SBTC = dynamic_cast<SectorAIGridChanged*>(e) )
	{
		if ( SBTC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
			u = (UPDATEENUM)(u | UPDATE_AIGRID);
		}
	}
}


float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = (unsigned int)(y/(float)SECTOR_WORLD_SIZE) * zWorld->GetNumSectorsWidth() + (unsigned int)(x/(float)SECTOR_WORLD_SIZE);
	
	if ( tIndex >= zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() )
		return std::numeric_limits<float>::infinity();

	if ( zTerrain[tIndex])
	{
		return zTerrain[tIndex]->GetYPositionAt(fmod(x, (float)SECTOR_WORLD_SIZE), fmod(y, (float)SECTOR_WORLD_SIZE));
	}
	
	return std::numeric_limits<float>::infinity();
}


CollisionData WorldRenderer::Get3DRayCollisionDataWithGround()
{
	// Get Applicable Sectors
	std::set< Vector2UINT > sectors;
	zWorld->GetSectorsInCicle( zGraphics->GetCamera()->GetPosition().GetXZ(), 100.0f, sectors );

	// Check For Collision
	for( auto i = sectors.begin(); i != sectors.end(); ++i )
	{
		unsigned int sectorIndex = i->y * zWorld->GetNumSectorsWidth() + i->x;

		CollisionData cd = zGraphics->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
			zGraphics->GetCamera()->GetPosition(), 
			zGraphics->GetCamera()->Get3DPickingRay(), 
			zTerrain[sectorIndex],
			(float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1));

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
	Entity* returnPointer = NULL;

	iCamera* cam = zGraphics->GetCamera();
	Vector3 camPos = cam->GetPosition();
	
	std::set<Entity*> closeEntities;
	zWorld->GetEntitiesInCircle(Vector2(cam->GetPosition().x, cam->GetPosition().z), 200.0f, closeEntities);

	float curDistance = 100000.0f;
	returnPointer = 0;

	for( auto i = closeEntities.begin(); i != closeEntities.end(); ++i )
	{
		CollisionData cd = zGraphics->GetPhysicsEngine()->GetCollisionRayMesh(
			cam->GetPosition(), 
			cam->Get3DPickingRay(), 
			zEntities[*i]);

		if(cd.collision)
		{
			float thisDistance = (Vector3(cd.posx,cd.posy,cd.posz) - camPos).GetLength();

			if( cam->Get3DPickingRay().GetDotProduct(Vector3(cd.posx,cd.posy,cd.posz) - camPos) > 0.0f && thisDistance < curDistance)
			{
				returnPointer = *i;
				curDistance = thisDistance;
			}
		}
	}

	return returnPointer;
}


void WorldRenderer::UpdateSectorHeightMap( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
		T->SetHeightMap( zWorld->GetSector(sectorCoords)->GetHeightMap() );
}


void WorldRenderer::UpdateSectorBlendMap( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		T->SetHeightMap( zWorld->GetSector(sectorCoords)->GetHeightMap() );

		float* data[2] = { zWorld->GetSector(sectorCoords)->GetBlendMap(), zWorld->GetSector(sectorCoords)->GetBlendMap2()  };
		unsigned int sizes[2] = { SECTOR_BLEND_SIZE, SECTOR_BLEND_SIZE };

		T->SetBlendMaps( 
			2,
			&sizes[0],
			&data[0] );
	}
}


void WorldRenderer::UpdateSectorTextures( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		const char* terrainTextures[8];
		std::string files[8];
		for( unsigned int x=0; x<8; ++x )
		{
			files[x] = "Media/Textures/";
			files[x] += zWorld->GetSector(sectorCoords)->GetTextureName(x);
			terrainTextures[x] = &files[x][0];
		}
		T->SetTextures(terrainTextures);
	}
}


void WorldRenderer::Update()
{
	while ( !zUpdatesRequired.empty() )
	{
		auto i = zUpdatesRequired.begin();

		if ( ( i->second & UPDATE_BLENDMAP ) == UPDATE_BLENDMAP )
			UpdateSectorBlendMap(i->first);

		if ( ( i->second & UPDATE_HEIGHTMAP ) == UPDATE_HEIGHTMAP )
			UpdateSectorHeightMap(i->first);

		if ( ( i->second & UPDATE_TEXTURES ) == UPDATE_TEXTURES )
			UpdateSectorTextures(i->first);

		if ( ( i->second & UPDATE_AIGRID ) == UPDATE_AIGRID )
			UpdateSectorAIGrid(i->first);

		zUpdatesRequired.erase(i);
	}
}

void WorldRenderer::ToggleAIGrid( bool state )
{
	zShowAIMap = state;

	if ( !zTerrain.empty() )
	{
		for( unsigned int i=0; i<zWorld->GetNumSectorsWidth()*zWorld->GetNumSectorsHeight(); ++i )
		{
			if ( zTerrain[i] )
			{
				// TODO Terrain Show Grid
				zTerrain[i]->UseAIMap(state);
			}
		}
	}
}

void WorldRenderer::UpdateSectorAIGrid( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		// Data Access
		AIGrid& sectorGrid = zWorld->GetSector(sectorCoords)->GetAIGrid();
		std::vector<unsigned char> &graphicsGrid = zAIGrids[T];
		graphicsGrid.resize(SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE);

		// Bit To Byte
		for( unsigned int x=0; x<SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE; ++x )
		{
			graphicsGrid[x] = sectorGrid[x] * 255;
		}

		T->SetAIGrid(SECTOR_AI_GRID_SIZE, &graphicsGrid[0]);
		T->SetAIGridThickness();
		T->UseAIMap(zShowAIMap);
	}
}

iTerrain* WorldRenderer::GetTerrain( const Vector2UINT& sectorCoords )
{
	if ( zTerrain.empty() ) return 0;
	unsigned int tIndex = sectorCoords.y * zWorld->GetNumSectorsWidth() + sectorCoords.x;
	return zTerrain[tIndex];
}

void WorldRenderer::CreateTerrain( const Vector2UINT& sectorCoords )
{
	if ( zTerrain.empty() )
		zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );

	// Create if it doesn't exist
	unsigned int tIndex = sectorCoords.y * zWorld->GetNumSectorsWidth() + sectorCoords.x;
	if ( !zTerrain[tIndex] )
	{
		Vector3 pos;
		pos.x = sectorCoords.x * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;
		pos.y = 0.0f;
		pos.z = sectorCoords.y * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;

		zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(FSECTOR_WORLD_SIZE, 1.0f, FSECTOR_WORLD_SIZE), SECTOR_HEIGHT_SIZE);
		zTerrain[tIndex]->SetTextureScale(10.0f);

		UPDATEENUM& u = zUpdatesRequired[sectorCoords];
		u = (UPDATEENUM)(u | UPDATE_ALL);
	}
}