#pragma once
#include "Vector.h"
#include "Observer.h"

enum ENTITYTYPE
{
	TREE
};

class Entity : public Observed
{
private:
	Vector3 zPosition;
	Vector3 zRotation;
	Vector3 zScale;
public:
	Entity();
	Entity(Vector3 pos);
	virtual ~Entity();

	void SetPosition(Vector3 pos);
	inline Vector3 GetPosition(){ return zPosition; }
	void SetRotation(Vector3 rot);
	inline Vector3 GetRotation(){ return zRotation; }
	void SetScale(Vector3 scale);
	inline Vector3 GetScale(){ return zScale; }
};


class EntityUpdatedEvent : public Event
{
public:
	Entity* entity;
	EntityUpdatedEvent(Entity* entity) : entity(entity){}
};