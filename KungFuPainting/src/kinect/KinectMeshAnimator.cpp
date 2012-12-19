#include "kinect/KinectMeshAnimator.h"

#include <math.h>
#include <assert.h>

using namespace DirectX;

namespace psyko
{
	KinectMeshAnimator::KinectMeshAnimator(SkeletonMeshController* animator, KinectController* kinectController)
		: initialized(false), animator(animator), kinectController(kinectController), trackingId(-1)
	{
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) { 
			bindings[i] = 0;
		}

		animator->ClearPose();
	}


	KinectMeshAnimator::~KinectMeshAnimator()
	{
	}

	void KinectMeshAnimator::Update(ID3D11DeviceContext* deviceContext)
	{
		if (!initialized) {
			//InitPoses();
			InitPoses();
			initialized = true;
		}

		SkeletonData skeletonData;
		int error = kinectController->GetSkeletonData(&skeletonData);
		if (error) return;

		int skeletonIndex = -1;
		skeletonIndex = GetActiveSkeleton(skeletonData.skeletonData);
		if (skeletonIndex < 0) return;
		
		NUI_SKELETON_DATA skeleton = skeletonData.skeletonData[skeletonIndex];
		
		UpdateOrientations(skeleton);
		

		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
			// do not process terminals
			if (i == NUI_SKELETON_POSITION_HAND_LEFT || i == NUI_SKELETON_POSITION_HAND_RIGHT ||
				i == NUI_SKELETON_POSITION_FOOT_LEFT || i == NUI_SKELETON_POSITION_FOOT_RIGHT) 
				continue;

			Joint* joint = bindings[i];
			if (joint) {				
				JointPose* jointPose = animator->GetLocalJointPose(joint->index);
				jointPose->orientation = orientations[i];
				jointPose->globalOrientation = true;
			}
		}

		animator->InvalidatePose();
		animator->UpdateMesh(deviceContext);
	}
	
	void KinectMeshAnimator::UpdateOrientations(NUI_SKELETON_DATA& skeleton)
	{
		orientations[NUI_SKELETON_POSITION_HIP_CENTER] = XMVectorSet(0.0, 1.0, 0.0, 0.0);
		orientations[NUI_SKELETON_POSITION_SPINE] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SHOULDER_CENTER);
		orientations[NUI_SKELETON_POSITION_SHOULDER_CENTER] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_SHOULDER_LEFT);
		orientations[NUI_SKELETON_POSITION_HEAD] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
		
		orientations[NUI_SKELETON_POSITION_SHOULDER_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
		orientations[NUI_SKELETON_POSITION_ELBOW_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
		orientations[NUI_SKELETON_POSITION_WRIST_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
		orientations[NUI_SKELETON_POSITION_HAND_LEFT] = GetSkeletonOrientation(skeleton, 0, 1);
		
		orientations[NUI_SKELETON_POSITION_SHOULDER_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
		orientations[NUI_SKELETON_POSITION_ELBOW_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
		orientations[NUI_SKELETON_POSITION_WRIST_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
		orientations[NUI_SKELETON_POSITION_HAND_RIGHT] = GetSkeletonOrientation(skeleton, 0, 1);
		
		orientations[NUI_SKELETON_POSITION_HIP_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
		orientations[NUI_SKELETON_POSITION_KNEE_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
		orientations[NUI_SKELETON_POSITION_ANKLE_LEFT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
		orientations[NUI_SKELETON_POSITION_FOOT_LEFT] = GetSkeletonOrientation(skeleton, 0, 1);
		
		orientations[NUI_SKELETON_POSITION_HIP_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);
		orientations[NUI_SKELETON_POSITION_KNEE_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
		orientations[NUI_SKELETON_POSITION_ANKLE_RIGHT] = GetSkeletonOrientation(skeleton, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
		orientations[NUI_SKELETON_POSITION_FOOT_RIGHT] = GetSkeletonOrientation(skeleton, 0, 1);
	}

	vector KinectMeshAnimator::GetSkeletonOrientation(NUI_SKELETON_DATA& skeleton, int index1, int index2)
	{
		vector orientation;
		Joint* joint1 = bindings[index1];
		Joint* joint2 = bindings[index2];
		if (!joint1 || !joint2) return XMVectorSet(0.0, 1.0, 0.0, 0.0);

		// INVERT Z
		orientation = XMVectorSet(	skeleton.SkeletonPositions[index2].x - skeleton.SkeletonPositions[index1].x,
									skeleton.SkeletonPositions[index2].y - skeleton.SkeletonPositions[index1].y,
									skeleton.SkeletonPositions[index1].z - skeleton.SkeletonPositions[index2].z,
									0.0);

		vector bindOrientation = bindOrientations[index1];
		vector axis = XMVector3Normalize(XMVector3Cross(bindOrientation, orientation));

		if (XMVector3Equal(axis, XMVectorZero()))
			return XMVectorSet(0.0, 1.0, 0.0, 0.0);

		float angle;
		XMStoreFloat(&angle, XMVector3Dot(bindOrientation, orientation));
		angle = acos(angle);
		return XMQuaternionRotationAxis(axis, angle);
	}

	int KinectMeshAnimator::GetActiveSkeleton(NUI_SKELETON_DATA* skeletonData)
	{
		for (int j = 0; j < NUI_SKELETON_COUNT; ++j) {
			NUI_SKELETON_DATA skeleton = skeletonData[j];
			if (skeleton.eTrackingState == NUI_SKELETON_TRACKED)
				return j;
		}

		return -1;
	}

	vector KinectMeshAnimator::GetBindDirection(int index1, int index2)
	{
		Joint* joint1 = bindings[index1];
		Joint* joint2 = bindings[index2];
		if (!joint1 || !joint2) return XMVectorSet(0.0, 1.0, 0.0, 0.0);
		matrix4x4 pose1 = XMMatrixInverse(0, XMLoadFloat4x4(&joint1->inverseBindPose));
		matrix4x4 pose2 = XMMatrixInverse(0, XMLoadFloat4x4(&joint2->inverseBindPose));
		vector dir = pose2.r[3] - pose1.r[3];
		return XMVector3Normalize(dir);
	}

	void KinectMeshAnimator::InitPoses()
	{
		animator->ClearPose();
		animator->UpdateGlobalPose();
		
		bindOrientations[NUI_SKELETON_POSITION_HIP_CENTER] = XMVectorSet(0.0, 1.0, 0.0, 0.0);
		bindOrientations[NUI_SKELETON_POSITION_SPINE] = GetBindDirection(NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SHOULDER_CENTER);
		bindOrientations[NUI_SKELETON_POSITION_SHOULDER_CENTER] = GetBindDirection(NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_SHOULDER_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_HEAD] = GetBindDirection(NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
		bindOrientations[NUI_SKELETON_POSITION_SHOULDER_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_ELBOW_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_WRIST_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_HAND_LEFT] = GetBindDirection(0, 1);
		bindOrientations[NUI_SKELETON_POSITION_SHOULDER_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
		bindOrientations[NUI_SKELETON_POSITION_ELBOW_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
		bindOrientations[NUI_SKELETON_POSITION_WRIST_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
		bindOrientations[NUI_SKELETON_POSITION_HAND_RIGHT] = GetBindDirection(0, 1);
		bindOrientations[NUI_SKELETON_POSITION_HIP_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_KNEE_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_ANKLE_LEFT] = GetBindDirection(NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
		bindOrientations[NUI_SKELETON_POSITION_FOOT_LEFT] = GetBindDirection(0, 1);
		bindOrientations[NUI_SKELETON_POSITION_HIP_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);		
		bindOrientations[NUI_SKELETON_POSITION_KNEE_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
		bindOrientations[NUI_SKELETON_POSITION_ANKLE_RIGHT] = GetBindDirection(NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
		bindOrientations[NUI_SKELETON_POSITION_FOOT_RIGHT] = GetBindDirection(0, 1);
	}

	void KinectMeshAnimator::LinkJoints(int kinectJoint, std::string skeletonJointName)
	{
		Skeleton* skeleton = animator->GetSkeleton();
		Joint* joint = skeleton->GetJointByName(skeletonJointName);
		assert(joint != 0 && "Could not find requested joint");		
		bindings[kinectJoint] = joint;
	}
}