#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& ref)
{
}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	for(int x = 0; x < 256; x++)
	{
		m_keys[x] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int input)
{
	return m_keys[input];
}