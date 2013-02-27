#pragma once

#include "Observer.h"
#include <Vector.h>

class WaterQuad;


class WaterQuadDeletedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

class WaterQuadEditedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

class WaterQuad : public Observed
{
	Vector3 zPositions[4];

public:
	WaterQuad();
	virtual ~WaterQuad();

	void SetPosition( unsigned int i, const Vector3& pos );
	const Vector3& GetPosition( unsigned int i );
};