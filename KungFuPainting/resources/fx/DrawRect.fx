cbuffer cbPerObject
{
	float4x4 transformMatrix;
	float2 texDimensions;
};

Texture2D rectTexture;
Texture2D<int> depthTexture;


SamplerState samplerState
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.Pos, 1.0f), transformMatrix);
	vout.TexCoord = vin.TexCoord;
	return vout;
}

float4 PSColor(VertexOut pin) : SV_Target
{
	return rectTexture.Sample(samplerState, pin.TexCoord);
}

float4 PSDepth(VertexOut pin) : SV_Target
{
	int d = depthTexture.Load(int3(pin.TexCoord*texDimensions, 0));

	// use the minimum of near mode and standard
    static const int minDepth = 300 << 3;

    // use the maximum of near mode and standard
    static const int maxDepth = 4000 << 3;

	float norm = float(d - minDepth) / float(maxDepth - minDepth);

	return float4(norm, norm, norm, 1.0);
}

float4 PSNormals(VertexOut pin) : SV_Target
{
	int3 coord = int3(pin.TexCoord*texDimensions, 0.0);
	int3 left = coord - int3(1, 0, 0);
	int3 right = coord + int3(1, 0, 0);
	int3 bottom = coord + int3(0, 1, 0);
	int3 top = coord - int3(0, 1, 0);

	const float range = 30.0;

	float dx = float(depthTexture.Load(left) - depthTexture.Load(right))/range;
	float dy = float(depthTexture.Load(top) - depthTexture.Load(bottom))/range;
	float3 norm = norm = normalize(float3(dx, dy, 2.0));
	
	return float4(norm, 1.0);
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetPixelShader(CompileShader(ps_4_0, PSColor()));
	}
}

technique11 DepthTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetPixelShader(CompileShader(ps_4_0, PSDepth()));
	}
}

technique11 NormalsTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetPixelShader(CompileShader(ps_4_0, PSNormals()));
	}
}