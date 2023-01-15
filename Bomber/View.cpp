#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "View.h"
#include "Model.h"

View::View()  {
	model = NULL;
	// Default cell width
	CellWidth = 30;
	CellHeight = 30;

	BorderSize = 50;
	lineWidth = 4;
	// Screen params
	SceneWidth = FIELD_TYPE0 * CellWidth			
		//+ (FIELD_TYPE0 + 1) * lineWidth	
		+ BorderSize * 2;					
	SceneHeight = FIELD_TYPE0 * CellHeight	
		//+ (FIELD_TYPE0 + 1) * lineWidth	
		+ BorderSize * 2;					
	// Zerofill
	hRC = NULL;
	hDC = NULL;
	hWnd = NULL;
	// Initialy windo is active
	active = true;

	angle = 1;
	selectedMenuItem = 1; // 1 - for menu item "Start", 2 - for "Score", 3 - for "Exit"
}

View::~View() { KillGLWindow(); }

void View::setObj(GLuint vertexList) { this->vertexList = vertexList; }

void View::menuUp() { if (selectedMenuItem > 1) selectedMenuItem--; }

void View::menuDown() { if (selectedMenuItem < 3) selectedMenuItem++; }

int View::getSelectedMenuItem() { return selectedMenuItem; }

void View::ReSetModel(Model *mod) {	
	model = mod;
	// Cell params
	CellWidth = 30;
	CellHeight = 30;
	// Separator size
	BorderSize = 50;
	lineWidth = 4;
	// Screen params
	SceneWidth = model->Width * CellWidth			
		//+ (model->Width + 1) * lineWidth			
		+ BorderSize * 2;							
	SceneHeight = model->Height * CellHeight		
		//+ (model->Height + 1) * lineWidth			
		+ BorderSize * 2;
	active = true;
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	
	if(w != SceneWidth || h != SceneHeight)	{
		MoveWindow(hWnd, rect.left, rect.top, SceneWidth, SceneHeight, true);
	}
}

void View::ReleaseModel() {
	if(model == NULL) return;
	delete model;
	model = NULL;
}

int View::InitGL( GLvoid ) {
	BuildFont();
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	return true;
}

void drawBitmapText(char *string, float x, float y, float z)
{
	char *c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

int View::DrawGLScene(GLvoid) {
	glClearColor(0.0, 0.4, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, SceneWidth, 0, SceneHeight, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);

	if (model == NULL) {

		glPushMatrix();
		glTranslatef(SceneWidth / 2, SceneHeight / 2 + 160 - selectedMenuItem * 80, -100);
		glColor3f(1.0, .23, 0.27);
		glScalef(0.1, 0.1, 0.1);
		glRotatef(angle, 0, 1, 0);
		glCallList(vertexList);
		glPopMatrix();

		// Bomb rotation
		angle++;
		if (angle >= 360) angle = 0;

		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		
		// Draw menu items
		drawBitmapText("Start", SceneWidth / 2 - 25, SceneHeight / 2 + 80, -100);
		drawBitmapText("Score", SceneWidth / 2 - 25, SceneHeight / 2, -100);
		drawBitmapText("Exit" , SceneWidth / 2 - 25, SceneHeight / 2 - 80, -100);		

		return true;
	}


	glLoadIdentity();

	glColor3f(0.0f, 0.0f, 0.0f);
	// Draw field lines
	glBegin(GL_LINES);
	glVertex3f(BorderSize, SceneHeight - BorderSize, -100);
	glVertex3f(SceneWidth - BorderSize, SceneHeight - BorderSize, -100);
	glVertex3f(BorderSize, BorderSize, -100);
	glVertex3f(SceneWidth - BorderSize, BorderSize, -100);
	glVertex3f(BorderSize, BorderSize, -100);
	glVertex3f(BorderSize, SceneHeight - BorderSize, -100);
	glVertex3f(SceneWidth - BorderSize, BorderSize, -100);
	glVertex3f(SceneWidth - BorderSize, SceneHeight - BorderSize, -100);
	glEnd();
	// Draw field
	for (int y = 0; y < model->Height; y++)
		for (int x = 0; x < model->Width; x++)
        {
			switch (model->GetCell(x, y).state)
            {
			case CellState::CLOSED:
                drawClosedField(x, y);
                break;
			case CellState::OPEN:
                if (!model->GetCell(x, y).hasMine)
                {
					// Calculate bombs around
                    int minesAround = 0;
                    for (int yy = y - 1; yy <= y + 1; ++yy)
                        for (int xx = x - 1; xx <= x + 1; ++xx)
                        {
                            if ((xx == x && yy == y) ||
								xx < 0 || xx >= model->Width ||
								yy < 0 || yy >= model->Height)
                                continue;
							if (model->GetCell(xx, yy).hasMine)
                                ++minesAround;
                        }
					drawOpenedField(x, y, minesAround);
                }
                else					
                    drawMine(x, y);
                break;
			case CellState::FLAG:
                drawFlag(x, y);
                break;
            }
        }
		// Print game stats
		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos3f(BorderSize, BorderSize / 2.0f, -100);
		// Elapsed time
		glPrint("Time:%d", model->timer);	
		// Unmarked bombs
		glRasterPos3f(SceneWidth - BorderSize * 2.0f, BorderSize / 2.0f, -100);
		glPrint("Bomb left:%d", model->BombCount - model->FlagCount);
	return true;
}

GLvoid View::ReSizeGLScene( GLsizei width, GLsizei height ) {
	if (height == 0) height = 1;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SceneWidth, 0, SceneHeight, 0.1, 1000);	
	glMatrixMode(GL_MODELVIEW);
}

