#pragma once

#include "Action.h"
#include "World/World.h"
#include "World/Entity.h"
#include <Vector.h>


class EntityPlacedAction : public Action
{
	Vector3 zPos, zRot, zScale;
	unsigned int zEntID;
	Entity* zEntCreated;
	World* zWorld;
public:
	EntityPlacedAction(
		World* world,
		const unsigned int& EntID,
		const Vector3& pos,
		const Vector3& rot,
		const Vector3& scale 
	) :
		zWorld(world),
		zEntID(EntID),
		zPos(pos),
		zRot(rot),
		zScale(scale)
	{
	}

	virtual void Execute()
	{
		zEntCreated = zWorld->CreateEntity(zEntID);
		zEntCreated->SetPosition(zPos);
		zEntCreated->SetRotation(zRot);
		zEntCreated->SetScale(zScale);
	}

	virtual void Undo()
	{
		if ( zEntCreated )
		{
			zWorld->RemoveEntity( zEntCreated );
			zEntCreated = 0;
		}
	}
};