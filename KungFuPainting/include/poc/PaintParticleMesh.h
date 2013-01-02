#ifndef _PSYKO_PAINTPARTICLEMESH_
#define _PSYKO_PAINTPARTICLEMESH_

#include <d3d11.h>
#include <DirectXMath.h>
#include "render/Mesh.h"
#include "core/Vertex.h"

namespace psyko
{
	struct PaintParticle;

	class PaintParticleMesh : public Mesh
	{
	public:
		PaintParticleMesh(ID3D11Device* device, unsigned int numParticles);
		virtual ~PaintParticleMesh();

		void Update(ID3D11DeviceContext* context, const PaintParticle* paintParticles);

	private:
		unsigned int numParticles;

		void Init();
	};

}

#endif