void View::drawClosedField(int x, int y) {
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
	glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
	glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
	glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
	glEnd();
	
	glColor3f(1.0f, 1.0f, 1.0f); 
    glBegin(GL_LINES);
	glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
    glEnd();
	
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
	glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
	glEnd();
}

void View::drawOpenedField(int x, int y, int minesAround) {
	drawOpenedField(x, y);
    if (minesAround > 0)
    {
        switch (minesAround)
        {
        case 1:
            glColor3f(0.0f, 1.0f, 0.0f);
            break;
        case 2:
            glColor3f(0.0f, 0.0f, 1.0f);
            break;
        case 3:
            glColor3f(1.0f, 0.0f, 0.0f);
            break;
        case 4:
            glColor3f(0.0f, 0.7f, 0.0f);
            break;
        case 5:
            glColor3f(0.5f, 0.4f, 0.0f);
            break;
        case 6:
            glColor3f(0.0f, 0.8f, 0.5f);
            break;
        case 7:
            glColor3f(0.1f, 0.1f, 0.1f);
            break;
        case 8:
            glColor3f(0.3f, 0.3f, 0.3f);
            break;
        }
		// Show bombs near current cell
		glRasterPos3f(BorderSize + x * CellWidth + (30 - 8) / 2 + 1, SceneHeight - (BorderSize + (y + 1) * CellHeight - 15), -100);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '0' + minesAround % 10);
    }
}

void View::drawOpenedField(int x, int y) {
	glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
	glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
	glColor3f(0.2f, 0.2f, 0.2f);
	glEnd();
    glBegin(GL_LINES);
	glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + y * CellHeight), -100);
	glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth - 1, SceneHeight - (BorderSize + (y + 1) * CellHeight - 1), -100);
    glEnd();
}

void View::drawFlag(int x, int y) {
	glColor3f(0.8f, 0.8f, 0.8f); 
    glBegin(GL_QUADS);
	glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + y * CellHeight), -100);
    glVertex3f(BorderSize + (x + 1) * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
    glVertex3f(BorderSize + x * CellWidth, SceneHeight - (BorderSize + (y + 1) * CellHeight), -100);
    glEnd();
	
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(BorderSize + x * CellWidth + CellWidth / 2, SceneHeight - (BorderSize + y * CellHeight + 3), -100);
    glVertex3f(BorderSize + x * CellWidth + CellWidth / 2, SceneHeight - (BorderSize + (y + 1) * CellHeight - 3), -100);
    glEnd();
	
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(BorderSize + x * CellWidth + CellWidth / 2, SceneHeight - (BorderSize + y * CellHeight + 3), -100);
    glVertex3f(BorderSize + x * CellWidth + CellWidth / 2 - 8, SceneHeight - (BorderSize + y * CellHeight + 3 + 10), -100);
    glVertex3f(BorderSize + x * CellWidth + CellWidth / 2, SceneHeight - (BorderSize + y * CellHeight + 3 + 10), -100);
	glEnd();
}

void View::drawMine(int x, int y) {
	drawOpenedField(x, y);
    glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f));
	glEnd();

	glLineWidth(2.5);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	float spikeSize = 8.5;

	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f - spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f));
	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f + spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f));

	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) + spikeSize);
	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) - spikeSize);

	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f - spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) + spikeSize);
	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f + spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) - spikeSize);

	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f + spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) + spikeSize);
	glVertex2f(BorderSize + x * CellWidth + CellWidth / 2.0f - spikeSize, SceneHeight - (BorderSize + y * CellHeight + CellHeight / 2.0f) - spikeSize);
	glEnd();

	glPointSize(1);
	glLineWidth(1.0);
}

POINT View::GetOGLPos(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
 
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	POINT p;
	p.x = posX;
	p.y = posY;
    return p;
}

POINT View::GetCellIndex(POINT point) {
	// Calc index
	POINT p;
	p.x = -1;
	p.y = -1;
	// Prevent calculation without model
	if(model == NULL) return p;
	// Find "X" coord
	for(int x = 0; x < model->Width && p.x == -1; x++) {
		double x1 = BorderSize + x * CellWidth;
		double x2 = BorderSize + (x + 1) * CellWidth;
		if(point.x > x1 && point.x < x2)
			p.x = x;
	}
	// Find "Y" coord
	for(int y = 0; y < model->Height && p.y == -1; y++)
	{
		double y1 = SceneHeight - (BorderSize + y * CellHeight);
		double y2 = SceneHeight - (BorderSize + (y + 1) * CellHeight);
		if(point.y < y1 && point.y > y2)
			p.y = y;
	}

	return p;
}

