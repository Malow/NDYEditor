//File: C++CLIWrapper.cpp

#include "C++CLIWrapper.h"
#include <sstream>

namespace System { namespace Windows { namespace Interop
{
	CppCLI::CppCLI()
	{
		//myInitMemoryCheck();
		m_GameEngine = new GameEngineWrapper();
	}

	CppCLI::~CppCLI()
	{
		if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
		//DumpMemoryLeaks();
	}

	HRESULT CppCLI::Init(IntPtr hWnd, int width, int height)
	{
		return (HRESULT)m_GameEngine->Init(*(unsigned int*)&hWnd, width, height);
	}

	void CppCLI::Shutdown()
	{
		if ( m_GameEngine ) 
		{
			m_GameEngine->Shutdown(); 
			delete m_GameEngine;
			m_GameEngine = 0;
		}
	}

	void CppCLI::ProcessFrame()
	{
		if ( m_GameEngine ) m_GameEngine->ProcessFrame();
	}

	void CppCLI::OnResize(int width, int height)
	{
		if ( m_GameEngine ) m_GameEngine->OnResize(width, height);
	}

	void CppCLI::CreateWorld( int x, int y )
	{
		this->m_GameEngine->CreateWorld(x, y);
	}

	void CppCLI::SaveWorldAs(String^ filePath)
	{
		char* lpText = nullptr;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(filePath).ToPointer();

			m_GameEngine->SaveWorldAs(lpText);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}
	void CppCLI::OpenWorld( String^ filePath )
	{
		char* lpText = nullptr;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(filePath).ToPointer();

			m_GameEngine->OpenWorld(lpText);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::OnLeftMouseUp( unsigned int x, unsigned int y )
	{
		if ( m_GameEngine ) m_GameEngine->OnLeftMouseUp(x,y);
	}

	void CppCLI::OnLeftMouseDown( unsigned int x, unsigned int y )
	{
		if ( m_GameEngine ) m_GameEngine->OnLeftMouseDown(x,y);
	}

	void CppCLI::ChangeMode( int mode )
	{
		if ( m_GameEngine ) this->m_GameEngine->ChangeMode(mode);
	}

	void CppCLI::Update()
	{
		if ( m_GameEngine ) this->m_GameEngine->Update();
	}

	void CppCLI::SetWindowFocused( bool value )
	{
		if ( m_GameEngine ) this->m_GameEngine->SetWindowFocused(value);
	}

	void CppCLI::SaveWorld()
	{
		if ( m_GameEngine ) this->m_GameEngine->SaveWorld();
	}

	void CppCLI::SetCreateModelPath( String^ filePath )
	{
		char* lpText = nullptr;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(filePath).ToPointer();

			m_GameEngine->SetCreateModelPath(lpText);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::ChangeCameraMode( String^ cameraMode )
	{
		char* lpText = nullptr;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(cameraMode).ToPointer();

			m_GameEngine->ChangeCameraMode(lpText);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::KeyDown( int key )
	{
		this->m_GameEngine->KeyDown(key);
	}

	void CppCLI::KeyUp( int key )
	{
		this->m_GameEngine->KeyUp(key);
	}

	void CppCLI::LockMouseToCamera()
	{
		this->m_GameEngine->LockMouseToCamera();
	}

	void CppCLI::GetSelectedInfo( String^ info, [Out] float% x, [Out] float% y, [Out] float% z )
	{
		char* lpText = nullptr;
		float tempx, tempy, tempz;
		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();

			m_GameEngine->GetSelectedInfo(lpText, tempx, tempy, tempz);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
		if(info == "scale")
		{
			x = tempx * 20;
			y = tempy * 20;
			z = tempz * 20;
		}
		else
		{
			x = tempx;
			y = tempy;
			z = tempz;
		}
		
	}

	void CppCLI::SetSelectedObjectInfo( String^ info, float x, float y, float z )
	{
		char* lpText = nullptr;
		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();
			if(info == "scale")
				m_GameEngine->SetSelectedObjectInfo(lpText, x * 0.05, y * 0.05, z * 0.05);
			else
			{
				m_GameEngine->SetSelectedObjectInfo(lpText, x, y, z);
			}
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::GetBrushAttr( String^ info, [Out] float% size )
	{
		char* lpText = nullptr;
		float tempSize;
		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();

			m_GameEngine->GetBrushAttr(lpText, tempSize);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
		size = tempSize;
	}
	void CppCLI::GetBrushAttr( String^ info, [Out] String^% tempString )
	{
		char* lpText1 = nullptr;
		char* lpText2 = new char[100];
		// Konvertera String^ -> char*
		try
		{
			lpText1 = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();


			m_GameEngine->GetBrushAttr(lpText1, lpText2);
		}
		finally
		{
			tempString = gcnew String(lpText2);
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText1)); // Free memory
			delete lpText2;
		}
	}


	void CppCLI::RemoveSelectedEntities()
	{
		m_GameEngine->RemoveSelectedEntities();
	}

	void CppCLI::GetNrOfSelectedEntities( [Out] int% x )
	{
		int tempx;
		m_GameEngine->GetNrOfSelectedEntities(tempx);
		x = tempx;
	}

	void CppCLI::MouseInsideFrame( bool flag )
	{
		m_GameEngine->MouseInsideFrame(flag);
	}

	void CppCLI::MouseMove( int x, int y )
	{
		m_GameEngine->MouseMove(x,y);
	}

	void CppCLI::SetBrushAttr( String^ info, float size )
	{
		char* lpText = nullptr;
		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();

			m_GameEngine->SetBrushAttr(lpText, size);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::SetBrushAttr( String^ info, String^ stringValue )
	{
		char* lpText = nullptr;
		char* lpText1 = nullptr;
		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(info).ToPointer();
			lpText1 = (char*)Marshal::StringToHGlobalAnsi(stringValue).ToPointer();

			m_GameEngine->SetBrushAttr(lpText, lpText1);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText1)); // Free memory
		}
	}

}}}