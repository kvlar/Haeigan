#pragma once

//-----
// Includes
//-----

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

//-----
// constants
//-----
const std::string VS_VERSION = "vs_5_0";
const std::string PS_VERSION = "ps_5_0";

using namespace std;

class BaseShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DMATRIX world;
		D3DMATRIX view;
		D3DMATRIX projection;
	};
public:
	BaseShaderClass(void);
	BaseShaderClass(const BaseShaderClass&);
	~BaseShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrix_buffer;

	ID3D11SamplerState* m_sampleState;
};

