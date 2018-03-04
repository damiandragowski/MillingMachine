#include <windows.h>		// Header File For Windows

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "mtxlib.h"
#include "Simulator.h"
#include "resource.h"

#include <fstream>

using namespace std;

#define MAX_LOADSTRING 256


#pragma warning(disable:4244)

float ToFloat(std::string is)
{
    std::stringstream Stream;
    float ReturnValue;

    Stream << is;
    Stream >> ReturnValue;

    return ReturnValue;
}


HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool    bStop = true;

struct _MainData
{
	matrix44 modelView;
	matrix44 invmodelView;
	POINT ptMousePos;
	bool  bMouseMoving;
	Simulator sim;
}MainData;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
LRESULT CALLBACK	ConnectDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	ConnectDlg1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	//glLoadIdentity();									// Reset The Modelview Matrix
}

void changeRotation(int degreesY, int degreesX) 
{
	if ( degreesX ) {
		matrix44 A=RotateRadMatrix44('x', ((float)degreesX)/360);
		MainData.modelView = A * MainData.modelView;

		A=RotateRadMatrix44('x', ((float)-degreesX)/360);

		MainData.invmodelView = MainData.invmodelView * A;
	}
	if ( degreesY ) {
		matrix44 A=RotateRadMatrix44('y', ((float)degreesY)/360);
		MainData.modelView = A * MainData.modelView;

		A=RotateRadMatrix44('y', ((float)-degreesY)/360);

		MainData.invmodelView = MainData.invmodelView * A;
	}

}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	GLfloat  ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat  diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat  specular[] = { 0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat	 lightPos[] = { 0.0f, 0.0f, 150.0f, 1.0f };

	MainData.modelView.identity();
	MainData.invmodelView.identity();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);						
	glEnable(GL_DEPTH_TEST);						
	glEnable(GL_POLYGON_SMOOTH);
	
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,specular);
	glDepthFunc(GL_LEQUAL);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	changeRotation(0, -290);
	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	gluLookAt(0.0,0.0,350.0,0.0,0.0,-100.0f,0.0,1.0,0.0);
	glMultMatrixf(MainData.modelView.getV());		//camera
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat low_shininess[] = { 5.0 };
glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	/*
	glBegin(GL_LINES);
		glColor3d(1.0,0.0,0.0);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(500.0,0.0,0.0);
		glColor3d(0.0,1.0,0.0);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(0.0,500.0,0.0);

		glColor3d(0.0,0.0,1.0);
		glVertex3d(0.0,0.0,0.0);
		glVertex3d(0.0,0.0,500.0);
	glEnd();*/

	MainData.sim.Draw();


	glFlush();

	return TRUE;										
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	static PAINTSTRUCT ps;
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}
		case WM_LBUTTONDOWN:
			{
				MainData.bMouseMoving=true;
				MainData.ptMousePos.x = LOWORD(lParam); 
				MainData.ptMousePos.y = HIWORD(lParam);
				return 0;
			}
		case WM_MOUSEMOVE:
			{
				if ( MainData.bMouseMoving ) {
					POINT Pos;
					Pos.x = LOWORD(lParam); 
					Pos.y = HIWORD(lParam);
					changeRotation(Pos.x-MainData.ptMousePos.x, Pos.y-MainData.ptMousePos.y);
					MainData.ptMousePos=Pos;
					DrawGLScene();					// Draw The Scene
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				}
				return 0;
			}
		case WM_PAINT:
			{
					HDC hdc = BeginPaint(hWnd, &ps);
					DrawGLScene();					// Draw The Scene
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
					EndPaint(hWnd, &ps);
					break;
			}
		case WM_TIMER:
			{
				MainData.sim.Move();
				HDC hdc=GetDC(hWnd);
				DrawGLScene();					// Draw The Scene
				SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				ReleaseDC(hWnd, hdc);
			}
			break;
		case WM_LBUTTONUP:
			{
				MainData.bMouseMoving=false;
				return 0;
			}
		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
struct _ss
{
	vector3 size;
	float eps;
	float minz;
}s;

char bufftemp[256];
int refreshRate=1;
HANDLE thread=0;

DWORD WINAPI ThreadProc( LPVOID lpParameter )
{
	int i=0;
	MainData.sim.StartMachine(hWnd);
	while( MainData.sim.Move() ) 
	{
		if ( refreshRate <= i ) {
			i=0;
			InvalidateRect(hWnd, 0, false);
		}
		++i;
		sprintf(bufftemp, "Wykonano: %f procent", MainData.sim.getProc());
		SetWindowText(hWnd, bufftemp);
		while( bStop ) {
			Sleep(5);
		}
	}
	MainData.sim.StopMachine(hWnd);
	SetWindowText(hWnd, "Milling Machine Simulator v0.9");

	DialogBox(hInstance, (LPCTSTR)IDD_DIALOGERRORS, 0, (DLGPROC)ConnectDlg1);

	return 0;
}


