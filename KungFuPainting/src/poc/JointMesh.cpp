#include "poc/JointMesh.h"
#include "core/Vertex.h"

namespace psyko
{
	JointMesh::JointMesh(ID3D11Device* device)
		: Mesh(device)
	{
		Init();
	}


	JointMesh::~JointMesh()
	{
	}

	void JointMesh::Init()
	{
		Vertex vertices[] = 
		{
			{float3(0.0, .02, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			
			{float3(-.02, -.02, -.02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(.02, -.02, -.02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},

			{float3(.02, -.02, -.02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(.0, -.02, .02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},

			{float3(-.02, -.02, -.02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)},
			{float3(.0, -.02, .02), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float2(0.0, 0.0), float4(0.0, 0.0, 1.0, 1.0)}
		};

		unsigned short indices[] =
		{
			0, 2, 1,
			0, 3, 4,
			0, 5, 6, 
			1, 2, 4
		};

		InitVertexData(vertices, 6, sizeof(Vertex), D3D11_USAGE_IMMUTABLE, 0);
		InitIndexData(indices, 12, DXGI_FORMAT_R16_UINT, D3D11_USAGE_IMMUTABLE, 0);
	}
}