#pragma once
#include "D3dclass.h"


D3DClass::D3DClass()
{
	m_swap_chain = 0;
	m_device = 0;
	m_device_context = 0;
	m_render_target_view = 0;
	m_depth_stencil_buffer = 0;
	m_depth_stencil_state = 0;
	m_depth_stencil_view = 0;
	m_raster_state = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screen_width, int screen_height, bool vsync, HWND hwnd, \
	bool fullscreen, float screen_depth, float screen_near)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapter_output;
	unsigned int num_modes, i, numerator, denominator, str_lenght;
	DXGI_MODE_DESC* display_mode_list;
	DXGI_ADAPTER_DESC adapter_desc;
	int error;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	D3D_FEATURE_LEVEL feature_level;
	ID3D11Texture2D* back_buffer_ptr;
	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	D3D11_RASTERIZER_DESC raster_desc;
	D3D11_VIEWPORT viewport;
	float field_of_view, screen_aspect;

	// store vsync
	m_vsync_enabled = vsync;

	// create dx graphics factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// use factory to create an adapter for graphics iface (video card)
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result)){
		return false;
	}
	
	//enumerate primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapter_output);
	if(FAILED(result))
	{

		return false;
	}

	// get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM 
	// display format for the adapter output(monitor)
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, \
		DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
	if(FAILED(result))
	{

		return false;
	}

	// create a list to hold all the possible display modes for this
	// monitor/video card combination
	display_mode_list = new DXGI_MODE_DESC[num_modes];
	if(!display_mode_list)
	{
		return false;
	}

	// fill that structure
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, \
		DXGI_ENUM_MODES_INTERLACED, &num_modes,display_mode_list);
	if(FAILED(result))
	{

		return false;
	}

	// find that damn matching mode and store numerator and denominator of refresh rate
	for(i=0; i < num_modes; i++)
	{
		if(display_mode_list[i].Width == (unsigned int)screen_width && \
			display_mode_list[i].Height == (unsigned int)screen_height)
		{
			numerator = display_mode_list[i].RefreshRate.Numerator;
			denominator = display_mode_list[i].RefreshRate.Denominator;
		}
	}

	//retrieve amount of memory and card name
	result = adapter->GetDesc(&adapter_desc);
	if(FAILED(result))
	{
		return false;
	}

	// store memory in MBs
	m_video_card_memory = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	// convert the name of card to char* and store it
	error = wcstombs_s(&str_lenght, m_video_card_desc, 128, adapter_desc.Description, 128);
	if(error)
	{
		return false;
	}

	// releasing some structures
	delete[] display_mode_list;
	display_mode_list = 0;

	adapter_output->Release();
	adapter_output = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	// DX initialization
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

	swap_chain_desc.BufferCount = 1;

	swap_chain_desc.BufferDesc.Width = screen_width;
	swap_chain_desc.BufferDesc.Height = screen_height;

	// set regular 32bit surface for BB
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	if(m_vsync_enabled)
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else 
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// set the usage of BB
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;

	//turn multisampling off
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	// set to fullscreen 
	swap_chain_desc.Windowed = !fullscreen;

	// Set the scan line ordering and scaling to unspecified.
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// discard contents after presenting
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// no more flags
	swap_chain_desc.Flags = 0;

	// FEATURE LEVEL
	// can be set to:
	// 9,10 or 11
	feature_level = M_FEATURE_LEVEL;

	// create swapchain and D3D devices
	// replace D3D_DRIVER_TYPE_HARDWARE with D3D_DRIVER_TYPE_REFERENCE 
	// if you don't have DX11 VCARD
	result = D3D11CreateDeviceAndSwapChain(NULL, M_DRIVER_TYPE, NULL, \
		0, &feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc, &m_swap_chain, \
		&m_device, NULL, &m_device_context);
	if(FAILED(result))
	{
		//TODO: Add D3D_DRIVER_TYPE_REFERENCE  fallback
		return false;
	}

	// almost there
	result = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
	if(FAILED(result))
	{
		return false;
	}
	// create render target view
	result = m_device->CreateRenderTargetView(back_buffer_ptr, NULL, &m_render_target_view);
	if(FAILED(result))
	{
		return false;
	}

	// release ptr as we won't need it anymore
	back_buffer_ptr->Release();
	back_buffer_ptr = 0;

	//depth buffer and stencil buffer
	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
	depth_buffer_desc.Width = screen_width;
	depth_buffer_desc.Height = screen_height;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;


	result = m_device->CreateTexture2D(&depth_buffer_desc, NULL, &m_depth_stencil_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// depth stencil buff
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	// stencil operations if pixel is front-facing
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//backface
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// create depth stencil state
	result = m_device->CreateDepthStencilState(&depth_stencil_desc, &m_depth_stencil_state);
	if(FAILED(result))
	{
		
		return false;
	}

	m_device_context->OMSetDepthStencilState(m_depth_stencil_state, 1);

	// depth stencil view desc
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	// create
	result = m_device->CreateDepthStencilView(m_depth_stencil_buffer,&depth_stencil_view_desc, &m_depth_stencil_view);
	if(FAILED(result))
	{
		
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);

	//rasterizer
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	//create rasterizer
	result = m_device->CreateRasterizerState(&raster_desc, &m_raster_state);
	if(FAILED(result))
	{
		
		return false;
	}

	// set it
	m_device_context->RSSetState(m_raster_state);

	//viewport
	viewport.Width = (float)screen_width;
	viewport.Height = (float)screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_device_context->RSSetViewports(1, &viewport);

	//setting projection matrix
	field_of_view = (float)D3DX_PI / 4.0f;
	screen_aspect = (float)screen_width / (float)screen_height;

	D3DXMatrixPerspectiveFovLH(&m_projection_matrix, field_of_view, screen_aspect, screen_near, screen_depth);

	//world matrix
	D3DXMatrixIdentity(&m_world_matrix);

	//ortho matrix for 2D render
	D3DXMatrixOrthoLH(&m_ortho_matrix, (float)screen_width, (float)screen_height, screen_near, screen_depth);

	// FINALLY!
	return true;

}

void D3DClass::Shutdown()
{
	// set to windowed so it won't throw anything
	if(m_swap_chain)
	{
		m_swap_chain->SetFullscreenState(false, NULL);
	}

	if(m_raster_state)
	{
		m_raster_state->Release();
		m_raster_state = 0;
	}

	if(m_depth_stencil_view)
	{
		m_depth_stencil_view->Release();
		m_depth_stencil_view = 0;
	}

	if(m_depth_stencil_state)
	{
		m_depth_stencil_state->Release();
		m_depth_stencil_state = 0;
	}

	if(m_depth_stencil_buffer)
	{
		m_depth_stencil_buffer->Release();
		m_depth_stencil_buffer = 0;
	}

	if(m_render_target_view)
	{
		m_render_target_view->Release();
		m_render_target_view = 0;
	}

	if(m_device_context)
	{
		m_device_context->Release();
		m_device_context = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swap_chain)
	{
		m_swap_chain->Release();
		m_swap_chain = 0;
	}
	return;

}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = {red, green, blue, alpha};

	// clear back buffer
	m_device_context->ClearRenderTargetView(m_render_target_view, color);

	// clear depth buff
	m_device_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	if(m_vsync_enabled)
	{
		// lock to screen rfrsh rate
		m_swap_chain->Present(1,0);
	}
	else 
	{
		m_swap_chain->Present(0,0);
	}
	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_device_context;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projection_matrix;
	return;
}


void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_world_matrix;
	return;
}


void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_ortho_matrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_video_card_desc);
	memory = m_video_card_memory;
	return;
}