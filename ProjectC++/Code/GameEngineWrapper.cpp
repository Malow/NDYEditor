#include <GameEngineWrapper.h>
#include "GameEngine.h"


GameEngineWrapper::GameEngineWrapper()
{
	m_GameEngine = new GameEngine();
}


GameEngineWrapper::~GameEngineWrapper()
{
	if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
}


unsigned int GameEngineWrapper::Init(unsigned int hWnd, int width, int height)	
{
	return m_GameEngine->Init(hWnd, width, height);
}


void GameEngineWrapper::Shutdown()
{
	if ( m_GameEngine ) delete m_GameEngine, m_GameEngine = 0;
}


void GameEngineWrapper::ProcessFrame()
{
	return m_GameEngine->ProcessFrame();
}


void GameEngineWrapper::OnResize(int width, int height)
{
	return m_GameEngine->OnResize(width, height);
}


char* GameEngineWrapper::ProcessText(char* msg)
{
	return m_GameEngine->ProcessText(msg);
}