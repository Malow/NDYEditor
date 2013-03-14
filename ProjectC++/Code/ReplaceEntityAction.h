#pragma once

#include "Action.h"
#include "World/World.h"
#include "World/Entity.h"


class ReplaceEntityAction : public Action, public Observer
{
	// Parameters
	Entity* zEntity;
	World* zWorld;
	unsigned int zNewType;

	// Undo Data
	unsigned int zOldType;

public:
	ReplaceEntityAction( World* world, Entity* ent, unsigned int newType ) :
		zWorld(world),
		zEntity(ent),
		zNewType(newType)
	{
		if ( zEntity ) zEntity->AddObserver(this);
	}

	ReplaceEntityAction::~ReplaceEntityAction()
	{
		if ( zEntity ) zEntity->RemoveObserver(this);
	}

	void Execute()
	{
		if ( zEntity )
		{
			zOldType = zEntity->GetType();
			zEntity->SetType(zNewType);
		}
	}

	void Undo()
	{
		if ( zEntity ) zEntity->SetType(zOldType);
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