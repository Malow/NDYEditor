#pragma once

#include "World.h"
#include "Graphics.h"
#include <iTerrain.h>


class WorldRenderer : Observer
{
	std::vector< iTerrain* > zTerrain;
	World* zWorld;
	GraphicsEngine* zGraphics;
public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

protected:
	virtual void onEvent( Event* e );
};