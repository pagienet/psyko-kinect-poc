#include "poc/Painting.h"
#include "poc/TestMaterial.h"
#include "poc/PlaneMesh.h"

using namespace DirectX;

namespace psyko
{
	Painting::Painting(ID3D11Device* device, ID3D11DeviceContext* context)
		: MeshInstance(device), texture(0), width(5.0), height(5.0)
	{
		InitTexture(context);
		TestMaterial* canvasMaterial = new TestMaterial(device);
		SetMesh(new PlaneMesh(device, width, height));
		SetMaterial(canvasMaterial);
		canvasMaterial->Init();
		SetActiveTechniqueByName("TextureTech");		
		canvasMaterial->SetTexture(texture);
	}

	Painting::~Painting()
	{
		if (mesh) delete mesh;
		if (material) delete material;
		if (texture) delete texture;
	}

	void Painting::SetProjectionMatrix(float4x4& projection)
	{
		static_cast<TestMaterial*>(material)->SetProjectionMatrix(projection);
	}

	void Painting::SetTransformMatrix(float4x4 matrix)
	{ 
		MeshInstance::SetTransformMatrix(matrix);
		XMStoreFloat4x4(&inverseTransformMatrix, XMMatrixInverse(0, XMLoadFloat4x4(&matrix)));
	}

	void Painting::Render(ID3D11DeviceContext* deviceContext)
	{
		MeshInstance::Render(deviceContext);

		material->ClearState(deviceContext);
		technique->GetPassByIndex(0)->Apply(0, deviceContext);
	}

	void Painting::InitTexture(ID3D11DeviceContext* context)
	{
		texture = new Texture2D(device);
		texture->InitBlank(512, 512, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_USAGE_DEFAULT, 0);
		
		float color[4] = { 1.0, 1.0, 1.0, 1.0 };
		ID3D11RenderTargetView* renderTargetView = texture->GetRenderTargetView();
		context->ClearRenderTargetView(renderTargetView, color);
	}
}