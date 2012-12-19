#ifndef _PSYKO_MATERIAL_
#define _PSYKO_MATERIAL_

#include <d3d11.h>
#include <string>
#include "d3dx11effect.h"

namespace psyko
{
	class MeshInstance;

	class Material
	{
	public:
		Material(ID3D11Device* device);
		virtual ~Material();

		//int InitFromCompiledEffect(std::string filename);
		int LoadFromRawEffect(std::wstring filename);
		
		ID3DX11EffectTechnique* GetTechniqueByName(std::string name);
		virtual void SetMeshInstanceState(const MeshInstance* meshInstance) const {}
		virtual void ClearState(ID3D11DeviceContext* context) {}

	protected:
		ID3D11Device* device;
		ID3DX11Effect* effect;
	};

	inline ID3DX11EffectTechnique* Material::GetTechniqueByName(std::string name)
	{
		return effect->GetTechniqueByName(name.c_str());
	}
}
#endif