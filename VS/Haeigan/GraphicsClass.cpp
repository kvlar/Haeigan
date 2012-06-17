#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_baseShader = 0;
	m_light = 0;
	m_model_count = 1;
}

GraphicsClass::GraphicsClass(const GraphicsClass& ref)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int& screen_width, int& screen_height, HWND hwnd)
{
	bool result;

	// Create D3D object
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	//initialize D3D object
	result = m_D3D->Initialize(screen_width, screen_height, true, hwnd, \
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Couldn't initialize D3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	for(int i = 0; i < m_model_count; i++)
	{
		ModelClass* m = new ModelClass;
		m_Models.push_back(m);
		result = m->Initialize(m_D3D->GetDevice(), "../Haeigan/data/cube.obj", L"../Haeigan/data/white_plastic.dds");
	}

	// Initialize the model object.
	
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_baseShader = new BaseShaderClass;
	if(!m_baseShader)
	{
		return false;
	}

	m_light = new LightClass;
	if(!m_light)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_baseShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the light object.
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(-1.0f, 1.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if(m_baseShader)
	{
		m_baseShader->Shutdown();
		delete m_baseShader;
		m_baseShader = 0;
	}

	if(m_light)
	{
		delete m_light;
		m_light = 0;
	}

	// Release the model object.
	while(!m_Models.empty())
	{
		ModelClass* m = m_Models.back();
		m_Models.pop_back();
		m->Shutdown();
		delete m;
		m = 0;
	}
	

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

static float rotation = 0.0f;
bool GraphicsClass::Frame(D3DXVECTOR3 movement, float rotationX, float rotationY)
{
	bool result;
	
	D3DXVECTOR3 cam_position = m_Camera->GetPosition();
	D3DXVECTOR3 cam_rotation = m_Camera->GetRotation();
	cam_rotation.y += rotationY;
	cam_rotation.x += rotationX;
	

	D3DXMATRIX view_matrix;
	view_matrix = m_Camera->GetRotationMatrix();

	D3DXVec3TransformCoord(&movement, &movement, &view_matrix);
	cam_position += movement;


	m_Camera->SetPosition(cam_position);
	m_Camera->SetRotation(cam_rotation);

	//render graphics scene
	result = Render(0.0f);
	if(!result)
	{
		return false;
	}
	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;
	// Clear buffers
	// Cornflower blue #6495ED :)
	m_D3D->BeginScene(100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 1.0f);
	
	// move it from here!!
	

	// this calculates the ViewMatrix!
	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	D3DXMATRIX world_modification;

	for(int i = 0; i < m_Models.size(); i++){

		world_modification = m_Models[i]->GetWorldTransformationMatrix();

		m_Models[i]->Render(m_D3D->GetDeviceContext());

		result = m_baseShader->Render(m_D3D->GetDeviceContext(), m_Models[i]->GetIndexCount(), \
			worldMatrix * world_modification, viewMatrix, projectionMatrix, m_Models[i]->GetTexture(), m_light->GetDirection(),
			m_light->GetDiffuseColor());

		if(!result)
		{
			return false;
		}
	}
	m_D3D->EndScene();
	return true;
}

CameraClass* GraphicsClass::GetCamera()
{
	return m_Camera;
}