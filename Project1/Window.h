#pragma once
#include "Header.h"

namespace Framework
{
	namespace Base
	{
		// Window Interface
		class IWindow
		{
			// Cons / Des
		protected:
			IWindow() = default;

		public:
			virtual ~IWindow() = default;

			// Getters / Setters
		public:

			virtual HWND Get_WindowHandle() = 0;

			// Pattern
		public:
			static IWindow* Instantiate(HINSTANCE hInstance, int nShowCmd, int screenWidth, int screenHeight, bool fullscreen);
		};
	}
}
