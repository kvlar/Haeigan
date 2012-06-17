#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_physx = 0;
}

SystemClass::SystemClass(const SystemClass& ref){}

SystemClass::~SystemClass()
{
}

bool SystemClass::InitializePhysX()
{
	NxSDKCreateError error_code;
	m_physx = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, (NxUserAllocator*)0,
		(NxUserOutputStream*)0, 
		NxPhysicsSDKDesc(),&error_code);

	if(!m_physx) return false;

	NxSceneDesc scene_desc;
	scene_desc.gravity.set(0,-9.81f, 0);
	m_physx_scene = m_physx->createScene(scene_desc);
	if(!m_physx_scene) return false;

	return true;
}

bool SystemClass::Initialize()
{
	int screen_width = 0;
	int screen_height = 0;
	bool result;

	InitializeWindows(screen_width, screen_height);

	result = InitializePhysX();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the PhysX.", L"Error", MB_OK);
		return false;
	}

	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screen_width, screen_height);
	if(!result) 
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screen_width, screen_height, m_hwnd, m_physx_scene);
	if(!result)
	{
		return false;
	}

	// initialize cube
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 rotation(0.0f, 0.0f, 0.0f);
	float scale = 1.0f;
	for(int i = 0; i < 8; i++){
		position.y += 8.0f;
		m_Graphics->AddCube(position, rotation, scale);
	}
	
	return true;
}

void SystemClass::ShutdownPhysX()
{
	if(m_physx_scene && m_physx)
	{
		m_physx->releaseScene(*m_physx_scene);
		m_physx_scene = 0;
	}
	if(m_physx)
	{
		NxReleasePhysicsSDK(m_physx);
		m_physx = 0;
	}
}

void SystemClass::Shutdown()
{
	// release graphics
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// release input
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	ShutdownPhysX();

	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// initialize msg
	ZeroMemory(&msg, sizeof(MSG));

	// loop until quit msg from system
	done = false;
	while(!done)
	{
		// handle windows msg
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				done = true;
			}
			

		}
		else
		{
			
			result = Frame();
			if(!result)
			{
				done = true;
			}
			if(m_Input->IsEscapePressed())
			{
				done = true;
			}
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;
	result = m_Input->Frame();
	if(!result) return false;

	D3DXVECTOR3 movement = m_Input->GetMovement();
	float rotationY = m_Input->GetYRotation();
	float rotationX = m_Input->GetXRotation();

	// some fun :)
	if(m_Input->IsKeyPressed(' '))
	{
		m_Graphics->AddCubeFromEye();
	}
	if(m_Input->IsKeyPressed('E'))
	{
		m_Graphics->AddStack(10, m_physx->getParameter(NX_SKIN_WIDTH));
	}
	if(m_Input->IsKeyPressed('R'))
	{
		m_Graphics->AddTower(10, m_physx->getParameter(NX_SKIN_WIDTH));
	}


	// do the graphics processing
	result = m_Graphics->Frame(movement, rotationX,rotationY);
	return result;	
}


// system callback for message processing
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& width, int& height)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX,posY;

	// get an external pointer to this object
	ApplicationHandle = this;

	// get the instance of the application
	m_hinstance = GetModuleHandle(NULL);

	// give the application a NAME
	m_applicationName = L"Haeigan Engine";

	// setup window class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)width;
		dmScreenSettings.dmPelsHeight = (unsigned long)height;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		width  = WINDOW_WIDTH;
		height = WINDOW_HEIGHT;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}
	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
				posX, posY, width, height, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;

}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// remove window
	DestroyWindow(m_hwnd);
	m_hwnd = 0;	

	// remove application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = 0;

	// release pointer
	ApplicationHandle = 0;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
