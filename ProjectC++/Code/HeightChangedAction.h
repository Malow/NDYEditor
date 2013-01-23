#pragma once

#include <Vector.h>
#include "World.h"
#include "Action.h"
#include "WorldRenderer.h"



class HeightChangedAction : public Action
{
	WorldRenderer* zRenderer;
	World* zWorld;
	Vector2 zCenter;
	float zBrushStrength;
	float zBrushSize;

public:
	HeightChangedAction( WorldRenderer* renderer, World* World, const Vector2& center, float brushStrength, float size ) :
		zRenderer(renderer),
		zWorld(World),
		zCenter(center),
		zBrushStrength(brushStrength),
		zBrushSize(size)
	{
	}


	virtual void Execute()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetHeightNodesInCircle(Vector2(zCenter.x, zCenter.y), zBrushSize, nodes) )
		{
			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				float distanceFactor = zBrushSize - Vector2(zCenter.x - i->x, zCenter.y - i->y).GetLength();
				if ( distanceFactor <= 0 ) continue;
				distanceFactor /= zBrushSize;

				try 
				{
					zWorld->ModifyHeightAt(i->x, i->y, zBrushStrength * distanceFactor);
				}
				catch(...)
				{
				}
			}
		}
	}


	virtual void Undo()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetHeightNodesInCircle(Vector2(zCenter.x, zCenter.y), zBrushSize, nodes) )
		{
			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				float distanceFactor = zBrushSize - Vector2(zCenter.x - i->x, zCenter.y - i->y).GetLength();
				if ( distanceFactor <= 0 ) continue;
				distanceFactor /= zBrushSize;

				try 
				{
					zWorld->ModifyHeightAt(i->x, i->y, -zBrushStrength * distanceFactor);
				}
				catch(...)
				{
				}
			}
		}
	}
};