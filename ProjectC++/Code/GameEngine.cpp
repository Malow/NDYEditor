#include "GameEngine.h"
#include "Graphics.h"
#include "MaloWFileDebug.h"

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
	zMouseMoved(false)
{
}


GameEngine::~GameEngine()
{
	if ( zWorldRenderer ) delete zWorldRenderer;
	if ( zAnchor ) zWorld->DeleteAnchor( zAnchor );
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

	if ( zAnchor ) 
	{
		zAnchor->position = Vector2(camera->GetPosition().x, camera->GetPosition().z);
		zAnchor->radius = GetGraphics()->GetEngineParameters()->FarClip;
	}
	if ( zWorld ) zWorld->Update();

	if(this->zMode == MODE::MOVE && !this->zTargetedEntities.empty())
	{
		CollisionData cd = this->zWorldRenderer->Get3DRayCollisionDataWithGround();
		
		if(cd.collision)
		{
			auto i = zTargetedEntities.begin();
			zMoveOffSet = Vector3(cd.posx, 0.0f, cd.posz) - zPrevPosOfSelected[(*i)];
			float tempY = zWorldRenderer->GetYPosFromHeightMap((*i)->GetPosition().x, (*i)->GetPosition().z);
			(*i)->SetPosition(zPrevPosOfSelected[(*i)] + zMoveOffSet + Vector3(0,tempY,0));

			for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
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
				ge->GetCamera()->SetPosition(Vector3(temp.x, yPos+10.0f, temp.z));
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
				zWorld->CreateEntity(Vector3(cd.posx, cd.posy, cd.posz),
				ENTITYTYPE::TREE/*This is not used yet*/, zCreateModelPath);
			}
		}
		else if ( this->zMode == MODE::RAISE || this->zMode == MODE::LOWER )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				std::vector<Vector2> nodes;
				if ( zWorld->GetHeightNodesInCircle(Vector2(cd.posx,cd.posz), zBrushSize, nodes) )
				{
					for( unsigned int x=0; x<nodes.size(); ++x )
					{
						float distanceFactor = zBrushSize - Vector2(cd.posx - nodes[x].x, cd.posz - nodes[x].y).GetLength();
						if ( distanceFactor < 0 ) continue;
						distanceFactor /= zBrushSize;

						try 
						{
							zWorld->ModifyHeightAt(nodes[x].x,nodes[x].y,(zMode==MODE::LOWER?-zBrushStrength:zBrushStrength)*distanceFactor);
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
				if ( zWorld->GetTextureNodesInCircle(Vector2(cd.posx,cd.posz), zBrushSize, nodes) )
				{
					for( auto i = nodes.begin(); i != nodes.end(); ++i )
					{
						float distanceFactor = zBrushSize - Vector2(cd.posx - i->x, cd.posz - i->y).GetLength();
						if ( distanceFactor < 0 ) continue;
						distanceFactor /= zBrushSize;

						Vector4 drawColor(0.0f,0.0f,0.0f,0.0f);
						drawColor[zTexBrushSelectedTex] = 1.0f;

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
				double length;
				double theta;
				double x;
				double z;
				vector<Entity*> insideCircle;
				if( zWorld->GetEntitiesInCircle(Vector2(cd.posx, cd.posz), zBrushSize, insideCircle) < zBrushStrength )
				{
					theta = ((double)rand() / (double)RAND_MAX) * M_PI * 2.0;
					length = ((double)rand() / (double)RAND_MAX) * zBrushSize;
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
	if ( zAnchor ) zWorld->DeleteAnchor( zAnchor );
	if ( zWorld ) delete zWorld, zWorld=0;
	this->zWorld = new World(this, x, y);
	this->zWorldRenderer = new WorldRenderer(zWorld,GetGraphics());
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
	if ( zMode == MODE::RAISE || zMode == MODE::LOWER || zMode == MODE::PLACEBRUSH || zMode == MODE::DRAWTEX )
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
	if ( zAnchor ) zWorld->DeleteAnchor( zAnchor );
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
		ge->GetCamera()->SetPosition(Vector3(ge->GetCamera()->GetPosition().x, 20, ge->GetCamera()->GetPosition().z));
		ge->ChangeCamera(CameraType::RTS);
		ge->GetCamera()->SetForward(oldForward);
		GetGraphics()->GetCamera()->SetUpdateCamera(false);
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
		zAnchor = WLE->world->CreateAnchor();
		GetGraphics()->GetCamera()->SetPosition( Vector3(0.0f,10.0f,0.0f) );
	}
}

void GameEngine::GetBrushAttr( char* info, float& size )
{
	if(string(info) == "InnerCircle") // Returns the inner circle size
		size = zBrushSize;
	if(string(info) == "OuterCircle") // Returns the outer circle size
		size = zBrushSize + zBrushSizeExtra;
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
