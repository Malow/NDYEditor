#pragma once

#include "World.h"
#include "Graphics.h"
#include <map>
#include <fstream>


class WorldRenderer : Observer
{
	std::vector< iTerrain* > zTerrain;
	World* zWorld;
	GraphicsEngine* zGraphics;
	std::map< Entity*, iMesh* > zEntities;

public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	CollisionData GetCollisionDataWithGround();
	float GetYPosFromHeightMap(unsigned int x, unsigned y);

protected:
	virtual void onEvent( Event* e );
};