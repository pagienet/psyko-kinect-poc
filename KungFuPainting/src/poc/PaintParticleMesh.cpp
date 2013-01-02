#include "poc/PaintParticleMesh.h"

#include "poc/ThrowPaintLogic.h"

namespace psyko
{
	PaintParticleMesh::PaintParticleMesh(ID3D11Device* device, unsigned int numParticles)
		: Mesh(device), numParticles(numParticles)
	{
		Init();
	}


	PaintParticleMesh::~PaintParticleMesh()
	{
	}

	void PaintParticleMesh::Init()
	{
		Vertex* vertices = new Vertex[numParticles*3];
		unsigned short* indices = new unsigned short[numParticles*3];

		for (unsigned int i = 0; i < numParticles*3; ++i) {
			vertices[i].position = float3(0.0, 0.0, 0.0);
			vertices[i].normal = float3(0.0, 0.0, 0.0);
			vertices[i].tangent = float3(0.0, 0.0, 0.0);
			vertices[i].bitangent = float3(0.0, 0.0, 0.0);
			vertices[i].uv = float2(0.0, 0.0);
			vertices[i].color = float4(1.0, 0.0, 1.0, 1.0);
			indices[i] = i;
		}

		InitVertexData(vertices, numParticles*3, sizeof(Vertex), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		InitIndexData(indices, numParticles*3, DXGI_FORMAT_R16_UINT, D3D11_USAGE_IMMUTABLE, 0);

		topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		delete[] indices;
		delete[] vertices;
	}

	void PaintParticleMesh::Update(ID3D11DeviceContext* context, const PaintParticle* paintParticles)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
		Vertex* vertices = (Vertex*)mappedSubResource.pData;

		for (unsigned int i = 0; i < numParticles; ++i) {
			unsigned int j = i*3;
			float x = paintParticles[i].position.x;
			float y = paintParticles[i].position.y;
			float z = paintParticles[i].position.z;
			vertices[j].position = float3(x, y+.05f, z);
			vertices[j+1].position = float3(x+.05f, y-.05f, z);
			vertices[j+2].position = float3(x-.05f, y-.05f, z);
			vertices[j].color = float4(1.0, 0.0, 1.0, 1.0);
			vertices[j+1].color = float4(1.0, 0.0, 1.0, 1.0);
			vertices[j+2].color = float4(1.0, 0.0, 1.0, 1.0);
			vertices[j].normal = float3(0.0, 0.0, 0.0);
			vertices[j+1].normal = float3(0.0, 0.0, 0.0);
			vertices[j+2].normal = float3(0.0, 0.0, 0.0);
			vertices[j].tangent = float3(0.0, 0.0, 0.0);
			vertices[j+1].tangent = float3(0.0, 0.0, 0.0);
			vertices[j+2].tangent = float3(0.0, 0.0, 0.0);
			vertices[j].bitangent = float3(0.0, 0.0, 0.0);
			vertices[j+1].bitangent = float3(0.0, 0.0, 0.0);
			vertices[j+2].bitangent = float3(0.0, 0.0, 0.0);
			vertices[j].uv = float2(0.0, 0.0);
			vertices[j+1].uv = float2(0.0, 0.0);
			vertices[j+2].uv = float2(0.0, 0.0);
		}

		context->Unmap(vertexBuffer, 0);
	}
}