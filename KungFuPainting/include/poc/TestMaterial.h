#ifndef _PSYKO_TESTMATERIAL_
#define _PSYKO_TESTMATERIAL_

#include <d3d11.h>
#include "render/MeshInstance.h"
#include "core/Vertex.h"
#include "render/Material.h"
#include "render/Texture2D.h"

namespace psyko
{
	class TestMaterial : public Material
	{
	public:
		TestMaterial(ID3D11Device* device);
		virtual ~TestMaterial();

		int Init() { return LoadFromRawEffect(L"fx/TestEffect.fx"); }

		virtual void SetMeshInstanceState(const MeshInstance* meshInstance) const;
		virtual void ClearState(ID3D11DeviceContext* context);
		void SetProjectionMatrix(float4x4 matrix) { projection = matrix; }		
		void SetTexture(Texture2D* value) { texture = value; }
		
	private:
		float4x4 projection;
		Texture2D* texture;
	};
}
#endif