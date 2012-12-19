cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

Texture2D diffuseTexture;

SamplerState samplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
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
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);
	vout.TexCoord = vin.TexCoord;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return diffuseTexture.Sample(samplerState, pin.TexCoord);
}

technique11 TextureTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}