#include "WaterQuad.h"


WaterQuad::WaterQuad()
{

}

WaterQuad::~WaterQuad()
{
	WaterQuadDeletedEvent WQDE;
	WQDE.zQuad = this;
	NotifyObservers(&WQDE);
}

void WaterQuad::SetPosition( unsigned int i, const Vector3& pos )
{
	zPositions[i] = pos;

	WaterQuadEditedEvent WQEE;
	WQEE.zQuad = this;
	NotifyObservers(&WQEE);
}

const Vector3& WaterQuad::GetPosition( unsigned int i )
{
	return zPositions[i];
}