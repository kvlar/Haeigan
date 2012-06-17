#pragma once

//-----
//includes
//-----

#include <d3d11.h>
#include <D3DX10math.h>

//------
// class includes
//------
#include "TextureClass.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>


class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 positon;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x,y,z;
		float tu, tv;
		float nx, ny, nz;
	};
	
public:
	ModelClass(void);
	ModelClass(const ModelClass&);
	~ModelClass(void);

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	void Translate(D3DXVECTOR3);
	void Rotate(D3DXVECTOR3);
	void Scale(float);
	D3DXMATRIX GetWorldTransformationMatrix();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer* m_vertex_buffer;
	ID3D11Buffer* m_index_buffer;
	int m_vertex_count, m_index_count;
	
	TextureClass* m_texture;
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_Rotation;
	float m_Scale;

	ModelType* m_model;
	int* m_model_indices;
};

