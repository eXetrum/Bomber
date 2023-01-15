#pragma once
#include "Model.h"
#include <windows.h>
#include "OpenGL\include\GL.h"
#include "OpenGL\include\glut.h"

class View {
public:
	// Ctor
	View();
	// Dtor
	~View();
	// Set vertices list from obj file
	void setObj(GLuint vertexList);
	// Change selected menu item
	void menuUp();
	void menuDown();
	int getSelectedMenuItem();
	// Set model
	void ReSetModel(Model *mod);
	// Remove model
	void ReleaseModel();
	// Initialize OpenGL
	int InitGL( GLvoid );
	// Draw scene items
	int DrawGLScene( GLvoid );
	// Resize
	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
	// Draw close cell
	void drawClosedField(int x, int y);
	// Draw open cell
    void drawOpenedField(int x, int y, int minesAround);
	void drawOpenedField(int x, int y);
	// Draw flag
    void drawFlag(int x, int y);
	// Draw bomb
    void drawMine(int x, int y);
	// Get OpenGL coords
	POINT GetOGLPos(int, int);
	// Get cell indes
	POINT GetCellIndex(POINT);
	// Create bitmap font
	GLvoid BuildFont(GLvoid);
	// Clear font resource
	GLvoid KillFont(GLvoid);
	// Print string
	GLvoid glPrint(const char *fmt, ...);
	// Window setup and create
	BOOL CreateGLWindow(WNDPROC WndProc, LPCWSTR title, int bits);
	// Destroy window
	GLvoid KillGLWindow( GLvoid );

	HGLRC hRC;
	HDC	hDC;
	HWND hWnd;
	HWND parentHWND;
	HINSTANCE hInstance;
	// Keyboard markers
	bool keys[256];
	// Window marker (active/hiden)
	bool active;
	// Cell params
	int CellWidth;
	int CellHeight;
	// Border param
	int BorderSize;
	// Line width between cells
	int lineWidth;
	// Font param
	GLuint	base;	
	// Draw params
	int SceneWidth, SceneHeight;
private:	
	GLuint vertexList;
	float angle;
	int selectedMenuItem;
	// Game model
	Model *model;
};