// Create font
GLvoid View::BuildFont(GLvoid)
{
	HFONT	font;										// Fond identifier
	HFONT	oldfont;									

	base = glGenLists(96);								// 96 symbols in list

	font = CreateFont(	-12,							// height
						0,								// width
						0,								
						0,								// orientation angle
						FW_BOLD,						// Bold
						FALSE,							// Italic
						FALSE,							// Stroke
						FALSE,							
						ANSI_CHARSET,					
						OUT_TT_PRECIS,					
						CLIP_DEFAULT_PRECIS,			
						ANTIALIASED_QUALITY,
						FF_DONTCARE|DEFAULT_PITCH,		
						L"Courier New");				// Font name

	oldfont = (HFONT)SelectObject(hDC, font);
	// Create font
	wglUseFontBitmaps(hDC, 32, 96, base);
	// Select old font
	SelectObject(hDC, oldfont);
	// Remove font
	DeleteObject(font);
}

GLvoid View::KillFont(GLvoid) { glDeleteLists(base, 96); }


GLvoid View::glPrint(const char *fmt, ...) {
	char		text[256];
	va_list		ap;
	if (fmt == NULL) return;
	
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);
	// Save list
	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	// Draw list
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

// Window initialize
BOOL View::CreateGLWindow(WNDPROC WndProc, LPCWSTR title, int bits) {
	GLuint					PixelFormat;
	WNDCLASS				wc;
	DWORD					dwExStyle;
	DWORD					dwStyle;
	RECT WindowRect;
	WindowRect.left			= (long)0;
	WindowRect.right		= (long)SceneWidth;
	WindowRect.top			= (long)0;
	WindowRect.bottom		= (long)SceneHeight;
	hInstance				= GetModuleHandle(NULL);
	wc.style				= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc			= (WNDPROC)WndProc;
	wc.cbClsExtra			= 0;
	wc.cbWndExtra			= 0;
	wc.hInstance			= hInstance;
	wc.hIcon				= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor				= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground		= NULL;
	wc.lpszMenuName			= NULL;
	
	wc.lpszClassName		= L"Bomber OpenGL Window Class";
	// Register window class
	if(!RegisterClass(&wc))
	{
		// Registration failed
		MessageBox( NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}
	// Setup window style
	dwExStyle						= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle							= WS_DLGFRAME;

	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );
	if(!( hWnd = CreateWindowEx(dwExStyle,							
								L"Bomber OpenGL Window Class",		// Class name
								title,								// Window title
								WS_CLIPSIBLINGS |					// Style
								WS_CLIPCHILDREN |
								dwStyle,							// Extended style
								0, 0,								// Window position
								WindowRect.right - WindowRect.left,
								WindowRect.bottom - WindowRect.top,
								NULL,								// No parent
								NULL,								// No menu
								hInstance,							// App handle
								NULL)))				
	{
		// Restore screen
		KillGLWindow();
		MessageBox( NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}

	HMENU hMenu = CreateMenu();
	HMENU hPopMenuFile = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hPopMenuFile, L"Game");

	AppendMenu(hPopMenuFile, MF_STRING, 1021, L"Start");
	AppendMenu(hPopMenuFile, MF_STRING, 1022, L"Statistic");
	AppendMenu(hPopMenuFile, MF_STRING, 1023, L"Exit");

	SetMenu(hWnd, hMenu);
	SetMenu(hWnd, hPopMenuFile);
	

	static  PIXELFORMATDESCRIPTOR pfd	=
	{
		sizeof(PIXELFORMATDESCRIPTOR),	
		1,								
		PFD_DRAW_TO_WINDOW |			
		PFD_SUPPORT_OPENGL |			
		PFD_DOUBLEBUFFER,				
		PFD_TYPE_RGBA,					
		bits,							
		0, 0, 0, 0, 0, 0,				
		0,								
		0,								
		0,								
		0, 0, 0, 0,						
		32,								
		0,								
		0,								
		PFD_MAIN_PLANE,					
		0,								
		0, 0, 0							
	};
	
	if(!(hDC = GetDC(hWnd))) {
		KillGLWindow();
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
		KillGLWindow();
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if(!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		KillGLWindow();
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if(!(hRC = wglCreateContext(hDC))) 	{
		KillGLWindow();
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if(!wglMakeCurrent(hDC, hRC)) {
		KillGLWindow();
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	if(!InitGL()) {
		KillGLWindow();
		MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}
	return true; 
}

GLvoid View::KillGLWindow(GLvoid) {

	if(hRC) {
		if(!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		if(!wglDeleteContext(hRC))	MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hRC = NULL;
	}

	if(hDC && !ReleaseDC(hWnd, hDC)) {
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}

	if(hWnd && !DestroyWindow(hWnd)) {
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}

	if(!UnregisterClass(L"Bomber OpenGL Window Class", hInstance)) {
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}
