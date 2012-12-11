#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"


GameEngine::GameEngine() :
	zScreenWidth(0),
	zScreenHeight(0),
	zMode(MODE::NONE),
	zWorld(0),
	zWorldRenderer(0),
	zLockMouseToCamera(0)
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
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->GetCamera()->SetUpdateCamera(false);
	GetGraphics()->SetFPSMax(30);
	GetGraphics()->StartRendering();
	
	

	return 0;
}


void GameEngine::Shutdown()
{
	FreeGraphics();
}


void GameEngine::ProcessFrame()
{
	float dt = GetGraphics()->Update();
	GraphicsEngine* ge = GetGraphics();
	if((ge->GetCamera()->GetCameraType() == CameraType::FPS || ge->GetCamera()->GetCameraType() == CameraType::RTS) && zLockMouseToCamera)
	{
		if(ge->GetKeyListener()->IsPressed('W'))
		{
			ge->GetCamera()->MoveForward(dt);
		}
		if(ge->GetKeyListener()->IsPressed('S'))
		{
			ge->GetCamera()->MoveBackward(dt);
		}
		if(ge->GetKeyListener()->IsPressed('A'))
		{
			ge->GetCamera()->MoveLeft(dt);
		}
		if(ge->GetKeyListener()->IsPressed('D'))
		{
			ge->GetCamera()->MoveRight(dt);
		}
	}
	if(ge->GetCamera()->GetCameraType() == CameraType::RTS)
	{
		Vector3 temp = GetGraphics()->GetCamera()->GetPosition();
		try
		{
			float yPos = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z);

			if(yPos == std::numeric_limits<float>::infinity())
			{
				yPos = GetGraphics()->GetCamera()->GetPosition().y;
			}
			ge->GetCamera()->SetPosition(Vector3(temp.x, yPos, temp.z));
		}
		catch(...)
		{
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
	if(zWorld != NULL)
	{
		if(this->zMode == MODE::PLACE)
		{
			CollisionData cd = zWorldRenderer->GetCollisionDataWithGround();
			if(cd.collision)
				zWorld->CreateEntity(Vector3(cd.posx, cd.posy, cd.posz) + (GetGraphics()->GetCamera()->GetForward() * 30),
				ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
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
	
	// TODO: Fix With Anchors, Load All Sectors for now
	zWorld->LoadAllSectors();
}


void GameEngine::ChangeMode( int mode )
{
	this->zMode = (MODE)mode;
	if(this->zMode == MODE::SELECT)
	{
	}
	else if(this->zMode == MODE::MOVE || this->zMode == MODE::NONE || this->zMode == MODE::ROT || this->zMode == MODE::PLACE)
	{
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
}


void GameEngine::SaveWorldAs( char* msg )
{
	if ( zWorld )
		zWorld->SaveFileAs( std::string(msg) );
}


void GameEngine::OpenWorld( char* msg )
{
	if ( zWorldRenderer )
		delete zWorldRenderer;

	if ( zWorld )
		delete zWorld;

	zWorld = new World(this, msg);
	zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());

	// TODO: Fix With Anchors, Load All Sectors for now
	zWorld->LoadAllSectors();
}


void GameEngine::SetWindowFocused( bool value )
{
	GetGraphics()->GetCamera()->SetActiveWindowDisabling(value);
}

void GameEngine::SaveWorld()
{
	if ( !zWorld )
		throw("World Does Not Exist!");

	zWorld->SaveFile();
}

void GameEngine::SetCreateModelPath( char* filePath )
{
	zCreateModelPath = std::string(filePath);
}

void GameEngine::ChangeCameraMode( char* cameraMode )
{
	string temp = std::string(cameraMode);
	GraphicsEngine* ge = GetGraphics();

	zLockMouseToCamera = false;
	GetGraphics()->GetCamera()->SetUpdateCamera(false);

	if(temp == "FPS" && ge->GetCamera()->GetCameraType() != CameraType::FPS)
	{
		Vector3 oldPos = GetGraphics()->GetCamera()->GetPosition();
		Vector3 oldForward = GetGraphics()->GetCamera()->GetForward();
		Vector3 oldUp = GetGraphics()->GetCamera()->GetUpVector();

		ge->ChangeCamera(CameraType::FPS);

		ge->GetCamera()->SetPosition(oldPos);
		ge->GetCamera()->SetForward(oldForward);
		ge->GetCamera()->SetUpVector(oldUp);


		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
	if(temp == "RTS" && ge->GetCamera()->GetCameraType() != CameraType::RTS)
	{
		Vector3 oldForward = GetGraphics()->GetCamera()->GetForward();
		ge->GetCamera()->SetPosition(Vector3(ge->GetCamera()->GetPosition().x, 20, ge->GetCamera()->GetPosition().z));

		ge->ChangeCamera(CameraType::RTS);

		ge->GetCamera()->SetForward(oldForward);
	}
	ge = NULL;
}

void GameEngine::KeyUp( int key )
{
	GetGraphics()->GetKeyListener()->KeyUp(key);
}

void GameEngine::KeyDown( int key )
{
	GetGraphics()->GetKeyListener()->KeyDown(key);
}

void GameEngine::LockMouseToCamera()
{
	zLockMouseToCamera = !zLockMouseToCamera;
	if(zLockMouseToCamera)
	{
		if(GetGraphics()->GetCamera()->GetCameraType() == CameraType::FPS)
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(
				Vector2(GetGraphics()->GetEngineParameters()->windowWidth / 2, 
				GetGraphics()->GetEngineParameters()->windowHeight / 2));

			GetGraphics()->GetCamera()->SetUpdateCamera(true);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
		}
	}
	else
	{
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	}
}
