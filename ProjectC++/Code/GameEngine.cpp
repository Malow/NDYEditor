#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"
#include "EntityList.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


GameEngine::GameEngine() :
	zMode(MODE::NONE),
	zWorld(0),
	zWorldRenderer(0),
	zLockMouseToCamera(0),
	zBrushSize(1.0f),			// 3 Meters Brush By Default
	zBrushSizeExtra(0.0f),
	zDrawBrush(false),
	zLeftMouseDown(false),
	zBrushStrength(1),			// 1 unit by default
	zTexBrushSelectedTex(0),
	zAnchor(0),
	zBrushLastPos(0.0f,0.0f),
	zMouseMoved(false),
	zCreateEntityType(0),
	zFPSLockToGround(false),
	zMovementMulti(1),
	zMaxSpeed(32),
	zRTSHeightFromGround(20),
	zWorldSavedFlag(true)
{
}


GameEngine::~GameEngine()
{
	if ( zWorld ) delete zWorld;
	FreeGraphics();
}


unsigned int GameEngine::Init(unsigned int hWnd)
{
	MaloW::ClearDebug();

	InitGraphics(hWnd);
	GetGraphics()->GetCamera()->SetUpdateCamera(false);
	GetGraphics()->CreateSkyBox("Media/skymap.dds");
	GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->SetFPSMax(60);
	GetGraphics()->StartRendering();

	// Entities
	LoadEntList("Entities.txt");

	// Start Camera
	ChangeCameraMode("FPS");
	LockMouseToCamera();

	// Random
	srand( (unsigned int)time(NULL) );

	return 0;
}


void GameEngine::ProcessFrame()
{
	GraphicsEngine* ge;
	ge = GetGraphics();
	if ( !ge )
		return;

	iCamera *camera = ge->GetCamera();
	float dt = GetGraphics()->Update();

	if ( zWorld ) 
	{
		if (zAnchor)
		{
			try
			{
				zAnchor->position = camera->GetPosition().GetXZ();
			}
			catch(...)
			{
			}
			zAnchor->radius = GetGraphics()->GetEngineParameters()->FarClip;
		}

		zWorld->Update();

		if ( zWorldRenderer )
		{
			zWorldRenderer->update();
		}

		GetGraphics()->SetSceneAmbientLight(zWorld->GetAmbientAtWorldPos(camera->GetPosition().GetXZ()));
	}
	

	if(this->zMode == MODE::MOVE && !this->zTargetedEntities.empty())
	{
		CollisionData cd = this->zWorldRenderer->Get3DRayCollisionDataWithGround();
		
		if(cd.collision)
		{
			auto i = zTargetedEntities.begin();
			zMoveOffSet = Vector3(cd.posx, 0.0f, cd.posz) - zPrevPosOfSelected[(*i)];
			float tempY = zWorldRenderer->GetYPosFromHeightMap((*i)->GetPosition().x, (*i)->GetPosition().z);
			(*i)->SetPosition(zPrevPosOfSelected[(*i)] + zMoveOffSet + Vector3(0,tempY,0));

			for ( auto it=zTargetedEntities.begin(); it != zTargetedEntities.end(); it++ )
			{
				tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);
				(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet + Vector3(0,tempY,0));
			}

			zWorldSavedFlag = false;
		}
	}

	if( ge->GetCamera()->GetCameraType() == CameraType::FPS )
	{
		if ( zLockMouseToCamera )
		{
			if(ge->GetKeyListener()->IsPressed('W'))
			{
				ge->GetCamera()->MoveForward(dt * zMovementMulti);
			}
			if(ge->GetKeyListener()->IsPressed('S'))
			{
				ge->GetCamera()->MoveBackward(dt * zMovementMulti);
			}
			if(ge->GetKeyListener()->IsPressed('A'))
			{
				ge->GetCamera()->MoveLeft(dt * zMovementMulti);
			}
			if(ge->GetKeyListener()->IsPressed('D'))
			{
				ge->GetCamera()->MoveRight(dt * zMovementMulti);
			}
		}

		if ( zFPSLockToGround && zWorldRenderer )
		{
			try
			{
				Vector3 temp = GetGraphics()->GetCamera()->GetPosition();
				if ( temp.x > 0.0f && temp.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
				{
					if ( temp.y > 0.0f && temp.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
					{
						temp.y = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z) + 1.8f;
						ge->GetCamera()->SetPosition(temp);
					}
				}
			}
			catch(...)
			{
			}
		}
	}
	else if(ge->GetCamera()->GetCameraType() == CameraType::RTS)
	{
		if(ge->GetKeyListener()->IsPressed('W'))
		{
			ge->GetCamera()->MoveForward(dt * zMovementMulti);
		}
		if(ge->GetKeyListener()->IsPressed('S'))
		{
			ge->GetCamera()->MoveBackward(dt * zMovementMulti);
		}
		if(ge->GetKeyListener()->IsPressed('A'))
		{
			ge->GetCamera()->MoveLeft(dt * zMovementMulti);
		}
		if(ge->GetKeyListener()->IsPressed('D'))
		{
			ge->GetCamera()->MoveRight(dt * zMovementMulti);
		}

		if ( zWorldRenderer )
		{
			try
			{
				Vector3 temp = GetGraphics()->GetCamera()->GetPosition();
				if ( temp.x > 0.0f && temp.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
				{
					if ( temp.y > 0.0f && temp.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
					{
						temp.y = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z) + zRTSHeightFromGround;
						ge->GetCamera()->SetPosition(temp);
					}
				}
			}
			catch(...)
			{
			}
		}
	}

	if ( zWorldRenderer && zDrawBrush )
	{
		if ( zMouseMoved )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				GetGraphics()->SetSpecialCircle(zBrushSize,zBrushSize+zBrushSizeExtra,Vector2(cd.posx,cd.posz));

				// Brush Drawing
				if ( zLeftMouseDown && Vector2(zBrushLastPos - Vector2(cd.posx, cd.posz) ).GetLength() > zBrushSize/2.0f )
				{
					this->OnLeftMouseDown(0,0);
					zBrushLastPos = Vector2(cd.posx, cd.posz);
				}
			}
			else
			{
				GetGraphics()->SetSpecialCircle(-1,0,Vector2(0,0));
			}
			zMouseMoved = false;
		}
	}
	else
	{
		GetGraphics()->SetSpecialCircle(-1,0,Vector2(0,0));
	}
}


