#include "GraphicsClass.h"
#include <cstdio>

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& ref)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int& screen_width, int& screen_height, HWND hwnd)
{
	bool result;

	// Create D3D object
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	//initialize D3D object
	result = m_D3D->Initialize(screen_width, screen_height, true, hwnd, \
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Couldn't initialize D3D", L"Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	//render graphics scene
	result = Render();
	if(!result)
	{
		return false;
	}
	return true;
}


bool GraphicsClass::Render()
{
	// Clear buffers
	// Cornflower blue #6495ED :)
	m_D3D->BeginScene(100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 1.0f);

	m_D3D->EndScene();
	return true;
}
