#include "Macros.h"
#include "Header.h"
#include "GameManager.h"

LRESULT WINAPI WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		result = DefWindowProc(hWnd, message, wParam, lParam);
	}

	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Framework::GameManager::IGameManager::Instantiate(hInstance, nShowCmd, SCREEN_WIDTH, SCREEN_HEIGHT, FULL_SCREEN);
	auto* pGameManager = Framework::GameManager::IGameManager::GetInstance();
	do
	{
		pGameManager->Run();

	} while (false);
	Framework::GameManager::IGameManager::Release();

	return 0;
}
