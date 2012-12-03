#include <GameEngineWrapper.h>
#include "GameEngine.h"


GameEngineWrapper::GameEngineWrapper()
{
	m_GameEngine = new GameEngine();
}


GameEngineWrapper::~GameEngineWrapper()
{
	if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
}


unsigned int GameEngineWrapper::Init(unsigned int hWnd, int width, int height)	
{
	return m_GameEngine->Init(hWnd, width, height);
}


void GameEngineWrapper::Shutdown()
{
	if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
}


void GameEngineWrapper::ProcessFrame()
{
	return m_GameEngine->ProcessFrame();
}


void GameEngineWrapper::OnResize(int width, int height)
{
	return m_GameEngine->OnResize(width, height);
}


char* GameEngineWrapper::ProcessText(char* msg)
{
	return m_GameEngine->ProcessText(msg);
}

void GameEngineWrapper::CreateWorld( int x, int y )
{
	this->m_GameEngine->CreateWorld(x, y);
}

void GameEngineWrapper::SaveWorld( char* msg )
{

}

void GameEngineWrapper::OpenWorld( char* msg )
{

}

void GameEngineWrapper::OnLeftMouseUp( unsigned int x, unsigned int y )
{
	m_GameEngine->OnLeftMouseUp(x,y);
}

void GameEngineWrapper::OnLeftMouseDown( unsigned int x, unsigned int y )
{
	m_GameEngine->OnLeftMouseDown(x,y);
}

void GameEngineWrapper::ModeSelect( int mode )
{
	this->m_GameEngine->SelectMode(mode);
}

