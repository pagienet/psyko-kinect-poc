#include "kinect/KinectController.h"

#include <assert.h>
#include "error/errorcodes.h"
#include "core/Vertex.h"

namespace psyko
{
	KinectController::KinectController()
		: sensor(0), 
		colorStreamEvent(INVALID_HANDLE_VALUE), colorImageStream(INVALID_HANDLE_VALUE), 
		depthStreamEvent(INVALID_HANDLE_VALUE), depthImageStream(INVALID_HANDLE_VALUE), 
		skeletonStreamEvent(INVALID_HANDLE_VALUE),
		colorWidth(0), colorHeight(0),
		depthWidth(0), depthHeight(0)
	{
		
	}


	KinectController::~KinectController()
	{
		if (sensor) {
			sensor->NuiShutdown();
			sensor->Release();
		}
		
		if (colorStreamEvent != INVALID_HANDLE_VALUE) 
			CloseHandle(colorStreamEvent);
		if (depthStreamEvent != INVALID_HANDLE_VALUE) 
			CloseHandle(depthStreamEvent);
		if (skeletonStreamEvent != INVALID_HANDLE_VALUE)
			CloseHandle(skeletonStreamEvent);
	}

	int KinectController::InitSensor()
	{
		HRESULT result = 0;
		int sensorCount;
		result = NuiGetSensorCount(&sensorCount);
		if (FAILED(result)) return NUI_SENSOR_QUERY_FAILED;
		int i = 0;
		int status = NUI_SENSOR_NOT_FOUND;

		while (sensor == 0 && i < sensorCount) {
			result = NuiCreateSensorByIndex(i, &sensor);
			
			if (FAILED(result)) continue;
			
			if (sensor->NuiStatus() != S_OK) {
				sensor->Release();
				sensor = 0;
				status = NUI_SENSOR_NOT_AVAILABLE;
			}
			++i;
		}

		return sensor? 0 : status;
	}

	int KinectController::InitStream(DWORD buffers)
	{
		const NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_640x480;

		assert(sensor != 0 && "No sensor device initialized. Make sure to call InitSensor and that it returns succesfully");
		assert(	colorStreamEvent == INVALID_HANDLE_VALUE && 
				depthStreamEvent == INVALID_HANDLE_VALUE &&
				skeletonStreamEvent == INVALID_HANDLE_VALUE &&
				"A stream is already initialized");
		HRESULT result = 0;

		// todo: what aPout high-quality color?
		result = sensor->NuiInitialize(buffers);
		if (FAILED(result)) return NUI_FAILED_TO_INITIALIZE_STREAM;

		if (buffers & (NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_HIGH_QUALITY_COLOR)) {
			int code = InitImageStream(NUI_IMAGE_TYPE_COLOR, &colorStreamEvent, &colorImageStream, resolution);
			if (code) return code;		
			NuiImageResolutionToSize(resolution, colorWidth, colorHeight);
		}

		if (buffers & NUI_INITIALIZE_FLAG_USES_DEPTH) {
			int code = InitImageStream(NUI_IMAGE_TYPE_DEPTH, &depthStreamEvent, &depthImageStream, resolution);
			if (code) return code;			
			NuiImageResolutionToSize(resolution, depthWidth, depthHeight);
		}

		if (buffers & NUI_INITIALIZE_FLAG_USES_SKELETON) {
			int code = InitSkeletonStream();
			if (code) return code;
		}

		return 0;
	}

	int KinectController::InitImageStream(NUI_IMAGE_TYPE imageType, HANDLE* streamEvent, HANDLE* stream, NUI_IMAGE_RESOLUTION resolution)
	{
		*streamEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		HRESULT result = NuiImageStreamOpen(imageType, resolution, 0, 2, *streamEvent, stream);
			
		if (FAILED(result))
			return NUI_FAILED_TO_INITIALIZE_STREAM;

		return 0;
	}

	int KinectController::InitSkeletonStream(bool seated)
	{
		skeletonStreamEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		HRESULT hr = sensor->NuiSkeletonTrackingEnable(skeletonStreamEvent, seated? NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT : 0); 
		return FAILED(hr) ? NUI_FAILED_TO_INITIALIZE_STREAM : 0;
	}

	int KinectController::DrawColorToTexture2D(ID3D11DeviceContext* context, Texture2D* texture)
	{
		return DrawToTexture(context, texture, colorImageStream);
	}

	int KinectController::DrawDepthToTexture2D(ID3D11DeviceContext* context, Texture2D* texture)
	{
		return DrawToTexture(context, texture, depthImageStream);
	}

	int KinectController::DrawToTexture(ID3D11DeviceContext* context, Texture2D* texture, HANDLE stream)
	{
		NUI_IMAGE_FRAME frame = {0};
		HRESULT result = sensor->NuiImageStreamGetNextFrame(stream, 0, &frame);
		if (FAILED(result)) return NUI_FAILED_TO_GET_FRAME;
		INuiFrameTexture* frameTex = frame.pFrameTexture;
		
		NUI_LOCKED_RECT rect;
		result = frameTex->LockRect(0, &rect, NULL, 0);
		if (FAILED(result)) return NUI_FAILED_TO_LOCK_TEXTURE;

		texture->CopyData(context, rect.pBits, rect.size);

		frameTex->UnlockRect(0);
		sensor->NuiImageStreamReleaseFrame(stream, &frame);

		return 0;
	}

	// todo: pass in some sort of Skeleton Joint/Bone data structure?
	int KinectController::GetSkeletonData(SkeletonData* skeletonData)
	{
		NUI_SKELETON_FRAME frame = {0};
		HRESULT result = sensor->NuiSkeletonGetNextFrame(0, &frame);

		if (FAILED(result)) return NUI_FAILED_TO_GET_FRAME;

		sensor->NuiTransformSmooth(&frame, NULL);

		for (int i = 0; i < NUI_SKELETON_COUNT; ++i) {
			skeletonData->skeletonData[i] = frame.SkeletonData[i];
		}

		return 0;
	}
}
