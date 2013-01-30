#pragma once

#include "Action.h"
#include "World.h"



class BlendChangedAction : public Action
{
	World* zWorld;
	Vector2 zCenter;
	float zBrushStrength;
	float zBrushSize;
	float zBrushSizeExtra;
	unsigned int zTexBrushSelectedTex;
	std::vector< std::pair< Vector2, BlendValues > > zChangesMade;
public:
	BlendChangedAction( World* World, const Vector2& center, float brushStrength, float size, float sizeExtra, unsigned int selectedTex ) :
		zWorld(World),
		zCenter(center),
		zBrushStrength(brushStrength),
		zBrushSize(size),
		zBrushSizeExtra(sizeExtra),
		zTexBrushSelectedTex(selectedTex)
	{
	}

	virtual void Execute()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetTextureNodesInCircle(zCenter, zBrushSize+zBrushSizeExtra, nodes) )
		{
			zChangesMade.clear();
			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				float factor = 1.0f;
				float distance = (zCenter - *i).GetLength();
				if ( zBrushSizeExtra > 0.0f && distance >= zBrushSize )
				{
					factor = zBrushSizeExtra - ( distance - zBrushSize );
					factor /= zBrushSizeExtra;
					if ( factor <= 0.0f ) factor = 0.0f;
				}

				if ( factor > 0.0f )
				{
					BlendValues drawColor;
					drawColor[zTexBrushSelectedTex] = zBrushStrength * factor;

					try
					{
						zChangesMade.push_back( std::pair<Vector2,BlendValues>(*i, zWorld->GetBlendingAt(*i)) );
						zWorld->ModifyBlendingAt(*i, drawColor);
					}
					catch(...)
					{
					}
				}
			}

		}
	}


	virtual void Undo()
	{
		for( auto i = zChangesMade.rbegin(); i != zChangesMade.rend(); ++i )
		{
			zWorld->SetBlendingAt(i->first, i->second);
		}

		zChangesMade.clear();
	}
};