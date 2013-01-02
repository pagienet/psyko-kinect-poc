#ifndef _PSYKO_THROWPAINTLOGIC_
#define _PSYKO_THROWPAINTLOGIC_

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <NuiApi.h>
#include "poc/JointPaintLogic.h"
#include "poc/PaintParticleMesh.h"
#include "poc/TestMaterial.h"
#include "render/Mesh.h"
#include "render/MeshInstance.h"

namespace psyko
{
	struct PaintParticle
	{
		bool thrown;
		float3 position;
		float3 velocity;

		PaintParticle() : thrown(false), position(0.0, 0.0, 0.0), velocity(0.0, 0.0, 0.0) {}
	};

	class ThrowPaintLogic : public JointPaintLogic
	{
	public:
		ThrowPaintLogic(Painting* painting, ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~ThrowPaintLogic();

		virtual void UpdateSim(matrix4x4& projection);
		virtual void UpdateJoint(matrix4x4& projection, const Vector4* jointPositions, int index);
		
	private:
		MeshInstance* splotchInstance;
		Mesh* splotchMesh;
		TestMaterial* splotchMaterial;
		TestMaterial* particleMaterial;
		Texture2D* texture;
		ID3D11BlendState* blendState;
		
		const unsigned int NUM_PARTICLES_PER_JOINT;
		float paintJointAdhesion;
		PaintParticle* particles[NUI_SKELETON_POSITION_COUNT];
		PaintParticleMesh* paintParticleMesh;
		MeshInstance* meshes[NUI_SKELETON_POSITION_COUNT];
		float3 previousJointPositions[NUI_SKELETON_POSITION_COUNT];
		bool particleThrown[NUI_SKELETON_POSITION_COUNT];
		bool previousPositionsInvalid[NUI_SKELETON_POSITION_COUNT];
		void UpdateParticle(PaintParticle& particle);
		void ThrowParticles(int index, const float3& velocity, const float3& position);
		double time;
		float dt;
	};
}


#endif