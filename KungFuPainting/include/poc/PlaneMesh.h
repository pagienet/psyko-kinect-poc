#ifndef _PSYKO_PLANEMESH_
#define _PSYKO_PLANEMESH_

#include "render/Mesh.h"

namespace psyko
{
	class PlaneMesh : public Mesh
	{
	public:
		PlaneMesh(ID3D11Device* device, float width, float height);
		virtual ~PlaneMesh();

	private:
		void Init(float width, float height);
	};
}
#endif