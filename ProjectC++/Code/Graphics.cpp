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
		return 0;

	return engine;
}

bool FreeGraphics()
{
	if ( !engine )
		return false;

	delete engine;

	engine = 0;

	return true;
}