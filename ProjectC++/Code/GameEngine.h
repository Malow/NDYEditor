#pragma once

#include "World.h"
#include "WorldRenderer.h"

enum MODE
{
	NONE,
	SELECT,
	MOVE,
	ROT,
};

class GameEngine : public Observer
{
private:
	int zScreenWidth;
	int zScreenHeight;

	World* zWorld;
	WorldRenderer *zWorldRenderer;

	MODE zMode;

public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);

	void SetWindowFocused(bool value);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	char* ProcessText(char* msg);

	void CreateWorld(int x, int y);
	void SaveWorldAs(char* msg);
	void OpenWorld(char* msg);

	void SelectMode(int mode);
};