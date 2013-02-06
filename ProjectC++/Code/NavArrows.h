#pragma once

#include <Vector.h>

class iMesh;
class GraphicsEngine;

class NavArrows
{
	GraphicsEngine* zGraphics;

	iMesh* zNavArrowX;
	iMesh* zNavArrowZ;
	iMesh* zNavArrowSpawn;

	float zArrowScale;

	Vector3 zSpawnPos;

public:
	NavArrows( GraphicsEngine* gfx );
	~NavArrows();

	void Update( const Vector3& camPos, const Vector3& camDir );
	void SetSpawnPos( const Vector3& spawnPos );
	void Show( bool flag );
};