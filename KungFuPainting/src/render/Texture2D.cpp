#include "render/Texture2D.h"
#include "WICTextureLoader.h"
#include "error/errorcodes.h"

namespace psyko
{
	Texture2D::Texture2D(ID3D11Device* device)
		: device(device), texture(0), shaderResourceView(0), renderTargetView(0), depthStencilView(0)
	{
	}

	Texture2D::~Texture2D()
	{
		if (texture) texture->Release();
		if (shaderResourceView) shaderResourceView->Release();
		if (renderTargetView) renderTargetView->Release();
		if (depthStencilView) depthStencilView->Release();
	}

	int Texture2D::InitBlank(unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_USAGE usage, UINT cpuAccessFlags)
	{
		if (texture) {
			texture->Release();
			texture = 0;
		}

		HRESULT result;
		this->width = width;
		this->height = height;
		D3D11_TEXTURE2D_DESC descriptor = {0};
		descriptor.Width = width;
		descriptor.Height = height;
		// todo: provide mip levels
		descriptor.MipLevels = 1;
		descriptor.ArraySize = 1;
		descriptor.Format = format;
		descriptor.SampleDesc.Count = 1;
		descriptor.SampleDesc.Quality = 0;
		descriptor.Usage = usage;
		
		if (format == DXGI_FORMAT_D24_UNORM_S8_UINT) {
			descriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		}
		else {
			descriptor.BindFlags = usage == D3D11_USAGE_DEFAULT? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
		}
		descriptor.CPUAccessFlags = cpuAccessFlags;
		descriptor.MiscFlags = 0;

		result = device->CreateTexture2D(&descriptor, NULL, &texture);
		if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;
					  
		if (format == DXGI_FORMAT_D24_UNORM_S8_UINT) {
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = format;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			result = device->CreateDepthStencilView(texture, &desc, &depthStencilView);
			if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;
		}
		else if (descriptor.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;
			result = device->CreateShaderResourceView(texture, &desc, &shaderResourceView);
			if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;
		}

		if (descriptor.BindFlags & D3D11_BIND_RENDER_TARGET) {
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			result = device->CreateRenderTargetView(texture, &desc, &renderTargetView);
			if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;
		}

		return 0;
	}
	
	int Texture2D::LoadWICFromFile(std::wstring filename, ID3D11DeviceContext* context)
	{
		if (texture) {
			texture->Release();
			texture = 0;
		}

		HRESULT result = DirectX::CreateWICTextureFromFile(device, context, filename.c_str(), (ID3D11Resource**)&texture, &shaderResourceView);
		if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;

		result = device->CreateShaderResourceView(texture, 0, &shaderResourceView);
		if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;

		result = device->CreateRenderTargetView(texture, 0, &renderTargetView);
		if (FAILED(result)) return D3D_TEXTURE_CREATION_FAILED;

		return 0;
	}

	int Texture2D::CopyData(ID3D11DeviceContext* context, void* data, unsigned int size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		HRESULT result = context->Map(texture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubResource);
		if (FAILED(result)) return D3D_FAILED_TO_MAP_RESOURCE;
		memcpy(mappedSubResource.pData, data, size);
		
		context->Unmap(texture, NULL);

		return 0;
	}

	int Texture2D::CopyFrom(ID3D11DeviceContext* context, Texture2D& source) 
	{ 
		context->CopyResource(texture, source.texture); 

		return 0;
	}
}