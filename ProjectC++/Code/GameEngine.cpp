#include "GameEngine.h"
#include "MaloWFileDebug.h"
#include "EntityList.h"
#include "HeightChangedAction.h"
#include "BlendChangedAction.h"
#include "EntityPlacedAction.h"
#include "TerrainSetHeightAction.h"
#include "EntityRemovedAction.h"
#include "AIGridChangeAction.h"
#include "GridCalculateAction.h"
#include "SmoothAction.h"
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
	zRightMouseDown(false),
	zBrushStrength(1),
	zTexBrushSelectedTex(0),
	zAnchor(0),
	zBrushLastPos(0.0f,0.0f),
	zMouseMoved(false),
	zCreateEntityType(0),
	zFPSLockToGround(false),
	zMovementMulti(2.95f),
	zMaxSpeed(32),
	zRTSHeightFromGround(20),
	zWorldSavedFlag(true),
	currentActionIndex(0),
	zCurrentActionGroup(0)
{
	zGraphics->GetCamera()->SetUpdateCamera(false);
	zGraphics->CreateSkyBox("Media/skymap.dds");
	zGraphics->GetKeyListener()->SetCursorVisibility(true);
	zGraphics->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	zGraphics->StartRendering();
	prevSunDir = Vector3(0, -1, 0);
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
	ClearActionHistory();
	if ( zWorld ) delete zWorld;
}


void GameEngine::ProcessFrame()
{
	iCamera *camera = zGraphics->GetCamera();
	float dt = zGraphics->Update();

	if ( zWorld ) 
	{
		if ( zAnchor )
		{
			zAnchor->position = camera->GetPosition().GetXZ();
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
		float tempY = 0;
		if(cd.collision)
		{
			auto i = zTargetedEntities.begin();
			zMoveOffSet = Vector3(cd.posx, 0.0f, cd.posz) - zPrevPosOfSelected[(*i)];
			if((*i)->GetPosition().x >= 0 && (*i)->GetPosition().z >= 0 && (*i)->GetPosition().x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE && (*i)->GetPosition().z < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE)
			{
				tempY = zWorldRenderer->GetYPosFromHeightMap((*i)->GetPosition().x, (*i)->GetPosition().z);
				(*i)->SetPosition(zPrevPosOfSelected[(*i)] + zMoveOffSet + Vector3(0,tempY,0));
			}
			Vector3 newPos = NULL;
			for ( auto it=zTargetedEntities.begin(); it != zTargetedEntities.end(); it++ )
			{
				newPos = zPrevPosOfSelected[(*it)] + zMoveOffSet;
				if(newPos.x  < 0.0f) continue;
				else if(newPos.x >= (this->zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE)) continue;
				if(newPos.z  < 0.0f) continue;
				else if(newPos.z >= (this->zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE)) continue;

				tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);
				(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet + Vector3(0,tempY,0));
			}

			zWorldSavedFlag = false;
		}
	}

	if( zGraphics->GetCamera()->GetCameraType() == CameraType::FPS )
	{
		if ( zFPSLockToGround && zWorld )
		{
			Vector3 pos = zGraphics->GetCamera()->GetPosition();
			Vector3 forward = zGraphics->GetCamera()->GetForward();
			forward.Normalize();
			Vector3 sideways = zGraphics->GetCamera()->GetRightVector();
			sideways.Normalize();

			pos += forward * (float)(zGraphics->GetKeyListener()->IsPressed('W') - zGraphics->GetKeyListener()->IsPressed('S')) * dt * 0.001f * zMovementMulti;
			pos += sideways * (float)(zGraphics->GetKeyListener()->IsPressed('A') - zGraphics->GetKeyListener()->IsPressed('D')) * dt * 0.001f * zMovementMulti;

			if ( pos.x >= 0.0f && pos.z >= 0.0f && pos.x < zWorld->GetWorldSize().x && pos.z < zWorld->GetWorldSize().y )
			{
				if ( !zWorld->IsBlockingAt(pos.GetXZ()) )
				{
					pos.y = zWorld->CalcHeightAtWorldPos(pos.GetXZ()) + 1.7f;
					zGraphics->GetCamera()->SetPosition( pos );
				}
			}
		}
		else if ( zLockMouseToCamera )
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
					if ( temp.z > 0.0f && temp.z < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
					{
						temp.y = zWorld->GetHeightAt(temp.x, temp.z) + zRTSHeightFromGround;
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
				zGraphics->SetSpecialCircle(zBrushSize, zBrushSize+zBrushSizeExtra, Vector2(cd.posx,cd.posz));

				// Brush Drawing
				if ( zLeftMouseDown && Vector2(zBrushLastPos - Vector2(cd.posx, cd.posz) ).GetLength() > zBrushSize/2.0f )
				{
					this->OnLeftMouseDown(0, 0);
					zBrushLastPos = Vector2(cd.posx, cd.posz);
				}
				else if ( zRightMouseDown && Vector2(zBrushLastPos - Vector2(cd.posx, cd.posz) ).GetLength() > zBrushSize/2.0f )
				{
					this->OnRightMouseDown(0, 0);
					zBrushLastPos = Vector2(cd.posx, cd.posz);
				}
			}
			else
			{
				zGraphics->SetSpecialCircle(-1.0f, 0.0f, Vector2(0.0f,0.0f));
			}
			zMouseMoved = false;
		}
	}
	else
	{
		zGraphics->SetSpecialCircle(-1.0f,0.0f,Vector2(0.0f,0.0f));
	}
}


