#include "Entity.h"



Entity::Entity( unsigned int type, const Vector3& pos, const Vector3& rot, const Vector3& scale ) :
	zType(type),
	zPosition(pos),
	zRotation(rot),
	zScale(scale),
	zSelected(false)
{
}

Entity::~Entity()
{
}

void Entity::SetPosition( const Vector3& pos )
{
	SetEdited(true);
	zPosition = pos;
	NotifyObservers( &EntityUpdatedEvent(this) );
}

void Entity::SetRotation( const Vector3& rot )
{
	SetEdited(true);
	zRotation = rot;
	NotifyObservers(&EntityUpdatedEvent(this));
}

void Entity::SetScale( const Vector3& scale )
{
	SetEdited(true);
	zScale = scale;
	NotifyObservers(&EntityUpdatedEvent(this));
}

void Entity::SetSelected( bool selected )
{
	zSelected = selected;
	NotifyObservers(&EntityUpdatedEvent(this));
}

