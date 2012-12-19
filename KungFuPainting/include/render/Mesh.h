#ifndef _PSYKO_MESH_
#define _PSYKO_MESH_

#include <d3d11.h>

namespace psyko
{
	class Mesh
	{
	public:
		Mesh(ID3D11Device* device);
		virtual ~Mesh();
		
		int InitVertexData(void* data, UINT numVertices, UINT stride, D3D11_USAGE usageHint, UINT cpuAccessFlags = 0);
		int InitIndexData(void* data, UINT numIndices, DXGI_FORMAT indexFormat, D3D11_USAGE usageHint, UINT cpuAccessFlags = 0);
		
		inline ID3D11Buffer* GetVertexBuffer() { return vertexBuffer; }
		inline const ID3D11Buffer* GetVertexBuffer() const { return vertexBuffer; }

		inline ID3D11Buffer* GetIndexBuffer() { return indexBuffer; }
		inline const ID3D11Buffer* GetIndexBuffer() const { return indexBuffer; }

		inline unsigned int NumIndices() const { return numIndices; }
		inline unsigned int NumVertices() const { return numVertices; }
		
		inline unsigned int VertexStride() const { return vertexStride; }
		inline DXGI_FORMAT IndexFormat() const { return indexFormat; }
		
		inline D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return topology; }
		
	protected:
		ID3D11Device* device;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;		
		
		unsigned int vertexStride;
		DXGI_FORMAT indexFormat;		
		unsigned int numVertices;
		unsigned int numIndices;

		D3D_PRIMITIVE_TOPOLOGY topology;
	};
}
#endif