void GameEngine::OnResize(int width, int height)
{
	GetGraphics()->ResizeGraphicsEngine(width, height);
}


void GameEngine::OnLeftMouseUp( unsigned int, unsigned int )
{
	zLeftMouseDown = false;
}


void GameEngine::OnLeftMouseDown( unsigned int, unsigned int )
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
					float tempY;
					for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
					{
						tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);
						(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet + Vector3(0, tempY, 0));
						zPrevPosOfSelected[(*it)] = (*it)->GetPosition();
					}
					zWorldSavedFlag = false;
				}
			}
		}
		else if(this->zMode == MODE::PLACE)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				Entity* ent = zWorld->CreateEntity(this->zCreateEntityType);
				ent->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
				zWorldSavedFlag = false;
			}
		}
		else if ( this->zMode == MODE::RAISE || this->zMode == MODE::LOWER )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				std::set<Vector2> nodes;
				if ( zWorld->GetHeightNodesInCircle(Vector2(cd.posx,cd.posz), zBrushSize, nodes) )
				{
					for( auto i = nodes.begin(); i != nodes.end(); ++i )
					{
						float distanceFactor = zBrushSize - Vector2(cd.posx - i->x, cd.posz - i->y).GetLength();
						if ( distanceFactor <= 0 ) continue;
						distanceFactor /= zBrushSize;

						try 
						{
							zWorld->ModifyHeightAt(i->x, i->y, (zMode == MODE::LOWER? -zBrushStrength : zBrushStrength) * distanceFactor);
						}
						catch(...)
						{
						}
					}
				}
				zWorldSavedFlag = false;
			}
			zBrushLastPos = Vector2(cd.posx, cd.posz);
			zLeftMouseDown = true;
		}
		else if ( zMode == MODE::DRAWTEX )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				std::set<Vector2> nodes;
				if ( zWorld->GetTextureNodesInCircle(Vector2(cd.posx,cd.posz), zBrushSize+zBrushSizeExtra, nodes) )
				{
					for( auto i = nodes.begin(); i != nodes.end(); ++i )
					{
						float factor = 1.0f;
						float distance = Vector2(cd.posx - i->x, cd.posz - i->y).GetLength();
						if ( zBrushSizeExtra > 0.0f && distance >= zBrushSize )
						{
							factor = zBrushSizeExtra - ( distance - zBrushSize );
							factor /= zBrushSizeExtra;
							if ( factor <= 0.0 ) factor = 0;
						}

						Vector4 drawColor(0.0f,0.0f,0.0f,0.0f);
						drawColor[zTexBrushSelectedTex] = zBrushStrength * factor;

						try 
						{
							zWorld->ModifyBlendingAt(i->x,i->y,drawColor);
						}
						catch(...)
						{
						}
					}
				}
				zWorldSavedFlag = false;
			}
			zBrushLastPos = Vector2(cd.posx, cd.posz);
			zLeftMouseDown = true;
		}
		else if(this->zMode == MODE::SELECT)
		{
			Entity* returnEntity = zWorldRenderer->Get3DRayCollisionWithMesh();
			if( returnEntity != NULL )
			{
				// Add To Selection
				if(!zTargetedEntities.count(returnEntity) && GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
				{
					zTargetedEntities.insert(returnEntity);
					zPrevPosOfSelected[returnEntity] = returnEntity->GetPosition();
					returnEntity->SetSelected(true);
				}
				else if(!GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))	// Set Selection
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
				else  // Unselect
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
		else if ( zMode == MODE::PLACEBRUSH )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if( cd.collision )
			{
				double length;
				double theta;
				double x;
				double z;
				vector<Entity*> insideCircle;
				if( zWorld->GetEntitiesInCircle(Vector2(cd.posx, cd.posz), zBrushSize, insideCircle) < zBrushStrength )
				{
					theta = ((double)rand() / (double)RAND_MAX) * M_PI * 2.0;
					length = ((double)rand() / (double)RAND_MAX) * zBrushSize;
					x = cd.posx + (cos(theta) * length);
					z = cd.posz + (sin(theta) * length);
					if ( x > 0.0 && z > 0.0 )
					{
						Vector2UINT sp = zWorld->WorldPosToSector(Vector2(x,z));
						if ( zWorld->IsSectorLoaded(sp.x, sp.y) )
						{
							Entity* ent = zWorld->CreateEntity(this->zCreateEntityType);
							ent->SetPosition( Vector3(x, zWorldRenderer->GetYPosFromHeightMap(x, z), z) );
						}
					}		
				}
				zBrushLastPos = Vector2(cd.posx, cd.posz);
				zLeftMouseDown = true;
				zWorldSavedFlag = false;
			}
		}
		else if ( zMode == MODE::SMOOTH )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if( cd.collision )
			{
				std::set<Vector2> nodes;
				if ( zWorld->GetHeightNodesInCircle(Vector2(cd.posx,cd.posz), zBrushSize+zBrushSizeExtra, nodes) )
				{
					// Target = center
					float targetHeight = zWorld->GetHeightAtWorldPos( cd.posx, cd.posz );

					for( auto i = nodes.begin(); i != nodes.end(); ++i )
					{
						float factor = 1.0f;
						float distance = Vector2(cd.posx - i->x, cd.posz - i->y).GetLength();
						if ( zBrushSizeExtra > 0.0f && distance >= zBrushSize )
						{
							factor = zBrushSizeExtra - ( distance - zBrushSize );
							factor /= zBrushSizeExtra;
							if ( factor <= 0.0 ) factor = 0;
						}

						float curHeight = zWorld->GetHeightAt( i->x, i->y );
						float dif = targetHeight - curHeight;

						try
						{
							zWorld->SetHeightAt( i->x, i->y, curHeight + dif * max(min(zBrushStrength,1.0f),0.0f) * factor );
						}
						catch(...)
						{
						}
					}
				}
				zWorldSavedFlag = false;
			}
			zBrushLastPos = Vector2(cd.posx, cd.posz);
			zLeftMouseDown = true;
		}
	}
}


