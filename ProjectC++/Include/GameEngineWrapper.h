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

	void KeyDown(int key);
	void KeyUp(int key);

	void OnLeftMouseUp( unsigned int x, unsigned int y );
	void OnLeftMouseDown( unsigned int x, unsigned int y );

	char* ProcessText(char* msg);

	void CreateWorld(int x, int y);
	void SaveWorldAs(char* msg);
	void SaveWorld();
	void OpenWorld(char* msg);

	void ChangeMode(int mode);
	void SetCreateModelPath(char* filePath);
	void ChangeCameraMode(char* cameraMode);
	void LockMouseToCamera();
};