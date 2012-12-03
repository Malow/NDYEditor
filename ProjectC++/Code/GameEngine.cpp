#include "GameEngine.h"
#include "Graphics.h"


GameEngine::GameEngine()
{
	this->m_ScreenWidth = 0;
	this->m_ScreenHeight = 0;
	this->m_mode = MODE::NONE;
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
	GetGraphics()->GetCamera()->SetUpdateCamera(false);

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

void GameEngine::SelectMode( int mode )
{
	this->m_mode = (MODE)mode;

	if(this->m_mode == MODE::SELECT)
	{
		GetGraphics()->GetKeyListener()->SetMousePosition(
			Vector2(GetGraphics()->GetEngineParameters()->windowWidth / 2, 
			GetGraphics()->GetEngineParameters()->windowHeight / 2));

		GetGraphics()->GetCamera()->SetUpdateCamera(true);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
	if(this->m_mode == MODE::MOVE || this->m_mode == MODE::NONE || this->m_mode == MODE::ROT)
	{
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
}

void GameEngine::SaveWorld( char* msg )
{

}

void GameEngine::OpenWorld( char* msg )
{

}

void GameEngine::SetWindowFocused( bool value )
{
	GetGraphics()->GetCamera()->SetActiveWindowDisabling(value);
}
