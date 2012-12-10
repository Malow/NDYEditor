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
			zEntities[ELE->entity] = GetGraphics()->CreateMesh("Media/scale.obj", ELE->entity->getPosition());
			return;
		}
		file.close();

		zEntities[ELE->entity] = GetGraphics()->CreateMesh(ELE->fileName.c_str(), ELE->entity->getPosition());
	}

}
