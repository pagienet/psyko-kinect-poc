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
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);
	vout.Color = vin.Color;
	vout.TexCoord = vin.TexCoord;
	return vout;
}

float4 PS(VertexOut pin, uniform bool useTexture) : SV_Target
{
	if (useTexture)
		return diffuseTexture.Sample(samplerState, pin.TexCoord);
	else 
		return pin.Color;
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetPixelShader(CompileShader(ps_4_0, PS(0)));
	}
}

technique11 TextureTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetPixelShader(CompileShader(ps_4_0, PS(1)));
	}
}