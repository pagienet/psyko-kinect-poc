#ifndef _PSYKO_JOINTPAINLOGIC_
#define _PSYKO_JOINTPAINLOGIC_

#include <Windows.h>
#include <d3d11.h>
#include <NuiApi.h>
#include "poc/JointPaintLogic.h"
#include "poc/TestMaterial.h"
#include "render/Mesh.h"
#include "render/MeshInstance.h"

namespace psyko
{
	class SimplePaintLogic : public JointPaintLogic
	{
	public:
		SimplePaintLogic(Painting* painting, ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~SimplePaintLogic();

		virtual void Activate();
		virtual void UpdateJoint(matrix4x4& projection, const Vector4* jointPositions, int index);
		
	private:
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		MeshInstance* splotchInstance;
		Mesh* splotchMesh;
		TestMaterial* splotchMaterial;
		Texture2D* texture;
		ID3D11BlendState* blendState;
	};
}
#endif
