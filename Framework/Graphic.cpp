#include "stdafx.h"

#include "Graphic.h"
#include "Macros.h"
#include "GameObject.h"

using namespace Framework;

CGraphic* CGraphic::__instance = nullptr;

void CGraphic::SetTransform(Matrix& orthographicMatrix, Matrix& identityMatrix, Matrix& viewMatrix)
{
	m_pDevice->SetTransform(D3DTS_PROJECTION, &orthographicMatrix);
	m_pDevice->SetTransform(D3DTS_WORLD, &identityMatrix);
	m_pDevice->SetTransform(D3DTS_VIEW, &viewMatrix);
}

bool CGraphic::Init(HWND hWind, bool fullscreen)
{
	bool result = false;
	do
	{
		// initialize Direct3D
		m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (!m_pDirect3D)
			break;
		
		// set Direct3D presentation parameters
		D3DPRESENT_PARAMETERS d3dpp;

		ZeroMemory(&d3dpp, sizeof(d3dpp));

		d3dpp.Windowed = !fullscreen;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount = 1;

		RECT rect;
		GetClientRect(hWind, &rect); // retrieve Window width & height 

		d3dpp.BackBufferWidth = rect.right + 1;
		d3dpp.BackBufferHeight = rect.bottom + 1;
		d3dpp.hDeviceWindow = hWind;

		// create Direct3D device
		m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWind,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pDevice);

		if (!m_pDevice)
			break;
		
		// clear the back buffer to black
		m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		// create pointer to the back buffer
		m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
		if (!m_pBackBuffer)
			break;
		
		// Initialize sprite helper from Direct3DX helper library
		D3DXCreateSprite(m_pDevice, &m_pSpriteHandler);
		if (!m_pSpriteHandler)
			break;
		
		result = true;
	} while (false);

	return result;
}

void CGraphic::Release()
{
	if (m_pDirect3D)
		m_pDirect3D->Release();

	if (m_pDevice)
		m_pDevice->Release();

	if (m_pBackBuffer)
		m_pBackBuffer->Release();

	if (m_pSpriteHandler)
		m_pSpriteHandler->Release();
}

bool CGraphic::Render(std::set<CGameObject*> list_game_objects)
{
	bool result = false;
	do
	{
		// Start rendering
		m_pDevice->BeginScene();
		// Clear back buffer with black color
		m_pDevice->ColorFill(m_pBackBuffer, nullptr, COLOR_BLACK);

		m_pSpriteHandler->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

		for (CGameObject* pGameObject : list_game_objects)
			pGameObject->Render();

		m_pSpriteHandler->End();

		// stop rendering
		m_pDevice->EndScene();

		// display back buffer content to the screen
		m_pDevice->Present(nullptr, nullptr, nullptr, nullptr);

		result = true;
	} while (false);

	return result;
}

void CGraphic::Draw(Texture* texture, Vector2 *position, Rect* pSourceRect, Vector2* offset)
{
	Vector3 *position3D = position ? new Vector3(position->x, position->y, 0) : nullptr;
	Vector3 *offset3D = offset ? new Vector3(offset->x, offset->y, 0) : nullptr;
	RECT* pRect = new RECT();

	if (pSourceRect) {
		pRect->top = pSourceRect->top;
		pRect->left = pSourceRect->left;
		pRect->right = pSourceRect->right;
		pRect->bottom = pSourceRect->bottom;
	}
	m_pSpriteHandler->Draw(texture, pRect, offset3D, position3D, COLOR_WHITE);
}

Texture* CGraphic::CreateTexture(LPCWSTR texturePath, DWORD &textureWidth, DWORD &textureHeight)
{
	Texture* m_texture = nullptr;
	do
	{
		D3DXIMAGE_INFO info;
		HRESULT hr = D3DXGetImageInfoFromFileW(texturePath, &info);
		if (hr != S_OK)
			break;

		textureWidth = info.Width;
		textureHeight = info.Height;

		hr = D3DXCreateTextureFromFileExW(
			m_pDevice,       // Pointer to Direct3D device object
			texturePath, // Path to the image to load
			info.Width,  // Texture width
			info.Height, // Texture height
			1,
			D3DUSAGE_DYNAMIC,
			D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			COLOR_BLACK, // Transparent color
			&info,
			nullptr,
			&m_texture // Created texture pointer

		);

		if (hr != S_OK)
			break;
		
	} while (false);

	return m_texture;
}

void CGraphic::Instantiate(HWND hWnd, bool fullscreen)
{
	if (!__instance)
	{
		SAFE_ALLOC(__instance, CGraphic);

		if (!__instance->Init(hWnd, fullscreen))
		{
			__instance->Release();
			SAFE_DELETE(__instance);
		}
	}
}

void CGraphic::Destroy()
{
	if (__instance)
	{
		__instance->Release();
		SAFE_DELETE(__instance);
	}
}

CGraphic* CGraphic::GetInstance()
{
	return __instance;
}
