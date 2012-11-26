#include "GameEngine.h"
#include "Graphics.h"


GameEngine::GameEngine()
{
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
}


GameEngine::~GameEngine()
{
	FreeGraphics();
}


unsigned int GameEngine::Init(unsigned int hWnd, int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;

	InitGraphics(hWnd);

	GetGraphics()->CreateSkyBox("Media/skymap.dds");

	GetGraphics()->StartRendering();

	return 0;
}


void GameEngine::Shutdown()
{
}


void GameEngine::ProcessFrame()
{
	GetGraphics()->Update();
}


void GameEngine::OnResize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}


char* GameEngine::ProcessText(char* msg)
{
	return msg;
}