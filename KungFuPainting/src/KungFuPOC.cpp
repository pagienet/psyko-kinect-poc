#include "KungFuPOC.h"

#include <d3d11.h>
#include "error/errorcodes.h"
#include "core/debug.h"
#include "core/Vertex.h"
#include "poc/MonsterMaterial.h"
#include "poc/JointMesh.h"
#include "poc/SimplePaintLogic.h"
#include "poc/ThrowPaintLogic.h"
#include "io/MD5Parser.h"

using namespace DirectX;

namespace psyko
{
	const float KungFuPOC::PAINTING_Z = 2.3f;
	const float KungFuPOC::CAMERA_Z = 5.0f;
	const float KungFuPOC::MINIMUM_PLAYER_DISTANCE = 0.5f;

	KungFuPOC::KungFuPOC()
		: Win32DirectXApplication(),
		colorDisplay(0), depthDisplay(0),
		jointMaterial(0), stickFigureMesh(0),
		meshInstance(0), painting(0)
	{
		backgroundColor[0] = 0.0;
		backgroundColor[1] = 0.0;
		backgroundColor[2] = 0.0;
		backgroundColor[3] = 1.0;
	}


	KungFuPOC::~KungFuPOC()
	{
		if (colorDisplay) delete colorDisplay;
		if (depthDisplay) delete depthDisplay;
		if (jointMaterial) delete jointMaterial;
		if (stickFigureMesh) delete stickFigureMesh;
		if (meshInstance) delete meshInstance;

		for (auto it = paintLogics.begin(); it != paintLogics.end(); ++it)
			delete *it;		
	}

	int KungFuPOC::SetUp()
	{	
		UpdateProjection();

		int code = 0;

		code = InitMaterial();
		if (code) return code;
		
		code = InitJointMeshes();
		if (code) return code;

		code = InitPainting();
		if (code) return code;

		code = InitPaintLogic();
		if (code) return code;

		code = kinectController.InitSensor();
		if (code) return code;

		code = kinectController.InitStream(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON);
		if (code) return code;

		colorDisplay = new KinectColorDisplay(device, deviceContext, &kinectController);
		colorDisplay->SetWidth(0.6f);
		colorDisplay->SetHeight(0.6f);
		colorDisplay->SetX(-0.9f);
		colorDisplay->SetY(-0.9f);
		code = colorDisplay->Init();
		if (code) return code;

		depthDisplay = new KinectDepthDisplay(device, deviceContext, &kinectController);
		depthDisplay->SetWidth(0.6f);
		depthDisplay->SetHeight(0.6f);
		depthDisplay->SetX(-0.2f);
		depthDisplay->SetY(-0.9f);
		code = depthDisplay->Init();
		if (code) return code;

		return 0;
	}

	void KungFuPOC::UpdateProjection()
	{
		matrix4x4 matrix = DirectX::XMMatrixPerspectiveFovLH(60.0f/180.0f*3.141592f, (float)windowWidth/(float)windowHeight, 0.1f, 1000.0f); 
		matrix4x4 cameraPos = DirectX::XMMatrixTranslation(0.0, 0.0, CAMERA_Z);
		DirectX::XMStoreFloat4x4(&projection, XMMatrixMultiply(cameraPos, matrix));
	}

	int KungFuPOC::InitMaterial()
	{
		jointMaterial = new TestMaterial(device);
		if (!jointMaterial) return MEMORY_ALLOCATION_FAILED;
		return jointMaterial->Init();
	}

