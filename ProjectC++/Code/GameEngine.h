#pragma once

#include "World.h"
#include "WorldRenderer.h"


enum MODE
{
	NONE,
	SELECT,
	MOVE,
	ROT,
	PLACE,
	RAISE,
	LOWER,
	PLACEBRUSH,
	DRAWTEX,
	SMOOTH
};

class GameEngine : public Observer
{
private:
	// Data
	World* zWorld;
	WorldRenderer *zWorldRenderer;
	WorldAnchor* zAnchor;

	// Camera
	bool zLockMouseToCamera;
	bool zFPSLockToGround;

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
public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd);

	void ProcessFrame();
	void OnResize(int width, int height);

	void SetWindowFocused(bool value);

	void KeyDown(int key);
	void KeyUp(int key);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	void CreateWorld(int x, int y);
	void SaveWorldAs(char* msg);
	void SaveWorld();
	void OpenWorld(char* msg);

	void RemoveSelectedEntities();

	void SetBrushAttr(char* info, float size);
	void SetBrushAttr(char* info, char* stringValue);
	void GetBrushAttr(char* info, float& size);
	void GetBrushAttr(char* info, char* string);
	void MouseInsideFrame(bool flag);
	void MouseMove(int x, int y);
	void ChangeMode(int mode);
	void ChangeCameraMode(char* cameraMode);
	void LockMouseToCamera();
	
	void GetSelectedInfo(char* info, float& x, float& y, float& z);
	void SetSelectedObjectInfo(char* info, float& x, float& y, float& z);
	void GetNrOfSelectedEntities(int& x);
	void SetEntityType(int value);
protected:
	virtual void onEvent( Event* e );
};