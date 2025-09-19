/*
	ns_immediate_gui.hpp

	Copyright (C) 2025 Nico Rajala. All rights reserved.

	An immediate mode GUI library for use with NSWindow library.

	Use at your own risk. No warranties are provided.

	---

	An immediate mode gui is defined and rendered every frame. It does not retain any state
	between frames, except for what the user of the library stores. This makes it very
	simple to use and integrate into existing rendering loops.
*/

#ifndef NS_IMMEDIATE_GUI_HPP
#define NS_IMMEDIATE_GUI_HPP

#include <windows.h>
#include <GL/gl.h>
#include <cstring>
#include <vector>
#include <algorithm>

#include "math/math.hpp"
using namespace NMATH;

namespace NSImgui {

	// 8x8 ASCII font (from https://github.com/dhepper/font8x8)
	static const unsigned char font8x8_basic[96][8] = {
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // (space)
		{0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // !
		{0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00}, // "
		{0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, // #
		{0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, // $
		{0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, // %
		{0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00}, // &
		{0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00}, // '
		{0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00}, // (
		{0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00}, // )
		{0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // *
		{0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, // +
		{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06}, // ,
		{0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00}, // -
		{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00}, // .
		{0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, // /
		{0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, // 0
		{0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00}, // 1
		{0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00}, // 2
		{0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00}, // 3
		{0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00}, // 4
		{0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00}, // 5
		{0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00}, // 6
		{0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00}, // 7
		{0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00}, // 8
		{0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00}, // 9
		{0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00}, // :
		{0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06}, // ;
		{0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00}, // <
		{0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00}, // =
		{0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // >
		{0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00}, // ?
		{0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00}, // @
		{0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, // A
		{0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00}, // B
		{0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00}, // C
		{0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00}, // D
		{0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00}, // E
		{0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00}, // F
		{0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00}, // G
		{0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, // H
		{0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // I
		{0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, // J
		{0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00}, // K
		{0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00}, // L
		{0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, // M
		{0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, // N
		{0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00}, // O
		{0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00}, // P
		{0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00}, // Q
		{0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00}, // R
		{0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00}, // S
		{0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // T
		{0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, // U
		{0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, // V
		{0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // W
		{0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, // X
		{0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, // Y
		{0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, // Z
		{0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, // [
		{0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, // backslash
		{0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, // ]
		{0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, // ^
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // _
		{0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, // `
		{0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00}, // a
		{0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00}, // b
		{0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00}, // c
		{0x38,0x30,0x30,0x3e,0x33,0x33,0x6E,0x00}, // d
		{0x00,0x00,0x1E,0x33,0x3f,0x03,0x1E,0x00}, // e
		{0x1C,0x36,0x06,0x0f,0x06,0x06,0x0F,0x00}, // f
		{0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F}, // g
		{0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, // h
		{0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00}, // i
		{0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E}, // j
		{0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00}, // k
		{0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // l
		{0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00}, // m
		{0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, // n
		{0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, // o
		{0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, // p
		{0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, // q
		{0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00}, // r
		{0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00}, // s
		{0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00}, // t
		{0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00}, // u
		{0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, // v
		{0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, // w
		{0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, // x
		{0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F}, // y
		{0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00}, // z
		{0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, // {
		{0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // |
		{0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, // }
		{0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}, // ~
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // (del)
	};

	// --- Core types and state and stuff ---
	struct WindowState;
	struct State {
		int mouseX, mouseY;
		bool mouseDown, mousePressed, mouseReleased;
		int hotItem, activeItem, lastWidget;
		int selectedWindow;
		WindowState* draggingWindow = nullptr;
		int dockHoverTarget = -1; // 0=left,1=right,2=top,3=bottom,4=center,-1=none
	};

	enum ResizeDir {
		None = 0,
		Left = 1,
		Right = 2,
		Bottom = 4,
		BottomLeft = Left | Bottom,
		BottomRight = Right | Bottom
	};

	struct WindowState {
		char title[64];
		float x, y, w, h;
		bool open;
		bool moving = false;
		bool resizing = false;
		float moveOffsetX = 0, moveOffsetY = 0;
		float resizeStartW = 0, resizeStartH = 0;
		float resizeStartX = 0, resizeStartY = 0;

		float userWidth = 0, userHeight = 0;
		bool userSized = false;

		float prevFloatX = 0, prevFloatY = 0, prevFloatW = 0, prevFloatH = 0;
		bool hasPrevFloatRect = false;

		float resizeStartWinX = 0;
		int resizingDir = 0;

		std::vector<WindowState*> dockedChildren;
		WindowState* dockParent = nullptr;
		int dockedTo = -1; // 0=left,1=right,2=top,3=bottom,4=center,-1=none
	};

	struct Layout {
		float cursorX, cursorY, startX, startY, availW;
		float spacingY;
		WindowState* win;
	};

	inline Layout*& GetLayout() {
		static Layout* layout = nullptr;
		return layout;
	}

	constexpr float WidgetMargin = 4.0f;

	// --- State management, windowing and layout stuff --- 

	inline State& GetState() {
		static State state;
		return state;
	}

	inline void NewFrame(int mouseX, int mouseY, bool mouseDown, bool mousePressed, bool mouseReleased) {
		State& s = GetState();
		s.mouseX = mouseX;
		s.mouseY = mouseY;
		s.mouseDown = mouseDown;
		s.mousePressed = mousePressed;
		s.mouseReleased = mouseReleased;
		s.hotItem = 0;
		s.lastWidget = 0;
	}

	inline std::vector<WindowState>& GetWindows() {
		static std::vector<WindowState> windows;
		return windows;
	}

	inline WindowState* FindWindow(const char* title) {
		auto& ws = GetWindows();
		for (auto& w : ws)
			if (std::strcmp(w.title, title) == 0)
				return &w;
		return nullptr;
	}

	inline WindowState* CreateOrGetWindow(const char* title, float x, float y, float w, float h) {
		WindowState* win = FindWindow(title);
		if (!win) {
			GetWindows().push_back(WindowState{});
			win = &GetWindows().back();
			std::strncpy(win->title, title, 63);
			win->title[63] = 0;
			win->x = x; win->y = y; win->w = w; win->h = h; win->open = true;
			win->dockParent = nullptr;
			win->dockedChildren.clear();
			win->dockedTo = -1;
		}
		return win;
	}

	inline int GetWindowIndex(WindowState* win) {
		auto& ws = GetWindows();
		for (size_t i = 0; i < ws.size(); i++) {
			if (&ws[i] == win) return (int)i;
		}
		return -1;
	}

	void handleResize(State& s, WindowState* win) {
		float mx = (float)s.mouseX, my = (float)s.mouseY;
		int winIdx = GetWindowIndex(win);

		const float edge = 6.0f;

		// Determine allowed resize directions based on docking
		bool allowLeft = true, allowRight = true, allowBottom = true;
		if (win->dockedTo != -1) {
			switch (win->dockedTo) {
			case 0: // left
			case 1: // right
				allowLeft = allowRight = true;
				allowBottom = false;
				break;
			case 2: // top
			case 3: // bottom
				allowLeft = allowRight = false;
				allowBottom = true;
				break;
			case 4: // center
			default:
				allowLeft = allowRight = allowBottom = true;
				break;
			}
		}
		switch (win->dockedTo) {
		case 0: // left
		case 1: // right
			allowLeft = allowRight = true;
			allowBottom = false;
			break;
		case 2: // top
		case 3: // bottom
			allowLeft = allowRight = false;
			allowBottom = true;
			break;
		case 4: // center
		case -1: // undocked
		default:
			allowLeft = allowRight = allowBottom = true;
			break;
		}

		bool overLeft = allowLeft && mx >= win->x - edge && mx <= win->x + edge && my > win->y + 24 && my < win->y + win->h - edge;
		bool overRight = allowRight && mx >= win->x + win->w - edge && mx <= win->x + win->w + edge && my > win->y + 24 && my < win->y + win->h - edge;
		bool overBottom = allowBottom && mx >= win->x + edge && mx <= win->x + win->w - edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;
		bool overBL = allowLeft && allowBottom && mx >= win->x - edge && mx <= win->x + edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;
		bool overBR = allowRight && allowBottom && mx >= win->x + win->w - edge && mx <= win->x + win->w + edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;

		if (s.selectedWindow == winIdx && s.mousePressed) {
			if (overBL) win->resizingDir = ResizeDir::BottomLeft;
			else if (overBR) win->resizingDir = ResizeDir::BottomRight;
			else if (overLeft) win->resizingDir = ResizeDir::Left;
			else if (overRight) win->resizingDir = ResizeDir::Right;
			else if (overBottom) win->resizingDir = ResizeDir::Bottom;
			else win->resizingDir = ResizeDir::None;

			if (win->resizingDir != ResizeDir::None) {
				win->resizing = true;
				win->resizeStartW = win->w;
				win->resizeStartH = win->h;
				win->resizeStartX = mx;
				win->resizeStartY = my;
				win->resizeStartWinX = win->x;
			}
		}
		if (!s.mouseDown) {
			win->resizing = false;
			win->resizingDir = ResizeDir::None;
		}

		if (win->resizing) {
			float minW = 128.0f, minH = 96.0f;
			if (win->resizingDir & ResizeDir::Left) {
				float dx = mx - win->resizeStartX;
				float newW = NMATH::maxf(minW, win->resizeStartW - dx);
				float newX = win->resizeStartWinX + dx;
				if (newW > minW) {
					win->x = newX;
					win->w = newW;
				}
			}
			if (win->resizingDir & ResizeDir::Right) {
				float dx = mx - win->resizeStartX;
				win->w = NMATH::maxf(minW, win->resizeStartW + dx);
			}
			if (win->resizingDir & ResizeDir::Bottom) {
				float dy = my - win->resizeStartY;
				win->h = NMATH::maxf(minH, win->resizeStartH + dy);
			}
			// Store user size for docked windows
			if (win->dockedTo == 0 || win->dockedTo == 1) { // left/right
				win->userWidth = win->w;
				win->userSized = true;
			}
			if (win->dockedTo == 2 || win->dockedTo == 3) { // top/bottom
				win->userHeight = win->h;
				win->userSized = true;
			}
		}

		// Cursor
		if (!win->resizing && s.selectedWindow == GetWindowIndex(win)) {
			const float edge = 6.0f;
			float mx = (float)s.mouseX, my = (float)s.mouseY;
			bool overLeft = mx >= win->x - edge && mx <= win->x + edge && my > win->y + 24 && my < win->y + win->h - edge;
			bool overRight = mx >= win->x + win->w - edge && mx <= win->x + win->w + edge && my > win->y + 24 && my < win->y + win->h - edge;
			bool overBottom = mx >= win->x + edge && mx <= win->x + win->w - edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;
			bool overBL = mx >= win->x - edge && mx <= win->x + edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;
			bool overBR = mx >= win->x + win->w - edge && mx <= win->x + win->w + edge && my >= win->y + win->h - edge && my <= win->y + win->h + edge;

			if (overBL)
				SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			else if (overBR)
				SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			else if (overLeft)
				SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			else if (overRight)
				SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			else if (overBottom)
				SetCursor(LoadCursor(NULL, IDC_SIZENS));
			else
				SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	inline void EndWindow() {
		GetLayout() = nullptr;
	}

	// --- Drawing ---

	inline void DrawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f) {
		glColor4f(r, g, b, a);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}

	inline void DrawRectOutline(float x, float y, float w, float h, float r, float g, float b) {
		glColor3f(r, g, b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}

	inline void DrawChar(float x, float y, char c, float r, float g, float b) {
		if (c < 32 || c > 127) return;
		const unsigned char* bitmap = font8x8_basic[c - 32];
		glColor3f(r, g, b);
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				if (bitmap[row] & (1 << col)) {
					float px = x + col;
					float py = y + row;
					glBegin(GL_QUADS);
					glVertex2f(px, py);
					glVertex2f(px + 1, py);
					glVertex2f(px + 1, py + 1);
					glVertex2f(px, py + 1);
					glEnd();
				}
			}
		}
	}

	inline void DrawText(float x, float y, const char* text, float r = 0, float g = 0, float b = 0) {
		float cx = x;
		for (const char* c = text; *c; ++c, cx += 8.0f)
			DrawChar(cx, y, *c, r, g, b);
	}

	// --- Widgets ---

	inline bool Button(const char* label, float x, float y, float w, float h) {
		State& s = GetState();
		int id = ++s.lastWidget;
		bool hovered = s.mouseX >= x && s.mouseX <= x + w && s.mouseY >= y && s.mouseY <= y + h;
		if (hovered) s.hotItem = id;
		bool pressed = false;

		if (hovered && s.mousePressed) s.activeItem = id;
		if (hovered && s.activeItem == id && s.mouseReleased) pressed = true;

		// Draw button
		if (s.activeItem == id && hovered)
			DrawRect(x, y, w, h, 0.6f, 0.6f, 0.8f);
		else if (hovered)
			DrawRect(x, y, w, h, 0.7f, 0.7f, 0.9f);
		else
			DrawRect(x, y, w, h, 0.8f, 0.8f, 0.95f);

		DrawRectOutline(x, y, w, h, 0.2f, 0.2f, 0.3f);

		// Center text
		float textWidth = (float)std::strlen(label) * 8.0f;
		DrawText(x + (w - textWidth) / 2, y + h / 2 - 4, label, 0, 0, 0);

		return pressed;
	}

	inline bool Checkbox(const char* label, bool* value, float x, float y) {
		State& s = GetState();
		int id = ++s.lastWidget;
		float boxSize = 16;
		bool hovered = s.mouseX >= x && s.mouseX <= x + boxSize && s.mouseY >= y && s.mouseY <= y + boxSize;
		if (hovered) s.hotItem = id;
		bool changed = false;

		if (hovered && s.mousePressed) s.activeItem = id;
		if (hovered && s.activeItem == id && s.mouseReleased) {
			*value = !*value;
			changed = true;
		}

		// Draw box
		DrawRect(x, y, boxSize, boxSize, 1, 1, 1);
		DrawRectOutline(x, y, boxSize, boxSize, 0.2f, 0.2f, 0.3f);
		if (*value) {
			DrawRect(x + 3, y + 3, boxSize - 6, boxSize - 6, 0.2f, 0.8f, 0.2f);
		}
		DrawText(x + boxSize + 6, y + boxSize - 4, label, 0, 0, 0);
		return changed;
	}

	inline void Label(const char* text) {
		Layout* l = GetLayout();
		if (!l) return;
		float x = l->cursorX + WidgetMargin;
		float y = l->cursorY;
		DrawText(x, y + 4, text, 1, 1, 1);
		l->cursorY += 20 + l->spacingY;
	}

	inline bool SliderFloat(const char* label, float* value, float min, float max) {
		Layout* l = GetLayout();
		if (!l) return false;
		float x = l->cursorX + WidgetMargin;
		float y = l->cursorY;
		float w = l->availW - WidgetMargin;
		float h = 20.0f;
		bool changed = false;

		// Draw background
		DrawRect(x, y, w, h, 0.85f, 0.85f, 0.90f);
		DrawRectOutline(x, y, w, h, 0.2f, 0.2f, 0.3f);

		// Calculate handle position
		float t = (*value - min) / (max - min);
		t = NMATH::clamp(t, 0.0f, 1.0f);
		float handleX = x + t * (w - 16);

		// Handle mouse
		State& s = GetState();
		int id = ++s.lastWidget;
		bool hovered = s.mouseX >= x && s.mouseX <= x + w && s.mouseY >= y && s.mouseY <= y + h;
		if (hovered) s.hotItem = id;
		static bool dragging = false;
		static int dragId = 0;
		if (hovered && s.mousePressed) {
			dragging = true;
			dragId = id;
		}
		if (!s.mouseDown) dragging = false;
		if (dragging && dragId == id) {
			float rel = (s.mouseX - x) / (w - 16);
			rel = NMATH::clamp(rel, 0.0f, 1.0f);
			float newValue = min + rel * (max - min);
			if (newValue != *value) {
				*value = newValue;
				changed = true;
			}
		}

		// Draw handle
		DrawRect(handleX, y, 16, h, 0.4f, 0.5f, 0.8f);

		// Draw label and value
		char buf[64];
		snprintf(buf, sizeof(buf), "%s: %.2f", label, *value);
		DrawText(x + 8, y + 4, buf, 0, 0, 0);

		l->cursorY += h + l->spacingY;
		return changed;
	}

	inline bool InputText(const char* label, char* buffer, int bufsize) {
		Layout* l = GetLayout();
		if (!l) return false;
		float x = l->cursorX + WidgetMargin;
		float y = l->cursorY;
		float w = l->availW - WidgetMargin;
		float h = 20.0f;
		bool changed = false;

		// Draw background
		DrawRect(x, y, w, h, 1, 1, 1);
		DrawRectOutline(x, y, w, h, 0.2f, 0.2f, 0.3f);

		// Handle mouse and keyboard
		State& s = GetState();
		int id = ++s.lastWidget;
		bool hovered = s.mouseX >= x && s.mouseX <= x + w && s.mouseY >= y && s.mouseY <= y + h;
		static int activeInput = 0;
		if (hovered && s.mousePressed) activeInput = id;
		bool active = (activeInput == id);

		// Draw text
		DrawText(x + 8, y + 4, buffer, 0, 0, 0);

		// Draw label
		DrawText(x + w - 8 - 8 * strlen(label), y + 4, label, 0.4f, 0.4f, 0.4f);

		// (You need to call this from your app's key input handler)
		// Example: if (active) { ... handle key input, update buffer, set changed = true; }

		l->cursorY += h + l->spacingY;
		return changed;
	}

	// In window versions
	inline bool Button(const char* label, float w = 80, float h = 30) {
		Layout* l = GetLayout();
		if (!l) return false;
		float x = l->cursorX, y = l->cursorY;
		l->cursorY += h + l->spacingY;
		return Button(label, x, y, w, h);
	}

	inline bool Checkbox(const char* label, bool* value) {
		Layout* l = GetLayout();
		if (!l) return false;
		float x = l->cursorX, y = l->cursorY;
		l->cursorY += 16 + l->spacingY;
		return Checkbox(label, value, x, y);
	}

	// --- GUI frame management ---

	// Call before drawing GUI widgets
	inline void BeginGUI(int fbWidth, int fbHeight) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, fbWidth, fbHeight, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
	}

	// Call after drawing GUI widgets
	inline void EndGUI() {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	// --- Docking system ---

	struct DockCube { float x, y, w, h; };

	struct DockDragState {
		WindowState* draggingWindow = nullptr;
		int dockHoverTarget = -1; // 0=left,1=right,2=top,3=bottom,4=center,-1=none
		WindowState* hoveredWindow = nullptr;
		bool hoveredGlobal = false;
		bool active = false;
		float dragOffsetX = 0, dragOffsetY = 0;
	};
	inline DockDragState& GetDockDragState() {
		static DockDragState state;
		return state;
	}

	// Draw dock targets for a window
	inline int DrawDockTargets(WindowState* win, float mx, float my, DockCube outCubes[5]) {
		float x = win->x, y = win->y, w = win->w, h = win->h;
		DockCube cubes[5] = {
			{x, y + h / 4, w / 6, h / 2}, // left
			{x + w - w / 6, y + h / 4, w / 6, h / 2}, // right
			{x + w / 4, y, w / 2, h / 6}, // top
			{x + w / 4, y + h - h / 6, w / 2, h / 6}, // bottom
			{x + w / 4, y + h / 4, w / 2, h / 2} // center
		};
		int hovered = -1;
		for (int i = 0; i < 5; ++i) {
			bool isHovered = mx >= cubes[i].x && mx <= cubes[i].x + cubes[i].w &&
				my >= cubes[i].y && my <= cubes[i].y + cubes[i].h;
			DrawRect(cubes[i].x, cubes[i].y, cubes[i].w, cubes[i].h,
				isHovered ? 0.3f : 0.2f, 0.5f, 1.0f, isHovered ? 0.5f : 0.3f);
			if (isHovered) hovered = i;
			if (outCubes) outCubes[i] = cubes[i];
		}
		return hovered;
	}

	// Draw dock targets for the global area (screen edges)
	inline int DrawGlobalDockTargets(float gx, float gy, float gw, float gh, float mx, float my, DockCube outCubes[4]) {
		// Only left/right/top/bottom, no center
		DockCube cubes[4] = {
			{gx, gy + gh / 4, gw / 32, gh / 2}, // left
			{gx + gw - gw / 32, gy + gh / 4, gw / 32, gh / 2}, // right
			{gx + gw / 4, gy, gw / 2, gh / 32}, // top
			{gx + gw / 4, gy + gh - gh / 32, gw / 2, gh / 32} // bottom
		};
		int hovered = -1;
		for (int i = 0; i < 4; ++i) {
			bool isHovered = mx >= cubes[i].x && mx <= cubes[i].x + cubes[i].w &&
				my >= cubes[i].y && my <= cubes[i].y + cubes[i].h;
			DrawRect(cubes[i].x, cubes[i].y, cubes[i].w, cubes[i].h,
				isHovered ? 0.3f : 0.2f, 0.7f, 1.0f, isHovered ? 0.5f : 0.3f);
			if (isHovered) hovered = i;
			if (outCubes) outCubes[i] = cubes[i];
		}
		return hovered;
	}

	// Remove a window from its parents dockedChildren
	inline void RemoveFromParent(WindowState* win) {
		if (win->dockParent) {
			auto& siblings = win->dockParent->dockedChildren;
			siblings.erase(std::remove(siblings.begin(), siblings.end(), win), siblings.end());
			win->dockParent = nullptr;
			win->dockedTo = -1;
		}
	}

	inline bool IsDescendant(WindowState* win, WindowState* possibleAncestor) {
		while (win) {
			if (win == possibleAncestor) return true;
			win = win->dockParent;
		}
		return false;
	}

	// Dock a window into another window or the global area
	inline void DockWindow(WindowState* win, WindowState* target, int dockTarget) {
		if (win == target || IsDescendant(win, target)) return;

		if (win->dockedTo == -1) {
			win->prevFloatX = win->x;
			win->prevFloatY = win->y;
			win->prevFloatW = win->w;
			win->prevFloatH = win->h;
			win->hasPrevFloatRect = true;
		}

		RemoveFromParent(win);
		if (target) {
			win->dockParent = target;
			win->dockedTo = dockTarget;
			target->dockedChildren.push_back(win);
			// Positioning will be handled in layout
		}
		else {
			win->dockParent = nullptr;
			win->dockedTo = dockTarget;
		}
	}

	// Dock a window to the global area (no parent)
	inline void DockWindowGlobal(WindowState* win, int dockTarget) {
		if (win->dockedTo == -1) {
			win->prevFloatX = win->x;
			win->prevFloatY = win->y;
			win->prevFloatW = win->w;
			win->prevFloatH = win->h;
			win->hasPrevFloatRect = true;
		}

		RemoveFromParent(win);
		win->dockParent = nullptr;
		win->dockedTo = dockTarget;
	}

	// Layout docked children inside a window
	inline void LayoutDockedChildren(WindowState* win) {
		if (win->dockedChildren.empty()) return;
		float x = win->x, y = win->y, w = win->w, h = win->h;
		int n = (int)win->dockedChildren.size();
		for (int i = 0; i < n; ++i) {
			WindowState* child = win->dockedChildren[i];
			if (child->dockedTo == -1) continue;

			switch (child->dockedTo) {
			case 0: // left
				child->x = x;
				child->y = y;
				child->w = w * 0.3f;
				child->h = h;
				break;
			case 1: // right
				child->x = x + w * 0.7f;
				child->y = y;
				child->w = w * 0.3f;
				child->h = h;
				break;
			case 2: // top
				child->x = x;
				child->y = y;
				child->w = w;
				child->h = h * 0.3f;
				break;
			case 3: // bottom
				child->x = x;
				child->y = y + h * 0.7f;
				child->w = w;
				child->h = h * 0.3f;
				break;
			case 4: // center
			default:
				child->x = x + w * 0.15f;
				child->y = y + h * 0.15f;
				child->w = w * 0.7f;
				child->h = h * 0.7f;
				break;
			}
			LayoutDockedChildren(child);
		}
	}

	// Layout all top-level windows (global area)
	inline void LayoutGlobalDockedWindows(float gx, float gy, float gw, float gh) {
		auto& windows = GetWindows();
		for (auto& win : windows) {
			if (!win.open) continue;
			if (!win.dockParent && win.dockedTo != -1) {
				// Layout based on dockedTo
				switch (win.dockedTo) {
				case 0: // left
					win.x = gx;
					win.y = gy;
					win.w = win.userSized ? win.userWidth : gw * 0.3f;
					win.h = gh;
					break;
				case 1: // right
					win.w = win.userSized ? win.userWidth : gw * 0.3f;
					win.x = gx + gw - win.w;
					win.y = gy;
					win.h = gh;
					break;
				case 2: // top
					win.x = gx;
					win.y = gy;
					win.w = gw;
					win.h = win.userSized ? win.userHeight : gh * 0.3f;
					break;
				case 3: // bottom
					win.x = gx;
					win.h = win.userSized ? win.userHeight : gh * 0.3f;
					win.y = gy + gh - win.h;
					win.w = gw;
					break;
				case 4: // center
				default:
					win.x = gx + gw * 0.15f;
					win.y = gy + gh * 0.15f;
					win.w = gw * 0.7f;
					win.h = gh * 0.7f;
					break;
				}
				LayoutDockedChildren(&win);
			}
		}
	}

	inline void EndFrame(float globalX, float globalY, float globalW, float globalH) {
		State& s = GetState();
		DockDragState& drag = GetDockDragState();
		if (s.mouseReleased)
			s.activeItem = 0;

		if (drag.active && drag.draggingWindow) {
			float mx = (float)s.mouseX, my = (float)s.mouseY;
			DockCube cubes[5];
			DockCube globalCubes[4];
			drag.hoveredWindow = nullptr;
			drag.hoveredGlobal = false;
			drag.dockHoverTarget = -1;

			// Check all windows for dock targets
			auto& windows = GetWindows();
			for (auto& win : windows) {
				if (!win.open || &win == drag.draggingWindow) continue;
				int hovered = DrawDockTargets(&win, mx, my, cubes);
				if (hovered != -1) {
					drag.hoveredWindow = &win;
					drag.dockHoverTarget = hovered;
				}
			}

			// Check global dock targets
			int globalHovered = DrawGlobalDockTargets(globalX, globalY, globalW, globalH, mx, my, globalCubes);
			if (globalHovered != -1) {
				drag.hoveredWindow = nullptr;
				drag.hoveredGlobal = true;
				drag.dockHoverTarget = globalHovered;
			}

			WindowState* win = drag.draggingWindow;

			// If window is docked and not over any dock target, undock immediately
			if (win->dockedTo != -1 && drag.dockHoverTarget == -1) {
				RemoveFromParent(win);
				win->dockedTo = -1;
				if (win->hasPrevFloatRect) {
					win->x = win->prevFloatX;
					win->y = win->prevFloatY;
					win->w = win->prevFloatW;
					win->h = win->prevFloatH;
				}
				// Recalculate drag offset so window sticks to mouse on title bar
				float titleBarY = win->y;
				float titleBarH = 24.0f;
				if (my < titleBarY || my > titleBarY + titleBarH)
					drag.dragOffsetY = titleBarH / 2.0f;
				else
					drag.dragOffsetY = my - win->y;
				drag.dragOffsetX = mx - win->x;
			}

			// Move the window with the mouse
			win->x = mx - drag.dragOffsetX;
			win->y = my - drag.dragOffsetY;

			// On mouse release, perform docking if over a dock target
			if (s.mouseReleased) {
				if (drag.dockHoverTarget != -1) {
					if (drag.hoveredGlobal) {
						DockWindowGlobal(win, drag.dockHoverTarget);
					}
					else if (drag.hoveredWindow) {
						DockWindow(win, drag.hoveredWindow, drag.dockHoverTarget);
					}
				}
				drag.draggingWindow = nullptr;
				drag.active = false;
			}
		}
	}

	inline bool BeginWindow(const char* title, float x, float y, float w, float h, float alpha, bool* pOpen = nullptr) {
		State& s = GetState();
		DockDragState& drag = GetDockDragState();
		auto& windows = GetWindows();
		WindowState* win = CreateOrGetWindow(title, x, y, w, h);
		if (!win->open) return false;
		if (pOpen) *pOpen = win->open;

		float mx = (float)s.mouseX, my = (float)s.mouseY;
		bool titleHovered = mx >= win->x && mx <= win->x + win->w && my >= win->y && my <= win->y + 24;
		bool resizeHovered = mx >= win->x + win->w - 12 && mx <= win->x + win->w && my >= win->y + win->h - 12 && my <= win->y + win->h;
		bool closeHovered = mx >= win->x + win->w - 24 && mx <= win->x + win->w - 8 && my >= win->y + 4 && my <= win->y + 20;

		int winIdx = GetWindowIndex(win);

		bool insideWindow = mx >= win->x && mx <= win->x + win->w && my >= win->y && my <= win->y + win->h;
		if (insideWindow && s.mousePressed && !closeHovered && !resizeHovered) {
			// Bring to front and select
			if (winIdx != (int)windows.size() - 1) {
				WindowState temp = *win;
				windows.erase(windows.begin() + winIdx);
				windows.push_back(temp);
				win = &windows.back();
				winIdx = (int)windows.size() - 1;
			}
			s.selectedWindow = winIdx;
		}

		if (titleHovered && s.mousePressed && !resizeHovered && !closeHovered) {
			drag.draggingWindow = win;
			drag.active = true;
			drag.dragOffsetX = mx - win->x;
			drag.dragOffsetY = my - win->y;
			for (size_t i = 0; i < windows.size(); ++i)
				windows[i].moving = false;
			win->moving = true;
			win->moveOffsetX = mx - win->x;
			win->moveOffsetY = my - win->y;
		}

		if (s.selectedWindow == winIdx && win->moving && !drag.active) {
			if (!s.mouseDown) win->moving = false;
			else {
				win->x = mx - win->moveOffsetX;
				win->y = my - win->moveOffsetY;
			}
		}
		else {
			win->moving = false;
		}

		handleResize(s, win);

		// Close
		if (closeHovered && s.mousePressed) {
			win->open = false;
			RemoveFromParent(win);
			if (pOpen) *pOpen = false;
			return false;
		}

		// Draw window background and border
		DrawRect(win->x, win->y, win->w, win->h, 0.30f, 0.30f, 0.30f, alpha);
		DrawRectOutline(win->x, win->y, win->w, win->h, 0.2f, 0.2f, 0.3f);
		// Draw title bar
		float barShade = (s.selectedWindow == winIdx) ? 0.07f : 0.13f;
		DrawRect(win->x, win->y, win->w, 24, barShade, barShade, barShade);
		DrawRectOutline(win->x, win->y, win->w, 24, 0.2f, 0.2f, 0.3f);
		DrawText(win->x + 8, win->y + 8, title, 1, 1, 1);

		// Draw close button
		DrawRect(win->x + win->w - 24, win->y + 4, 16, 16, closeHovered ? 0.8f : 0.7f, 0.3f, 0.3f);
		DrawRectOutline(win->x + win->w - 24, win->y + 4, 16, 16, 0.2f, 0.2f, 0.3f);
		DrawText(win->x + win->w - 20, win->y + 8, "X", 1, 1, 1);

		static Layout layout;
		layout.win = win;
		layout.startX = win->x + 8;
		layout.startY = win->y + 32;
		layout.cursorX = layout.startX;
		layout.cursorY = layout.startY;
		layout.availW = win->w - 16;
		layout.spacingY = 8;
		GetLayout() = &layout;

		return true;
	}

}

#endif