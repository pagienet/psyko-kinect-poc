#ifndef _PSYKO_TEXTURE2D_
#define _PSYKO_TEXTURE2D_

#include <d3d11.h>
#include <string>

namespace psyko 
{
	class Texture2D
	{
	public:
		Texture2D(ID3D11Device* device);
		virtual ~Texture2D();

		int InitBlank(unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_USAGE usage, UINT cpuAccessFlags);
		int LoadWICFromFile(std::wstring filename, ID3D11DeviceContext* context);

		inline ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView; }
		inline ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView; }
		inline ID3D11RenderTargetView* GetRenderTargetView() { return renderTargetView; }
		int CopyData(ID3D11DeviceContext* context, void* data, unsigned int size);
		int CopyFrom(ID3D11DeviceContext* context, Texture2D& source);
		unsigned int GetWidth() { return width; }
		unsigned int GetHeight() { return height; }

	private:
		unsigned int width;
		unsigned int height;
		ID3D11Device* device;
		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* shaderResourceView;
		ID3D11DepthStencilView* depthStencilView;
		ID3D11RenderTargetView* renderTargetView;	
	};
}
#endif