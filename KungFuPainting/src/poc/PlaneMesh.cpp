#include "poc/PlaneMesh.h"

#include "core/Vertex.h"

namespace psyko
{

	PlaneMesh::PlaneMesh(ID3D11Device* device, float width, float height)
		: Mesh(device)
	{
		Init(width, height);
	}


	PlaneMesh::~PlaneMesh()
	{
	}

	void PlaneMesh::Init(float width, float height)
	{
		float halfWidth = width*.5;
		float halfHeight = height*.5;

		Vertex vertices[] = 
		{
			{float3(-halfWidth, halfHeight, 0.0), float3(0.0, 0.0, -1.0), float3(1.0, 0.0, 0.0), float3(0.0, -1.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(halfWidth, halfHeight, 0.0), float3(0.0, 0.0, -1.0), float3(1.0, 0.0, 0.0), float3(0.0, -1.0, 0.0), float2(1.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(halfWidth, -halfHeight, 0.0), float3(0.0, 0.0, -1.0), float3(1.0, 0.0, 0.0), float3(0.0, -1.0, 0.0), float2(1.0, 1.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(-halfWidth, -halfHeight, 0.0), float3(0.0, 0.0, -1.0), float3(1.0, 0.0, 0.0), float3(0.0, -1.0, 0.0), float2(0.0, 1.0), float4(0.0, 0.0, 1.0, 1.0)}
		};

		unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		InitVertexData(vertices, 6, sizeof(Vertex), D3D11_USAGE_IMMUTABLE, 0);
		InitIndexData(indices, 6, DXGI_FORMAT_R16_UINT, D3D11_USAGE_IMMUTABLE, 0);
	}
}