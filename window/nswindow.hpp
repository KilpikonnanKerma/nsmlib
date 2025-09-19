/*

	Copyright (C) 2025 Nico Rajala. All rights reserved.

	NSWindow.hpp
	A simple cross-platform (win32 currently so not yet ;) ) windowing library using native APIs.

	Use at your own risk. No warranties are provided.

*/

#ifndef NSWINDOW_HPP
#define NSWINDOW_HPP

#include <stdexcept>
#include <queue>

#if defined(_WIN32)
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include <windows.h>
#endif

#ifdef NSWINDOW_IMPL_OPENGL
#include <gl/GL.h>
#endif

namespace NSWindow {

	constexpr int MaxTitleLen = 64;
	constexpr int MaxTextInput = 64;
	constexpr int MaxEvents = 16;

	enum class EventType {
		None, Close, Resize, Move, FocusGained, FocusLost, Minimized, Maximized, Restored
	};

	struct Event {
		EventType type;
		union {
			struct { int w, h; } size;
			struct { int x, y; } pos;
		};
	};

	struct WindowDesc {
		char title[MaxTitleLen] = "NSWindow";
		int width = 800;
		int height = 600;

		bool resizable = true;
		bool fullscreen = false;
		bool borderless = false;
	};

	class Window {
	public:
		Window(const WindowDesc& desc) {
			impl = new Impl(desc);
			impl->createGLContext();
		}

		~Window() {
			delete impl;
		}
		
		void pollEvents() { impl->pollEvents(); }

		bool shouldClose() const { return impl->shouldClose; }

		void* nativeHandle() const { return impl->nativeHandle(); }

		void setTitle(const char* title) { impl->setTitle(title); }
		const char* getTitle() const { return impl->title; }

		void setPosition(int x, int y) { impl->setPosition(x, y); }
		void getPosition(int& x, int& y) const { impl->getPosition(x, y); }

		void setSize(int w, int h) { impl->setSize(w, h); }
		void getSize(int& w, int& h) const { impl->getSize(w, h); }

		void setTopMost(bool topmost) { impl->setWindowTopMost(*this, topmost); }

		bool keyDown(int key) const { return impl->keyDown(key); }

		bool mouseButtonDown(int button) const { return impl->mouseButtonDown(button); }
		void getMousePosition(int& x, int& y) const { impl->getMousePosition(x, y); }

		void showCursor(bool show) { impl->showCursor(show); }
		void setCursorPos(int x, int y) { impl->setCursorPos(x, y); }
		bool pollEvent(Event& e) { return impl->pollEvent(e); }

		void swapBuffers() { impl->swapBuffers(); }

		float getDPIScale() { return impl->getDpiScaleForWindow(*this); }

#ifdef _WIN32
		HWND hwnd() const { return static_cast<HWND>(impl->nativeHandle()); }
		HDC hdc() const { return impl->hdc; }
#endif

#ifdef NSWINDOW_IMPL_OPENGL
		void makeGLCurrent() { impl->makeGLCurrent(); }
		void setVSync(int interval) { impl->setVSync(interval); }
#endif

	private:
		struct Impl {
			HWND hwnd = nullptr;
			char title[MaxTitleLen];
			bool shouldClose = false;
			bool keys[256] = {};
			bool mouseButtons[5] = {};
			int mouseX = 0, mouseY = 0;
			int mouseWheel = 0, mouseHWheel = 0;
			char textInputBuffer[MaxTextInput];
			std::queue<Event> eventQueue;

			HDC hdc = nullptr;
			HGLRC hglrc = nullptr;

#ifdef NSWINDOW_IMPL_OPENGL
			typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
#endif

			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
				Impl* self = reinterpret_cast<Impl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

