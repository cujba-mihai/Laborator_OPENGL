#include <windows.h>
#include <GL/gl.h>
#pragma comment(lib,"OpenGL32.lib")
#include <GL/glu.h>
#pragma comment(lib,"Glu32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")
#include "glaux.h"
#pragma comment(lib,"Glaux.lib")



void CALLBACK resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CALLBACK display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Draw X-axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-5.0f, 0.0f, 0.0f);
    glVertex3f(5.0f, 0.0f, 0.0f);
    glEnd();

    // Draw Y-axis in green
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -5.0f, 0.0f);
    glVertex3f(0.0f, 5.0f, 0.0f);
    glEnd();

    auxSwapBuffers();
}

int main() {
    auxInitPosition(0, 0, 800, 600);
    auxInitDisplayMode(AUX_RGB | AUX_DEPTH | AUX_DOUBLE);
    auxInitWindow(L"OpenGL Example");
    auxIdleFunc(display);
    auxReshapeFunc(resize);

    glEnable(GL_DEPTH_TEST);

    auxMainLoop(display);
    return 0;
}
