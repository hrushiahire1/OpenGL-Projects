// header files
#include <windows.h>
#include <stdio.h>	// for file I/O functions

#define _USE_MATH_DEFINES 1
#include <math.h>

// OpenGL header files
#include <GL/GL.h>
#include <GL/GLU.h>

// my header files
#include "OGL.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define SAFFRON_PLANE 1
#define WHITE_PLANE 2
#define GREEN_PLANE 3
#define A_ON_PLANE -1

// OpenGL Libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Glu32.lib")

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;
BOOL gbActiveWindow = FALSE;
FILE* gpFile = NULL;
int giWidth, giHeight;

int giFlag = -1;
float gf_firstI = -10.0f;
float gf_secondI = -10.0f;
float gf_N = 10.0f;
float gf_A = 15.0f;
float gf_orange_r = 0.0f;
float gf_orange_g = 0.0f;
float gf_orange_b = 0.0f;
float gf_white = 0.0f;
float gf_green_r = 0.0f;
float gf_green_g = 0.0f;
float gf_green_b = 0.0f;
float gf_x_whiteplane = -20.0f;
float gf_x_saffronplane = -10.0f;
float gf_y_saffronplane = 7.0f;
float gf_x_greenplane = -10.0f;
float gf_y_greenplane = 7.0f;
float gf_saffron_angle = 180.0f;
float gf_green_r_angle = 180.0f;
//float gf_rotate_s_angle = -180.0f;

// entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {

	// function declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	void ToggleFullScreen(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyAssignment");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// code
	if (fopen_s(&gpFile, "Logfile.txt", "w") != 0) {

		MessageBox(NULL, TEXT("Creation of Logfile failed\nExiting..."), TEXT("Error"), MB_OK);
		exit(0);
	}
	else {

		fprintf(gpFile, "Logfile successfully opened\n");
	}

	// initialize wndclass
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;

	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// register wndclass
	if (!RegisterClassEx(&wndclass)) {

		MessageBox(NULL, TEXT("wndclass was not registered"), TEXT("Error"), MB_ICONERROR);
		exit(0);
	}

	giWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	giHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	// create window in memory
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("HA : Dynamic India"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(giWidth / 2) - (WIN_WIDTH / 2),
		(giHeight / 2) - (WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,	// parent window handle
		NULL,	// menu handle
		hInstance,
		NULL	// struct CREATESTRUCT pointer -> creation parameter
	);

	if (hwnd == NULL) {

		MessageBox(NULL, TEXT("Window was not created in memory"), TEXT("Error"), MB_ICONERROR);
		exit(0);
	}

	ghwnd = hwnd;

	// initialize
	iRetVal = initialize();

	if (iRetVal == -1) {

		fprintf(gpFile, "ChoosePixelFormat() Failed\n");
		uninitialize();
	}
	else if (iRetVal == -2) {

		fprintf(gpFile, "SetPixelFormat() Failed\n");
		uninitialize();
	}
	else if (iRetVal == -3) {

		fprintf(gpFile, "CreateOpenGLContext() Failed\n");
		uninitialize();
	}
	else if (iRetVal == -4) {

		fprintf(gpFile, "Making OpenGL context as current context Failed\n");
		uninitialize();
	}
	else {

		fprintf(gpFile, "initialize() successfully\n");
	}

	// show window on screen
	ShowWindow(hwnd, iCmdShow);	// sets WM_VISIBLE flag

	// foregrounding and focusing the window
	SetForegroundWindow(hwnd);	// top most z order
	SetFocus(hwnd);

	ToggleFullScreen();

	// game loop
	while (bDone == FALSE) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {

				bDone = TRUE;
			}
			else {

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {

			if (gbActiveWindow == TRUE) {

				// render the scene
				display();

				// update the scene
				update();
			}
		}
	}

	uninitialize();
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	// function declarations
	void ToggleFullScreen(void);
	void resize(int, int);

	// code
	switch (iMsg) {
	case WM_SETFOCUS:
		fprintf(gpFile, "Inside WM_SETFOCUS\n");
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		fprintf(gpFile, "Inside WM_KILLFOCUS\n");
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		fprintf(gpFile, "Inside WM_ERASEBKGND");
		return 0;
	case WM_CHAR:
		fprintf(gpFile, "Inside WM_CHAR\n");
		switch (wParam) {
		case 'F':
		case 'f':
			// fprintf(gpFile, "iMsg = %u, wParam = %lld, lParam = %lld\n", iMsg, wParam, lParam);
			ToggleFullScreen();
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		fprintf(gpFile, "Inside WM_KEYDOWN\n");
		switch (wParam) {
		case 27:	// ascii of escape key
			DestroyWindow(hwnd);
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		fprintf(gpFile, "Inside WM_SIZE\n");
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_CLOSE:
		fprintf(gpFile, "Inside WM_CLOSE\n");
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		fprintf(gpFile, "Inside WM_DESTROY\n");
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullScreen(void) {

	// variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// code
	// initialize WINDOWPLACEMENT
	fprintf(gpFile, "Inside ToggleFullScreen()\n");
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE) {

		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {

			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {

				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else {

		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}
int initialize(void) {

	// function declarations
	void resize(int, int);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// code
	fprintf(gpFile, "Inside initialize()\n");
	PlaySound(MAKEINTRESOURCE(MYSOUND), NULL, SND_ASYNC | SND_RESOURCE | SND_LOOP);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));	// internally calls memset()

	// initialization of PIXELFORMATDESCRIPTOR
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;	// 24 also can be used

	// get DC (Device context)
	ghdc = GetDC(ghwnd);

	// choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {

		return -1;
	}

	// set the chosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {

		return -2;
	}

	// create OpenGL rendering context -> bridging API
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {

		return -3;
	}

	// make the rendering context as the current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {

		return -4;
	}

	// here starts OpenGL code
	// clear the screen using black color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// warm-up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height) {

	// code
	fprintf(gpFile, "Inside resize()\n");

	// to avoid divide by 0 (illegal instruction) in future code
	if (height <= 0) {

		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.5f, 20.0f);
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void) {

	// function declarations
	void DrawI(void);
	void DrawN(void);
	void DrawD(void);
	void DrawA(int);
	void DrawPlane(int);
	// void ToggleFullScreen(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	switch (giFlag) {
	default:
	case 5:
		glLoadIdentity();
		glTranslatef(gf_x_saffronplane, gf_y_saffronplane, -10.0f);
		glRotatef((gf_saffron_angle), 0.0f, 0.0f, 1.0f);
		glScalef(0.25f, 0.25f, 0.25f);
		DrawPlane(SAFFRON_PLANE);
		glLoadIdentity();
		glTranslatef(gf_x_whiteplane, 0.0f, -10.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.25f, 0.25f, 0.25f);
		DrawPlane(WHITE_PLANE);
		glLoadIdentity();
		glTranslatef(gf_x_greenplane, gf_y_greenplane, -10.0f);
		glRotatef((gf_green_r_angle)-180.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.25f, 0.25f, 0.25f);
		DrawPlane(GREEN_PLANE);
	case 4:
		glLoadIdentity();
		glTranslatef(gf_A, 0.0f, -12.0f);
		glScalef(0.96f, 1.0f, 1.0f);
		DrawA(0);
	case 3:
		glLoadIdentity();
		glTranslatef(2.5f, gf_secondI, -12.0f);
		DrawI();
	case 2:
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -12.0f);
		glScalef(1.1f, 1.0f, 1.0f);
		DrawD();
	case 1:
		glLoadIdentity();
		glTranslatef(-2.5f, gf_N, -12.0f);
		DrawN();
	case 0:
		glLoadIdentity();
		glTranslatef(gf_firstI, 0.0f, -12.0f);
		DrawI();
	}

	SwapBuffers(ghdc);

	if (giFlag == -1) {

		Sleep(3000);
		giFlag = 0;
	}
}

