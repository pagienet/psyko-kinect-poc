#ifndef _PSYKO_MESHINSTANCE_
#define _PSYKO_MESHINSTANCE_

#include <d3d11.h>
#include <string>
#include "render/Material.h"
#include "render/Mesh.h"
#include "core/math.h"

namespace psyko
{
	class MeshInstance
	{
	public:
		MeshInstance(ID3D11Device* device);
		~MeshInstance();

		inline Mesh* GetMesh() { return mesh; }
		inline const Mesh* GetMesh() const { return mesh; }
		int SetMesh(Mesh* value);
		
		inline Material* GetMaterial() { return material; }
		inline const Material* GetMaterial() const { return material; }
		int SetMaterial(Material* value);
		virtual void SetTransformMatrix(float4x4 matrix) { transform = matrix; }
		float4x4 GetTransformMatrix() const { return transform; }

		int SetActiveTechniqueByName(std::string value);
		virtual void Render(ID3D11DeviceContext* deviceContext) const;

	protected:
		ID3D11Device* device;
		Mesh* mesh;
		Material* material;
		ID3D11InputLayout* vertexLayout;
		ID3DX11EffectTechnique* technique;
		float4x4 transform;

		int UpdateVertexLayout();
		void AssignBuffers(ID3D11DeviceContext* deviceContext) const;		
	};

	inline int MeshInstance::SetMesh(Mesh* value)
	{
		if (mesh == value) return 0;
		mesh = value;
		return UpdateVertexLayout();
	}

	inline int MeshInstance::SetMaterial(Material* value)
	{
		if (material == value) return 0;
		material = value;
		return UpdateVertexLayout();
	}

	inline int MeshInstance::SetActiveTechniqueByName(std::string techniqueName)
	{
		ID3DX11EffectTechnique* newTechnique = material->GetTechniqueByName(techniqueName);
		if (technique == newTechnique) return 0;
		technique = newTechnique;
		return UpdateVertexLayout();
	}
}
#endif