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
	zMaxSpeed(20),
	zRTSHeightFromGround(20)
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
	GetGraphics()->SetSunLightProperties(Vector3(0.5f, -1.0f, 0.0f));
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
			Vector3 temp = GetGraphics()->GetCamera()->GetPosition();
			try
			{
				float yPos = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z);

				if(yPos == std::numeric_limits<float>::infinity())
				{
					yPos = GetGraphics()->GetCamera()->GetPosition().y;
				}
				ge->GetCamera()->SetPosition(Vector3(temp.x, yPos+1.8f, temp.z));
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
			Vector3 temp = GetGraphics()->GetCamera()->GetPosition();
			try
			{
				float yPos = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z);

				if(yPos == std::numeric_limits<float>::infinity())
				{
					yPos = GetGraphics()->GetCamera()->GetPosition().y;
				}
				ge->GetCamera()->SetPosition(Vector3(temp.x, yPos + this->zRTSHeightFromGround, temp.z));
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
						if ( distanceFactor < 0 ) continue;
						distanceFactor /= zBrushSize;

						try 
						{
							zWorld->ModifyHeightAt(i->x,i->y,(zMode==MODE::LOWER?-zBrushStrength:zBrushStrength)*distanceFactor);
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
							zWorld->SetHeightAt( i->x, i->y, curHeight + dif * zBrushStrength * factor );
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
	}
}


void GameEngine::CreateWorld( int width, int height )
{
	if ( zWorld ) delete zWorld, zWorld=0;
	this->zWorld = new World(this, width, height);
	this->zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
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
		zWorld->SaveFileAs( std::string(msg) );
}


void GameEngine::OpenWorld( char* msg )
{
	if ( zWorld ) delete zWorld, zWorld = 0;
	zWorld = new World(this, msg);
	zWorldRenderer = new WorldRenderer(zWorld, GetGraphics());
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
		Vector3 tempPos = ge->GetCamera()->GetPosition();
		float yPos = this->zWorldRenderer->GetYPosFromHeightMap(tempPos.x, tempPos.z);
		this->zRTSHeightFromGround = tempPos.y - yPos;
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
	else if( key == VK_SUBTRACT)
	{
		if((zMovementMulti / 2) >= 1)
			zMovementMulti/=2;
	}
	else if( key == VK_ADD)
	{
		if((zMovementMulti * 2) < zMaxSpeed)
			zMovementMulti*=2;
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
	if(string(info) == "Dir") // Returns the sunlight dir
	{
		Vector3 temp = GetGraphics()->GetSunLightDirection();
		x = temp.x;
		y = temp.y;
		z = temp.z;
	}
	if(string(info) == "Color")
	{
		Vector3 temp = GetGraphics()->GetSunLightColor();
		x = temp.x * 255;
		y = temp.y * 255;
		z = temp.z * 255;
	}
}

void GameEngine::SetSunInfo( char* info, float x, float y, float z )
{
	if(string(info) == "Dir") // Returns the sunlight dir
	{
		GetGraphics()->SetSunLightProperties(Vector3(x, y, z));
	}
	if(string(info) == "Color")
	{
		Vector3 temp = GetGraphics()->GetSunLightDirection();
		GetGraphics()->SetSunLightProperties(temp, Vector3(x, y, z));
	}
}

void GameEngine::GetAmbientLight( char* info, float& x, float& y, float& z )
{
	if(string(info) == "Color")
	{
		Vector3 light = GetGraphics()->GetSceneAmbientLight();
		x = light.x * 255;
		x = light.y * 255;
		x = light.z * 255;
	}
}

void GameEngine::SetAmbientLight( char* info, float x, float y, float z )
{
	if(string(info) == "Color")
	{
		GetGraphics()->SetSceneAmbientLight(Vector3(x, y, z));
	}
}