void update(void) {

	// code
	if (giFlag == 0) {
		gf_firstI = gf_firstI + 0.002f;

		if (gf_firstI >= -5.0f) {

			giFlag++;
		}
	}
	else if (giFlag == 1) {

		gf_N = gf_N - 0.002f;
		if (gf_N <= 0.0f) {

			giFlag++;
		}
	}
	else if (giFlag == 2) {

		gf_orange_r = gf_orange_r + 0.0002f;
		gf_orange_g = gf_orange_g + 0.0001f;
		gf_white = gf_white + 0.0002f;
		gf_green_g = gf_green_g + 0.0002f;

		if (gf_white >= 1.0f) {

			gf_orange_r = 1.0f;
			gf_orange_g = 0.6f;
			gf_orange_b = 0.2f;
			gf_green_r = 0.074509f;
			gf_green_g = 0.533333f;
			gf_green_b = 0.031372f;
			giFlag++;
		}
	}
	else if (giFlag == 3) {

		gf_secondI = gf_secondI + 0.002f;
		if (gf_secondI >= 0.0f) {

			giFlag++;
		}
	}
	else if (giFlag == 4){

		gf_A = gf_A - 0.002f;
		if (gf_A <= 5.0f) {

			giFlag++;
		}
	}
	else if (giFlag >= 5) {

			if (gf_saffron_angle <= 270.0f) {
				gf_x_whiteplane = gf_x_whiteplane + 0.0015f;
				gf_saffron_angle = gf_saffron_angle + 0.008f;
				gf_green_r_angle = gf_green_r_angle - 0.008f;
				gf_x_saffronplane = -3.5f + 6.2f * cos(gf_saffron_angle * (M_PI / 180.0f));
				gf_y_saffronplane = 6.5f + 6.2f * sin(gf_saffron_angle * (M_PI / 180.0f));
				gf_x_greenplane = -3.5f + 6.2f * cos(gf_green_r_angle * (M_PI / 180.0f));
				gf_y_greenplane = -6.5f + 6.2f * sin(gf_green_r_angle * (M_PI / 180.0f));
			}
			else if (gf_x_saffronplane <= 3.5f) {
				//fprintf(gpFile, "line : x = %f\n", gf_x_saffronplane);
				gf_x_saffronplane = gf_x_saffronplane + 0.0012f;
				gf_x_whiteplane = gf_x_whiteplane + 0.0012f;
				gf_x_greenplane = gf_x_greenplane + 0.0012f;
			}
			else if (gf_green_r_angle >= 0.0f) {

				if (gf_green_r_angle <= 84.0f) {
					giFlag = 6;
				}
				gf_x_whiteplane = gf_x_whiteplane + 0.0012f;
				gf_saffron_angle = gf_saffron_angle + 0.008f;
				gf_green_r_angle = gf_green_r_angle - 0.008f;
				gf_x_saffronplane = 3.5f + 6.2f * cos(gf_saffron_angle * (M_PI / 180.0f));
				gf_y_saffronplane = 6.5f + 6.2f * sin(gf_saffron_angle * (M_PI / 180.0f));
				gf_x_greenplane = 3.5f + 6.2f * cos(gf_green_r_angle * (M_PI / 180.0f));
				gf_y_greenplane = -6.5f + 6.2f * sin(gf_green_r_angle * (M_PI / 180.0f));
			}
	}
}

