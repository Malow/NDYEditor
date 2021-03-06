#pragma once

#include <GraphicsEngine.h>
#include <windows.h>

#ifdef _DEBUG
#pragma comment(lib, "NDYGFXD.lib")
#else
#pragma comment(lib, "NDYGFX.lib")
#endif

bool InitGraphics( unsigned int hWnd );
GraphicsEngine* GetGraphics();
bool FreeGraphics();