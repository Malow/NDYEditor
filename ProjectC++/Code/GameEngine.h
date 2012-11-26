#pragma once



class GameEngine
{
private:
	int m_ScreenWidth;
	int m_ScreenHeight;

public:
	GameEngine();
	~GameEngine();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);

	char* ProcessText(char* msg);
};