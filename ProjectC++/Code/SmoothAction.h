#pragma once

#include "Action.h"
#include "World/World.h"


class SmoothAction : public Action
{
	World* zWorld;
	Vector2 zCenter;
	float zBrushSize;
	float zBrushSizeExtra;
	float zBrushStrength;
	std::vector< std::pair<Vector2, float> > zPrevChanges;
public:
	SmoothAction( 
		World* world, 
		const Vector2& center, 
		const float& brushSize, 
		const float& brushSizeExtra, 
		const float& strength ) :
		zWorld(world),
		zCenter(center),
		zBrushSize(brushSize),
		zBrushSizeExtra(brushSizeExtra),
		zBrushStrength(strength)
	{
		if ( zBrushStrength > 1.0f ) zBrushStrength = 1.0f;
		if ( zBrushStrength < 0.0f ) zBrushStrength = 0.0f;
	}

	void Execute()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize+zBrushSizeExtra, nodes) )
		{
			// Target = center
			float targetHeight = zWorld->GetHeightAt( zCenter );

			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				float factor = 1.0f;
				float distance = (zCenter - *i).GetLength();
				if ( zBrushSizeExtra > 0.0f && distance >= zBrushSize )
				{
					factor = zBrushSizeExtra - ( distance - zBrushSize );
					factor /= zBrushSizeExtra;
					if ( factor <= 0.0 ) factor = 0;
				}

				float curHeight = zWorld->GetHeightAt(*i);
				float dif = targetHeight - curHeight;

				try
				{
					zPrevChanges.push_back( std::pair<Vector2, float>(*i, zWorld->GetHeightAt(*i)) );
					zWorld->SetHeightAt( i->x, i->y, curHeight + dif * zBrushStrength * factor );
				}
				catch(...)
				{
				}
			}
		}

		// Update Normals
		std::set<Vector2> nodes2;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize+zBrushSizeExtra+1.0f, nodes2) )
		{
			for ( auto i = nodes2.begin(); i != nodes2.end(); ++i )
			{
				try
				{
					zWorld->SetNormalAt(*i, zWorld->CalcNormalAt(*i));
				}
				catch(...)
				{
				}
			}
		}
	}

	void Undo()
	{
		for ( auto i = zPrevChanges.begin(); i != zPrevChanges.end(); ++i )
		{
			zWorld->SetHeightAt(i->first.x, i->first.y, i->second);
		}

		// Update Normals
		std::set<Vector2> nodes2;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize+zBrushSizeExtra+1.0f, nodes2) )
		{
			for ( auto i = nodes2.begin(); i != nodes2.end(); ++i )
			{
				try
				{
					zWorld->SetNormalAt(*i, zWorld->CalcNormalAt(*i));
				}
				catch(...)
				{
				}
			}
		}

		zPrevChanges.clear();
	}
};