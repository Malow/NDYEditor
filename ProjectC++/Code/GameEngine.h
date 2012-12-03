#pragma once

#include "World.h"

class GameEngine : public Observer
{
private:
	int m_ScreenWidth;
	int m_ScreenHeight;

	World* m_World;

public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	char* ProcessText(char* msg);

	void CreateWorld(int x, int y);
	void SaveWorld(char* msg);
	void OpenWorld(char* msg);

	void SelectMode(int mode);
};