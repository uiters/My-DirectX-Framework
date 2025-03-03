// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "Macros.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// reference additional headers your program requires here

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <XInput.h>
#include <DirectXMath.h>

#include <Windows.h>

#include "tinyxml2.h"

#include <string>
#include <list>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include<ctime>

struct Bound;
struct Texture;
// Window event callback
LRESULT WINAPI WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// Library files for linking
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "XInput.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "X3DAudio.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")


// Type definition
typedef LPWSTR WString;
typedef LPCWSTR CWString;
typedef LPCSTR CString;
//typedef unsigned char Byte;

typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR2 Vector2;

typedef D3DCOLOR Color;
typedef IDirect3D9 Direct3D;
typedef IDirect3DDevice9 Device;
typedef IDirect3DSurface9 Surface;
typedef D3DXMATRIX Matrix;
typedef Bound Rect;

typedef IDirectInput8W Input;
typedef IDirectInputDevice8W InputDevice;
typedef DIMOUSESTATE MouseState;

typedef struct CUSTOMVERTEX
{
	FLOAT x, y, z;    // from the D3DFVF_XYZRHW flag
	DWORD color;    // from the D3DFVF_DIFFUSE flag
};

#include "Bound.h"
#include "Texture.h"
#include "Debug.h"
#include "Sprite.h"