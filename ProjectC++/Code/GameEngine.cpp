#include "GameEngine.h"


GameEngine::GameEngine()
{
	m_pHge = NULL;

	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
}


GameEngine::~GameEngine()
{
	m_pHge->System_Shutdown();
	m_pHge->Release();
}


unsigned int GameEngine::Init(unsigned int hWnd, int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;

	m_pHge = hgeCreate(HGE_VERSION);
	//m_pHge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	//m_pHge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	m_pHge->System_SetState(HGE_FPS, 100);
	m_pHge->System_SetState(HGE_WINDOWED, true);
	m_pHge->System_SetState(HGE_SCREENWIDTH, width);
	m_pHge->System_SetState(HGE_SCREENHEIGHT, height);
	m_pHge->System_SetState(HGE_SCREENBPP, 32);
	m_pHge->System_SetState(HGE_HWNDPARENT, (HWND)hWnd);

	if(m_pHge->System_Initiate())
	{
		//Init hge stuff and so on
	}

	return 0;
}


void GameEngine::Shutdown()
{
}


void GameEngine::ProcessFrame()
{
	FrameFunc();
	RenderFunc();
}


void GameEngine::OnResize(int width, int height)
{
	MoveWindow(m_pHge->System_GetState(HGE_HWND),
           0, 0, width, height, true);

	m_ScreenWidth = width;
	m_ScreenHeight = height;
}


bool GameEngine::FrameFunc()
{
	if(m_pHge->Input_KeyDown(HGEK_ESCAPE))
		return true;

	return false;
}


void GameEngine::RenderFunc()
{
	m_pHge->Gfx_BeginScene();
	m_pHge->Gfx_Clear(ARGB(255, 0, 0, 255));
	m_pHge->Gfx_EndScene();
}


char* GameEngine::ProcessText(char* msg)
{
	return msg;
}