// The Action of removing an entity from world

#pragma once

#include "Action.h"
#include "World/World.h"


class EntityRemovedAction : public Action
{
	Vector3 zPos, zRot, zScale;
	unsigned int zEntID;
	Entity* zEnt;
	World* zWorld;
public:
	EntityRemovedAction(
		World* world,
		Entity* entity
		) :
		zWorld(world),
		zEnt(entity)
	{
	}

	virtual void Execute()
	{
		if ( zEnt )
		{
			zEntID = zEnt->GetType();
			zPos = zEnt->GetPosition();
			zRot = zEnt->GetRotation();
			zScale = zEnt->GetScale();

			zWorld->RemoveEntity(zEnt);
			zEnt = 0;
		}
	}

	virtual void Undo()
	{
		if ( !zEnt )
		{
			zEnt = zWorld->CreateEntity( zEntID );
			zEnt->SetPosition(zPos);
			zEnt->SetRotation(zRot);
			zEnt->SetScale(zScale);
		}
	}
};