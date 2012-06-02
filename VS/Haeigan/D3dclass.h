#pragma once
//----------
// LINKING
//----------
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//----------
// INCLUDES
//----------
#include <dxgi.h>
#include <D3Dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

//----------
// CONSTS
//----------

const D3D_FEATURE_LEVEL M_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;
const D3D_DRIVER_TYPE M_DRIVER_TYPE = D3D_DRIVER_TYPE_HARDWARE;

//----------
// Classname: D3DClass
//----------

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_video_card_memory;
	char m_video_card_desc[128];
	
	IDXGISwapChain* m_swap_chain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_device_context;
	ID3D11RenderTargetView* m_render_target_view;
	ID3D11Texture2D* m_depth_stencil_buffer;
	ID3D11DepthStencilState* m_depth_stencil_state;
	ID3D11DepthStencilView* m_depth_stencil_view;
	ID3D11RasterizerState* m_raster_state;

	D3DXMATRIX m_projection_matrix;
	D3DXMATRIX m_world_matrix;
	D3DXMATRIX m_ortho_matrix;
};
