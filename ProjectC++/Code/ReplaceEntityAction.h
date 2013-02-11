#pragma once

#include "Action.h"
#include "World.h"


class ReplaceEntityAction : public Action
{
	Entity* zEntity;
	World* zWorld;
	unsigned int zNewType;

public:
	ReplaceEntityAction( World* world, Entity* ent, unsigned int newType ) :
		zWorld(world),
		zEntity(ent),
		zNewType(newType)
	{
	}

	void Execute()
	{

	}

	void Undo()
	{
	}
};