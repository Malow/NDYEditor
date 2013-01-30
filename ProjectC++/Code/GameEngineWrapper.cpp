#include "GameEngineWrapper.h"
#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"



GameEngineWrapper::GameEngineWrapper(unsigned int hWnd) :
	m_GameEngine(0)
{
	// Clear Previous Debug
	MaloW::ClearDebug();

	// Initialize Graphics
	InitGraphics(hWnd);

	// Initialize Engine
	m_GameEngine = new GameEngine(GetGraphics());
}


GameEngineWrapper::~GameEngineWrapper()
{
	delete m_GameEngine;
	FreeGraphics();
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
	m_GameEngine->ChangeMode(mode);
}


void GameEngineWrapper::SetWindowFocused( bool value )
{
	m_GameEngine->SetWindowFocused(value);
}


void GameEngineWrapper::SaveWorld()
{
	m_GameEngine->SaveWorld();
}

void GameEngineWrapper::ChangeCameraMode( char* cameraMode )
{
	m_GameEngine->ChangeCameraMode(cameraMode);
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
	m_GameEngine->LockMouseToCamera();
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


void GameEngineWrapper::GetCameraInfo( char* info, float& x, float& y, float& z )
{
	try
	{
		m_GameEngine->GetCameraInfo(info, x, y, z);
	}
	catch(...)
	{
	}
}


void GameEngineWrapper::GetSunInfo( char* info, float& x, float& y, float& z )
{
	try
	{
		m_GameEngine->GetSunInfo(info, x, y, z);
	}
	catch(...)
	{
	}
}


void GameEngineWrapper::SetSunInfo( char* info, float x, float y, float z )
{
	m_GameEngine->SetSunInfo(info, x, y, z);
}


void GameEngineWrapper::GetAmbientLight( char* info, float& x, float& y, float& z )
{
	try
	{
		m_GameEngine->GetAmbientLight(info, x, y, z);
	}
	catch(...)
	{
	}
}


void GameEngineWrapper::SetAmbientLight( char* info, float x, float y, float z )
{
	m_GameEngine->SetAmbientLight(info, x, y, z);
}


void GameEngineWrapper::HasWorldBeenSaved( int& flag )
{
	flag = m_GameEngine->HasWorldBeenSaved();
}


void GameEngineWrapper::CountEntitiesInSector( int& num )
{
	num = m_GameEngine->CountEntitiesInSector();
}

void GameEngineWrapper::OnRightMouseDown( unsigned int x, unsigned int y )
{
	m_GameEngine->OnRightMouseDown(x,y);
}

void GameEngineWrapper::CalculateAIGrid()
{
	m_GameEngine->CalculateAIGrid();
}

void GameEngineWrapper::OnRightMouseUp( unsigned int x, unsigned int y )
{
	m_GameEngine->OnRightMouseUp(x,y);
}
float GameEngineWrapper::GetSunOnOff()
{
	return m_GameEngine->GetSunOnOff();
}

void GameEngineWrapper::SunOnOff( bool value )
{
	m_GameEngine->SunOnOff(value);
}

void GameEngineWrapper::GetWorldSize( float& x, float& y )
{
	m_GameEngine->GetWorldSize( x, y );
}

void GameEngineWrapper::TeleportTo( float x, float y, float z )
{
	m_GameEngine->TeleportTo(x, y, z);
}

void GameEngineWrapper::ToggleShadows( bool flag )
{
	m_GameEngine->ToggleShadows(flag);
}
