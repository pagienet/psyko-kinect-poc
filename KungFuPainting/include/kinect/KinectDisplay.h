#ifndef _PSYKO_KINECTDISPLAY_
#define _PSYKO_KINECTDISPLAY_

#include <d3d11.h>
#include "kinect/KinectController.h"
#include "render/Mesh.h"
#include "render/MeshInstance.h"

namespace psyko 
{
	template<class MaterialType>
	class KinectDisplay
	{
	public:
		KinectDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller, DXGI_FORMAT textureFormat, std::string technique);
		virtual ~KinectDisplay();

		int Init();
		void Draw();
		virtual int Update() = 0;

		inline float GetWidth() { return material.GetWidth(); }
		inline float GetHeight() { return material.GetHeight(); }
		inline void SetWidth(float value) { material.SetWidth(value); }
		inline void SetHeight(float value) { material.SetHeight(value); }

		inline float GetX() const { return material.GetX(); }
		inline float GetY() const { return material.GetY(); }
		inline void SetX(float value) { material.SetX(value); }
		inline void SetY(float value) { material.SetY(value); }

	protected:
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		KinectController* controller;
		Texture2D texture;
		MaterialType material;
		int textureWidth, textureHeight;
		
	private:
		Mesh mesh;
		MeshInstance meshInstance;
		DXGI_FORMAT textureFormat;
		std::string technique;

		int InitMesh();
		int InitMeshInstance();
	};

	template<class MaterialType>
	KinectDisplay<MaterialType>::KinectDisplay(ID3D11Device* device, ID3D11DeviceContext* context, KinectController* controller, DXGI_FORMAT textureFormat, std::string technique)
		: device(device), context(context), controller(controller), 
		texture(device), material(device), mesh(device), meshInstance(device), textureFormat(textureFormat), technique(technique),
		textureWidth(0), textureHeight(0)
	{
	}

	template<class MaterialType>
	KinectDisplay<MaterialType>::~KinectDisplay()
	{
	}

	template<class MaterialType>
	int KinectDisplay<MaterialType>::Init()
	{
		int code = texture.InitBlank(textureWidth, textureHeight, textureFormat, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		if (code) return code;

		material.SetTexture(&texture);
		code = material.Init();
		if (code) return code;

		code = InitMesh();
		if (code) return code;
		
		code = InitMeshInstance();
		if (code) return code;

		return 0;
	}

	template<class MaterialType>
	void KinectDisplay<MaterialType>::Draw()
	{		
		meshInstance.Render(context);
	}

	template<class MaterialType>
	int KinectDisplay<MaterialType>::InitMesh()
	{
		UINT code = 0;
		Vertex vertexData[4] = {	
								{	float3(0.0, 0.0, 0.0), float3(0.0, 0.0, -1.0), float3(0.0, 0.0, 1.0), float3(0.0, 1.0, 0.0), float2(0.0, 1.0), float4(0.0, 0.0, 1.0, 1.0) },
								{	float3(1.0, 0.0, 0.0), float3(0.0, 0.0, -1.0), float3(0.0, 0.0, 1.0), float3(0.0, 1.0, 0.0), float2(1.0, 1.0), float4(0.0, 1.0, 0.0, 1.0) },
								{	float3(1.0, 1.0, 0.0), float3(0.0, 0.0, -1.0), float3(0.0, 0.0, 1.0), float3(0.0, 1.0, 0.0), float2(1.0, 0.0), float4(1.0, 0.0, 0.0, 1.0) },
								{	float3(0.0, 1.0, 0.0), float3(0.0, 0.0, -1.0), float3(0.0, 0.0, 1.0), float3(0.0, 1.0, 0.0), float2(0.0, 0.0), float4(1.0, 0.0, 0.0, 1.0) }
							 };
		unsigned short indexData[6] = { 2, 1, 0, 3, 2, 0 };
		
		code = mesh.InitVertexData(vertexData, 4, sizeof(Vertex), D3D11_USAGE_IMMUTABLE);
		if (code) return code;
		code = mesh.InitIndexData(indexData, 6, DXGI_FORMAT_R16_UINT, D3D11_USAGE_IMMUTABLE);
		if (code) return code;

		return 0;
	}

	template<class MaterialType>
	int KinectDisplay<MaterialType>::InitMeshInstance()
	{
		int code = 0;
		code = meshInstance.SetMesh(&mesh);
		if (code) return code;
		code = meshInstance.SetMaterial(&material);
		if (code) return code;
		return meshInstance.SetActiveTechniqueByName(technique);
	}
}
#endif