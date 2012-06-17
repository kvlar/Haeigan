#pragma once

//-------------
// preprocessing directives
//-------------

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

//-------------
// includes
//-------------

#include <Windows.h>

#include <NxPhysics.h>

#include "InputClass.h"
#include "GraphicsClass.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;


//-------------
//SystemClass
//-------------
class SystemClass 
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	bool InitializePhysX();
	void ShutdownPhysX();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;

private: // PHYSX
	NxPhysicsSDK* m_physx;
	NxScene* m_physx_scene;
};

//-------------
//FUNCTION PROTOTYPES 
//-------------
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//-------------
//GLOBALS 
//-------------
static SystemClass* ApplicationHandle = 0;

