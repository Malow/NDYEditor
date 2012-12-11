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

