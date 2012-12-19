#include "skeleton/Skeleton.h"

namespace psyko
{

	Skeleton::Skeleton(int numJoints)
		: numJoints(numJoints), joints(0)
	{
		joints = new Joint[numJoints];
	}


	Skeleton::~Skeleton()
	{
		if (joints)
			delete[] joints;
	}

	Joint* Skeleton::GetJointByName(std::string name)
	{
		for (int i = 0; i < numJoints; ++i)
			if (joints[i].name == name) return &joints[i];

		return 0;
	}
}