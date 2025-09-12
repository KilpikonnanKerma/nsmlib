#include "nswindow.hpp"
#include <iostream>
#include <cmath>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <gl/GL.h>

using namespace NSWindow;

int main() {
	Window window(WindowDesc{ "OpenGL Triangle", 800, 600, true, false });

	// Create OpenGL context if not already created in your header
	window.makeGLCurrent();

	float angle = 0.0f;

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

		// Swap buffers
		SwapBuffers((HDC)window.nativeHandle());
	}

	return 0;
}