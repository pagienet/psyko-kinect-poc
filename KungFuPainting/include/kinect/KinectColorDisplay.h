#ifndef _PSYKO_KINECTCOLORDISPLAY_
#define _PSYKO_KINECTCOLORDISPLAY_

#include "kinect/KinectDisplay.h"
#include "render/DrawRectMaterial.h"

namespace psyko
{
	class KinectColorDisplay : public KinectDisplay<DrawRectMaterial>
	{
	public:
		KinectColorDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller);
		virtual ~KinectColorDisplay();

		virtual int Update();
	};
}
#endif