#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& ref){}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screen_width = 0;
	int screen_height = 0;
	bool result;

	InitializeWindows(screen_width, screen_height);

	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	m_Input->Initialize();

	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screen_width, screen_height, m_hwnd);
	if(!result)
	{
		return false;
	}

	return true;
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
		delete m_Input;
		m_Input = 0;
	}

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
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				result = Frame();
				if(!result)
				{
					done = true;
				}
			}

		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;
	// check if user pressed escape
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// do the graphics processing
	result = m_Graphics->Frame();
	return result;	
}


// system callback for message processing
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}