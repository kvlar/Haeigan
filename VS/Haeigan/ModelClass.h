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


class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 positon;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	ModelClass(void);
	ModelClass(const ModelClass&);
	~ModelClass(void);

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertex_buffer;
	ID3D11Buffer* m_index_buffer;
	int m_vertex_count, m_index_count;

	TextureClass* m_texture;

};

