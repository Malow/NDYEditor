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

void GameEngineWrapper::SaveWorldAs( char* msg )
{
	m_GameEngine->SaveWorldAs(msg);
}

void GameEngineWrapper::OpenWorld( char* msg )
{
	m_GameEngine->OpenWorld(msg);
}

void GameEngineWrapper::OnLeftMouseUp( unsigned int x, unsigned int y )
{
	m_GameEngine->OnLeftMouseUp(x,y);
}

void GameEngineWrapper::OnLeftMouseDown( unsigned int x, unsigned int y )
{
	m_GameEngine->OnLeftMouseDown(x,y);
}

void GameEngineWrapper::ChangeMode( int mode )
{
	this->m_GameEngine->ChangeMode(mode);
}

void GameEngineWrapper::Update()
{
	this->m_GameEngine->ProcessFrame();
}

void GameEngineWrapper::SetWindowFocused( bool value )
{
	this->m_GameEngine->SetWindowFocused(value);
}

void GameEngineWrapper::SaveWorld()
{
	this->m_GameEngine->SaveWorld();
}

void GameEngineWrapper::SetCreateModelPath( char* filePath )
{
	this->m_GameEngine->SetCreateModelPath(filePath);
}

void GameEngineWrapper::ChangeCameraMode( char* cameraMode )
{
	this->m_GameEngine->ChangeCameraMode(cameraMode);
}

void GameEngineWrapper::KeyUp( int key )
{
	m_GameEngine->KeyUp(key);
}

void GameEngineWrapper::KeyDown( int key )
{
	m_GameEngine->KeyDown(key);
}

void GameEngineWrapper::SetCameraUpdate( bool value )
{
	this->m_GameEngine->SetCameraUpdate(value);
}

