#ifndef _PSYKO_STICKFIGUREMESH_
#define _PSYKO_STICKFIGUREMESH_

#include <d3d11.h>
#include <NuiApi.h>
#include "render/Mesh.h"

namespace psyko
{
	class StickFigureMesh : public Mesh
	{
	public:
		StickFigureMesh(ID3D11Device* device);
		virtual ~StickFigureMesh();

		void Update(ID3D11DeviceContext* context, const Vector4* jointPositions);

	private:
		void Init();
	};

}

#endif