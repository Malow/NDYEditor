#pragma once

#include "World.h"
#include "WorldRenderer.h"
#include "Action.h"

#ifdef _DEBUG
#include <vld.h>
#endif // _DEBUG

enum MODE
{
	NONE = 0,
	SELECT = 1,
	MOVE = 2,
	PLACE = 3,
	RAISE = 4,
	LOWER = 5,
	PLACEBRUSH = 6,
	DRAWTEX = 7,
	SMOOTH = 8,
	DELETEBRUSH = 9,
	RESETBRUSH = 10,
	AIGRIDBRUSH = 11
};



class GameEngine : public Observer
{
private:
	// Data
	World* zWorld;
	WorldRenderer *zWorldRenderer;
	WorldAnchor* zAnchor;
	bool zWorldSavedFlag;
	Vector3 prevSunDir;

	// Camera
	bool zLockMouseToCamera;
	bool zFPSLockToGround;
	float zMovementMulti;
	const float zMaxSpeed;
	float zRTSHeightFromGround;

	// Current Tool
	MODE zMode;
	float zBrushSize;
	float zBrushSizeExtra;
	float zBrushStrength;

	// Entity Place Brush
	unsigned int zCreateEntityType;

	// Entity Select Brush
	std::set<Entity*> zTargetedEntities;
	std::map<Entity*, Vector3> zPrevPosOfSelected;
	Vector3 zMoveOffSet;

	// Brush
	bool zDrawBrush;
	bool zMouseMoved;
	bool zLeftMouseDown;
	Vector2 zBrushLastPos;

	// Texture Brush
	unsigned int zTexBrushSelectedTex;
	unsigned int zSelectedSectorX, zSelectedSectorY;

	// Graphics Engine
	GraphicsEngine* zGraphics;

	// Action History
	std::vector< Action* > zActionHistory;
	unsigned int currentActionIndex;
	ActionGroup *zCurrentActionGroup;

	//Walking on ground
	bool zRightMouseDown;
public:
	GameEngine( GraphicsEngine* GE );
	virtual ~GameEngine();

	void ProcessFrame();
	void OnResize(int width, int height);

	void SetWindowFocused(bool value);

	void KeyDown(int key);
	void KeyUp(int key);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );
	void OnRightMouseDown( unsigned int x, unsigned int y );
	void OnRightMouseUp( unsigned int x, unsigned int y );

	void CreateWorld(int x, int y);
	void SaveWorldAs(char* msg);
	void SaveWorld();
	void OpenWorld(char* msg);

	void RemoveSelectedEntities();

	void CalculateAIGrid();
	float GetSunOnOff();
	void SunOnOff(bool value);

	// Action History
	void ApplyAction( Action* a );
	void UndoAction();
	void RedoAction();
	void ClearActionHistory();

	void SetBrushAttr(char* info, float size);
	void SetBrushAttr(char* info, char* stringValue);
	void GetBrushAttr(char* info, float& size);
	void GetBrushAttr(char* info, char* string);
	void MouseInsideFrame(bool flag);
	void MouseMove(int x, int y);
	void ChangeMode(unsigned int mode);
	void ChangeCameraMode(char* cameraMode);
	void LockMouseToCamera();
	void MoveObjectToSurface();
	void GetWorldSize(float &x, float&y);

	void GetSelectedInfo(char* info, float& x, float& y, float& z);
	void SetSelectedObjectInfo(char* info, float& x, float& y, float& z);
	void GetSunInfo(char* info, float& x, float& y, float& z);
	void GetNrOfSelectedEntities(int& x);
	void SetEntityType(int value);
	void GetCameraInfo(char* info, float& x, float& y, float& z);
	void SetSunInfo(char* info, float x, float y, float z);
	void GetAmbientLight(char* info, float& x, float& y, float& z);
	void SetAmbientLight(char* info, float x, float y, float z);
	void TeleportTo( float x, float y, float z );
	void ToggleShadows(bool flag);

	// Returns Number of entities in current sector
	int CountEntitiesInSector();
	int HasWorldBeenSaved();

protected:
	virtual void OnEvent( Event* e );
};