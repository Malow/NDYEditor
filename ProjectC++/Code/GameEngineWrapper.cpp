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


unsigned int GameEngineWrapper::Init(unsigned int hWnd)	
{
	return m_GameEngine->Init(hWnd);
}


void GameEngineWrapper::Shutdown()
{
	if ( m_GameEngine ) 
	{
		delete m_GameEngine;
		m_GameEngine = 0;
	}
}


void GameEngineWrapper::ProcessFrame()
{
	return m_GameEngine->ProcessFrame();
}


void GameEngineWrapper::OnResize(int width, int height)
{
	return m_GameEngine->OnResize(width, height);
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


void GameEngineWrapper::LockMouseToCamera()
{
	this->m_GameEngine->LockMouseToCamera();
}


void GameEngineWrapper::GetSelectedInfo( char* info, float& x, float& y, float& z)
{
	try
	{
		m_GameEngine->GetSelectedInfo(info, x, y, z);
	}
	catch(...)
	{
	}
}


void GameEngineWrapper::SetSelectedObjectInfo( char* info, float x, float y, float z )
{
	m_GameEngine->SetSelectedObjectInfo(info, x, y, z);
}


void GameEngineWrapper::GetBrushAttr( char* info, float& size )
{
	m_GameEngine->GetBrushAttr(info, size);
}


void GameEngineWrapper::GetBrushAttr( char* info, char* tempString)
{
	m_GameEngine->GetBrushAttr(info, tempString);
}


void GameEngineWrapper::RemoveSelectedEntities()
{
	m_GameEngine->RemoveSelectedEntities();
}


void GameEngineWrapper::GetNrOfSelectedEntities( int& x )
{
	m_GameEngine->GetNrOfSelectedEntities(x);
}


void GameEngineWrapper::MouseMove( int x, int y )
{
	m_GameEngine->MouseMove(x,y);
}


void GameEngineWrapper::SetBrushAttr( char* info, float size )
{
	m_GameEngine->SetBrushAttr(info, size);
}


void GameEngineWrapper::SetBrushAttr( char* info, char* stringValue )
{
	m_GameEngine->SetBrushAttr(info, stringValue);
}

void GameEngineWrapper::SetEntityType( int value )
{
	m_GameEngine->SetEntityType(value);
}

