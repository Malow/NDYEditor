#pragma once

class GameEngine;

#if defined _DLL_IMPORT
#define DLL_USAGE __declspec(dllimport)
#else
#define DLL_USAGE __declspec(dllexport)
#endif

class DLL_USAGE GameEngineWrapper
{
private:
	GameEngine* m_GameEngine;
public:
	GameEngineWrapper();
	~GameEngineWrapper();

	unsigned int Init(unsigned int hWnd, int width, int height);
	void Shutdown();

	void ProcessFrame();
	void OnResize(int width, int height);
	void Update();
	void SetWindowFocused(bool value);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	char* ProcessText(char* msg);

	void CreateWorld(int x, int y);
	void SaveWorld(char* msg);
	void OpenWorld(char* msg);

	void ModeSelect(int mode);
};