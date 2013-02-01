#pragma once

#include "World.h"
#include <GraphicsEngine.h>
#include <map>
#include <fstream>

enum UPDATEENUM
{
	UPDATE_NOTHING=0,
	UPDATE_HEIGHTMAP=1,
	UPDATE_BLENDMAP=2,
	UPDATE_TEXTURES=4,
	UPDATE_AIGRID=8
};

class WorldRenderer : Observer
{
	std::vector< iTerrain* > zTerrain;
	std::map< iTerrain*, std::vector<unsigned char> > zAIGrids;

	World* zWorld;
	GraphicsEngine* zGraphics;
	std::map< Entity*, iMesh* > zEntities;

	std::map< Vector2UINT, UPDATEENUM > zUpdatesRequired;
	bool zShowAIMap;

public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	CollisionData GetCollisionDataWithGround();
	CollisionData Get3DRayCollisionDataWithGround();
	Entity* Get3DRayCollisionWithMesh();
	float GetYPosFromHeightMap(float x, float y);

	void update();
	void ToggleAIGrid( bool state );

	virtual void OnEvent( Event* e );

protected:
	void UpdateSectorTextures( unsigned int x, unsigned int y );
	void UpdateSectorHeightMap( unsigned int x, unsigned int y );
	void UpdateSector( unsigned int x, unsigned int y );
	void UpdateSectorBlendMap( unsigned int x, unsigned int y );
	void UpdateSectorAIGrid( unsigned int x, unsigned int y );
};