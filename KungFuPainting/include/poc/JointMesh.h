#include "render/Mesh.h"

namespace psyko
{
	class JointMesh : public Mesh
	{
	public:
		JointMesh(ID3D11Device* device);
		virtual ~JointMesh();

	private:
		void Init();
	};

}