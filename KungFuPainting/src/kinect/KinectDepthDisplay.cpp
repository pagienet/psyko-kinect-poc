#include "kinect/KinectDepthDisplay.h"

namespace psyko
{

	KinectDepthDisplay::KinectDepthDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller)
		: KinectDisplay(device, context, controller, DXGI_FORMAT_R16_SINT, "DepthTech")
	{
		textureWidth = controller->GetDepthWidth();
		textureHeight = controller->GetDepthHeight();
		material.SetTexWidth(textureWidth);
		material.SetTexHeight(textureHeight);
	}

	KinectDepthDisplay::~KinectDepthDisplay()
	{
	}

	

	int KinectDepthDisplay::Update()
	{
		return controller->DrawDepthToTexture2D(context, &texture);
	}
}