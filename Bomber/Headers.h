#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "OpenGL\\libs\\OpenGL32.lib")
//#pragma comment(lib, "OpenGL\\libs\\glu.lib")
#pragma comment(lib, "OpenGL\\libs\\glut.lib")
#pragma comment(lib, "OpenGL\\libs\\glut32.lib")
#pragma comment(lib, "OpenGL\\libs\\glew32")

#include "Game.h"
#include "Model.h"
#include "View.h"

#include "ObjReader .h"

#include <Commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>
using namespace std;

#define IDC_BUTTON                      1001
#define IDC_SPIN1                       1004
#define IDC_SPIN2                       1005
#define IDC_STATIC_1					1007
#define IDC_STATIC_2					1008
#define IDD_CHILDLG						1100
#define IDD_DIALOGBAR					103
#define IDC_EDIT1						1009
#define IDC_MENU_1						1021
#define IDC_MENU_2						1022
#define IDC_MENU_3						1023