				switch (uMsg) {
				case WM_CLOSE:
					if (self) {
						self->shouldClose = true;
						self->eventQueue.push({EventType::Close});
					}
					break;
				case WM_KEYDOWN:
					if (self && wParam < 256) self->keys[wParam] = true;
					break;
				case WM_KEYUP:
					if (self && wParam < 256) self->keys[wParam] = false;
					break;
				case WM_LBUTTONDOWN:
					if (self) self->mouseButtons[0] = true;
					break;
				case WM_LBUTTONUP:
					if (self) self->mouseButtons[0] = false;
					break;
				case WM_RBUTTONDOWN:
					if (self) self->mouseButtons[1] = true;
					break;
				case WM_RBUTTONUP:
					if (self) self->mouseButtons[1] = false;
					break;
				case WM_MBUTTONDOWN:
					if (self) self->mouseButtons[2] = true;
					break;
				case WM_MBUTTONUP:
					if (self) self->mouseButtons[2] = false;
					break;
				case WM_MOUSEMOVE:
					if (self) {
						self->mouseX = GET_X_LPARAM(lParam);
						self->mouseY = GET_Y_LPARAM(lParam);
					}
					break;
				case WM_MOUSEWHEEL:
					if (self) self->mouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
					break;
				case WM_MOUSEHWHEEL:
					if (self) self->mouseHWheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
					break;
				case WM_CHAR:
				    if (self) {
				        size_t len = strnlen_s(self->textInputBuffer, MaxTextInput - 1);
				        if (len < MaxTextInput - 1) {
				            self->textInputBuffer[len] = (char)wParam;
				            self->textInputBuffer[len + 1] = '\0';
				        }
				    }
				    break;
				case WM_SIZE:
				    if (self) {
				        int w = LOWORD(lParam), h = HIWORD(lParam);
				        self->eventQueue.push({EventType::Resize, {{w, h}}});
				        if (wParam == SIZE_MINIMIZED)
				            self->eventQueue.push({EventType::Minimized});
				        else if (wParam == SIZE_MAXIMIZED)
				            self->eventQueue.push({EventType::Maximized});
				        else if (wParam == SIZE_RESTORED)
				            self->eventQueue.push({EventType::Restored});
				    }
				    break;
				case WM_MOVE:
				    if (self) {
				        int x = (int)(short)LOWORD(lParam), y = (int)(short)HIWORD(lParam);
				        self->eventQueue.push({EventType::Move, {{x, y}}});
				    }
				    break;
				case WM_SETFOCUS:
				    if (self) self->eventQueue.push({EventType::FocusGained});
				    break;
				case WM_KILLFOCUS:
				    if (self) self->eventQueue.push({EventType::FocusLost});
				    break;
				default:
					break;
				}
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}

			Impl(const WindowDesc& desc) {
				strncpy(title, desc.title, MaxTitleLen - 1);
				title[MaxTitleLen - 1] = '\0';

				WNDCLASS wc = {};
				wc.lpfnWndProc = WindowProc;
				wc.hInstance = GetModuleHandle(nullptr);
				wc.lpszClassName = "NSWindowClass";
				RegisterClass(&wc);

				RECT rect = { 0, 0, desc.width, desc.height };
				AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

				DWORD style = WS_OVERLAPPEDWINDOW;
				if (!desc.resizable) style &= ~WS_THICKFRAME;
				if (desc.borderless) style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);

				hwnd = CreateWindowExW(
					0, L"NSWindowClass", std::wstring(title, title + strnlen(title, MaxTitleLen)).c_str(),
					style | WS_VISIBLE,
					CW_USEDEFAULT, CW_USEDEFAULT,
					rect.right - rect.left, rect.bottom - rect.top,
					nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

				if (!hwnd) {
					throw std::runtime_error("[NSWindow] Failed to create window");
				}

				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			}

			~Impl() {
				if (hwnd) {
					DestroyWindow(hwnd);
				}
			}

