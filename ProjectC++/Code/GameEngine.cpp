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

	InitGraphics(hWnd);

	zCreateModelPath = "";
	GraphicsEngine* ge = GetGraphics();
	ge->CreateSkyBox("Media/skymap.dds");
	ge->StartRendering();
	ge->GetKeyListener()->SetCursorVisibility(true);
	ge->SetFPSMax(30);
	ge->GetCamera()->SetUpdateCamera(false);
	ge->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	ge->SetSunLightProperties(Vector3(0.5f, -1.0f, 0.0f));
	ge->Update();
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
	if(this->zMode == MODE::MOVE && !this->zTargetedEntities.empty())
	{
		CollisionData cd = this->zWorldRenderer->Get3DRayCollisionDataWithGround();

		if(cd.collision)
		{
			auto i = zTargetedEntities.begin();
			(*i)->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
		}
	}


	if((ge->GetCamera()->GetCameraType() == CameraType::FPS) && zLockMouseToCamera)
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
	else if(ge->GetCamera()->GetCameraType() == CameraType::RTS)
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
		if(this->zMode == MODE::MOVE)
		{
			if(!zTargetedEntities.empty())
			{
				CollisionData cd = this->zWorldRenderer->Get3DRayCollisionDataWithGround();

				if(cd.collision)
				{
					auto i = zTargetedEntities.begin();

					(*i)->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));

					this->zPrevPosOfSelected = Vector3(-1, -1, -1);
				}
			}
		}
		else if(this->zMode == MODE::PLACE)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
				zWorld->CreateEntity(Vector3(cd.posx, cd.posy, cd.posz),
				ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
		}
		else if(this->zMode == MODE::SELECT)
		{
			Entity* returnEntity = zWorldRenderer->Get3DRayCollisionWithMesh();
			if(returnEntity != NULL)
			{
				zTargetedEntities.clear();
				zTargetedEntities.insert(returnEntity);
			}
			else
			{
				zTargetedEntities.clear();
			}
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
	if(this->zMode == MODE::MOVE)
	{
		if(!this->zTargetedEntities.empty())
		{
			auto i = zTargetedEntities.begin();
			this->zPrevPosOfSelected = (*i)->GetPosition();
		}
	}
	else
	{
		if(this->zPrevPosOfSelected.x != -1)
		{
			if(!this->zTargetedEntities.empty())
			{
				auto i = zTargetedEntities.begin();
				(*i)->SetPosition(this->zPrevPosOfSelected);
			}
		}
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

void GameEngine::GetSelectedInfo( char* info, float& x, float& y, float& z)
{
	if(zTargetedEntities.empty())
		throw("No selected entity");

	auto i = zTargetedEntities.begin();

	if(std::string(info) == "pos")
	{
		x = (*i)->GetPosition().x;
		y = (*i)->GetPosition().y;
		z = (*i)->GetPosition().z;
	}
	else if(std::string(info) == "rot")
	{
		x = (*i)->GetRotation().x;
		y = (*i)->GetRotation().y;
		z = (*i)->GetRotation().z;
	}
	else if(std::string(info) == "scale")
	{
		x = (*i)->GetScale().x;
		y = (*i)->GetScale().y;
		z = (*i)->GetScale().z;
	}
}

void GameEngine::SetSelectedObjectInfo( char* info, float& x, float& y, float& z )
{
	if(zTargetedEntities.empty())
		throw("No selected entity");
	string compareString = string(info);
	auto i = zTargetedEntities.begin();

	if(compareString == "pos")
	{
		(*i)->SetPosition(Vector3(x, y, z));
	}
	else if(compareString == "rot")
	{
		(*i)->SetRotation(Vector3(x, y, z));
	}
	else if(compareString == "scale")
	{
		(*i)->SetScale(Vector3(x, y, z));
	}
}
