//File C++CLIWrapper.h

/*
* Created by: Ermin Hrkalovic (ERH)
* Email: ermin.hrkalovic@bth.se
*/

#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

//forwarddeklarationen måste ligga innan alla "using" !!! annars tolkas
//det av visual studio som att GameEngineWrapper-klassen ligger i ett namespace
#include <GameEngineWrapper.h>

using namespace System;
using namespace System::Windows::Interop;
using namespace System::Windows;
using namespace System::Runtime::InteropServices;

namespace System { namespace Windows { namespace Interop
{
	public ref class CppCLI
	{
	protected:
		GameEngineWrapper* m_GameEngine;
	public:
		CppCLI();
		~CppCLI();

		HRESULT Init(IntPtr hWnd);
		void Shutdown();

		void ProcessFrame();
		void OnResize(int width, int height);
		void SetWindowFocused(bool value);

		void KeyDown(int key);
		void KeyUp(int key);

		void OnLeftMouseUp( unsigned int x, unsigned int y );
		void OnLeftMouseDown( unsigned int x, unsigned int y );

		void CreateWorld(int x, int y);
		void SaveWorldAs( String^ filePath );
		void SaveWorld();
		void OpenWorld( String^ filePath );
		void SetBrushAttr(String^ info, float size );
		void SetBrushAttr(String^ info, String^ size );
		void GetBrushAttr(String^ info, [Out] float% size);
		void GetBrushAttr(String^ info, [Out] String^% tempString);

		void RemoveSelectedEntities();

		void MouseMove(int x, int y);

		void ChangeMode(int mode);
		void ChangeCameraMode( String^ cameraMode);
		void LockMouseToCamera();
		void GetSelectedInfo( String^ info, [Out] float% x, [Out] float% y, [Out] float% z );
		void GetCameraInfo( String^ info, [Out] float% x, [Out] float% y, [Out] float% z );
		void SetSelectedObjectInfo( String^ info, float x, float y, float z );
		void GetNrOfSelectedEntities([Out] int% x);
		void SetEntityType(int value);

	};
}}}