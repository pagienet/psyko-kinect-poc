#include "poc/MonsterMaterial.h"
#include "render/MeshInstance.h"

namespace psyko
{
	MonsterMaterial::MonsterMaterial(ID3D11Device* device)
		: Material(device), texture(0),
			projection(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)
	{
	}


	MonsterMaterial::~MonsterMaterial()
	{
	}

	void MonsterMaterial::SetMeshInstanceState(const MeshInstance* meshInstance) const
	{
		float4x4 mvp;
		float4x4 meshTransform = meshInstance->GetTransformMatrix();
		matrix4x4 tr = DirectX::XMLoadFloat4x4(&meshTransform);
		matrix4x4 pr = DirectX::XMLoadFloat4x4(&projection);
		pr = DirectX::XMMatrixMultiply(tr, pr);
		DirectX::XMStoreFloat4x4(&mvp, pr); 
		ID3DX11EffectMatrixVariable* mvpVar = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
		mvpVar->SetMatrix((float*)&mvp);	
		ID3DX11EffectShaderResourceVariable* shaderResource = effect->GetVariableByName("diffuseTexture")->AsShaderResource();
		shaderResource->SetResource(texture->GetShaderResourceView());
	}
}