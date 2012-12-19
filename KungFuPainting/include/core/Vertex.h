#ifndef _PSYKO_VERTEX_
#define _PSYKO_VERTEX_

#include "core/math.h"

namespace psyko
{	
	struct Vertex
	{
		float3 position;
		float3 normal;
		float3 tangent;
		float3 bitangent;
		float2 uv;
		float4 color;
	};
}
#endif