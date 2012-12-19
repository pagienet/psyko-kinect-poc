#ifndef _PSYKO_KINECT_
#define _PSYKO_KINECT_

#include <vector>
#include <Windows.h>
#include <NuiApi.h>
#include "render/Texture2D.h"


namespace psyko
{
	enum SkeletonOrientations
	{
		SKELETON_ORIENTATION_HIP_ROOT = 0,
		SKELETON_ORIENTATION_HIP_CENTER_TO_SPINE = 1,
		SKELETON_ORIENTATION_SPINE_TO_SHOULDER_CENTER = 2,
		SKELETON_ORIENTATION_SHOULDER_CENTER_TO_HEAD = 3,
		SKELETON_ORIENTATION_SHOULDER_CENTER_TO_SHOULDER_LEFT = 4,
		SKELETON_ORIENTATION_LEFT_SHOULDER_TO_ELBOW = 5,
		SKELETON_ORIENTATION_LEFT_ELBOW_TO_WRIST = 6,
		SKELETON_ORIENTATION_LEFT_WRIST_TO_HAND = 7,
		SKELETON_ORIENTATION_SHOULDER_CENTER_TO_SHOULDER_RIGHT = 8,
		SKELETON_ORIENTATION_RIGHT_SHOULDER_TO_ELBOW = 9,
		SKELETON_ORIENTATION_RIGHT_ELBOW_TO_WRIST = 10,
		SKELETON_ORIENTATION_RIGHT_WRIST_TO_HAND = 11,
		SKELETON_ORIENTATION_HIP_CENTER_TO_HIP_LEFT = 12,
		SKELETON_ORIENTATION_LEFT_HIP_TO_KNEE = 13,
		SKELETON_ORIENTATION_LEFT_KNEE_TO_ANKLE = 14,
		SKELETON_ORIENTATION_LEFT_ANKLE_TO_FOOT = 15,
		SKELETON_ORIENTATION_HIP_CENTER_TO_HIP_RIGHT = 16,
		SKELETON_ORIENTATION_RIGHT_HIP_TO_KNEE = 17,
		SKELETON_ORIENTATION_RIGHT_KNEE_TO_ANKLE = 18,
		SKELETON_ORIENTATION_RIGHT_ANKLE_TO_FOOT = 19
	};

	struct SkeletonData
	{
		NUI_SKELETON_DATA skeletonData[NUI_SKELETON_COUNT];
	};

	class KinectController
	{
	public:
		KinectController();
		~KinectController();

		int InitSensor();
		int InitStream(DWORD buffers);
		
		inline HANDLE ColorStreamEvent() const { return colorStreamEvent; }
		inline HANDLE DepthStreamEvent() const { return depthStreamEvent; }
		inline HANDLE SkeletonStreamEvent() const { return skeletonStreamEvent; }
		
		inline DWORD GetColorWidth() const { return colorWidth; }
		inline DWORD GetColorHeight() const { return colorHeight; }
		inline DWORD GetDepthWidth() const { return depthWidth; }
		inline DWORD GetDepthHeight() const { return depthHeight; }
		
		int DrawColorToTexture2D(ID3D11DeviceContext* context, Texture2D* texture);
		int DrawDepthToTexture2D(ID3D11DeviceContext* context, Texture2D* texture);

		int GetSkeletonData(SkeletonData* skeletonData);

	private:
		INuiSensor* sensor;
		HANDLE colorStreamEvent;
		HANDLE colorImageStream;
		HANDLE depthStreamEvent;
		HANDLE depthImageStream;
		HANDLE skeletonStreamEvent;
		DWORD colorWidth;
		DWORD colorHeight;
		DWORD depthWidth;
		DWORD depthHeight;

		int DrawToTexture(ID3D11DeviceContext* context, Texture2D* texture, HANDLE stream);
		int InitImageStream(NUI_IMAGE_TYPE imageType, HANDLE* event, HANDLE* stream, NUI_IMAGE_RESOLUTION resolution);
		int InitSkeletonStream(bool seated = false);
	};

}
#endif