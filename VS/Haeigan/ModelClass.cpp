#include "ModelClass.h"


ModelClass::ModelClass(void)
{
	m_vertex_buffer = 0;
	m_index_buffer = 0;
}


ModelClass::~ModelClass(void)
{
}

ModelClass::ModelClass(const ModelClass& ref){}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

int ModelClass::GetIndexCount()
{
	return m_index_count;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// boring 
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
	D3D11_SUBRESOURCE_DATA vertex_data, index_data;
	HRESULT result;
	// end of boring

	m_vertex_count = 3;
	m_index_count = 3;

	vertices = new VertexType[m_vertex_count];
	if(!vertices){ return false; }

	indices = new unsigned long[m_index_count];
	if(!indices){ return false; }

	// init vertices clockwise
	vertices[0].positon = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //bottom left
	vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].positon = D3DXVECTOR3(0.0f, 1.0f, 0.0f); //top middle
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].positon = D3DXVECTOR3(0.0f, 0.0f, 1.0f); //bottom right
	vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// now we set the descriptions of the buffer and then create them
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = sizeof(VertexType) * m_vertex_count;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	// set vertex data
	vertex_data.pSysMem = vertices;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	// create vertex buffer
	result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &m_vertex_buffer);
	if(FAILED(result)){ return false; }

	// repeat for index buffer
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = sizeof(unsigned long) * m_index_count;
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.StructureByteStride = 0;

	index_data.pSysMem = indices;
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	//create index buffer
	result = device->CreateBuffer(&index_buffer_desc, &index_data, &m_index_buffer);
	if(FAILED(result)){ return false; }

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}


void ModelClass:: ShutdownBuffers()
{
	if(m_index_buffer)
	{
		m_index_buffer->Release();
		m_index_buffer = 0;
	}

	if(m_vertex_buffer)
	{ 
		m_vertex_buffer->Release();
		m_vertex_buffer = 0;
	}
	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* device_context)
{
	unsigned int stride;
	unsigned int offset;

	// set the buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;
	
	// set the vertexbuffer to actice so it can be rendered
	device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// the same for index buffer
	device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// set the type of primitive we'll be rendering
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
