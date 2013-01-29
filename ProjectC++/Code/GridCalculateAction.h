#pragma once

#include "Action.h"
#include "World.h"
#include <vector>



class GridCalculateAction : public Action
{
	World* zWorld;
	Vector2UINT zSector;
	std::vector< std::pair< Vector2, bool > > zPrevBlocks;
public:

	GridCalculateAction( World *world, const Vector2UINT& sector ) :
		zWorld(world),
		zSector(sector)
	{
	}


	void Execute()
	{		
		float density = ( (float)SECTOR_WORLD_SIZE / (float)SECTOR_AI_GRID_SIZE );

		Vector2 beginCorner;
		beginCorner.x = zSector.x * SECTOR_WORLD_SIZE;
		beginCorner.y = zSector.y * SECTOR_WORLD_SIZE;

		Vector2 endCorner;
		endCorner.x = beginCorner.x + (float)(SECTOR_AI_GRID_SIZE) * density;
		endCorner.y = beginCorner.y + (float)(SECTOR_AI_GRID_SIZE) * density;

		// Record
		for( float x=beginCorner.x; x<endCorner.x; x+=density )
		{
			for( float y=beginCorner.y; y<endCorner.y; y+=density )
			{
				Vector2 centerPoint = Vector2(x,y);
				centerPoint += density * 0.5f;
				try
				{
					zPrevBlocks.push_back( std::pair<Vector2, bool>(centerPoint, zWorld->IsBlockingAt(centerPoint) ) );
				}
				catch(...)
				{

				}
			}
		}

		for( float x=beginCorner.x; x<endCorner.x; x+=density )
		{
			for( float y=beginCorner.y; y<endCorner.y; y+=density )
			{
				Vector2 centerPoint = Vector2(x,y);
				centerPoint += density * 0.5f;

				try
				{
					Vector3 normal = zWorld->CalcNormalAt(centerPoint);
					if ( normal.GetDotProduct(Vector3(0.0f, 1.0f, 0.0f)) < 0.8f )
					{
						zWorld->SetBlockingAt(centerPoint, true);
					}
					else
					{
						zWorld->SetBlockingAt(centerPoint, false);
					}
				}
				catch(...)
				{

				}
			}
		}

		std::set<Entity*> ents;
		if ( zWorld->GetEntitiesInRect(Rect(Vector2(zSector.x, zSector.y), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE)), ents) )
		{
			for( auto e = ents.cbegin(); e != ents.cend(); ++e )
			{
				std::set< Vector2 > blocks;
				if ( zWorld->GetAINodesInCircle((*e)->GetPosition().GetXZ(), GetEntBlockRadius((*e)->GetType()), blocks) )
				{
					for( auto b = blocks.begin(); b != blocks.end(); ++b )
					{
						zWorld->SetBlockingAt(*b, true);
					}
				}
			}
		}
	}

	void Undo()
	{
		if ( !zPrevBlocks.empty() )
		{
			for ( auto i = zPrevBlocks.rbegin(); i != zPrevBlocks.rend(); ++i )
			{
				zWorld->SetBlockingAt(i->first, i->second);	
			}
			zPrevBlocks.clear();
		}
	}
};