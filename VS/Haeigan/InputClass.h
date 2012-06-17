#pragma once

//-----
// DirectInput is Stupid
//
#define DIRECTINPUT_VERSION 0x0800

//-----
// Linking
//-----
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//-----
// Includes
//-----
#include "D3dclass.h"
#include <dinput.h>

#include "HMath.h"

//--------
// consts
//--------

const float SPEED_FACTOR = 0.2f;

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	bool IsEscapePressed();
	D3DXVECTOR3 GetMovement();
	float GetYRotation();
	float GetXRotation();
	void GetMouseLocation(int&, int&);

	bool IsKeyPressed(char);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboard_state[256];
	DIMOUSESTATE m_mouse_state;

	int m_screen_width, m_screen_height;
	int m_mouse_x, m_mouse_y;
};