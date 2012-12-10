#pragma once
#include "Vector.h"


enum ENTITYTYPE
{
	TREE
};

class Entity
{
private:
	Vector3 zPosition;
	Vector3 zRotation;
	Vector3 zScale;
public:
	Entity();
	Entity(Vector3 pos);
	virtual ~Entity();

	inline void setPosition(Vector3 pos){ zPosition = pos; }
	inline Vector3 getPosition(){ return zPosition; }
	inline void setRotation(Vector3 rot){ zRotation = rot; }
	inline Vector3 getRotation(){ return zRotation; }
	inline void setScale(Vector3 scale){ zScale = scale; } 
	inline Vector3 getScale(){ return zScale; }
};