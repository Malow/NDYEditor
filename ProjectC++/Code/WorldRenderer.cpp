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
			unsigned int tIndex = SLE->y * zWorld->GetNumSectorsWidth() + SLE->x;
			Vector3 pos(SLE->x * 32.0f + 16.0f, 0.0f, SLE->y * 32.0f + 16.0f);
			zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(32.0f,32.0f,32.0f), 32);

			// TODO: Remove When FPS camera is implemented, this is just for testing
			zGraphics->GetCamera()->SetPosition(pos+Vector3(10.0f,1.0f,0.0f));
		}
	}
}
