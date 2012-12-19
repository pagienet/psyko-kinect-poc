#ifndef _PSYKO_MONSTERMATERIAL_
#define _PSYKO_MONSTERMATERIAL_

#include "render/material.h"
#include "render/Texture2D.h"
#include "core/math.h"

namespace psyko
{

	class MonsterMaterial : public Material
	{
	public:
		MonsterMaterial(ID3D11Device* device);
		virtual ~MonsterMaterial();
		int Init() { return LoadFromRawEffect(L"fx/MonsterEffect.fx"); }

		void SetTexture(Texture2D* value) { texture = value; }
		void SetProjectionMatrix(float4x4 matrix) { projection = matrix; }		

	protected:
		void SetMeshInstanceState(const MeshInstance* meshInstance) const;

	private:
		Texture2D* texture;
		float4x4 projection;
	};

}

#endif