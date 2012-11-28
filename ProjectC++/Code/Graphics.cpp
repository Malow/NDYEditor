#include "Graphics.h"

namespace {
	static GraphicsEngine* engine = 0;
}

bool InitGraphics( unsigned int hWnd )
{
	engine = CreateGraphicsEngineInWindow(hWnd, "Config.cfg");
	return (engine != 0);
}

GraphicsEngine* GetGraphics()
{
	if ( !engine )
		throw("Engine Not Initialized!");

	return engine;
}

bool FreeGraphics()
{
	if ( !engine )
		return false;

	delete engine;

	return true;
}