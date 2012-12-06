#include "WorldRenderer.h"


WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics)
{
	zWorld->addObserver(this);
	zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );
}


WorldRenderer::~WorldRenderer()
{
	zWorld->removeObserver(this);

	// Clean Terrain
	for( unsigned int x=0; x<zTerrain.size(); ++x )
	{
		delete zTerrain[x];
	}
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
	if ( SectorLoadedEvent* SLE = dynamic_cast<SectorLoadedEvent*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			unsigned int tIndex = SLE->sector.y * zWorld->GetNumSectorsWidth() + SLE->sector.x;
			Vector3 pos(SLE->sector.x * 32.0f + 16.0f, 0.0f, SLE->sector.y * 32.0f + 16.0f);
			zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(32.0f,32.0f,32.0f), 32);

			// TODO: Remove When FPS camera is implemented, this is just for testing
			zGraphics->GetCamera()->SetPosition(pos+Vector3(10.0f,1.0f,0.0f));
		}
	}
	if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		
		std::ifstream file;
		file.open(ELE->fileName);
		if(!file)
		{
			zEntities[ELE->entity] = GetGraphics()->CreateMesh("Media/scale.obj", ELE->entity->getPosition());
			return;
		}
		file.close();
		delete file;

		zEntities[ELE->entity] = GetGraphics()->CreateMesh(ELE->fileName.c_str(), ELE->entity->getPosition());
	}

}
