#pragma once

#include "Action.h"
#include "World.h"
#include "Entity.h"


class MoveEntityAction : public Action, public Observer
{
	// Parameters
	Entity* zEntity;
	World* zWorld;
	Vector3 zNewPosition;

	// Undo Data
	Vector3 zOldPosition;

public:
	MoveEntityAction( World* world, Entity* ent, const Vector3& newPosition ) :
		zWorld(world),
		zEntity(ent),
		zNewPosition(newPosition)
	{
		if ( zEntity ) zEntity->AddObserver(this);
	}

	MoveEntityAction::~MoveEntityAction()
	{
		if ( zEntity ) zEntity->RemoveObserver(this);
	}

	void Execute()
	{
		if ( zEntity )
		{
			zOldPosition = zEntity->GetPosition();
			zEntity->SetPosition(zNewPosition);
		}
	}

	void Undo()
	{
		if ( zEntity ) zEntity->SetPosition(zOldPosition);
	}

protected:
	virtual void OnEvent( Event* e )
	{
		if ( EntityDeletedEvent* ERE = dynamic_cast<EntityDeletedEvent*>(e) )
		{
			if ( ERE->entity == zEntity ) zEntity = 0;
		}
	}
};