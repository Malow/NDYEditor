#pragma once

#include <hge.h>
#include <hgesprite.h>

class GameEngine
{
private:
	HGE * m_pHge;

	int m_ScreenWidth;
	int m_ScreenHeight;
private:
	void RenderFunc();
	bool FrameFunc();
public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);

	char* ProcessText(char* msg);
};