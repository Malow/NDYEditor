#pragma once

#include "Action.h"
#include "World.h"



class AIGridChangeAction : public Action
{
	World* zWorld;
	Vector2 zCenter;
	float zBrushSize;
	bool zFlag;
	std::vector< std::pair< Vector2, bool > > zChangesMade;
public:
	AIGridChangeAction( World* World, const Vector2& center, const float& size, const bool& flag ) :
		zWorld(World),
		zCenter(center),
		zBrushSize(size),
		zFlag(flag)
	{
	}

	virtual void Execute()
	{
		std::set<Vector2> nodes;
		if ( zWorld->GetAINodesInCircle(zCenter, zBrushSize, nodes) )
		{
			for( auto i = nodes.begin(); i != nodes.end(); ++i )
			{
				zChangesMade.push_back(std::pair<Vector2, bool>(zCenter, zWorld->IsBlockingAt(*i)));
				zWorld->SetBlockingAt(*i, zFlag);
			}
		}
	}


	virtual void Undo()
	{
		for( auto i = zChangesMade.rbegin(); i != zChangesMade.rend(); ++i )
		{
			zWorld->SetBlockingAt(i->first, i->second);
		}

		zChangesMade.clear();
	}
};