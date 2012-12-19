#ifndef _PSYKO_KINECTDEPTHDISPLAY_
#define _PSYKO_KINECTDEPTHDISPLAY_

#include "kinect/KinectDisplay.h"
#include "render/DrawDepthMaterial.h"

namespace psyko
{
	class KinectDepthDisplay : public KinectDisplay<DrawDepthMaterial>
	{
	public:
		KinectDepthDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller);
		virtual ~KinectDepthDisplay();

		virtual int Update();
	};
}
#endif