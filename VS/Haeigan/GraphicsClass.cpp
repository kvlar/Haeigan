#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& ref)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int& screen_width, int& screen_height, HWND hwnd)
{
	return true;
}

void GraphicsClass::Shutdown()
{
	return;
}


bool GraphicsClass::Frame()
{
	return true;
}


bool GraphicsClass::Render()
{
	return true;
}
