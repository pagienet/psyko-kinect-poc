#include "skeleton/SkeletonMeshController.h"

#include <assert.h>
#include "core/Vertex.h"

using namespace DirectX;

namespace psyko
{
	SkeletonMeshController::SkeletonMeshController(ID3D11Device* device, Skeleton* skeleton, unsigned int weightsPerVertex)
		: skeleton(skeleton), weightsPerVertex(weightsPerVertex), vertexJointBindings(0), vertices(0), mesh(0), localPoses(0), globalPoses(0), globalPosesDirty(true)
	{
		mesh = new Mesh(device);		
	}

	SkeletonMeshController::~SkeletonMeshController()
	{
		if (mesh) delete mesh;
		if (vertices) delete[] vertices;
		if (vertexJointBindings) delete[] vertexJointBindings;
		if (localPoses) delete[] localPoses;
		if (globalPoses) delete[] globalPoses;
	}

	int SkeletonMeshController::InitVertexData(Vertex* data, UINT numVertices)
	{	
		this->numVertices = numVertices;
		
		vertexJointBindings = new VertexJointBinding[numVertices*weightsPerVertex];
		vertices = new Vertex[numVertices];
		
		// todo: transform by inverse bind matrices first for performance?
		memcpy(vertices, data, sizeof(Vertex)*numVertices);
		
		return mesh->InitVertexData(data, numVertices, sizeof(Vertex), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	}
	
	int SkeletonMeshController::InitIndexData(void* data, UINT numIndices, DXGI_FORMAT format)
	{
		return mesh->InitIndexData(data, numIndices, format, D3D11_USAGE_IMMUTABLE, 0);
	}

	void SkeletonMeshController::UpdateMesh(ID3D11DeviceContext* deviceContext)
	{
		if (globalPosesDirty) {
			UpdateGlobalPose();
			MorphMesh(deviceContext);
		}
	}

	void SkeletonMeshController::UpdateGlobalPose()
	{
		globalPosesDirty = false;
		
		for (int i = 0; i < skeleton->NumJoints(); ++i)
		{
			JointPose* local = localPoses + i;
			Joint* joint = skeleton->joints + i;

			matrix4x4 globalMatrix;
			
			if (joint->parentIndex == -1) {	
				// just copy root
				matrix4x4 localOrientationMatrix = XMMatrixRotationQuaternion(local->orientation);
				globalMatrix = XMMatrixMultiply(localOrientationMatrix, XMMatrixTranslationFromVector(local->translation));
			}
			else {
				matrix4x4 parentMatrix = XMLoadFloat4x4(globalPoses + joint->parentIndex);
				
				if (local->globalOrientation) {
					matrix4x4 bindPose = XMMatrixInverse(0, XMLoadFloat4x4(&joint->inverseBindPose));
					vector bindOrientation = XMQuaternionRotationMatrix(bindPose);
					globalMatrix = XMMatrixRotationQuaternion(XMQuaternionMultiply(bindOrientation, local->orientation));
					globalMatrix.r[3] = XMVector4Transform(local->translation, parentMatrix);					
				}
				else {				
					globalMatrix = XMMatrixRotationQuaternion(local->orientation);
					globalMatrix.r[3] = local->translation;
					globalMatrix = XMMatrixMultiply(globalMatrix, parentMatrix);
				}
			}

			XMStoreFloat4x4(globalPoses + i, globalMatrix);
		}

		// prepend invBinds
		for (int i = 0; i < skeleton->NumJoints(); ++i)
		{
			Joint* joint = skeleton->joints + i;
			matrix4x4 invBindPose = XMLoadFloat4x4(&joint->inverseBindPose);
			XMStoreFloat4x4(globalPoses + i, XMMatrixMultiply(invBindPose, XMLoadFloat4x4(globalPoses + i)));
		}
	}

	void SkeletonMeshController::MorphMesh(ID3D11DeviceContext* deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		deviceContext->Map(mesh->GetVertexBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		Vertex* output = (Vertex*)subResource.pData;
		
		for (unsigned int i = 0; i < numVertices; ++i) {			
			Vertex* vertex = vertices + i;
			vector pos = XMVectorSet(vertex->position.x, vertex->position.y, vertex->position.z, 1.0);
			vector v = XMVectorSet(0.0, 0.0, 0.0, 0.0);

			for (unsigned int j = 0; j < weightsPerVertex; ++j) {		
				VertexJointBinding* binding = vertexJointBindings + i*weightsPerVertex + j;
				
				float weight = binding->jointWeight;
				if (weight > 0.0f) {
					matrix4x4 pose = XMLoadFloat4x4(globalPoses + binding->jointIndex);
					vector tr = XMVector4Transform(pos, pose);
					tr = XMVectorScale(tr, weight);
					v = XMVectorAdd(v, tr);					
				}
			}

			float3 target;
			XMStoreFloat3(&target, v);
			output[i] = *vertex;
			output[i].position.x = target.x;
			output[i].position.y = target.y;
			output[i].position.z = target.z;
		}
		deviceContext->Unmap(mesh->GetVertexBuffer(), 0);
	}

	void SkeletonMeshController::ClearPose()
	{		
		if (localPoses) delete[] localPoses;
		if (globalPoses) delete[] globalPoses;

		localPoses = new JointPose[skeleton->NumJoints()];
		globalPoses = new float4x4[skeleton->NumJoints()];

		for (int i = 0; i < skeleton->NumJoints(); ++i)
		{
			Joint* joint = skeleton->joints + i;
			matrix4x4 m = XMLoadFloat4x4(&joint->inverseBindPose);
			m = XMMatrixInverse(0, m);
			
			if (joint->parentIndex != -1) {
				matrix4x4 parent = XMLoadFloat4x4(&(skeleton->joints[joint->parentIndex].inverseBindPose));
				m = XMMatrixMultiply(m, parent);
			}

			localPoses[i].orientation = XMQuaternionRotationMatrix(m);
			localPoses[i].translation = m.r[3];
		}

		globalPosesDirty = true;
	}
}