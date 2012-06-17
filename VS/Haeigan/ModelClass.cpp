#include "ModelClass.h"


ModelClass::ModelClass(void)
{
	m_vertex_buffer = 0;
	m_index_buffer = 0;
	m_texture = 0;
	m_Scale = 1.0f;
	ZeroMemory(m_Rotation, sizeof(m_Rotation));
	ZeroMemory(m_Position, sizeof(m_Position));
	m_vertex_count = 0;
	m_index_count = 0;
	m_model = 0;
}


ModelClass::~ModelClass(void)
{
}

ModelClass::ModelClass(const ModelClass& ref){}

bool ModelClass::Initialize(ID3D11Device* device, char* model_filename, WCHAR* texture_filename)
{
	bool result;

	result = LoadModel(model_filename);
	if(!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	

	//initialize texture
	result = LoadTexture(device, texture_filename);
	if(!result)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	ReleaseModel();
	ReleaseTexture();
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

	vertices = new VertexType[m_vertex_count];
	if(!vertices){ return false; }

	indices = new unsigned long[m_index_count];
	if(!indices){ return false; }

	// init vertices clockwise
	/* Old piece of .. code
	vertices[0].positon = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //bottom left
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].positon = D3DXVECTOR3(-1.0f, 1.0f, 0.0f); //top left
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);


	vertices[2].positon = D3DXVECTOR3(1.0f, 1.0f, 0.0f); //top right
	vertices[2].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[3].positon = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //bottom right
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;
	*/

	for(int i = 0; i < m_vertex_count; i++)
	{
		ModelType* m = &m_model[i];
		vertices[i].positon = D3DXVECTOR3(m->x, m->y, m->z);
		vertices[i].texture = D3DXVECTOR2(m->tu, m->tv);
		vertices[i].normal = D3DXVECTOR3(m->nx, m->ny, m->nz);

		indices[i] = i;
	}

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

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_texture = new TextureClass;
	if(!m_texture)
	{
		return false;
	}

	// initialize texture object
	result = m_texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}
	return true;
}

