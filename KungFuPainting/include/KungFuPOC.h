#ifndef _PSYKO_KUNGFUPOC_
#define _PSYKO_KUNGFUPOC_

#include "core/Win32DirectXApplication.h"
#include "kinect/KinectController.h"
#include "kinect/KinectColorDisplay.h"
#include "kinect/KinectDepthDisplay.h"
#include "kinect/KinectMeshAnimator.h"
#include "poc/JointPaintLogic.h"
#include "poc/Painting.h"
#include "poc/StickFigureMesh.h"
#include "poc/TestMaterial.h"
#include "render/Mesh.h"
#include "render/MeshInstance.h"
#include "render/Texture2D.h"
#include "skeleton/Skeleton.h"
#include "skeleton/SkeletonMeshController.h"

namespace psyko
{	
	class KungFuPOC : public Win32DirectXApplication
	{
	public:
		KungFuPOC();
		virtual ~KungFuPOC();
		virtual int SetUp();

	protected:
		virtual int ProcessApplication();
		virtual void OnResize(unsigned int width, unsigned int height);
		
	private:
		static const float PAINTING_Z;
		static const float CAMERA_Z;
		static const float MINIMUM_PLAYER_DISTANCE;
		float backgroundColor[4];

		KinectController kinectController;
		KinectColorDisplay* colorDisplay;
		KinectDepthDisplay* depthDisplay;

		TestMaterial* jointMaterial;
		float4x4 projection;
		StickFigureMesh* stickFigureMesh;
		MeshInstance* meshInstance;
		Painting* painting;
		std::vector<JointPaintLogic*> paintLogics;

		void UpdateKinectColorData();
		void UpdateKinectDepthData();
		void UpdateKinectSkeletonData();
		int InitJointMeshes();
		int InitMaterial();
		int InitPainting();
		int InitPaintLogic();
		void UpdateProjection();
		int GetActiveSkeleton(NUI_SKELETON_DATA* skeletonData);		
	};

}
#endif