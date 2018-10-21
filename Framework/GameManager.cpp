#include "GameManager.h"
#include "Macros.h"
#include "Window.h"
#include "Graphic.h"
#include "Input.h"

using namespace Framework;

CGameManager* CGameManager::__instance = nullptr;
	
void CGameManager::SetCurrentScene(CScene* scene)
{
	m_currentScene = scene;
}

CScene* CGameManager::GetCurrentScene()
{
	return m_currentScene;
}

bool CGameManager::Init(HINSTANCE hInstance, int nShowCmd, int screenWidth, int screenHeight, bool fullscreen)
{
	bool result = false;
	do
	{
		m_pWindow = CWindow::Instantiate(hInstance, nShowCmd, screenWidth, screenHeight, fullscreen);

		if (!m_pWindow)
		{
			OutputDebugStringA("[Error] CWindow::Instantiate failed\n");
			break;
		}
		HWND hWnd = m_pWindow->Get_WindowHandle();

		// Init Direct3DCore
		CGraphic::Instantiate(hWnd, fullscreen);

		if (!CGraphic::GetInstance())
		{
			OutputDebugStringA("[Error] CGraphic::Instantiate failed\n");
			break;

		CInput::Instantiate();
		m_pInput = CInput::GetInstance();
		if (!m_pInput)
			break;

		result = true;
	} while (false);

	return result;
}

void CGameManager::Release()
{
	SAFE_DELETE(m_pWindow);
	CScene::Destroy(m_currentScene);
	CInput::Destroy();
}

bool CGameManager::Run()
{
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / FRAME_RATE;

	bool done = false;
	while (!done)
	{
		MSG message = {};
		if (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
		{
			// check for escape key (to exit program)
			if (message.message == WM_QUIT)
				done = true;

			// translate message and send back to WinProc
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD now = GetTickCount();
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			m_currentScene->Update(dt);
			m_currentScene->Render();
			/*for (auto lis_game_object : lis_game_objects)
			{
				lis_game_object->Update();
				auto x = lis_game_object->Get_Transform()->m_position.x;
				auto y = lis_game_object->Get_Transform()->m_position.y;
				if (y <= 0 && x <= SCREEN_WIDTH / 2)
					lis_game_object->Get_Transform()->m_position.x += dt / 10;
				else if (y < SCREEN_HEIGHT / 2 && x >= SCREEN_WIDTH / 2)
				{
					lis_game_object->Get_Transform()->m_position.y += dt / 10;
				}
				else if (x > 0 && y >= SCREEN_HEIGHT / 2)
					lis_game_object->Get_Transform()->m_position.x -= dt / 10;
				else
					lis_game_object->Get_Transform()->m_position.y -= dt / 10;
			}
*/
// process game loop
			/*bool renderResult = m_pGraphic->Render(m_gameObjectList);
			if (!renderResult)
				break;*/
			}
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return true;
}

//void CGameManager::AddGameObject(CGameObject* pGameObject)
//{
//	m_gameObjectList.push_back(pGameObject);
//}

void CGameManager::Instantiate(HINSTANCE hInstance, int nShowCmd, int screenWidth, int screenHeight, bool fullscreen)
{
	if (!__instance)
	{
		if (!__instance)
			SAFE_ALLOC(__instance, CGameManager)

		if (!__instance->Init(hInstance, nShowCmd, screenWidth, screenHeight, fullscreen))
			SAFE_DELETE(__instance);
	}
}

void CGameManager::Destroy()
{
	if(__instance)
	{
		__instance->Release();
		SAFE_DELETE(__instance);
	}
}

CGameManager * CGameManager::GetInstance()
{
	return __instance;
}
