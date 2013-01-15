#pragma once

#include "World.h"
#include "Graphics.h"
#include <map>
#include <fstream>

enum UPDATEENUM
{
	UPDATE_NOTHING=0,
	UPDATE_HEIGHTMAP=1,
	UPDATE_BLENDMAP=2,
	UPDATE_TEXTURES=4
};

class WorldRenderer : Observer
{
	std::vector< iTerrain* > zTerrain;
	World* zWorld;
	GraphicsEngine* zGraphics;
	std::map< Entity*, iMesh* > zEntities;

	std::map< Vector2UINT, UPDATEENUM > zUpdatesRequired;

public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	CollisionData GetCollisionDataWithGround();
	CollisionData Get3DRayCollisionDataWithGround();
	Entity* Get3DRayCollisionWithMesh();
	float GetYPosFromHeightMap(float x, float y);

	void update();

	virtual void onEvent( Event* e );

protected:
	void UpdateSectorTextures( unsigned int x, unsigned int y );
	void UpdateSectorHeightMap( unsigned int x, unsigned int y );
	void UpdateSector( unsigned int x, unsigned int y );
	void UpdateSectorBlendMap( unsigned int x, unsigned int y );
};