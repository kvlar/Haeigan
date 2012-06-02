#pragma once
#include <d3d11.h>
#include <D3DX10math.h>

class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 positon;
		D3DXVECTOR4 color;
	};

public:
	ModelClass(void);
	ModelClass(const ModelClass&);
	~ModelClass(void);

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertex_buffer;
	ID3D11Buffer* m_index_buffer;
	int m_vertex_count, m_index_count;

};

