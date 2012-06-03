#include "TextureClass.h"

#include "Logger.h"
static Logger* m_logger = Logger::Get_instance();

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// load the texture
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL,\
		&m_texture, NULL);
	if(FAILED(result))
	{
		m_logger->Error("Load texture failed");
		return false;
	}
	return true;
}

void TextureClass::Shutdown()
{
	// release the texture
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}


TextureClass::TextureClass(void)
{
	m_texture = 0;
}

TextureClass::TextureClass(const TextureClass& ref){}

TextureClass::~TextureClass(void)
{
}
