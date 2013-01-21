#include "GameEngine.h"
#include "MaloWFileDebug.h"
#include "EntityList.h"
#include <math.h>
#include <time.h>

const float M_PI = 3.141592f;


GameEngine::GameEngine( GraphicsEngine* GE ) :
	zGraphics(GE),
	zMode(MODE::NONE),
	zWorld(0),
	zWorldRenderer(0),
	zLockMouseToCamera(0),
	zBrushSize(1.0f),
	zBrushSizeExtra(0.0f),
	zDrawBrush(false),
	zLeftMouseDown(false),
	zBrushStrength(1),
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
	zGraphics->GetCamera()->SetUpdateCamera(false);
	zGraphics->CreateSkyBox("Media/skymap.dds");
	zGraphics->GetKeyListener()->SetCursorVisibility(true);
	zGraphics->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	zGraphics->StartRendering();

	// Entities
	LoadEntList("Entities.txt");

	// Start Camera
	ChangeCameraMode("FPS");
	LockMouseToCamera();

	// Random
	srand( (unsigned int)time(NULL) );
}


GameEngine::~GameEngine()
{
	if ( zWorld ) delete zWorld;
}


void GameEngine::ProcessFrame()
{
	iCamera *camera = zGraphics->GetCamera();
	float dt = zGraphics->Update();

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
			zAnchor->radius = zGraphics->GetEngineParameters()->FarClip;
		}

		zWorld->Update();

		if ( zWorldRenderer )
		{
			zWorldRenderer->update();
		}

		zGraphics->SetSceneAmbientLight(zWorld->GetAmbientAtWorldPos(camera->GetPosition().GetXZ()));
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

	if( zGraphics->GetCamera()->GetCameraType() == CameraType::FPS )
	{
		if ( zLockMouseToCamera )
		{
			if(zGraphics->GetKeyListener()->IsPressed('W'))
				zGraphics->GetCamera()->MoveForward(dt * zMovementMulti);

			if(zGraphics->GetKeyListener()->IsPressed('S'))
				zGraphics->GetCamera()->MoveBackward(dt * zMovementMulti);

			if(zGraphics->GetKeyListener()->IsPressed('A'))
				zGraphics->GetCamera()->MoveLeft(dt * zMovementMulti);

			if(zGraphics->GetKeyListener()->IsPressed('D'))
				zGraphics->GetCamera()->MoveRight(dt * zMovementMulti);
		}

		if ( zFPSLockToGround && zWorldRenderer )
		{
			try
			{
				Vector3 temp = zGraphics->GetCamera()->GetPosition();
				if ( temp.x > 0.0f && temp.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
				{
					if ( temp.y > 0.0f && temp.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
					{
						temp.y = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z) + 1.8f;
						zGraphics->GetCamera()->SetPosition(temp);
					}
				}
			}
			catch(...)
			{
			}
		}
	}
	else if(zGraphics->GetCamera()->GetCameraType() == CameraType::RTS)
	{
		if(zGraphics->GetKeyListener()->IsPressed('W'))
		{
			zGraphics->GetCamera()->MoveForward(dt * zMovementMulti);
		}
		if(zGraphics->GetKeyListener()->IsPressed('S'))
		{
			zGraphics->GetCamera()->MoveBackward(dt * zMovementMulti);
		}
		if(zGraphics->GetKeyListener()->IsPressed('A'))
		{
			zGraphics->GetCamera()->MoveLeft(dt * zMovementMulti);
		}
		if(zGraphics->GetKeyListener()->IsPressed('D'))
		{
			zGraphics->GetCamera()->MoveRight(dt * zMovementMulti);
		}

		if ( zWorldRenderer )
		{
			try
			{
				Vector3 temp = zGraphics->GetCamera()->GetPosition();
				if ( temp.x > 0.0f && temp.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
				{
					if ( temp.y > 0.0f && temp.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
					{
						temp.y = zWorldRenderer->GetYPosFromHeightMap(temp.x, temp.z) + zRTSHeightFromGround;
						zGraphics->GetCamera()->SetPosition(temp);
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
				zGraphics->SetSpecialCircle(zBrushSize,zBrushSize+zBrushSizeExtra,Vector2(cd.posx,cd.posz));

				// Brush Drawing
				if ( zLeftMouseDown && Vector2(zBrushLastPos - Vector2(cd.posx, cd.posz) ).GetLength() > zBrushSize/2.0f )
				{
					this->OnLeftMouseDown(0,0);
					zBrushLastPos = Vector2(cd.posx, cd.posz);
				}
			}
			else
			{
				zGraphics->SetSpecialCircle(-1,0,Vector2(0,0));
			}
			zMouseMoved = false;
		}
	}
	else
	{
		zGraphics->SetSpecialCircle(-1,0,Vector2(0,0));
	}
}


void GameEngine::OnResize(int width, int height)
{
	zGraphics->ResizeGraphicsEngine(width, height);
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
				if(!zTargetedEntities.count(returnEntity) && zGraphics->GetKeyListener()->IsPressed(VK_SHIFT))
				{
					zTargetedEntities.insert(returnEntity);
					zPrevPosOfSelected[returnEntity] = returnEntity->GetPosition();
					returnEntity->SetSelected(true);
				}
				else if(!zGraphics->GetKeyListener()->IsPressed(VK_SHIFT))	// Set Selection
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
	this->zWorldRenderer = new WorldRenderer(zWorld, zGraphics);
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
	zWorldRenderer = new WorldRenderer(zWorld, zGraphics);
	zWorldSavedFlag = true;
}


void GameEngine::SetWindowFocused( bool flag )
{
	zGraphics->GetCamera()->SetActiveWindowDisabling(flag);
}


void GameEngine::SaveWorld()
{
	if ( zWorld )
	{
		zWorld->SetStartCamera( zGraphics->GetCamera()->GetPosition(), zGraphics->GetCamera()->GetForward() );
		zWorld->SaveFile();
		zWorldSavedFlag = true;
	}
}


void GameEngine::ChangeCameraMode( char* cameraMode )
{
	string temp = std::string(cameraMode);

	if( temp == "FPS" )
	{
		Vector3 oldPos = zGraphics->GetCamera()->GetPosition();
		Vector3 oldForward = zGraphics->GetCamera()->GetForward();
		Vector3 oldUp = zGraphics->GetCamera()->GetUpVector();

		zGraphics->ChangeCamera(CameraType::FPS);

		zGraphics->GetCamera()->SetPosition(oldPos);
		zGraphics->GetCamera()->SetForward(oldForward);
		zGraphics->GetCamera()->SetUpVector(oldUp);

		zLockMouseToCamera = true;

		zGraphics->GetCamera()->SetUpdateCamera(true);
		zGraphics->GetKeyListener()->SetCursorVisibility(false);
	}

	if( temp == "RTS" )
	{
		Vector3 oldForward = zGraphics->GetCamera()->GetForward();
		Vector3 camPos = zGraphics->GetCamera()->GetPosition();

		if ( camPos.x > 0.0f && camPos.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
		{
			if ( camPos.y > 0.0f && camPos.y < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
			{
				float yPos = this->zWorldRenderer->GetYPosFromHeightMap(camPos.x, camPos.z);
				this->zRTSHeightFromGround = camPos.y - yPos;
			}
		}
		
		zGraphics->ChangeCamera(CameraType::RTS);
		zGraphics->GetCamera()->SetForward(oldForward);
		zGraphics->GetCamera()->SetUpdateCamera(false);
	}
}


void GameEngine::KeyUp( int key )
{
	zGraphics->GetKeyListener()->KeyUp(key);

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
	zGraphics->GetKeyListener()->KeyDown(key);
}


void GameEngine::LockMouseToCamera()
{
	if( zGraphics->GetCamera()->GetCameraType() == CameraType::FPS )
	{
		zLockMouseToCamera = !zLockMouseToCamera;
		if ( zLockMouseToCamera )
		{
			zGraphics->GetCamera()->SetUpdateCamera(true);
			zGraphics->GetKeyListener()->SetCursorVisibility(false);
		}
		else
		{
			zGraphics->GetCamera()->SetUpdateCamera(false);
			zGraphics->GetKeyListener()->SetCursorVisibility(true);
		}
	}
}


void GameEngine::GetSelectedInfo( char* info, float& x, float& y, float& z)
{
	if( zTargetedEntities.empty() )
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
		zGraphics->GetCamera()->SetPosition( WLE->world->GetStartCamPos() );
		zGraphics->GetCamera()->SetForward( WLE->world->GetStartCamRot() );
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
		Vector3 cameraPos = zGraphics->GetCamera()->GetPosition();
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
		if ( zWorld ) 
			temp = zWorld->GetSunDir();
		else
			temp = zGraphics->GetSunLightDirection();
	}
	else if(string(info) == "Color")
	{
		if ( zWorld ) 
			temp = zWorld->GetSunColor();
		else
			temp = zGraphics->GetSunLightColor();
	}

	x = temp.x;
	y = temp.y;
	z = temp.z;
}


void GameEngine::SetSunInfo( char* info, float x, float y, float z )
{
	if( string(info) == "Dir" ) // Returns the sunlight dir
	{
		if ( zWorld ) zWorld->SetSunProperties( Vector3(x, y, z), zWorld->GetSunColor(), zWorld->GetSunIntensity() );
	}
	else if( string(info) == "Color" )
	{
		if ( zWorld ) zWorld->SetSunProperties( zWorld->GetSunDir(), Vector3(x, y, z), zWorld->GetSunIntensity() );
	}

	zWorldSavedFlag = false;
}


void GameEngine::GetAmbientLight( char* info, float& x, float& y, float& z )
{
	Vector3 light(x,y,z);

	if( string(info) == "Color" )
	{
		if ( zWorld ) 
			light = zWorld->GetWorldAmbient();
		else
			light = zGraphics->GetSceneAmbientLight();
	}

	x = light.x;
	y = light.y;
	z = light.z;
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
	
	Vector2UINT cSector = zWorld->WorldPosToSector( zGraphics->GetCamera()->GetPosition().GetXZ() );

	std::set<Entity*> entities;
	Rect r( Vector2(cSector.x * (float)SECTOR_WORLD_SIZE, cSector.y * (float)SECTOR_WORLD_SIZE), Vector2((float)SECTOR_WORLD_SIZE, (float)SECTOR_WORLD_SIZE) );

	return zWorld->GetEntitiesInRect(r,entities);
}


int GameEngine::HasWorldBeenSaved()
{
	return zWorldSavedFlag;
}
