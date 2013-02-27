#pragma once

#include <Vector.h>
#include "World.h"
#include "Action.h"
#include "WorldRenderer.h"



class HeightChangedAction : public Action
{
	World* zWorld;
	Vector2 zCenter;
	float zBrushStrength;
	float zBrushSize;

public:
	HeightChangedAction( World* World, const Vector2& center, float brushStrength, float size ) :
		zWorld(World),
		zCenter(center),
		zBrushStrength(brushStrength),
		zBrushSize(size)
	{
	}


	virtual void Execute()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize, nodes) )
		{
			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				float distanceFactor = zBrushSize - (zCenter - *i).GetLength();
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

		std::set<Vector2> nodes2;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize+1, nodes2) )
		{
			for( auto i = nodes2.begin(); i != nodes2.end(); ++i )
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

		std::set<Vector2> nodes2;
		if ( zWorld->GetHeightNodesInCircle(zCenter, zBrushSize+1.0f, nodes2) )
		{
			for( auto i = nodes2.begin(); i != nodes2.end(); ++i )
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
};