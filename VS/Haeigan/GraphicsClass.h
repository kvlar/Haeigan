#pragma once
//----------
// INCLUDES
//----------
#define NOMINMAX
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "BaseShaderClass.h"
#include "LightClass.h"

#include <vector>

#include <NxPhysics.h>

//----------
// CONSTANTS
//----------
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;




class GraphicsClass
{
	struct InnerModelData
	{
		int id;
	};
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int&, int&, HWND, NxScene*);
	
	void Shutdown();
	bool Frame(D3DXVECTOR3, float, float);

	CameraClass* GetCamera();
	bool AddCube(D3DXVECTOR3, D3DXVECTOR3, float);
	void AddCubeFromEye();
	void AddStack(int, float);
	void AddTower(int, float);

private:
	bool Render(float);
	bool InitializePhysX(NxScene*);
	bool AddPhysXCube(D3DXVECTOR3, D3DXVECTOR3, float);
	bool AddPhysXCube(D3DXVECTOR3, D3DXVECTOR3, float, D3DXVECTOR3);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	std::vector<ModelClass*> m_Models;
	BaseShaderClass* m_baseShader;
	LightClass* m_light;
	int m_model_count;

	//PhysX
	NxScene* n_scene;
};