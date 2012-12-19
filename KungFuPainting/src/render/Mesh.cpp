#include "render/Mesh.h"
#include "error/errorcodes.h"

namespace psyko
{
	int CreateBuffer(ID3D11Device* device, void* data, UINT byteWidth, D3D11_USAGE usageHint, UINT cpuAccessFlags, UINT bindFlags, ID3D11Buffer** buffer);

	Mesh::Mesh(ID3D11Device* device)
		:	device(device), vertexBuffer(0), indexBuffer(0), vertexStride(0), indexFormat(DXGI_FORMAT_UNKNOWN), numVertices(0), numIndices(0),
		topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
	}

	Mesh::~Mesh()
	{
		if (vertexBuffer) vertexBuffer->Release();
		if (indexBuffer) indexBuffer->Release();
	}

	int Mesh::InitVertexData(void* data, UINT numVertices, UINT vertexStride, D3D11_USAGE usageHint, UINT cpuAccessFlags)
	{
		if (vertexBuffer) {
			vertexBuffer->Release();
			vertexBuffer = 0;
		}
		this->vertexStride = vertexStride;
		this->numVertices = numVertices;
		return CreateBuffer(device, data, numVertices*vertexStride, usageHint, cpuAccessFlags, D3D11_BIND_VERTEX_BUFFER, &vertexBuffer); 
	}

	int Mesh::InitIndexData(void* data, UINT numIndices, DXGI_FORMAT indexFormat, D3D11_USAGE usageHint, UINT cpuAccessFlags)
	{
		if (indexBuffer) {
			indexBuffer->Release();
			indexBuffer = 0;
		}
		// todo: allow something other than UINT, such as shorts
		this->indexFormat = indexFormat;
		this->numIndices = numIndices;
		
		unsigned int indexSize = 0;
		if (indexFormat == DXGI_FORMAT_R16_UINT)
			indexSize = 2;
		else if (indexFormat == DXGI_FORMAT_R32_UINT)
			indexSize = 4;
		else 
			return INVALID_PARAMETER;
		
		return CreateBuffer(device, data, indexSize*numIndices, usageHint, cpuAccessFlags, D3D11_BIND_INDEX_BUFFER, &indexBuffer); 		
	}

	int CreateBuffer(ID3D11Device* device, void* data, UINT byteWidth, D3D11_USAGE usageHint, UINT cpuAccessFlags, UINT bindFlags, ID3D11Buffer** buffer)
	{
		D3D11_BUFFER_DESC descriptor;
		descriptor.ByteWidth = byteWidth;
		descriptor.Usage = usageHint;
		descriptor.BindFlags = bindFlags;
		descriptor.CPUAccessFlags = cpuAccessFlags;
		descriptor.MiscFlags = 0;
		descriptor.StructureByteStride = 0;
		
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		HRESULT result = device->CreateBuffer(&descriptor, &initData, buffer);
		return FAILED(result)? D3D_BUFFER_CREATION_FAILED : 0;
	}
}