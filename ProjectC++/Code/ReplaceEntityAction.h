#pragma once

#include "Action.h"
#include "World.h"


class ReplaceEntityAction : public Action
{
	// Parameters
	Entity* zEntity;
	World* zWorld;
	unsigned int zNewType;

	// New Entity
	Entity* zNewEntity;

	// Old Entity 
	unsigned int zOldType;

public:
	ReplaceEntityAction( World* world, Entity* ent, unsigned int newType ) :
		zWorld(world),
		zEntity(ent),
		zNewType(newType)
	{
	}

	void Execute()
	{
		zOldType = zEntity->GetType();

		zNewEntity = zWorld->CreateEntity(zNewType);
		zNewEntity->SetPosition(zEntity->GetPosition());
		zNewEntity->SetRotation(zEntity->GetRotation());
		zNewEntity->SetScale(zEntity->GetScale());

		zWorld->RemoveEntity(zEntity);
	}

	void Undo()
	{
		zEntity = zWorld->CreateEntity(zOldType);
		zEntity->SetPosition(zNewEntity->GetPosition());
		zEntity->SetRotation(zNewEntity->GetRotation());
		zEntity->SetScale(zNewEntity->GetScale());

		zWorld->RemoveEntity(zNewEntity);
	}
};