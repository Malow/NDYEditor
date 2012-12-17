#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


GameEngine::GameEngine() :
	zScreenWidth(0),
	zScreenHeight(0),
	zMode(MODE::NONE),
	zWorld(0),
	zWorldRenderer(0),
	zLockMouseToCamera(0),
	zBrushSize(3.0f),			// 3 Meters Brush By Default
	zBrushSizeExtra(0.0f),
	zDrawBrush(false),
	zMouseInsideFrame(false),
	zLeftMouseDown(false),
	zBrushStrength(1)		// 1 unit by default

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

	GetGraphics()->CreateSkyBox("Media/skymap.dds");
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->GetCamera()->SetUpdateCamera(false);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->SetSunLightProperties(Vector3(0.5f, -1.0f, 0.0f));
	GetGraphics()->SetFPSMax(30);
	GetGraphics()->StartRendering();

	srand( time(NULL) );

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
			zMoveOffSet = Vector3(cd.posx, cd.posy, cd.posz) - zPrevPosOfSelected[(*i)]; // TODO SET cd.posy TO ZERO WHEN BELOW IS FIXED
			/*float tempY = zWorldRenderer->GetYPosFromHeightMap((*i)->GetPosition().x, (*i)->GetPosition().z); REMOVED UNTILL FIXED*/
			(*i)->SetPosition(zPrevPosOfSelected[(*i)] + zMoveOffSet /*+ Vector3(0,tempY,0)*/); // TODO REMOVED UNTILL FIXED

			for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
			{
				/*tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);*/ // TODO REMOVED UNTILL FIXED
				(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet /*+ Vector3(0,tempY,0)*/); // TODO REMOVED UNTILL FIXED
			}
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
		if ( zWorldRenderer )
		{
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
	zLeftMouseDown = false;
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
					/*float tempY;*/ // TODO REMOVED UNTILL FIXED
					for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
					{
						//tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z); // TODO REMOVED UNTILL FIXED
						(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet /*+ Vector3(0, tempY, 0)*/); // TODO REMOVED UNTILL FIXED
						zPrevPosOfSelected[(*it)] = (*it)->GetPosition();
					}
				}
			}
		}
		else if(this->zMode == MODE::PLACE)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				zWorld->CreateEntity(Vector3(cd.posx, cd.posy, cd.posz),
				ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
			}
		}
		else if ( this->zMode == MODE::RAISE || this->zMode == MODE::LOWER )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				for( unsigned int x = cd.posx - zBrushSize; x < cd.posx + zBrushSize; ++x )
				{
					for( unsigned int y = cd.posz - zBrushSize; y < cd.posz + zBrushSize; ++y )
					{
						// TODO: Vector2 Instead of Vector3
						float distanceFactor = zBrushSize - Vector3(cd.posx - x, 0.0f, cd.posz - y).GetLength();
						if ( distanceFactor < 0 ) continue;
						distanceFactor /= zBrushSize;

						try 
						{
							zWorld->ModifyHeightAt(x,y,(zMode==MODE::LOWER?-0.5f:0.5f)*distanceFactor);
						}
						catch(...)
						{
						}
					}
				}
			}
			zBrushLastPos = Vector2(cd.posx, cd.posz);
			zLeftMouseDown = true;
		}
		else if(this->zMode == MODE::SELECT)
		{
			Entity* returnEntity = zWorldRenderer->Get3DRayCollisionWithMesh();
			if(returnEntity != NULL)
			{
				if(!zTargetedEntities.count(returnEntity) && GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
				{
					zTargetedEntities.insert(returnEntity);
					zPrevPosOfSelected[returnEntity] = returnEntity->GetPosition();
					returnEntity->SetSelected(true);
				}
				else if(!GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
				{
					if(!zTargetedEntities.count(returnEntity))
					{
						for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
							(*it)->SetSelected(false);
						zPrevPosOfSelected.clear();
						zTargetedEntities.clear();

						zTargetedEntities.insert(returnEntity);
						zPrevPosOfSelected[returnEntity] = returnEntity->GetPosition();
						returnEntity->SetSelected(true);
					}
					else
					{
						for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
							(*it)->SetSelected(false);
						zPrevPosOfSelected.clear();
						zTargetedEntities.clear();
					}
					
				}
				else
				{
					zTargetedEntities.erase(returnEntity);
					returnEntity->SetSelected(false);
					zPrevPosOfSelected.erase(returnEntity);
				}
			}
			else
			{
				for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
					(*it)->SetSelected(false);
				zPrevPosOfSelected.clear();
				zTargetedEntities.clear();
			}
		}
		else if(this->zMode == MODE::PLACEBRUSH)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				float length;
				float theta;
				float x;
				float z;
				int temp = zBrushSize * 1000;
				vector<Entity*> insideCicrle;
				zWorld->GetEntitiesInCircle(Vector3(cd.posx, cd.posy, cd.posz), zBrushSize, insideCicrle);
				if(insideCicrle.size() < zBrushStrength)
				{
					theta = (rand() / (float)RAND_MAX) * M_PI * 2;
					length = (rand() / (float)RAND_MAX) * zBrushSize;
					x = (cos(theta) * length);
					z = (sin(theta) * length);

					zWorld->CreateEntity(Vector3(cd.posx+x, cd.posy, cd.posz+z),
						ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
				}
				zBrushLastPos = Vector2(cd.posx, cd.posz);
				zLeftMouseDown = true;
			}

		}
	}
}