void ModelClass::ReleaseTexture()
{
	if(m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
	return;
}

//-----
// Utility functions
//----

void ModelClass::Translate(D3DXVECTOR3 move)
{
	m_Position += move;
}
void ModelClass::Rotate(D3DXVECTOR3 rotation)
{
	m_Rotation = rotation;
}
void ModelClass::Scale(float scale_factor)
{
	m_Scale += scale_factor;
}

D3DXMATRIX ModelClass::GetWorldTransformationMatrix()
{
	D3DXMATRIX translation, scale, rotate;
	D3DXMatrixTranslation(&translation, m_Position.x, m_Position.y, m_Position.z);
	D3DXMatrixScaling(&scale, m_Scale, m_Scale, m_Scale);
	D3DXMatrixRotationYawPitchRoll(&rotate, m_Rotation.y, m_Rotation.x, m_Rotation.z);

	return scale * rotate * translation;
}

//------
// loads model, move it to a separate class
//------
bool ModelClass::LoadModel(char* filename)
{
	std::ifstream fin;
	char inputline[255];
	int i;

	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	fin.getline(inputline, 255);
	while(inputline[0] != 'v')
	{
		fin.getline(inputline,255);
	}

	

	std::vector<float> vertex_data;
	std::vector<float> tex_data;
	std::vector<float> normal_data;
	std::vector<int> indices;

	std::string line;
	float x,y,z;
	std::string part;

	while(inputline[1] == ' ')
	{
		// vertex info
		line.assign(inputline, 2, 255);
		
		part.erase();
		for(i = 0; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		x = std::atof(part.c_str());
		
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		y = std::atof(part.c_str());
	
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		z = std::atof(part.c_str());
		
		vertex_data.push_back(x);
		vertex_data.push_back(y);
		vertex_data.push_back(-1.0f * z);

		fin.getline(inputline,255);
	}

	while(inputline[1] == 't')
	{
		// vertex info
		line.assign(inputline, 3, 255);
		
		part.erase();
		for(i = 0; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		x = std::atof(part.c_str());
		
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		y = std::atof(part.c_str());
			
		tex_data.push_back(x);
		tex_data.push_back(1.0f - y);

		fin.getline(inputline,255);
	}

	//normals
	while(inputline[1] == 'n')
	{
		// vertex info
		line.assign(inputline, 3, 255);
		
		part.erase();
		for(i = 0; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		x = std::atof(part.c_str());
		
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		y = std::atof(part.c_str());
	
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		z = std::atof(part.c_str());
		
		normal_data.push_back(x);
		normal_data.push_back(y);
		normal_data.push_back(-1.0f * z);

		fin.getline(inputline,255);
	}

	// ok, vertex data is ready, now it's time for the faces
	while(inputline[0] != 'f')
	{
		fin.getline(inputline,255);
	}
	int vsize = vertex_data.size();
	int vtsize = tex_data.size();
	std::vector<ModelType> complete_data;
	
	

	int v_num, vt_num, vn_num, first, last;

	//parse faces
	while(inputline[1] == ' ')
	{
		// vertex info
		line.assign(inputline, 2, 255);
		
		part.erase();
		for(i = 0; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		ModelType t1,t2,t3;
		first = part.find_first_of("/");
		last = part.find_last_of("/");

		v_num = std::atoi(part.substr(0, first).c_str());
		vt_num = std::atoi(part.substr(first+1, last).c_str());
		vn_num = std::atoi(part.substr(last+1, part.size()).c_str());
		t1.nx = normal_data[3*(vn_num-1)];
		t1.ny = normal_data[3*(vn_num-1)+1];
		t1.nz = normal_data[3*(vn_num-1)+2];

		t1.x = vertex_data[3*(v_num-1)];
		t1.y = vertex_data[3*(v_num-1)+1];
		t1.z = vertex_data[3*(v_num-1)+2];


		t1.tu = tex_data[2*(vt_num-1)];
		t1.tv = tex_data[2*(vt_num-1)+1];
		
		


		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		first = part.find_first_of("/");
		last = part.find_last_of("/");

		v_num = std::atoi(part.substr(0, first).c_str());
		vt_num = std::atoi(part.substr(first+1, last).c_str());
		vn_num = std::atoi(part.substr(last+1, part.size()).c_str());
		t2.nx = normal_data[3*(vn_num-1)];
		t2.ny = normal_data[3*(vn_num-1)+1];
		t2.nz = normal_data[3*(vn_num-1)+2];

		t2.x = vertex_data[3*(v_num-1)];
		t2.y = vertex_data[3*(v_num-1)+1];
		t2.z = vertex_data[3*(v_num-1)+2];


		t2.tu = tex_data[2*(vt_num-1)];
		t2.tv = tex_data[2*(vt_num-1)+1];
		
		
		
	
		i++;
		part.erase();
		for(; i < line.size() && line[i] != ' '; i++)
		{
			part.push_back(line[i]);
		}
		first = part.find_first_of("/");
		last = part.find_last_of("/");

		v_num = std::atoi(part.substr(0, first).c_str());
		vt_num = std::atoi(part.substr(first+1, last).c_str());
		vn_num = std::atoi(part.substr(last+1, part.size()).c_str());
		t3.nx = normal_data[3*(vn_num-1)];
		t3.ny = normal_data[3*(vn_num-1)+1];
		t3.nz = normal_data[3*(vn_num-1)+2];

		t3.x = vertex_data[3*(v_num-1)];
		t3.y = vertex_data[3*(v_num-1)+1];
		t3.z = vertex_data[3*(v_num-1)+2];


		t3.tu = tex_data[2*(vt_num-1)];
		t3.tv = tex_data[2*(vt_num-1)+1];

		complete_data.push_back(t1);
		complete_data.push_back(t3);
		complete_data.push_back(t2);
		
		if(!fin.getline(inputline,255)) break;
	}


	m_model = new ModelType[complete_data.size()];
	vsize = complete_data.size();
	m_vertex_count = vsize;
	m_index_count = vsize;
	for(int v = 0; v < vsize; v++)
	{
		m_model[v] = complete_data[v];
		
	}
	return true;
}

void ModelClass::ReleaseModel()
{
	delete m_model;
	m_model = 0;
}