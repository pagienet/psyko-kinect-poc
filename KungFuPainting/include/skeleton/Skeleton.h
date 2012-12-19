#ifndef _PSYKO_SKELETON_
#define _PSYKO_SKELETON_

#include <string>
#include "core/math.h"

namespace psyko
{
	struct VertexJointBinding
	{
		int jointIndex;
		float jointWeight;
	};

	struct Joint
	{
	public:		
		unsigned int index;
		std::string name;
		int parentIndex;
		float4x4 inverseBindPose;

		Joint() : index(0), parentIndex(-1), inverseBindPose(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0) {}
		~Joint() {}
	};

	struct JointPose
	{
		vector orientation;
		vector translation;
		bool globalOrientation;

		JointPose() : globalOrientation(false) {
			float4 vec(0.0, 0.0, 0.0, 0.0);
			orientation = DirectX::XMLoadFloat4(&vec);
			vec.w = 1.0;
			translation = DirectX::XMLoadFloat4(&vec);
		}
	};

	class Skeleton
	{
	public:
		Joint* joints;

		Skeleton(int numJoints);
		~Skeleton();
		int NumJoints() { return numJoints; }
		Joint* GetJointByName(std::string name);

	private:
		int numJoints;
	};
}

#endif