void GameEngine::OnResize(int width, int height)
{
	zGraphics->ResizeGraphicsEngine(width, height);
}


void GameEngine::OnLeftMouseUp( unsigned int, unsigned int )
{
	if ( zCurrentActionGroup && ( zMode == LOWER || zMode == RAISE || zMode == DRAWTEX || zMode == DELETEBRUSH || zMode == RESETBRUSH || zMode == AIGRIDBRUSH) )
		zCurrentActionGroup = 0;

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
					Vector3 newPos = NULL;
					for ( auto it=zTargetedEntities.begin() ; it != zTargetedEntities.end(); it++ )
					{
						(*it)->SetSelected(false);

						newPos = zPrevPosOfSelected[(*it)] + zMoveOffSet;
						if(newPos.x < 0.0f) continue;
						else if(newPos.x > (this->zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE)) continue;
						if(newPos.z < 0.0f) continue;
						else if(newPos.z > (this->zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE)) continue;
						
						tempY = zWorldRenderer->GetYPosFromHeightMap((*it)->GetPosition().x, (*it)->GetPosition().z);
						(*it)->SetPosition(zPrevPosOfSelected[(*it)] + zMoveOffSet + Vector3(0, tempY, 0));
					}
					zPrevPosOfSelected.clear();
					zTargetedEntities.clear();

					zWorldSavedFlag = false;
				}
			}
		}
		else if(this->zMode == MODE::RESETBRUSH)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				if ( !zCurrentActionGroup )
				{
					zCurrentActionGroup = new ActionGroup();
					ApplyAction(zCurrentActionGroup);
				}

				TerrainSetHeightAction *TSHA = new TerrainSetHeightAction(
					zWorld,
					Vector2(cd.posx, cd.posz),
					zBrushSize,
					0
					);

				TSHA->Execute();

				zCurrentActionGroup->zActions.push_back( TSHA );
				zWorldSavedFlag = false;
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
			zLeftMouseDown = true;
		}
		else if (this->zMode == MODE::AIGRIDBRUSH)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				// Create Event Group
				if ( !zCurrentActionGroup )
				{
					zCurrentActionGroup = new ActionGroup();
					ApplyAction(zCurrentActionGroup);
				}

				AIGridChangeAction* AIGCA = new AIGridChangeAction( 
					zWorld, 
					Vector2(cd.posx, cd.posz), 
					zBrushSize, 
					true );

				AIGCA->Execute();

				zCurrentActionGroup->zActions.push_back(AIGCA);

				zWorldSavedFlag = false;
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
			zLeftMouseDown = true;
		}
		else if (this->zMode == MODE::DELETEBRUSH)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				std::set<Entity*> entities;
				if ( zWorld->GetEntitiesInCircle(Vector2(cd.posx, cd.posz), zBrushSize, entities) )
				{
					// Create Event Group
					if ( !zCurrentActionGroup )
					{
						zCurrentActionGroup = new ActionGroup();
						ApplyAction(zCurrentActionGroup);
					}

					// Delete Entities
					for( auto i = entities.begin(); i != entities.end(); ++i )
					{
						EntityRemovedAction* ERA = new EntityRemovedAction( 
							zWorld,
							*i );

						ERA->Execute();
						zCurrentActionGroup->zActions.push_back( ERA );
					}

					zWorldSavedFlag = false;
				}
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
			zLeftMouseDown = true;
		}
		else if(this->zMode == MODE::PLACE)
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				EntityPlacedAction *EPA = new EntityPlacedAction(
					zWorld, 
					zCreateEntityType,
					Vector3(cd.posx, cd.posy, cd.posz),
					Vector3(0.0f, (float)rand()/float(RAND_MAX)*360.0f, 0.0f),
					Vector3(1.0f, 1.0f, 1.0f)
					);

				ApplyAction(EPA);
				zWorldSavedFlag = false;
			}
		}
		else if ( this->zMode == MODE::RAISE || this->zMode == MODE::LOWER )
		{
			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				if ( !zCurrentActionGroup )
				{
					zCurrentActionGroup = new ActionGroup();
					ApplyAction(zCurrentActionGroup);
				}

				HeightChangedAction* HCA = new HeightChangedAction( 
					zWorld, 
					Vector2(cd.posx, cd.posz),
					(zMode == MODE::LOWER? -zBrushStrength : zBrushStrength),
					zBrushSize );

				HCA->Execute();
				zCurrentActionGroup->zActions.push_back( HCA );
				zWorldSavedFlag = false;
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
			zLeftMouseDown = true;
		}
		else if ( zMode == MODE::DRAWTEX )
		{
			if ( !zCurrentActionGroup )
			{
				zCurrentActionGroup = new ActionGroup();
				ApplyAction(zCurrentActionGroup);
			}

			CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
			if(cd.collision)
			{
				BlendChangedAction* BCA = new BlendChangedAction(
					zWorld,
					Vector2(cd.posx, cd.posz),
					zBrushStrength,
					zBrushSize,
					zBrushSizeExtra,
					zTexBrushSelectedTex );

				BCA->Execute();
				zCurrentActionGroup->zActions.push_back( BCA );
				zWorldSavedFlag = false;
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
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
				std::set<Entity*> insideCircle;
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
				if ( !zCurrentActionGroup )
				{
					zCurrentActionGroup = new ActionGroup();
					ApplyAction(zCurrentActionGroup);
				}

				SmoothAction* SM = new SmoothAction(
					zWorld,
					Vector2(cd.posx, cd.posz),
					zBrushSize,
					zBrushSizeExtra,
					zBrushStrength );

				SM->Execute();
				zCurrentActionGroup->zActions.push_back(SM);

				zWorldSavedFlag = false;
				zBrushLastPos = Vector2(cd.posx, cd.posz);
			}
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


void GameEngine::ChangeMode( unsigned int mode )
{
	if ( zMode == AIGRIDBRUSH && mode != AIGRIDBRUSH )
	{
		if ( zWorldRenderer )
		{
			zWorldRenderer->ToggleAIGrid(false);
		}
	}
	else if ( mode == AIGRIDBRUSH )
	{
		if ( zWorldRenderer )
		{
			zWorldRenderer->ToggleAIGrid(true);
		}
	}

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
	if ( zMode == MODE::RAISE || 
		zMode == MODE::LOWER || 
		zMode == MODE::PLACEBRUSH || 
		zMode == MODE::DRAWTEX || 
		zMode == MODE::SMOOTH || 
		zMode == MODE::DELETEBRUSH || 
		zMode == MODE::RESETBRUSH ||
		zMode == MODE::AIGRIDBRUSH )
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
	else if( temp == "RTS" )
	{
		Vector3 oldForward = zGraphics->GetCamera()->GetForward();
		Vector3 camPos = zGraphics->GetCamera()->GetPosition();

		if ( camPos.x > 0.0f && camPos.x < zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE )
		{
			if ( camPos.z > 0.0f && camPos.z < zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE )
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
	else if ( key == 110 )
	{
		// DISABLED UNTIL FIXED
		if ( false && zWorld )
		{
			Vector3 startPos;
			startPos.x = zWorld->GetNumSectorsWidth() * SECTOR_WORLD_SIZE * 0.5f;
			startPos.z = zWorld->GetNumSectorsHeight() * SECTOR_WORLD_SIZE * 0.5f;
			startPos.y = zWorld->GetHeightAt(startPos.x, startPos.y) + 1.7f; 
			zGraphics->GetCamera()->SetPosition(startPos);
		}
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
	else if( key == (int)'R')
	{
		this->zMovementMulti = 2.95f; // m/s
	}
	else if ( key == (int)'Z' )
	{
		if ( zGraphics->GetKeyListener()->IsPressed(17) )
		{
			UndoAction();
		}
	}
	else if ( key == (int)'Y' )
	{
		if ( zGraphics->GetKeyListener()->IsPressed(17) )
		{
			RedoAction();
		}
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
		ClearActionHistory();
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
	if ( !zTargetedEntities.empty() )
	{
		ActionGroup *AG = new ActionGroup();

		for( auto i = zTargetedEntities.begin(); i != zTargetedEntities.end(); ++i )
		{
			AG->zActions.push_back( new EntityRemovedAction(zWorld, *i) );
		}

		ApplyAction( AG );

		zTargetedEntities.clear();
		zPrevPosOfSelected.clear();
	}

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
	if( !zTargetedEntities.empty() )
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
		zWorldSavedFlag = false;
	}
	else if( string(info) == "Color" )
	{
		if ( zWorld ) zWorld->SetSunProperties( zWorld->GetSunDir(), Vector3(x, y, z), zWorld->GetSunIntensity() );
		zWorldSavedFlag = false;
	}
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
		zWorldSavedFlag = false;
	}
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

void GameEngine::ApplyAction( Action* a )
{
	// Invalidate Previous Future
	while ( currentActionIndex < zActionHistory.size())
	{
		delete *zActionHistory.rbegin();
		zActionHistory.pop_back();
	}

	a->Execute();
	zActionHistory.push_back(a);
	currentActionIndex++; 

	if ( zActionHistory.size() > 100 )
	{
		delete *zActionHistory.begin();
		zActionHistory.erase(zActionHistory.begin());
		currentActionIndex--;
	}
}

void GameEngine::UndoAction()
{
	if ( currentActionIndex == 0 ) return;

	if ( zCurrentActionGroup && *zActionHistory.rbegin() == zCurrentActionGroup )
	{
		zCurrentActionGroup = 0;
	}

	currentActionIndex--;
	zActionHistory[currentActionIndex]->Undo();
}

void GameEngine::ClearActionHistory()
{
	currentActionIndex = 0;
	while ( !zActionHistory.empty() )
	{
		if(*zActionHistory.rbegin() == zCurrentActionGroup)
			zCurrentActionGroup = 0;

		delete *zActionHistory.rbegin();
		zActionHistory.pop_back();
	}

	if ( zCurrentActionGroup ) delete zCurrentActionGroup, zCurrentActionGroup=0;
}

void GameEngine::RedoAction()
{
	if ( zActionHistory.size() <= currentActionIndex ) return;
	zActionHistory[currentActionIndex]->Execute();
	currentActionIndex++;	
}

void GameEngine::CalculateAIGrid()
{
	if ( zWorld )
	{
		GridCalculateAction* GCA = new GridCalculateAction(zWorld, zWorld->WorldPosToSector(zGraphics->GetCamera()->GetPosition().GetXZ()));
		ApplyAction(GCA);
		zWorldSavedFlag = false;
	}
}

void GameEngine::OnRightMouseDown( unsigned int x, unsigned int y )
{
	if (this->zMode == MODE::AIGRIDBRUSH)
	{
		CollisionData cd = zWorldRenderer->Get3DRayCollisionDataWithGround();
		if(cd.collision)
		{
			// Create Event Group
			if ( !zCurrentActionGroup )
			{
				zCurrentActionGroup = new ActionGroup();
				ApplyAction(zCurrentActionGroup);
			}

			AIGridChangeAction* AIGCA = new AIGridChangeAction( 
				zWorld, 
				Vector2(cd.posx, cd.posz), 
				zBrushSize, 
				false );

			AIGCA->Execute();

			zCurrentActionGroup->zActions.push_back(AIGCA);

			zWorldSavedFlag = false;
			zBrushLastPos = Vector2(cd.posx, cd.posz);
		}
		zRightMouseDown = true;
	}
	
}

void GameEngine::OnRightMouseUp( unsigned int x, unsigned int y )
{
	if ( zCurrentActionGroup && ( zMode == LOWER || zMode == RAISE || zMode == DRAWTEX || zMode == DELETEBRUSH || zMode == RESETBRUSH || zMode == AIGRIDBRUSH ) )
		zCurrentActionGroup = 0;

	zRightMouseDown = false;
}

float GameEngine::GetSunOnOff()
{
	return zGraphics->GetSunLightDirection().GetLength();
}

void GameEngine::SunOnOff(bool value)
{
	if(value)
	{
		zGraphics->SetSunLightProperties(prevSunDir, zGraphics->GetSunLightColor());
	}
	else
	{
		prevSunDir = zGraphics->GetSunLightDirection();
		zGraphics->SetSunLightDisabled();
	}
}

void GameEngine::GetWorldSize( float &x, float&y )
{
	if ( zWorld ) 
	{
		x = zWorld->GetWorldSize().x;
		y = zWorld->GetWorldSize().y;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
	}
}
