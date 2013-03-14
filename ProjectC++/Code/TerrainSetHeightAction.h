#pragma once

#include "World/World.h"
#include "Action.h"
#include <vector>


class TerrainSetHeightAction : public Action
{
	World* zWorld;
	Vector2 zPos;
	float zRadius;
	float zHeight;

	std::vector< std::pair<Vector2, float> > zPrevHeights;
public:
	TerrainSetHeightAction( 
		World *world, 
		const Vector2& pos,
		float radius,
		float height
		) :
		zWorld(world),
		zPos(pos),
		zRadius(radius),
		zHeight(height)
	{
	}


	virtual void Execute()
	{
		std::set< Vector2 > nodes;
		zWorld->GetHeightNodesInCircle( zPos, zRadius, nodes );

		for( auto i = nodes.begin(); i != nodes.end(); ++i )
		{
			zPrevHeights.push_back(std::pair<Vector2,float>(*i, zWorld->GetHeightAt(*i)));
			zWorld->SetHeightAt( i->x, i->y, zHeight );
		}

		for( auto i = zPrevHeights.begin(); i != zPrevHeights.end(); ++i )
		{
			try
			{
				zWorld->SetNormalAt(i->first, zWorld->CalcNormalAt(i->first));
			}
			catch(...)
			{
			}
		}
	}


	virtual void Undo()
	{
		for( auto i = zPrevHeights.begin(); i != zPrevHeights.end(); ++i )
		{
			zWorld->SetHeightAt( i->first.x, i->first.y, i->second );
		}

		for( auto i = zPrevHeights.begin(); i != zPrevHeights.end(); ++i )
		{
			try
			{
				zWorld->SetNormalAt(i->first, zWorld->CalcNormalAt(i->first));
			}
			catch(...)
			{
			}
		}

		zPrevHeights.clear();
	}
};