void GameEngine::CreateWorld( int width, int height )
{
	if ( zWorld ) delete zWorld, zWorld=0;
	this->zWorld = new World(this, width, height);
	this->zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
	zWorldSavedFlag = false;
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
	if ( zMode == MODE::RAISE || zMode == MODE::LOWER || zMode == MODE::PLACEBRUSH || zMode == MODE::DRAWTEX || zMode == MODE::SMOOTH )
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
	{
		zWorld->SaveFileAs( std::string(msg) );
		zWorldSavedFlag = true;
	}
}


void GameEngine::OpenWorld( char* msg )
{
	if ( zWorld ) delete zWorld, zWorld = 0;
	zWorld = new World(this, msg);
	zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
	zWorldSavedFlag = true;
}


void GameEngine::SetWindowFocused( bool flag )
{
	if ( GetGraphics() )
		GetGraphics()->GetCamera()->SetActiveWindowDisabling(flag);
}


void GameEngine::SaveWorld()
{
	if ( zWorld )
	{
		zWorld->SetStartCamera( GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward() );
		zWorld->SaveFile();
		zWorldSavedFlag = true;
	}
}


void GameEngine::ChangeCameraMode( char* cameraMode )
{
	string temp = std::string(cameraMode);
	GraphicsEngine* ge = GetGraphics();

	if( temp == "FPS" )
	{
		Vector3 oldPos = GetGraphics()->GetCamera()->GetPosition();
		Vector3 oldForward = GetGraphics()->GetCamera()->GetForward();
		Vector3 oldUp = GetGraphics()->GetCamera()->GetUpVector();

		ge->ChangeCamera(CameraType::FPS);

		ge->GetCamera()->SetPosition(oldPos);
		ge->GetCamera()->SetForward(oldForward);
		ge->GetCamera()->SetUpVector(oldUp);

		zLockMouseToCamera = true;

		GetGraphics()->GetCamera()->SetUpdateCamera(true);
		GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
	}

	if( temp == "RTS" )
	{
		Vector3 oldForward = GetGraphics()->GetCamera()->GetForward();
		Vector3 camPos = ge->GetCamera()->GetPosition();

		if ( camPos.x > 0.0f && camPos.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
		{
			if ( camPos.y > 0.0f && camPos.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
			{
				float yPos = this->zWorldRenderer->GetYPosFromHeightMap(camPos.x, camPos.z);
				this->zRTSHeightFromGround = camPos.y - yPos;
			}
		}
		
		ge->ChangeCamera(CameraType::RTS);
		ge->GetCamera()->SetForward(oldForward);
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
	}
}


void GameEngine::KeyUp( int key )
{
	GetGraphics()->GetKeyListener()->KeyUp(key);
	if ( key == 32 )
	{
		zFPSLockToGround = !zFPSLockToGround;
	}
	else if ( key == 96 )	// keypad 0
	{
		zMovementMulti = 1.0;
	}
	else if( key == VK_SUBTRACT)
	{
		if (zMovementMulti / 2.0f)
			zMovementMulti /= 2.0f;
	}
	else if( key == VK_ADD)
	{
		if((zMovementMulti * 2.0f) < zMaxSpeed)
			zMovementMulti *= 2.0f;
	}
	else if( key == 220)
	{
		this->MoveObjectToSurface();
	}
	else if( key == (int)'C')
	{
		this->ChangeCameraMode("FPS");
	}
	else if( key == (int)'V')
	{
		this->ChangeCameraMode("RTS");
	}
}


void GameEngine::KeyDown( int key )
{
	GetGraphics()->GetKeyListener()->KeyDown(key);
}


void GameEngine::LockMouseToCamera()
{
	if( GetGraphics()->GetCamera()->GetCameraType() == CameraType::FPS )
	{
		zLockMouseToCamera = !zLockMouseToCamera;
		if ( zLockMouseToCamera )
		{
			GetGraphics()->GetCamera()->SetUpdateCamera(true);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
		}
		else
		{
			GetGraphics()->GetCamera()->SetUpdateCamera(false);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
		}
	}
}


void GameEngine::GetSelectedInfo( char* info, float& x, float& y, float& z)
{
	if(zTargetedEntities.empty())
	{
		x = 0;
		y = 0;
		z = 0;
		return;
	}

	auto i = zTargetedEntities.begin();

	if( std::string(info) == "pos" )
	{
		x = (*i)->GetPosition().x;
		y = (*i)->GetPosition().y;
		z = (*i)->GetPosition().z;
	}
	else if( std::string(info) == "rot" )
	{
		x = (*i)->GetRotation().x;
		y = (*i)->GetRotation().y;
		z = (*i)->GetRotation().z;
	}
	else if( std::string(info) == "scale" )
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

	string compareString(info);
	auto i = zTargetedEntities.begin();

	if(compareString == "pos")
	{
		(*i)->SetPosition(Vector3(x, y, z));
		zPrevPosOfSelected[(*i)] = Vector3(x, y, z);
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


void GameEngine::onEvent( Event* e )
{
	if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
	{
		// Create Anchor
		zAnchor = WLE->world->CreateAnchor();
		GetGraphics()->GetCamera()->SetPosition( WLE->world->GetStartCamPos() );
		GetGraphics()->GetCamera()->SetForward( WLE->world->GetStartCamRot() );
	}
	else if ( WorldDeletedEvent* WDE = dynamic_cast<WorldDeletedEvent*>(e) )
	{
		if ( zWorldRenderer ) delete zWorldRenderer, zWorldRenderer = 0;
		if ( zAnchor ) zWorld->DeleteAnchor( zAnchor );
		if ( zWorld ) zWorld = 0;
	}
	else if ( EntityRemovedEvent *ERE = dynamic_cast<EntityRemovedEvent*>(e) )
	{
		zTargetedEntities.erase(ERE->entity);
	}
}


void GameEngine::GetBrushAttr( char* info, float& size )
{
	if(string(info) == "InnerCircle") // Returns the inner circle size
		size = zBrushSize;
	if(string(info) == "OuterCircle") // Returns the outer circle size
		size = zBrushSizeExtra;
	if(string(info) == "Strength") // Returns the strength size
		size = zBrushStrength;
}


void GameEngine::GetBrushAttr( char* info, char* SChar )
{
	std::string tempString = "";
	if( strlen(info) == 4 && info[0] == 'T' && info[1] == 'e' && info[2] == 'x')
	{
		CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
		if(cd.collision)
		{
			Vector2 worldPos( cd.posx, cd.posz );
			zSelectedSectorX = zWorld->WorldPosToSector( worldPos ).x;
			zSelectedSectorY = zWorld->WorldPosToSector( worldPos ).y;

			unsigned int texID = info[3] - 48;
			tempString = zWorld->GetSectorTexture(zSelectedSectorX, zSelectedSectorY, texID);
		}
	}

	// Fill Output String
	for(unsigned int i = 0; i < tempString.length(); i++)
		SChar[i] = tempString[i];

	// Null Terminate
	SChar[tempString.length()] = 0;
}


void GameEngine::RemoveSelectedEntities()
{
	while(!zTargetedEntities.empty())
	{
		zWorld->RemoveEntity(*zTargetedEntities.begin());
	}

	zTargetedEntities.clear();
	zPrevPosOfSelected.clear();
}


void GameEngine::GetNrOfSelectedEntities( int& x )
{
	x = zTargetedEntities.size();
}


void GameEngine::MouseMove( int, int )
{
	zMouseMoved = true;
}


void GameEngine::SetBrushAttr( char* info, float size )
{
	if(string(info) == "InnerCircle") // sets the inner circle size
	{
		zBrushSize = size;
	}
	else if(string(info) == "InnerAndOuterCircle") // sets the outer circle size
	{
		zBrushSize = size;
		zBrushSizeExtra = 0;
	}
	else if(string(info) == "OuterCircle") // sets the outer circle size
	{
		zBrushSizeExtra = size;
	}
	else if(string(info) == "Strength") // sets the strength size
	{
		zBrushStrength = size;
	}
	else if(string(info) == "DrawTex")
	{
		zTexBrushSelectedTex = (unsigned int)size;
	}
}


void GameEngine::SetBrushAttr( char* info, char* stringValue )
{
	if( strlen(info) == 4 && info[0] == 'T' && info[1] == 'e' && info[2] == 'x' )
	{
		unsigned int texID = info[3] - 48;
		zWorld->SetSectorTexture(zSelectedSectorX, zSelectedSectorY, stringValue, texID);
	}
}


void GameEngine::SetEntityType( int value )
{
	this->zCreateEntityType = value;
}


void GameEngine::GetCameraInfo( char* info, float& x, float& y, float& z )
{
	if(string(info) == "Position") // Returns the camera pos
	{
		Vector3 cameraPos = GetGraphics()->GetCamera()->GetPosition();
		x = cameraPos.x;
		y = cameraPos.y;
		z = cameraPos.z;
	}
}


void GameEngine::MoveObjectToSurface()
{
	if( zTargetedEntities.size() > 0 )
	{
		float tempY;
		Vector3 tempPos;
		for ( auto it=zTargetedEntities.begin(); it != zTargetedEntities.end(); it++ )
		{
			tempPos = (*it)->GetPosition();
			tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);
			(*it)->SetPosition(Vector3(tempPos.x, tempY, tempPos.z));
		}
	}
}


void GameEngine::GetSunInfo(char* info, float& x, float& y, float& z )
{

	Vector3 temp(x,y,z);

	if(string(info) == "Dir") // Returns the sunlight dir
	{
		temp = GetGraphics()->GetSunLightDirection();
		if ( zWorld ) temp = zWorld->GetSunDir();
		
	}
	else if(string(info) == "Color")
	{
		temp = GetGraphics()->GetSunLightColor();
		if ( zWorld ) temp = zWorld->GetSunColor();
	}

	x = temp.x;
	y = temp.y;
	z = temp.z;
}


void GameEngine::SetSunInfo( char* info, float x, float y, float z )
{
	if(string(info) == "Dir") // Returns the sunlight dir
	{
		if ( zWorld ) zWorld->SetSunProperties( Vector3(x, y, z), zWorld->GetSunColor(), zWorld->GetSunIntensity() );
	}
	else if(string(info) == "Color")
	{
		if ( zWorld ) zWorld->SetSunProperties( zWorld->GetSunDir(), Vector3(x, y, z), zWorld->GetSunIntensity() );
	}

	zWorldSavedFlag = false;
}


void GameEngine::GetAmbientLight( char* info, float& x, float& y, float& z )
{
	if(string(info) == "Color")
	{
		Vector3 light = GetGraphics()->GetSceneAmbientLight();
		if ( zWorld ) light = zWorld->GetWorldAmbient();
		x = light.x;
		y = light.y;
		z = light.z;
	}
}


void GameEngine::SetAmbientLight( char* info, float x, float y, float z )
{
	if(string(info) == "Color")
	{
		if ( zWorld ) zWorld->SetWorldAmbient(Vector3(x, y, z));
	}
	zWorldSavedFlag = false;
}


int GameEngine::CountEntitiesInSector()
{
	if ( !zWorld ) return 0;
	
	Vector2UINT cSector = zWorld->WorldPosToSector( GetGraphics()->GetCamera()->GetPosition().GetXZ() );

	std::set<Entity*> entities;
	Rect r( Vector2(cSector.x * SECTOR_WORLD_SIZE, cSector.y * SECTOR_WORLD_SIZE), Vector2(SECTOR_WORLD_SIZE, SECTOR_WORLD_SIZE) );

	return zWorld->GetEntitiesInRect(r,entities);
}


int GameEngine::HasWorldBeenSaved()
{
	return zWorldSavedFlag;
}