void uninitialize(void) {

	// function declarations
	void ToggleFullScreen(void);

	// code
	fprintf(gpFile, "Inside uninitialize()\n");

	if (gbFullScreen) {

		ToggleFullScreen();
	}

	if (wglGetCurrentContext() == ghrc) {

		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc) {

		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc) {

		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd) {

		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// stop sound
	PlaySound(NULL, 0, 0);

	if (gpFile) {

		fprintf(gpFile, "Logfile successfully closed\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void DrawQuad(void) {

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(0.074509f, 0.533333f, 0.031372f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
}

void DrawQuad_D(void) {

	glBegin(GL_QUADS);

	glColor3f(gf_orange_r, gf_orange_g, gf_orange_b);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glColor3f(gf_white, gf_white, gf_white);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(gf_white, gf_white, gf_white);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(gf_green_r, gf_green_g, gf_green_b);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
}

void DrawI(void) {

	// function declarations
	void DrawQuad(void);

	// code
	DrawQuad();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.25f, 0.5f, 0.0f);
	glVertex3f(-1.0f, 0.5f, 0.0f);
	glVertex3f(-1.0f, -0.5f, 0.0f);
	glVertex3f(-0.25f, -0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(1.0f, 0.5f, 0.0f);
	glVertex3f(1.0f, -0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void DrawN(void) {

	// function declaration
	void DrawQuad(void);

	// code
	DrawQuad();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.60f, 0.50f, 0.0f);
	glVertex3f(-0.60f, -1.0f, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);
	glVertex3f(-0.60f, 0.50f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.60f, -0.50f, 0.0f);
	glVertex3f(0.60f, 1.0f, 0.0f);
	glVertex3f(-0.50f, 1.0f, 0.0f);
	glVertex3f(0.60f, -0.50f, 0.0f);
	glEnd();
}

void DrawD(void) {

	// function declarations
	void DrawQuad_D(void);

	// code
	DrawQuad_D();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(-0.4f, 0.5f, 0.0f);
	glVertex3f(-0.4f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.8f, 0.5f, 0.0f);
	glVertex3f(-1.0f, 0.5f, 0.0f);
	glVertex3f(-1.0f, -0.5f, 0.0f);
	glVertex3f(-0.8f, -0.5f, 0.0f);
	glEnd();
}

void DrawA(int val) {

	// function declarations
	void DrawQuad(void);

	// code
	DrawQuad();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.35f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(-0.35f, 1.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.35f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.1f, 0.6f, 0.0f);
	glVertex3f(-0.1f, 0.6f, 0.0f);
	glVertex3f(-0.5f, -1.0f, 0.0f);
	glVertex3f(0.5f, -1.0f, 0.0f);
	glEnd();

	// 3 middle lines
	if (giFlag == 6 || val == -1) {

		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.21f, 0.15f, 0.0f);
		glVertex3f(-0.21f, 0.15f, 0.0f);
		glVertex3f(-0.26f, 0.0f, 0.0f);
		glVertex3f(0.26f, 0.0f, 0.0f);
		glEnd();
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.26f, 0.0f, 0.0f);
		glVertex3f(-0.26f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.15f, 0.0f);
		glVertex3f(0.3f, -0.15f, 0.0f);
		glEnd();
		glBegin(GL_QUADS);
		glColor3f(0.074509f, 0.533333f, 0.031372f);
		glVertex3f(0.3f, -0.15f, 0.0f);
		glVertex3f(-0.3f, -0.15f, 0.0f);
		glVertex3f(-0.35f, -0.3f, 0.0f);
		glVertex3f(0.35f, -0.3f, 0.0f);
		glEnd();
	}
}

void DrawF(void) {

	// function declaration
	void DrawQuad(void);

	// code
	DrawQuad();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.75f, 0.0f);
	glVertex3f(0.0f, 0.75f, 0.0f);
	glVertex3f(0.0f, 0.25f, 0.0f);
	glVertex3f(1.0f, 0.25f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(-0.5f, -1.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
}

void DrawPlane(int cFlag) {

	// function declaration
	void DrawI(void);
	void DrawA(int);

	// central triangle
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.25f, 0.0f, 0.0f);
	glVertex3f(1.25f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.25f, 0.0f, 0.0f);
	glVertex3f(-2.25f, -0.8f, 0.0f);
	glVertex3f(-1.25f, -1.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.25f, 0.0f, 0.0f);
	glVertex3f(2.25f, -0.8f, 0.0f);
	glVertex3f(1.25f, -1.0f, 0.0f);
	glEnd();

	// smoke
	glBegin(GL_QUADS);

	if (cFlag == 1)
		glColor3f(1.0f, 0.6f, 0.2f);
	else if (cFlag == 2)
		glColor3f(1.0f, 1.0f, 1.0f);
	else if (cFlag == 3)
		glColor3f(0.074509f, 0.533333f, 0.031372f);

	glVertex3f(0.0f, -0.7f, 0.0f);
	glVertex3f(0.0f, -0.8f, 0.0f);
	glVertex3f(-0.65f, -2.0f, 0.0f);
	glVertex3f(0.65f, -2.0f, 0.0f);
	glEnd();

	// tail
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, -0.05f, 0.0f);
	glVertex3f(-0.55f, -0.5f, 0.0f);
	glVertex3f(0.0f, -0.8f, 0.0f);
	glVertex3f(0.55f, -0.5f, 0.0f);
	glEnd();

	// I
	glTranslatef(-0.6f, 0.0f, 0.0f);
	glScalef(0.2f, 0.2f, 0.0f);
	DrawI();

	// A
	glTranslatef(3.3f, 0.2f, 0.0f);
	DrawA(A_ON_PLANE);

	// F
	glTranslatef(3.2f, -0.5f, 0.0f);
	DrawF();
}