			float getDpiScaleForWindow(const Window& win) {
				HWND hwnd = static_cast<HWND>(win.nativeHandle());
				HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
				UINT xdpi = 96, ydpi = 96;
				typedef HRESULT(WINAPI* GetDpiForMonitorProc)(HMONITOR, int, UINT*, UINT*);
				static HMODULE shcore = LoadLibraryA("shcore.dll");
				static GetDpiForMonitorProc getDpi = shcore ? (GetDpiForMonitorProc)GetProcAddress(shcore, "GetDpiForMonitor") : nullptr;
				if (getDpi) {
					getDpi(monitor, 0 /* MDT_EFFECTIVE_DPI */, &xdpi, &ydpi);
				} else {
					HDC hdc = GetDC(hwnd);
					xdpi = GetDeviceCaps(hdc, LOGPIXELSX);
					ydpi = GetDeviceCaps(hdc, LOGPIXELSY);
					ReleaseDC(hwnd, hdc);
				}
				return xdpi / 96.0f;
			}

			void pollEvents() {
				MSG msg;
				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			bool pollEvent(Event& e) {
				if (eventQueue.empty()) return false;
				e = eventQueue.front();
				eventQueue.pop();
				return true;
			}

			void setTitle(const char* newTitle) {
				strncpy(title, newTitle, MaxTitleLen - 1);
				title[MaxTitleLen - 1] = '\0';
				SetWindowText(hwnd, title);
			}

			void setPosition(int x, int y) { SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
			void getPosition(int& x, int& y) const {
				RECT r; GetWindowRect(hwnd, &r); x = r.left; y = r.top;
			}

			void setSize(int w, int h) { SetWindowPos(hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
			void getSize(int& w, int& h) const {
				RECT r; GetClientRect(hwnd, &r); w = r.right - r.left; h = r.bottom - r.top;
			}

			inline void setWindowTopMost(Window& win, bool topmost) {
				HWND hwnd = static_cast<HWND>(win.nativeHandle());
				SetWindowPos(hwnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			}

			bool keyDown(int key) const {
				if (key < 0 || key >= 256) return false;
				return keys[key];
			}

			bool mouseButtonDown(int button) const {
				if (button < 0 || button >= 5) return false;
				return mouseButtons[button];
			}

			void showCursor(bool show) { ShowCursor(show); }
			void setCursorPos(int x, int y) { POINT pt = { x, y }; ClientToScreen(hwnd, &pt); SetCursorPos(pt.x, pt.y); }

			void getMousePosition(int& x, int& y) const {
				x = mouseX;
				y = mouseY;
			}

			void* nativeHandle() const {
				return hwnd;
			}

			void swapBuffers() {
				SwapBuffers(hdc);
			}

			// OpenGL context management

#ifdef NSWINDOW_IMPL_OPENGL

			void setVSync(int interval) {
				if (!wglSwapIntervalEXT) {
					wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
				}
				if (wglSwapIntervalEXT) {
					wglSwapIntervalEXT(interval);
				}
			}

			void createGLContext() {
				hdc = GetDC(hwnd);

				PIXELFORMATDESCRIPTOR pfd = {};
				pfd.nSize = sizeof(pfd);
				pfd.nVersion = 1;
				pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
				pfd.iPixelType = PFD_TYPE_RGBA;
				pfd.cColorBits = 32;
				pfd.cDepthBits = 24;
				pfd.cStencilBits = 8;
				pfd.iLayerType = PFD_MAIN_PLANE;

				int pf = ChoosePixelFormat(hdc, &pfd);
				SetPixelFormat(hdc, pf, &pfd);

				hglrc = wglCreateContext(hdc);
				wglMakeCurrent(hdc, hglrc);

				wglSwapIntervalEXT = nullptr;
			}

			void destroyGLContext() {
				if (hglrc) {
					wglMakeCurrent(nullptr, nullptr);
					wglDeleteContext(hglrc);
					hglrc = nullptr;
				}
				if (hdc) {
					ReleaseDC(hwnd, hdc);
					hdc = nullptr;
				}
			}

			void makeGLCurrent() { wglMakeCurrent(hdc, hglrc); }
#endif
		};

		Impl* impl;
	};
}

#endif