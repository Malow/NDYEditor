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
	LOWER
};

class GameEngine : public Observer
{
private:
	int zScreenWidth;
	int zScreenHeight;

	World* zWorld;
	WorldRenderer *zWorldRenderer;

	MODE zMode;
	CameraType zCameraType;

	std::string zCreateModelPath;
	bool zLockMouseToCamera;

	float zBrushSize;

public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);

	void SetWindowFocused(bool value);

	void KeyDown(int key);
	void KeyUp(int key);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	char* ProcessText(char* msg);

	void CreateWorld(int x, int y);
	void SaveWorldAs(char* msg);
	void SaveWorld();
	void OpenWorld(char* msg);

	void SetBrushSize(float size);
	void ChangeMode(int mode);
	void SetCreateModelPath(char* filePath);
	void ChangeCameraMode(char* cameraMode);
	void LockMouseToCamera();

protected:
	virtual void onEvent( Event* e );
};