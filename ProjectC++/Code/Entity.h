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

	inline void SetPosition(Vector3 pos){ zPosition = pos; }
	inline Vector3 GetPosition(){ return zPosition; }
	inline void SetRotation(Vector3 rot){ zRotation = rot; }
	inline Vector3 GetRotation(){ return zRotation; }
	inline void SetScale(Vector3 scale){ zScale = scale; } 
	inline Vector3 GetScale(){ return zScale; }
};