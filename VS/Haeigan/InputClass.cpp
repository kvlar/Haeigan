#include "InputClass.h"

InputClass::InputClass()
{
	m_direct_input = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& ref)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screen_width, int screen_height)
{
	HRESULT result;

	m_screen_width = screen_width;
	m_screen_height = screen_height;

	m_mouse_x = m_mouse_y = 0;

	// initialize direct input
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION,  IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if(FAILED(result))
	{
		return false;
	}
	//now the keyboard
	result = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}
	//and mouse
	result = m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}
	return true;
}

void InputClass::Shutdown()
{
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}
	if(m_direct_input)
	{
		m_direct_input->Release();
		m_direct_input = 0;
	}
}

bool InputClass::Frame()
{
	bool result;
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}
	result = ReadMouse();
	if(!result)
	{
		return false;
	}
	ProcessInput();
	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = m_keyboard->GetDeviceState(sizeof(m_keyboard_state), (LPVOID)&m_keyboard_state);
	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else 
		{
			return false;
		}
	}
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouse_state);
	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else 
		{
			return false;
		}
	}
	return true;
}

void InputClass::ProcessInput()
{
	m_mouse_x += m_mouse_state.lX;
	m_mouse_y += m_mouse_state.lY;
	m_mouse_x = HMath::Clamp(m_mouse_x, 0, m_screen_width);
	m_mouse_y = HMath::Clamp(m_mouse_y, 0, m_screen_height);

	return;
}

bool InputClass::IsEscapePressed()
{
	if(m_keyboard_state[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsKeyPressed(char c)
{
	switch(c)
	{
	case 'Y':
		return (m_keyboard_state[DIK_Y] & 0x80);
	
	case 'E':
		return (m_keyboard_state[DIK_E] & 0x80);

	case 'R':
		return (m_keyboard_state[DIK_R] & 0x80);
	case 'T': 
		return (m_keyboard_state[DIK_T] & 0x80);
	case ' ':
		return (m_keyboard_state[DIK_SPACE] & 0x80);
	}
	return false;
}

D3DXVECTOR3 InputClass::GetMovement()
{
	float x = 0.0f,y = 0.0f,z = 0.0f;
	if(m_keyboard_state[DIK_W] & 0x80) z = 1.0f;
	if(m_keyboard_state[DIK_S] & 0x80) z = -1.0f;
	if(m_keyboard_state[DIK_A] & 0x80) x = -1.0f;
	if(m_keyboard_state[DIK_D] & 0x80) x = 1.0f;
	
	
	
	D3DXVECTOR3 out(x,y,z);
	return SPEED_FACTOR * out;
}

float InputClass::GetYRotation()
{
	if(m_mouse_state.rgbButtons[0]  & 0x80)
	{
		return (float)m_mouse_state.lX * SPEED_FACTOR;
	}
	return 0.0f;
}

float InputClass::GetXRotation()
{
	if(m_mouse_state.rgbButtons[0]  & 0x80)
	{
		return (float)m_mouse_state.lY * SPEED_FACTOR;
	}
	return 0.0f;
}