int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	fullscreen=false;

	// Create Our OpenGL Window
	if (!CreateGLWindow("Milling Machine Simulator v0.9",1280,1024,32,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (GetMessage(&msg,0, 0,0) )
	{
		TranslateMessage(&msg);				// Translate The Message
		DispatchMessage(&msg);				// Dispatch The Message
		if (keys[VK_F1])						// Is F1 Being Pressed?
		{
			keys[VK_F1]=FALSE;					// If So Make Key FALSE
			KillGLWindow();						// Kill Our Current Window
			fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
			// Recreate Our OpenGL Window
			if (!CreateGLWindow("Milling Machine Simulator v0.9",1280,1024,16,fullscreen))
			{
				return 0;						// Quit If Window Was Not Created
			}
		}
		if ( keys[VK_UP] ) {
			keys[VK_UP]=false;
			refreshRate++;
		}
		if ( keys[VK_DOWN] ) {
			keys[VK_DOWN]=false;
			refreshRate--;
		}
		if ( keys[VK_LEFT] ) {
			keys[VK_LEFT]=false;
			MainData.sim.Move();
			InvalidateRect(hWnd, 0, false);
		}
		if ( keys[VK_SPACE] )
		{
			keys[VK_SPACE] = false;
			bStop = !bStop;
		}
		if ( keys[VK_F5] )
		{
			keys[VK_F5] = FALSE;
			TerminateThread(thread, 0);
		}
		if ( keys[VK_F4] )
		{
			keys[VK_F4] = FALSE;
			// dialog z materialem
			if ( DialogBox(hInstance, (LPCTSTR)IDD_DIALOGMAT, hWnd, (DLGPROC)ConnectDlg) == IDOK )
			{
				// zmien material
				MainData.sim.InitMat(s.eps,s.size,s.minz,5);
			}
	

		}
		if ( keys[VK_F3] )
		{
			keys[VK_F3] = FALSE;
		}
		if ( keys[VK_F2] )
		{
			keys[VK_F2] = FALSE;

			OPENFILENAME ofn;       // common dialog box structure
			char szFile[260];       // buffer for file name

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			//
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			//
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 
			DWORD id;
			if (GetOpenFileName(&ofn)==TRUE) 
				if ( !MainData.sim.LoadPath(ofn.lpstrFile) )
					MessageBox(0, TEXT("File Read Error"), TEXT("FileReadError"), 0);
				else 
					thread = CreateThread(0, 0, ThreadProc, 0, THREAD_PRIORITY_HIGHEST, &id);
		}
	}
	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}

LRESULT CALLBACK ConnectDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR * xx = "150";
	static TCHAR * yy = "150";
	static TCHAR * zz = "150";
	static TCHAR * minzz = "20";
	static TCHAR * dokl = "0.1";


	switch (message)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_EDIT1, xx);
			SetDlgItemText(hDlg, IDC_EDIT2, yy);
			SetDlgItemText(hDlg, IDC_EDIT3, zz);
			SetDlgItemText(hDlg, IDC_EDIT4, minzz);
			SetDlgItemText(hDlg, IDC_EDIT5, dokl);
			return TRUE;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK) {
				TCHAR xx1[MAX_LOADSTRING];
				TCHAR yy1[MAX_LOADSTRING];
				TCHAR zz1[MAX_LOADSTRING];
				TCHAR minzz1[MAX_LOADSTRING];
				TCHAR dokl1[MAX_LOADSTRING];

				int len = GetDlgItemText(hDlg, IDC_EDIT1, xx1, MAX_LOADSTRING);
				if ( len == 0 ) {
					MessageBox(hDlg, TEXT("Wrong number"), TEXT("error"), 0);
					return TRUE;
				}
				len = GetDlgItemText(hDlg, IDC_EDIT2, yy1, MAX_LOADSTRING);
				if ( len == 0 ) {
					MessageBox(hDlg, TEXT("Wrong number"), TEXT("error"), 0);
					return TRUE;
				}
				len = GetDlgItemText(hDlg, IDC_EDIT3, zz1, MAX_LOADSTRING);
				if ( len == 0 ) {
					MessageBox(hDlg, TEXT("Wrong number"), TEXT("error"), 0);
					return TRUE;
				}
				len = GetDlgItemText(hDlg, IDC_EDIT4, minzz1, MAX_LOADSTRING);
				if ( len == 0 ) {
					MessageBox(hDlg, TEXT("Wrong number"), TEXT("error"), 0);
					return TRUE;
				}
				len = GetDlgItemText(hDlg, IDC_EDIT5, dokl1, MAX_LOADSTRING);
				if ( len == 0 ) {
					MessageBox(hDlg, TEXT("Wrong number"), TEXT("error"), 0);
					return TRUE;
				}

				s.eps = ToFloat(dokl1);
				s.minz = ToFloat(minzz1);
				s.size[0] = ToFloat(xx1);
				s.size[1] = ToFloat(yy1);
				s.size[2] = ToFloat(zz1);
				
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK ConnectDlg1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			for ( list<string>::iterator i=MainData.sim.errors.begin(); i != MainData.sim.errors.end();++i)
				SendDlgItemMessage(hDlg, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)((*i).c_str()));
			return TRUE;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}