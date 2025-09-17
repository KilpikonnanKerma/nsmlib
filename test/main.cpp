#include <iostream>
#include <cmath>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#endif

#define NSWINDOW_IMPL_OPENGL
#include "nswindow.hpp"

#include "ns_immediate_gui.hpp"

using namespace NSWindow;

int main() {
	Window window(WindowDesc{ "OpenGL Triangle", 800, 600, true, false });

	window.makeGLCurrent();

	float angle = 0.0f;
	bool show_demo = false;
	bool prevMouseDown = false;

	while (!window.shouldClose()) {
		window.pollEvents();

		NSWindow::Event e;
		while (window.pollEvent(e)) {
			if (e.type == EventType::Resize) {
				glViewport(0, 0, e.size.w, e.size.h);
			}
		}

		// Rotate with left/right arrow keys
		if (window.keyDown(VK_LEFT))  angle -= 1.0f;
		if (window.keyDown(VK_RIGHT)) angle += 1.0f;

		// OpenGL rendering
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int fbw, fbh, mx, my;
		window.getSize(fbw, fbh);
		window.getMousePosition(mx, my);
		bool mouseDown = window.mouseButtonDown(0);
		bool mousePressed = mouseDown && !prevMouseDown;
		bool mouseReleased = !mouseDown && prevMouseDown;
		prevMouseDown = mouseDown;

		NSImgui::NewFrame(mx, my, mouseDown, mousePressed, mouseReleased);

		NSImgui::BeginGUI(fbw, fbh);

		static bool show_demo = true;
		if (NSImgui::BeginWindow("Demo Window", 100, 100, 240, 180, &show_demo)) {
			if (NSImgui::Button("Rotate")) angle += 10.0f;
			NSImgui::Checkbox("Show Demo", &show_demo);
			NSImgui::EndWindow();
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(angle, 0.0f, 0.0f, 1.0f);

		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0); glVertex2f(0.0f, 0.5f);
		glColor3f(0, 1, 0); glVertex2f(-0.5f, -0.5f);
		glColor3f(0, 0, 1); glVertex2f(0.5f, -0.5f);
		glEnd();

		NSImgui::EndGUI();

		NSImgui::EndFrame();

		// Swap buffers
		window.swapBuffers();
	}

	return 0;
}