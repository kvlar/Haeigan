#pragma once
//----------
// INCLUDES
//----------
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "BaseShaderClass.h"
#include "LightClass.h"

//----------
// CONSTANTS
//----------
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
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
	bool Frame(D3DXVECTOR3, float, float);

	CameraClass* GetCamera();

private:
	bool Render(float);
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	BaseShaderClass* m_baseShader;
	LightClass* m_light;
};