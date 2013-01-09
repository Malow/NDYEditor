#include "WorldRenderer.h"


WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics)
{
	zWorld->AddObserver(this);
	zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );

	// Loop Through Loaded Sectors And Create Render Objects
	for( unsigned int x=0; x<zWorld->GetNumSectorsWidth(); ++x )
	{
		for( unsigned int y=0; y<zWorld->GetNumSectorsHeight(); ++y )
		{
			UpdateSector(x,y);
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
			if ( zTerrain.size() )
				throw("Terrain Size Already Set");
	
			zTerrain.resize( WLE->world->GetNumSectorsWidth() * WLE->world->GetNumSectorsHeight() );
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
	else if ( SectorLoadedEvent* SLE = dynamic_cast<SectorLoadedEvent*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			UpdateSector(SLE->x, SLE->y);
		}
	}
	else if ( SectorHeightMapChanged* SHMC = dynamic_cast<SectorHeightMapChanged*>(e) )
	{
		if ( SHMC->world == zWorld )
		{
			UpdateSectorHeightMap(SHMC->sectorx, SHMC->sectory);
		}
	}
	else if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		std::ifstream file;
		file.open(ELE->fileName);
		if(!file)
		{
			zEntities[ELE->entity] = GetGraphics()->CreateMesh("Media/scale.obj", ELE->entity->GetPosition());
			zEntities[ELE->entity]->Scale(ELE->entity->GetScale());
			ELE->entity->AddObserver(this);
			return;
		}
		file.close();

		zEntities[ELE->entity] = GetGraphics()->CreateMesh(ELE->fileName.c_str(), ELE->entity->GetPosition());
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

		//zEntities[EUE->entity]->Scale(EUE->entity->GetScale());
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
			UpdateSectorBlendMap( SHMC->sectorx, SHMC->sectory );
		}
	}
	else if ( SectorBlendTexturesChanged* SBTC = dynamic_cast<SectorBlendTexturesChanged*>(e) )
	{
		if ( SBTC->world == zWorld )
		{
			UpdateSectorTextures(SBTC->sectorX,SBTC->sectorY);
		}
	}
}


float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = (unsigned int)y/SECTOR_LENGTH * zWorld->GetNumSectorsWidth() + (unsigned int)x/SECTOR_LENGTH;
	if(zTerrain.size() > tIndex)
	{
		if ( zTerrain[tIndex] )
		{
			return zTerrain[tIndex]->GetYPositionAt(fmod(x, (float)SECTOR_LENGTH), fmod(y, (float)SECTOR_LENGTH));
		}
	}
	return std::numeric_limits<float>::infinity();
}


CollisionData WorldRenderer::Get3DRayCollisionDataWithGround()
{
	// Get Applicable Sectors
	Vector2 camPos( GetGraphics()->GetCamera()->GetPosition().x, GetGraphics()->GetCamera()->GetPosition().z );
	std::set< Vector2 > sectors;
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
	CollisionData cd;
	std::vector<Entity*> closeEntities;
	zWorld->GetEntitiesInCircle(Vector2(cam->GetPosition().x, cam->GetPosition().z), 200.0f, closeEntities);
	if( closeEntities.size() == 0)
		return NULL;

	returnPointer = closeEntities.at(0);
	cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(cam->GetPosition(), 
		cam->Get3DPickingRay(), 
		zEntities[closeEntities.at(0)]);

	if(cd.collision)
	{
		found = true;
	}

	while(counter < zEntities.size() && !found)
	{
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
			cam->GetPosition(), 
			cam->Get3DPickingRay(), 
			zEntities[closeEntities.at(counter)]);

		if(cd.collision)
		{
			found = true;
			if((returnPointer->GetPosition() - camPos).GetLength() > (closeEntities.at(counter)->GetPosition() - camPos).GetLength())
			{
				returnPointer = closeEntities.at(counter);
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
		unsigned int tIndex = sectorY * zWorld->GetNumSectorsWidth() + sectorX;
		Vector3 pos(sectorX * 32.0f + 16.0f, 0.0f, sectorY * 32.0f + 16.0f);

		zTerrain[tIndex] = zGraphics->CreateTerrain(pos,
			Vector3(SECTOR_WORLD_SIZE, 1.0f, SECTOR_WORLD_SIZE),
			SECTOR_HEIGHT_SIZE);

		// Update Textures
		UpdateSectorTextures(sectorX,sectorY);

		// Update Blend Map
		UpdateSectorBlendMap(sectorX,sectorY);

		// Update Height Map
		UpdateSectorHeightMap(sectorX,sectorY);
	}
}


void WorldRenderer::UpdateSectorTextures( unsigned int sectorX, unsigned int sectorY )
{
	if ( zWorld->IsSectorLoaded(sectorX,sectorY) )
	{
		unsigned int tIndex = sectorY * zWorld->GetNumSectorsWidth() + sectorX;

		const char* terrainTextures[4];

		// Special Sector Textures for 0,0
		if ( sectorX == 0 && sectorY == 0 )
		{
			terrainTextures[0] = "Media/Textures/Red.png";
			terrainTextures[1] = "Media/Textures/Green.png";
			terrainTextures[2] = "Media/Textures/Blue.png";
			terrainTextures[3] = "Media/Textures/BallTexture.png";
			zTerrain[tIndex]->SetTextures(terrainTextures);
		}
		else
		{
			std::string files[4];
			for( unsigned int x=0; x<4; ++x )
			{
				files[x] = "Media/Textures/";
				files[x] += &zWorld->GetSector(sectorX,sectorY)->GetTextureNames()[TEXTURE_NAME_LENGTH*x];
				terrainTextures[x] = &files[x][0];
			}
			zTerrain[tIndex]->SetTextures(terrainTextures);
		}
		
	}
}
