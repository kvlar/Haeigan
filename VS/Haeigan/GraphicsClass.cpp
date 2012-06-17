#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_baseShader = 0;
	m_light = 0;
	m_model_count = 0;
	n_scene = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& ref)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::InitializePhysX(NxScene* scene)
{
	n_scene = scene;
	
	// plane
	NxPlaneShapeDesc plane_desc;
	plane_desc.normal = NxVec3(0,1,0);
	plane_desc.d = 0.0f;

	NxMaterial* defaultMaterial = n_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
	
	NxActorDesc plane_actor;
	plane_actor.shapes.pushBack(&plane_desc);
	n_scene->createActor(plane_actor);

	return true;
}

bool GraphicsClass::Initialize(int& screen_width, int& screen_height, HWND hwnd, NxScene* scene)
{
	bool result;

	result = InitializePhysX(scene);
	if(!result) return false;

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
	m_Camera->SetPosition(0.0f, 5.0f, -20.0f);
	

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

	n_scene->simulate(1.0f/60.0f);

	// Clear buffers
	// Cornflower blue #6495ED :)
	//m_D3D->BeginScene(100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 1.0f);
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// move it from here!!
	

	// this calculates the ViewMatrix!
	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	D3DXMATRIX world_modification;

	/*
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
	*/
	int nb_Actors = n_scene->getNbActors();
	NxActor** actors = n_scene->getActors();
	while(nb_Actors--)
	{
		NxActor* actor = *actors++;
		if(!actor->userData) continue;

		// render current actor
		unsigned int model_id = (unsigned int)actor->userData - 1;
		NxMat34 w_matrx = actor->getGlobalPose();
		w_matrx.getColumnMajor44(world_modification);
		m_Models[model_id]->Render(m_D3D->GetDeviceContext());
		m_baseShader->Render(m_D3D->GetDeviceContext(), m_Models[model_id]->GetIndexCount(), \
			worldMatrix * world_modification, viewMatrix, projectionMatrix, m_Models[model_id]->GetTexture(), m_light->GetDirection(),
			m_light->GetDiffuseColor());
	}
	m_D3D->EndScene();

	n_scene->flushStream();
	n_scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	return true;
}

CameraClass* GraphicsClass::GetCamera()
{
	return m_Camera;
}

bool GraphicsClass::AddCube(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float scale)
{
	bool result;
	ModelClass* m = new ModelClass;
	m_Models.push_back(m);
	result = m->Initialize(m_D3D->GetDevice(), "../Haeigan/data/cube.obj", L"../Haeigan/data/white_plastic.dds");
	result = AddPhysXCube(position, rotation, scale);
	return result;
}

bool GraphicsClass::AddPhysXCube(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float scale)
{
	return AddPhysXCube(position, rotation, scale, D3DXVECTOR3(0,0,0));
}

bool GraphicsClass::AddPhysXCube(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float scale, D3DXVECTOR3 velocity)
{
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping = 0.5f;
	bodyDesc.linearVelocity = NxVec3(velocity.x, velocity.y, velocity.z);
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(scale, scale, scale);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 10.0f;
	actorDesc.globalPose.t = NxVec3(position.x, position.y, position.z);
	n_scene->createActor(actorDesc)->userData = (void*)(m_Models.size());
	return true;
}

void GraphicsClass::AddCubeFromEye()
{
	D3DXVECTOR3 position = m_Camera->GetPosition();
	D3DXVECTOR3 rotation(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 velocity = m_Camera->GetForward();
	D3DXVec3Normalize(&velocity, &velocity);
	velocity *= 100.0f;
	bool result;
	ModelClass* m = new ModelClass;
	m_Models.push_back(m);
	result = m->Initialize(m_D3D->GetDevice(), "../Haeigan/data/cube.obj", L"../Haeigan/data/white_plastic.dds");
	result = AddPhysXCube(position, rotation, 1.0f, velocity);
}

void GraphicsClass::AddStack(int size, float skin_width)
{
	D3DXVECTOR3 rotation(0.0f, 0.0f, 0.0f);
	const float cubeSize = 1.0f;
	const float spacing = -2.0f * skin_width;
	D3DXVECTOR3 pos(0.0f, cubeSize, 0.0f);
	float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	while(size)
	{
		for(int i = 0; i < size; i++)
		{
			pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
			AddCube(pos, rotation, 1.0f);
		}
		offset += cubeSize;
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

void GraphicsClass::AddTower(int size, float skin_width)
{
	D3DXVECTOR3 rotation(0.0f, 0.0f, 0.0f);
	const float cubeSize = 1.0f;
	const float spacing = -2.0f * skin_width;
	D3DXVECTOR3 pos(0.0f, cubeSize, 0.0f);
	while(size)
	{
		AddCube(pos, rotation, 1.0f);
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}