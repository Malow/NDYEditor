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
		if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
	}

	void CppCLI::ProcessFrame()
	{
		m_GameEngine->ProcessFrame();
	}

	void CppCLI::OnResize(int width, int height)
	{
		m_GameEngine->OnResize(width, height);
	}

	String^ CppCLI::ProcessText(String^ msg)
	{
		char* lpText = nullptr;
		String^ returnText;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(msg).ToPointer();

			returnText = gcnew String(m_GameEngine->ProcessText(lpText));
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}

		return returnText;
	}

	void CppCLI::CreateWorld( int x, int y )
	{
		this->m_GameEngine->CreateWorld(x, y);
	}

	void CppCLI::SaveWorld(String^ filePath)
	{
		char* lpText = nullptr;

		// Konvertera String^ -> char*
		try
		{
			lpText = (char*)Marshal::StringToHGlobalAnsi(filePath).ToPointer();

			m_GameEngine->SaveWorld(lpText);
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

			m_GameEngine->SaveWorld(lpText);
		}
		finally
		{
			Marshal::FreeHGlobal((IntPtr) const_cast<char*>(lpText)); // Free memory
		}
	}

	void CppCLI::OnLeftMouseUp( unsigned int x, unsigned int y )
	{
		m_GameEngine->OnLeftMouseUp(x,y);
	}

	void CppCLI::OnLeftMouseDown( unsigned int x, unsigned int y )
	{
		m_GameEngine->OnLeftMouseDown(x,y);
	}

	void CppCLI::ModeSelect( int mode )
	{
		this->m_GameEngine->ModeSelect(mode);
	}

	void CppCLI::Update()
	{
		this->m_GameEngine->Update();
	}

	void CppCLI::SetWindowFocused( bool value )
	{
		this->m_GameEngine->SetWindowFocused(value);
	}

}}}