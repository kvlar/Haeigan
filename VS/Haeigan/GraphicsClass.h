#pragma once
//----------
// INCLUDES
//----------
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "BaseShaderClass.h"

//----------
// CONSTANTS
//----------
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int&, int&, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	BaseShaderClass* m_baseShader;
};