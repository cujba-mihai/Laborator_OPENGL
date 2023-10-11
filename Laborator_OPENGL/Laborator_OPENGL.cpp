﻿#include <windows.h>
#include <GL/gl.h>
#pragma comment(lib,"OpenGL32.lib")
#include <GL/glu.h>
#pragma comment(lib,"Glu32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")
#include "glaux.h"
#pragma comment(lib,"Glaux.lib")
void CALLBACK resize(int width, int height)
{
	// Aici se indică partea ferestrei în care
	// se efectuează afişarea de către OpenGL.
	GLuint wp = width < height ? width - 20 : height - 20;
	glViewport(10, 10, wp, wp);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
// Stabilim tipul proiecţiei.
// glOrtho - paralelă
// glFrustum - perspectivă
// Parametrii la ambele funcţii sunt identici
// Ei stabilesc volumul care va fi vizibil
// peretele stânga - 6.2 unităţi în stânga
// dreapta - 6.2 unităţi în dreapta
// apoi pereţiile de jos şi de sus,
// şi în sfârşit pereţiile din faţă şi din spate
glOrtho(-6.2, 6.2, -6.2, 6.2, 2., 12.);
// glFrustum(-5,5, -5,5, 2,12);
glMatrixMode(GL_MODELVIEW);
}
void CALLBACK display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glClear( GL_COLOR_BUFFER_BIT );
	// glClear( GL_DEPTH_BUFFER_BIT );
	glPushMatrix();
	glTranslated(0., 0., -6.0);
	glRotated(35., 1., 0., 0.);
	glRotated(-35., 0., 1., 0.);
	glPointSize(10.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glColor3d(0, 0, 0);
	glVertex3f(0.f, 0.f, 0.f);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
	glLineWidth(1.5f);
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);
	// Axa X
	glColor3d(0., 0., 0.);
	glVertex3d(-5.5, 0., 0.);
	glColor3d(1., 0., 0.);
	glVertex3d(5.5, 0., 0.);
	// Axa Y
	glColor3d(0., 0., 0.);
	glVertex3d(0., -5.5, 0.);
	glColor3d(0., 1., 0.);
	glVertex3d(0., 5.5, 0.);
	// Axa Z
	glColor3d(0., 0., 0.);
	glVertex3d(0., 0., -5.5);
	glColor3d(0., 0., 1.);
	glVertex3d(0., 0., 5.5);
	glEnd();
	// Con X
	glColor3d(1, 0, 0);
	glPushMatrix();
	glTranslated(5.3f, 0.0f, 0.0f);
	glRotated(90, 0.0f, 1.0f, 0.0f);
	auxSolidCone(0.1f, 0.2f);
	glPopMatrix();
	// Con Y
	glColor3d(0, 1, 0);
	glPushMatrix();
	glTranslated(0.0f, 5.3f, 0.0f);
	glRotated(-90, 1.0f, 0.0f, 0.0f);
	auxSolidCone(0.1f, 0.2f);
	glPopMatrix();
	// Con Z
	glColor3d(0, 0, 1);
	glPushMatrix();
	glTranslated(0.0f, 0.0f, 5.3f);
	auxSolidCone(0.1f, 0.2f);
	glPopMatrix();
	//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
	// Aici vom adăuga fragmente de cod pentru diferite
// exemple, pe parcursul studierii funcţiilor OpenGL
 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	glPopMatrix();
	auxSwapBuffers();
}
int main()
{
	// Stabilim coordonatele ferestrei pe ecran
	// coltul stânga sus (0,0)
	// lăţimea şi înălţimea - 500
	auxInitPosition(0, 0, 500, 500);
	// Stabilim parametrii ale contextului OpenGL
	auxInitDisplayMode(AUX_RGB | AUX_DEPTH | AUX_DOUBLE);
	// Cream o fereastră pe ecran
	auxInitWindow(L"OpenGL");
		// Această fereastră va primi mesajele
		// de la tastatura, mouse-ul, timer-ul şi orice alte mesaje.
		// În timp ce nu vine nici un mesaj
		// va fi apelată ciclic funcţia display.
		// Astfel se creează animaţia.
		// Dacă avem nevoie de o imagine statică
		// comentăm următoarea linie
		auxIdleFunc(display);
	// In cazul când se schimbă dimensiunile ferestrei
	// vor fi primit mesajul corespunzător.
	// În Windows - acesta este WM_SIZE.
	// Indicăm funcţia resize ca funcţie care va fi
	// apelată fiecare dată când se schimbă dimensiunile
	// ferestrei
	auxReshapeFunc(resize);
	// Mai departe stabilim un set de teste si parametri
// Testul de transparentă, adică va fi luat în consideraţie
 // al patrulea parametru în glColor
	glEnable(GL_ALPHA_TEST);
	// testul de adâncime
	glEnable(GL_DEPTH_TEST);
	// funcţia glColor va stabili
	// proprietăţile materialelor.
	// De aceia nu este nevoie de apelul suplimentar
	// a funcţiei glMaterialfv
	glEnable(GL_COLOR_MATERIAL);
	// Permitem iluminarea
	glEnable(GL_LIGHTING);
	// Aprindem sursa de lumină cu numărul 0
	glEnable(GL_LIGHT0);
	// Stabilim poziţia sursei de lumină
	float pos[4] = { 3,3,3,1 };
	float dir[3] = { -1,-1,-1 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
	// Permitem amestecarea culorilor
	// (pentru suprafeţe transparente)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Stabilim culoarea de umplere iniţială a ferestrei
	glClearColor(1.f, 1.f, 1.f, 1.f);
	// Indicăm că funcţia display() va fi folosită pentru
	// redesenarea ferestrei.
	// Această funcţie va fi apelată de fiecare dată când
	// apare necesitatea de a redesena fereastra.
	// De exemplu, când fereastra se desfăşoară
	// pe întregul ecran (mesajul WM_PAINT in Windows)
	auxMainLoop(display);
	return 0;
}
