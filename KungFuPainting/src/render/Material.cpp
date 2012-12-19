#include "render/Material.h"

#include <d3dcompiler.h>
#include "core/debug.h"
#include "core/Vertex.h"
#include "error/errorcodes.h"
#include "render/Mesh.h"
#include "render/MeshInstance.h"


namespace psyko
{
	Material::Material(ID3D11Device* device)
		: device(device), effect(0)
	{
	}

	Material::~Material()
	{
		if (effect) effect->Release();
	}

	int Material::LoadFromRawEffect(std::wstring filename)
	{
		UINT flags1 = 0;
		UINT flags2 = 0;	// not used for now

#ifdef _DEBUG
		flags1 |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob* compiledShader = 0;
		ID3D10Blob* compilationMessages = 0;
		HRESULT result = 0;
		
		result = D3DCompileFromFile(filename.c_str(), 0, 0, 0, "fx_5_0", flags1, flags2, &compiledShader, &compilationMessages);

		if (compilationMessages != 0) {
			DEBUG_TRACE("Errors:\n" << reinterpret_cast<char*>(compilationMessages->GetBufferPointer()));
			
			Alert(reinterpret_cast<char*>(compilationMessages->GetBufferPointer()));
			compilationMessages->Release();
			return D3D_EFFECT_CREATION_FAILED;
		}
	
		if (FAILED(result))  {
			Alert("Failed to compile effect (not a compiler error)");
			return D3D_EFFECT_CREATION_FAILED;
		}

		result = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), flags2, device, &effect);
		compiledShader->Release();

		if (FAILED(result)) {
			Alert("Could not create effect from memory");
			return D3D_EFFECT_CREATION_FAILED;
		}

		return 0;
	}
}