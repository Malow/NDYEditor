#include "GameEngine.h"
#include "Graphics.h"


GameEngine::GameEngine()
{
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
}


GameEngine::~GameEngine()
{
}


unsigned int GameEngine::Init(unsigned int hWnd, int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;

	InitGraphics(hWnd);

	GetGraphics()->CreateSkyBox("Media/skymap.dds");

	GetGraphics()->StartRendering();
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->SetFPSMax(30);

	return 0;
}


void GameEngine::Shutdown()
{
	FreeGraphics();
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

void GameEngine::OnLeftMouseUp( unsigned int x, unsigned int y )
{

}

void GameEngine::OnLeftMouseDown( unsigned int x, unsigned int y )
{

}

void GameEngine::CreateWorld( int x, int y )
{
	this->m_World = new World(this, x, y);
}
