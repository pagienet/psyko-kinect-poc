#include "render/DrawDepthMaterial.h"

namespace psyko
{

	DrawDepthMaterial::DrawDepthMaterial(ID3D11Device* device)
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

		texDimensions[0] = 1.0;
		texDimensions[1] = 1.0;
	}

	DrawDepthMaterial::~DrawDepthMaterial()
	{
	}

	void DrawDepthMaterial::SetMeshInstanceState(const MeshInstance* meshInstance) const
	{		
		ID3DX11EffectMatrixVariable* mvp = effect->GetVariableByName("transformMatrix")->AsMatrix();
		mvp->SetMatrix(transform);	
		
		effect->GetVariableByName("texDimensions")->SetRawValue(texDimensions, 0, sizeof(float)*2);
		
		ID3DX11EffectShaderResourceVariable* shaderResource = effect->GetVariableByName("depthTexture")->AsShaderResource();
		shaderResource->SetResource(texture->GetShaderResourceView());
	}
}