void GameEngine::CreateWorld( int x, int y )
{
	if ( zWorldRenderer ) delete zWorldRenderer, zWorldRenderer=0;
	if ( zWorld ) delete zWorld, zWorld=0;
	this->zWorld = new World(this, x, y);
	this->zWorldRenderer = new WorldRenderer(zWorld,GetGraphics());
	
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
			for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
				this->zPrevPosOfSelected[(*it)] = (*it)->GetPosition();
		}
	}
	else
	{
		if(!this->zPrevPosOfSelected.empty())
		{
			if(!this->zTargetedEntities.empty())
			{
				for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
					(*it)->SetPosition(this->zPrevPosOfSelected[(*it)]);
			}
		}
	}

	// Brush Control
	if ( zMode == MODE::RAISE || zMode == MODE::LOWER || zMode == MODE::PLACEBRUSH)
	{
		zDrawBrush = true;
	}
	else
	{
		zDrawBrush = false;
	}
}


void GameEngine::SaveWorldAs( char* msg )
{
	if ( zWorld )
		zWorld->SaveFileAs( std::string(msg) );
}


void GameEngine::OpenWorld( char* msg )
{
	if ( zWorldRenderer ) delete zWorldRenderer, zWorldRenderer = 0;
	if ( zWorld ) delete zWorld, zWorld = 0;
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
		return;
		//throw("No selected entity");

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
		return;
		//throw("No selected entity");
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


void GameEngine::SetBrushSize( float size )
{
	zBrushSize = size;
}


void GameEngine::SetBrushSizeExtra( float size )
{
	//Set Size on outer circle
}


void GameEngine::onEvent( Event* e )
{
	if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
	{
		
	}
}

void GameEngine::GetBrushSize( char* info, float& size )
{
	if(string(info) == "InnerCircle") // Returns the inner circle size
		size = zBrushSize;
	if(string(info) == "OuterCircle") // Returns the outer circle size
		size = zBrushSize + zBrushSizeExtra;
	if(string(info) == "Strength") // Returns the strength size
		size = zBrushStrength;

}


void GameEngine::RemoveSelectedEntities()
{
	for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
		zWorld->RemoveEntity((*it));
	zTargetedEntities.clear();
	zPrevPosOfSelected.clear();
}


void GameEngine::GetNrOfSelectedEntities( int& x )
{
	x = zTargetedEntities.size();
}


void GameEngine::MouseMove( int x, int y )
{
	bool checkCollision = false;

	if ( zDrawBrush ) checkCollision = true;
	if ( zMode == RAISE || zMode == LOWER || zMode == PLACEBRUSH) checkCollision = true;

	if ( checkCollision && zWorldRenderer )
	{
		CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
		if(cd.collision)
		{
			GetGraphics()->SetSpecialCircle(zBrushSize,zBrushSize+zBrushSizeExtra,Vector2(cd.posx,cd.posz));
			
			// Brush Drawing
			if ( zLeftMouseDown && Vector2(zBrushLastPos - Vector2(cd.posx, cd.posz) ).GetLength() > zBrushSize/2 )
			{
				this->OnLeftMouseDown(x,y);
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
		}
	}
}


void GameEngine::MouseInsideFrame( bool flag )
{
	zMouseInsideFrame = flag;
}

void GameEngine::SetBrushAttr( char* info, float size )
{
	if(string(info) == "InnerCircle") // sets the innercircle size
		zBrushSize = size;
	else if(string(info) == "OuterCircle") // sets the outercircle size
		zBrushSizeExtra = size;
	else if(string(info) == "Strength") // sets the strength size
		zBrushStrength = size; //TODO SET STRENGTH
}
