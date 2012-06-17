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

#include <dinput.h>

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
	void GetMouseLocation(int&, int&);

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