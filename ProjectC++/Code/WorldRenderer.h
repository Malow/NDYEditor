#pragma once

#include "World.h"
#include <GraphicsEngine.h>
#include <map>

enum UPDATEENUM
{
	UPDATE_NOTHING=0,
	UPDATE_HEIGHTMAP=1,
	UPDATE_BLENDMAP=2,
	UPDATE_TEXTURES=4,
	UPDATE_AIGRID=8,
	UPDATE_ALL=15
};

struct WaterCollisionData
{
	// Specifies the quad hit
	WaterQuad* quad;
	
	// Specifies the corner hit
	unsigned int cornerIndex;

	// Position of collision
	Vector3 position;
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
	
	std::map< WaterQuad*, iWaterPlane* > zWaterQuads;

	struct WaterCubes
	{
		iMesh* zCubes[4];
	};

	std::map< WaterQuad*, WaterCubes > zWaterBoxes;
	bool zShowWaterBoxes;

public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	WaterCollisionData GetCollisionWithWaterBoxes();
	CollisionData Get3DRayCollisionDataWithGround();
	Entity* Get3DRayCollisionWithMesh();
	float GetYPosFromHeightMap(float x, float y);

	void Update();
	void ToggleAIGrid(bool state);
	void ToggleWaterBoxes(bool flag);

	virtual void OnEvent( Event* e );

protected:
	void UpdateSectorTextures( const Vector2UINT& sectorCoords );
	void UpdateSectorHeightMap( const Vector2UINT& sectorCoords );
	void UpdateSector( const Vector2UINT& sectorCoords );
	void UpdateSectorBlendMap( const Vector2UINT& sectorCoords );
	void UpdateSectorAIGrid( const Vector2UINT& sectorCoords );
	void CreateTerrain( const Vector2UINT& sectorCoords );
	iTerrain* GetTerrain( const Vector2UINT& sectorCoords );

	void UpdateWaterBoxes( WaterQuad* water );

	void CreateEntity( Entity* e );
	void SetEntityGraphics( Entity* e );
	void SetEntityTransformation( Entity* e );
	void DeleteEntity( Entity* e );
};