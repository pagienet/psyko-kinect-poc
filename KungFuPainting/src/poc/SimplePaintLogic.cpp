#include "poc/SimplePaintLogic.h"

#include <math.h>
#include <d3d11.h>
#include "core/debug.h"
#include "poc/PlaneMesh.h"

using namespace DirectX;

namespace psyko
{
	SimplePaintLogic::SimplePaintLogic(Painting* painting, ID3D11Device* device, ID3D11DeviceContext* context)
		: JointPaintLogic(painting), device(device), deviceContext(context), splotchInstance(0), splotchMesh(0), splotchMaterial(0), texture(0), blendState(0)
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		device->CreateBlendState(&blendDesc, &blendState);
		
		texture = new Texture2D(device);
		texture->LoadWICFromFile(L"textures/paintSmudge.png", context);
		
		splotchMesh = new PlaneMesh(device, 1.0, 1.0);
		splotchMaterial = new TestMaterial(device);
		splotchMaterial->Init();
		splotchMaterial->SetTexture(texture);
		splotchInstance = new MeshInstance(device);
		splotchInstance->SetMesh(splotchMesh);
		splotchInstance->SetMaterial(splotchMaterial);
		splotchInstance->SetActiveTechniqueByName("TextureTech");
	}


	SimplePaintLogic::~SimplePaintLogic()
	{
		if (blendState) blendState->Release();
		if (splotchInstance) delete splotchInstance;
		if (splotchMaterial) delete splotchMaterial;
		if (splotchMesh) delete splotchMesh;
		if (texture) delete texture;
	}

	void SimplePaintLogic::Activate() 
	{
		// just init
		splotchInstance->GetMaterial()->SetMeshInstanceState(0);
	}

	void SimplePaintLogic::UpdateJoint(matrix4x4& projection, const Vector4* jointPositions, int index)
	{
		// todo: use projection from camera to project points and unproject them unto the painting to figure out XY coords
		float dz = jointPositions[index].z - jointPositions[0].z;
		matrix4x4 inversePaintingMatrix = XMLoadFloat4x4(&painting->GetInverseTransformMatrix());
		
		vector jointPos = XMVectorSet(jointPositions[index].x, jointPositions[index].y, jointPositions[index].z, 1.0);
		vector local = XMVector4Transform(jointPos, inversePaintingMatrix);
		float4 pos;
		XMStoreFloat4(&pos, local);
		
		DEBUG_TRACE("Global position " << index << ": " << jointPositions[index].x << ", " << jointPositions[index].y << ", " << jointPositions[index].z);
		DEBUG_TRACE("Local position " << index << ": " << pos.x << ", " << pos.y << ", " << pos.z);
		
		// make sure is close to canvas and limb is protruding
		if (pos.z > 0.05 || dz < .25) return;

		const float splotchSize = .1f;
		float hw = painting->GetWidth() * .5f;
		float hh = painting->GetHeight() * .5f;
		
		// project joint so we can find projection ray
		vector projectedPoint = XMVector4Transform(jointPos, projection);
		projectedPoint /= XMVectorSplatW(projectedPoint);
		
		// get a projection on the near plane for ray origin
		projectedPoint = XMVectorSetZ(projectedPoint, 0.0);
		
		// calc line dir in world space
		vector lineOrigin = XMVector4Transform(projectedPoint, XMMatrixInverse(0, projection));
		lineOrigin /= XMVectorSplatW(lineOrigin);
		vector lineDir = jointPos - lineOrigin;
		
		matrix4x4 matrix = XMLoadFloat4x4(&painting->GetTransformMatrix());
		vector normal = matrix.r[2];
		vector planePoint = matrix.r[3];

		vector dot1 = XMVector3Dot(planePoint - lineOrigin, normal);
		vector dot2 = XMVector3Dot(lineDir, normal);
		vector t = dot1 / dot2;
		planePoint = lineOrigin + t*lineDir;
		planePoint = XMVector4Transform(planePoint, inversePaintingMatrix);
		XMStoreFloat4(&pos, planePoint);
		
		DEBUG_TRACE("Intersection " << index << ": " << pos.x << ", " << pos.y << ", " << pos.z);

		// t = ((planePoint - lineOrigin) . planeNormal) / (lineDir . planeNormal)

		if (pos.x > -hw && pos.x < hw &&
			pos.y > -hh && pos.y < hh)			  
		{
			pos.x /= hw;
			pos.y /= hh;
			float4x4 transform(
				splotchSize, 0.0, 0.0, 0.0,
				0.0, splotchSize, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				pos.x - splotchSize*.5, pos.y - splotchSize*.5, 0.0, 1.0
			);
			DEBUG_TRACE("Painting " << index);			
			splotchInstance->SetTransformMatrix(transform);
			
			deviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
			splotchInstance->Render(deviceContext);
			deviceContext->OMSetBlendState(0, 0, 0xffffffff);
		}
	}
}