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

		HRESULT Init(IntPtr hWnd, int width, int height);
		void Shutdown();

		void ProcessFrame();
		void OnResize(int width, int height);

		String^ ProcessText(String^ text);

		void OnLeftMouseUp( unsigned int x, unsigned int y );
		void OnLeftMouseDown( unsigned int x, unsigned int y );

		void CreateWorld(int x, int y);
		void SaveWorld( String^ filePath );
		void OpenWorld( String^ filePath );

		void ModeSelect(int mode);
	};
}}}