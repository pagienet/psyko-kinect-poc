#include "poc/TestMaterial.h"

namespace psyko
{

	TestMaterial::TestMaterial(ID3D11Device* device)																		  
		: Material(device), texture(0),
		projection(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)
	{

	}


	TestMaterial::~TestMaterial()
	{
	}

	void TestMaterial::SetMeshInstanceState(const MeshInstance* meshInstance) const
	{
		float4x4 mvp;
		if (meshInstance) {
			float4x4 meshTransform = meshInstance->GetTransformMatrix();
			matrix4x4 tr = DirectX::XMLoadFloat4x4(&meshTransform);
			matrix4x4 pr = DirectX::XMLoadFloat4x4(&projection);
			pr = DirectX::XMMatrixMultiply(tr, pr);
			DirectX::XMStoreFloat4x4(&mvp, pr); 
			ID3DX11EffectMatrixVariable* mvpVar = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
			mvpVar->SetMatrix((float*)&mvp);
		}

		if (texture != 0) {
			ID3DX11EffectShaderResourceVariable* shaderResource = effect->GetVariableByName("diffuseTexture")->AsShaderResource();
			shaderResource->SetResource(texture->GetShaderResourceView());
		}
	}

	void TestMaterial::ClearState(ID3D11DeviceContext* context) 
	{
		if (texture != 0) {
			ID3DX11EffectShaderResourceVariable* shaderResource = effect->GetVariableByName("diffuseTexture")->AsShaderResource();
			shaderResource->SetResource(0);
		}
	}
}