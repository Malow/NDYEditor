#include "WorldRenderer.h"


WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics)
{
	zWorld->AddObserver(this);
	zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );
}


WorldRenderer::~WorldRenderer()
{
	zWorld->RemoveObserver(this);

	// Clean Terrain
	for( unsigned int x=0; x<zTerrain.size(); ++x )
	{
		zGraphics->DeleteTerrain(zTerrain[x]);
	}

	for( auto i = zEntities.cbegin(); i != zEntities.cend(); ++i )
	{
		delete i->second;
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
	else if ( SectorLoadedEvent* SLE = dynamic_cast<SectorLoadedEvent*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			unsigned int tIndex = SLE->y * zWorld->GetNumSectorsWidth() + SLE->x;
			Vector3 pos(SLE->x * 32.0f + 16.0f, 0.0f, SLE->y * 32.0f + 16.0f);
			zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(32.0f,32.0f,32.0f), SECTOR_LENGTH);

			// Blend Maps
			const char* terrainTextures[3];
			terrainTextures[0] = "Media/TerrainTexture.png";
			terrainTextures[1] = "Media/BallTexture.png";
			terrainTextures[2] = "Media/TerrainTexture.png";
			zTerrain[tIndex]->SetTextures(terrainTextures);
			zTerrain[tIndex]->SetBlendMap( SECTOR_LENGTH, SLE->world->GetSector(SLE->x, SLE->y)->GetBlendMap() );

			// Height Map
			zTerrain[tIndex]->SetHeightMap( SLE->world->GetSector(SLE->x, SLE->y)->GetHeightMap() );

			// TODO: Remove When FPS camera is implemented, this is just for testing
			zGraphics->GetCamera()->SetPosition(pos+Vector3(10.0f,1.0f,0.0f));
		}
	}
	else if ( SectorHeightMapChanged* SHMC = dynamic_cast<SectorHeightMapChanged*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			unsigned int tIndex = SLE->y * zWorld->GetNumSectorsWidth() + SLE->x;
			zTerrain[tIndex]->SetHeightMap( SLE->world->GetSector(SLE->x, SLE->y)->GetHeightMap() );
		}
	}
	else if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		std::ifstream file;
		file.open(ELE->fileName);
		if(!file)
		{
			zEntities[ELE->entity] = GetGraphics()->CreateMesh("Media/scale.obj", ELE->entity->GetPosition());
			return;
		}
		file.close();

		zEntities[ELE->entity] = GetGraphics()->CreateMesh(ELE->fileName.c_str(), ELE->entity->GetPosition());
	}

}

CollisionData WorldRenderer::GetCollisionDataWithGround()
{
	Vector3 position = Vector3(0, 0, 0);
	int counter = 0;
	bool found = false;

	iCamera* cam = GetGraphics()->GetCamera();
	iPhysicsEngine* pe = GetGraphics()->GetPhysicsEngine();
	CollisionData cd;
	while(counter < zTerrain.size() && !found)
	{
		cd = pe->GetCollisionRayTerrain(cam->GetPosition(), cam->GetForward(), zTerrain[counter]);
		if(cd.collision)
		{
			position = Vector3(cd.posx, cd.posy, cd.posz);
			found = true;
		}
		counter++;
	}
	cam = NULL;
	pe = NULL;

	return cd;
}

float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = y/SECTOR_LENGTH * zWorld->GetNumSectorsWidth() + x/SECTOR_LENGTH;
	if(zTerrain.size() > tIndex)
	{
		return zTerrain[tIndex]->GetYPositionAt(fmod(x, SECTOR_LENGTH), fmod(y, SECTOR_LENGTH));
	}
	return std::numeric_limits<float>::infinity();
}

CollisionData WorldRenderer::Get3DRayCollisionDataWithGround()
{
	Vector3 position = Vector3(0, 0, 0);
	int counter = 0;
	bool found = false;

	iCamera* cam = GetGraphics()->GetCamera();
	iPhysicsEngine* pe = GetGraphics()->GetPhysicsEngine();
	CollisionData cd;
	while(counter < zTerrain.size() && !found)
	{
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayTerrain(cam->GetPosition(), cam->Get3DPickingRay(), zTerrain[counter]);
		if(cd.collision)
		{
			position = Vector3(cd.posx, cd.posy, cd.posz);
			found = true;
		}
		counter++;
	}
	cam = NULL;
	pe = NULL;

	return cd;
}

CollisionData WorldRenderer::Get3DRayCollisionWithMesh()
{
	int counter = 0;
	bool found = false;

	iCamera* cam = GetGraphics()->GetCamera();
	iPhysicsEngine* pe = GetGraphics()->GetPhysicsEngine();
	CollisionData cd;
	while(counter < zEntities.size() && !found)
	{
		/*cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(cam->GetPosition(), cam->Get3DPickingRay(), );
		if(cd.collision)
		{
			found = true;
		}
		counter++;*/
	}
	cam = NULL;
	pe = NULL;

	return cd;
}