	int KungFuPOC::InitPainting()
	{
		painting = new Painting(device, deviceContext);
		painting->SetTransformMatrix(
			float4x4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, PAINTING_Z, 1.0f
			)
		);
		return 0;
	}

	int KungFuPOC::InitPaintLogic()
	{
		//SimplePaintLogic* simple = new SimplePaintLogic(painting, device, deviceContext);
		ThrowPaintLogic* throwLogic = new ThrowPaintLogic(painting, device, deviceContext);
		//simple->AddJoint(NUI_SKELETON_POSITION_HAND_LEFT);
		//simple->AddJoint(NUI_SKELETON_POSITION_HAND_RIGHT);
		throwLogic->AddJoint(NUI_SKELETON_POSITION_HAND_LEFT);
		throwLogic->AddJoint(NUI_SKELETON_POSITION_HAND_RIGHT);

		//paintLogics.push_back(simple);
		paintLogics.push_back(throwLogic);
		return 0;
	}

	int KungFuPOC::ProcessApplication()
	{
		if (WaitForSingleObject(kinectController.DepthStreamEvent(), 0) == WAIT_OBJECT_0)
			UpdateKinectDepthData();

		if (WaitForSingleObject(kinectController.ColorStreamEvent(), 0) == WAIT_OBJECT_0)
			UpdateKinectColorData();

		deviceContext->OMSetRenderTargets(1, &backBufferView, depthStencilView);
		SetViewport(0, 0, (float)windowWidth, (float)windowHeight);

		deviceContext->ClearRenderTargetView(backBufferView, backgroundColor);
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (WaitForSingleObject(kinectController.SkeletonStreamEvent(), 0) == WAIT_OBJECT_0)
			UpdateKinectSkeletonData();		

		deviceContext->OMSetRenderTargets(1, &backBufferView, depthStencilView);
		SetViewport(0, 0, (float)windowWidth, (float)windowHeight);

		jointMaterial->SetProjectionMatrix(projection);
		painting->SetProjectionMatrix(projection);

		colorDisplay->Draw();
		depthDisplay->Draw();
		painting->Render(deviceContext);		
		meshInstance->Render(deviceContext);

		swapChain->Present(0, 0);
	
		return 0;
	}
	
	void KungFuPOC::UpdateKinectColorData()
	{
		colorDisplay->Update();
	}

	void KungFuPOC::UpdateKinectDepthData()
	{
		depthDisplay->Update();
	}

	void KungFuPOC::UpdateKinectSkeletonData()
	{
		float3 offset(0.0, 0.0, 0.0);
		
		SkeletonData data;
		kinectController.GetSkeletonData(&data);
		int skeletonIndex = GetActiveSkeleton(data.skeletonData);
		if (skeletonIndex < 0) return;

		NUI_SKELETON_DATA skeleton = data.skeletonData[skeletonIndex];
		Vector4* positions = skeleton.SkeletonPositions;
		
		float dz = 0.0;
		for (int i = 0; i != NUI_SKELETON_POSITION_COUNT; ++i) {			
			Vector4& pos = positions[i];
			pos.z = -pos.z;
			// don't let player come too close to the painting!
			if (i == 0) {
				if (pos.z > PAINTING_Z - MINIMUM_PLAYER_DISTANCE) {
					dz = pos.z - (PAINTING_Z - MINIMUM_PLAYER_DISTANCE);
				}
			}
			pos.z -= dz;
		}

		stickFigureMesh->Update(deviceContext, positions);

		matrix4x4 projectionMtx = XMLoadFloat4x4(&projection);
		for (auto it = paintLogics.begin(); it != paintLogics.end(); ++it) {
			deviceContext->OMSetRenderTargets(1, &backBufferView, depthStencilView);
			SetViewport(0, 0, (float)windowWidth, (float)windowHeight);
			(*it)->Update(projectionMtx, deviceContext, positions);
		}
	}

	void KungFuPOC::OnResize(unsigned int width, unsigned int height)
	{
		Win32DirectXApplication::OnResize(width, height);
		UpdateProjection();
	}

	int KungFuPOC::InitJointMeshes()
	{
		stickFigureMesh = new StickFigureMesh(device);
		meshInstance = new MeshInstance(device);
		meshInstance->SetMesh(stickFigureMesh);
		meshInstance->SetMaterial(jointMaterial);
		meshInstance->SetActiveTechniqueByName("ColorTech");
		return 0;
	}

	int KungFuPOC::GetActiveSkeleton(NUI_SKELETON_DATA* skeletonData)
	{
		for (int j = 0; j < NUI_SKELETON_COUNT; ++j) {
			NUI_SKELETON_DATA skeleton = skeletonData[j];
			if (skeleton.eTrackingState == NUI_SKELETON_TRACKED)
				return j;
		}

		return -1;
	}
}