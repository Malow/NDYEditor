#include "Entity.h"
Entity::Entity()
{
	zPosition = Vector3(0, 0, 0);
	zRotation = Vector3(0, 0, 0);
	zScale = Vector3(0.05f, 0.05f, 0.05f);
}

Entity::Entity( Vector3 pos )
{
	zPosition = pos;
	zRotation = Vector3(0, 0, 0);
	zScale = Vector3(0.05f, 0.05f, 0.05f);
}

Entity::~Entity()
{

}

void Entity::SetPosition( Vector3 pos )
{
	zPosition = pos;
	NotifyObservers( &EntityUpdatedEvent(this) );
}

void Entity::SetRotation( Vector3 rot )
{
	zRotation = rot;
	NotifyObservers(&EntityUpdatedEvent(this));
}

void Entity::SetScale( Vector3 scale )
{
	zScale = scale;
	NotifyObservers(&EntityUpdatedEvent(this));
}

