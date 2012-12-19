#ifndef _PSYKO_SKELETONMESHCONTROLLER_
#define _PSYKO_SKELETONMESHCONTROLLER_

#include <d3d11.h>
#include "core/Vertex.h"
#include "render/Mesh.h"
#include "skeleton/Skeleton.h"

namespace psyko
{
	class SkeletonMeshController
	{
	public:
		SkeletonMeshController(ID3D11Device* device, Skeleton* skeleton, unsigned int weightsPerVertex);
		~SkeletonMeshController();

		int InitVertexData(Vertex* data, UINT numVertices);
		int InitIndexData(void* data, UINT numIndices, DXGI_FORMAT format);

		Mesh* GetMesh() { return mesh; }
		Skeleton* GetSkeleton() { return skeleton; }
		float4x4 GetGlobalPose(unsigned index) { return globalPoses[index]; }
		JointPose* GetLocalJointPose(unsigned int index) { return &localPoses[index]; }
		VertexJointBinding* GetJointBindings() { return vertexJointBindings; }

		void UpdateMesh(ID3D11DeviceContext* deviceContext);
		void ClearPose();
		void InvalidatePose() { globalPosesDirty = true; }
		void UpdateGlobalPose();

	private:
		Mesh* mesh;
		Skeleton* skeleton;
		unsigned int weightsPerVertex;
		unsigned int numVertices;
		Vertex* vertices;
		JointPose* localPoses;
		float4x4* globalPoses;
		bool globalPosesDirty;

		VertexJointBinding* vertexJointBindings;
		
		void MorphMesh(ID3D11DeviceContext* deviceContext);
	};
}
#endif