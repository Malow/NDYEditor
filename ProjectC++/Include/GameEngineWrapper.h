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

	unsigned int Init(unsigned int hWnd);
	void Shutdown();

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
	void MouseMove(int x, int y);

	void RemoveSelectedEntities();

	void SetBrushAttr(char* info, float size);
	void SetBrushAttr(char* info, char* stringValue);
	void GetBrushAttr(char* info, float& size);
	void GetBrushAttr(char* info, char* tempString);
	void ChangeMode(int mode);
	void ChangeCameraMode(char* cameraMode);
	void LockMouseToCamera();
	void GetSelectedInfo(char* info, float& x, float& y, float& z);
	void SetSelectedObjectInfo( char* info, float x, float y, float z );
	void GetNrOfSelectedEntities(int& x);
	void SetEntityType(int value);
	void GetCameraInfo(char* info, float& x, float& y, float& z);
};