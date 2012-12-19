#include "render/DrawRectMaterial.h"

namespace psyko
{

	DrawRectMaterial::DrawRectMaterial(ID3D11Device* device)
		: Material(device), texture(0)
	{
		for (int i = 0; i < 16; ++i) {
			if (i % 5 == 0) 
				transform[i] = 1.0;
			else if (i == 12 || i == 13)
				transform[i] = 1.0;
			else
				transform[i] = 0.0;
		}
	}

	DrawRectMaterial::~DrawRectMaterial()
	{
	}

	void DrawRectMaterial::SetMeshInstanceState(const MeshInstance* meshInstance) const
	{		
		ID3DX11EffectMatrixVariable* mvp = effect->GetVariableByName("transformMatrix")->AsMatrix();
		mvp->SetMatrix(transform);	
		
		ID3DX11EffectShaderResourceVariable* shaderResource = effect->GetVariableByName("rectTexture")->AsShaderResource();
		shaderResource->SetResource(texture->GetShaderResourceView());
	}
}