#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"

GameEngine::GameEngine() :
	zScreenWidth(0),
	zScreenHeight(0),
	zMode(MODE::NONE),
	zWorld(0),
	zWorldRenderer(0)
{
}


GameEngine::~GameEngine()
{
	if ( zWorldRenderer ) delete zWorldRenderer;
	if ( zWorld ) delete zWorld;
}


unsigned int GameEngine::Init(unsigned int hWnd, int width, int height)
{
	MaloW::ClearDebug();

	zScreenWidth = width;
	zScreenHeight = height;

	/*GetGraphics()->GetEngineParameters()->windowWidth = zScreenWidth;
	GetGraphics()->GetEngineParameters()->windowHeight = zScreenHeight;
	GetGraphics()->GetEngineParameters()->SaveToFile("config.cfg");*/

	InitGraphics(hWnd);

	zCreateModelPath = "";

	GetGraphics()->CreateSkyBox("Media/skymap.dds");

	GetGraphics()->StartRendering();
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->SetFPSMax(30);
	GetGraphics()->GetCamera()->SetUpdateCamera(false);

	zCameraType = CameraType::FPS;

	return 0;
}


void GameEngine::Shutdown()
{
	FreeGraphics();
}


void GameEngine::ProcessFrame()
{
	float dt = GetGraphics()->Update();
	if(this->zCameraType == CameraType::FPS)
	{
		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
		{
			GetGraphics()->GetCamera()->MoveForward(dt);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))
		{
			GetGraphics()->GetCamera()->MoveBackward(dt);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
		{
			GetGraphics()->GetCamera()->MoveLeft(dt);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))
		{
			GetGraphics()->GetCamera()->MoveRight(dt);
		}
	}
}


void GameEngine::OnResize(int width, int height)
{
	zScreenWidth = width;
	zScreenHeight = height;
	GetGraphics()->ResizeGraphicsEngine(zScreenWidth, zScreenHeight);
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
	int i = 0;
	if(zWorld != NULL)
	{
		if(this->zMode == MODE::PLACE)
		{
			// Create a tree on the position of the raytraced position
			zWorld->CreateEntity(GetGraphics()->GetCamera()->GetPosition() + (GetGraphics()->GetCamera()->GetForward() * 30), ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
		}
	}
}


void GameEngine::CreateWorld( int x, int y )
{
	if ( zWorldRenderer )
		delete zWorldRenderer;

	if ( zWorld )
		delete zWorld;

	this->zWorld = new World(this, x, y);
	zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
	
	// Load All Sectors
	zWorld->LoadAllSectors();
}


void GameEngine::ChangeMode( int mode )
{
	this->zMode = (MODE)mode;
	if(this->zMode == MODE::SELECT)
	{
		GetGraphics()->GetKeyListener()->SetMousePosition(
			Vector2(GetGraphics()->GetEngineParameters()->windowWidth / 2, 
			GetGraphics()->GetEngineParameters()->windowHeight / 2));

		GetGraphics()->GetCamera()->SetUpdateCamera(true);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
		GetGraphics()->GetCamera()->SetActiveWindowDisabling(true);
	}
	if(this->zMode == MODE::MOVE || this->zMode == MODE::NONE || this->zMode == MODE::ROT || this->zMode == MODE::PLACE)
	{
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
}


void GameEngine::SaveWorldAs( char* msg )
{
	if ( zWorld )
		zWorld->SaveFile();
}


void GameEngine::OpenWorld( char* msg )
{
	if ( zWorldRenderer )
		delete zWorldRenderer;

	if ( zWorld )
		delete zWorld;

	zWorld = new World(this, msg);
	zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
}


void GameEngine::SetWindowFocused( bool value )
{
	GetGraphics()->GetCamera()->SetActiveWindowDisabling(value);
}

void GameEngine::SaveWorld( char* msg )
{
	//TODO
	// Save World from "Save" command and not "Save As"
}

void GameEngine::SetCreateModelPath( char* filePath )
{
	zCreateModelPath = std::string(filePath);
}

void GameEngine::ChangeCameraMode( char* cameraMode )
{
	string temp = std::string(cameraMode);
	if(temp == "FPS")
	{
		GetGraphics()->ChangeCamera(CameraType::FPS);
		zCameraType = CameraType::FPS;
	}
	if(temp == "RTS")
	{
		GetGraphics()->ChangeCamera(CameraType::RTS);
		zCameraType = CameraType::RTS;
	}
}

void GameEngine::KeyUp( int key )
{
	GetGraphics()->GetKeyListener()->KeyUp(key);
}

void GameEngine::KeyDown( int key )
{
	GetGraphics()->GetKeyListener()->KeyDown(key);
}

void GameEngine::SetCameraUpdate( bool value )
{
	GetGraphics()->GetCamera()->SetUpdateCamera(value);
}
