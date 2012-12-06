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
	zScreenWidth = width;
	zScreenHeight = height;

	InitGraphics(hWnd);

	zCreateModelPath = "";

	GetGraphics()->CreateSkyBox("Media/skymap.dds");

	GetGraphics()->StartRendering();
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->SetFPSMax(30);
	GetGraphics()->GetCamera()->SetUpdateCamera(false);
	MaloW::ClearDebug();

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
	zScreenWidth = width;
	zScreenHeight = height;
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
	if(this->zMode == MODE::PLACETREE)
	{
		// Create a tree on the position of the raytraced position
		zWorld->CreateEntity(GetGraphics()->GetCamera()->GetPosition() + (GetGraphics()->GetCamera()->GetForward() * 30), ENTITYTYPE::TREE, zCreateModelPath);
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
	}
	if(this->zMode == MODE::MOVE || this->zMode == MODE::NONE || this->zMode == MODE::ROT || this->zMode == MODE::PLACETREE)
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
