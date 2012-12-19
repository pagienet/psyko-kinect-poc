#ifndef _PSYKO_KINECTMESHCONTROLLER_
#define _PSYKO_KINECTMESHCONTROLLER_

#include <d3d11.h>
#include <vector>
#include "kinect/KinectController.h"
#include "skeleton/SkeletonMeshController.h"

namespace psyko
{
	class KinectMeshAnimator
	{
	public:
		KinectMeshAnimator(SkeletonMeshController* animator, KinectController* kinectController);
		~KinectMeshAnimator();

		void Update(ID3D11DeviceContext* deviceContext);
		void LinkJoints(int kinectJoint, std::string kinectJointName);

	private:
		int trackingId;
		bool initialized;
		SkeletonMeshController* animator;
		KinectController* kinectController;
		Joint* bindings[NUI_SKELETON_POSITION_COUNT];
		vector bindOrientations[NUI_SKELETON_POSITION_COUNT];
		vector orientations[NUI_SKELETON_POSITION_COUNT];
		
		void InitPoses();
		void UpdateOrientations(NUI_SKELETON_DATA& skeleton);
		vector GetBindDirection(int index1, int index2);
		vector GetSkeletonOrientation(NUI_SKELETON_DATA& skeleton, int index1, int index2);
		int GetActiveSkeleton(NUI_SKELETON_DATA* skeletonData);
		void GetWorldOrientations(NUI_SKELETON_BONE_ORIENTATION* localOrientations, vector* worldOrientations);
	};
}
#endif