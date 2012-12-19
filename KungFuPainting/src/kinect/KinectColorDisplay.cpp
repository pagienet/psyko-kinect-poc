#include "kinect/KinectColorDisplay.h"

namespace psyko
{

	KinectColorDisplay::KinectColorDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller)
		: KinectDisplay(device, context, controller, DXGI_FORMAT_B8G8R8A8_UNORM, "ColorTech")
	{
		textureWidth = controller->GetColorWidth();
		textureHeight = controller->GetColorHeight();
	}

	KinectColorDisplay::~KinectColorDisplay()
	{
	}

	int KinectColorDisplay::Update()
	{
		return controller->DrawColorToTexture2D(context, &texture);
	}
}