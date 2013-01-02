#ifndef _PSYKO_PAINTING_
#define _PSYKO_PAINTING_

#include <d3d11.h>
#include "render/MeshInstance.h"
#include "render/Texture2D.h"

namespace psyko
{
	class Painting : public MeshInstance
	{
	public:
		Painting(ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~Painting();

		void SetProjectionMatrix(float4x4& projection);
		virtual void SetTransformMatrix(float4x4 matrix);
		float4x4 GetInverseTransformMatrix() { return inverseTransformMatrix; }
		ID3D11RenderTargetView* GetRenderTargetView() { return texture->GetRenderTargetView(); }
		float GetWidth() { return width; }
		float GetHeight() { return height; }
		unsigned int GetTextureWidth() { return texture->GetWidth(); }
		unsigned int GetTextureHeight() { return texture->GetHeight(); }
		virtual void Render(ID3D11DeviceContext* deviceContext);

		float GetZ() const { return z; }

	private:
		Texture2D* texture;
		float width;
		float height;
		float z;

		void InitTexture(ID3D11DeviceContext* context);
		float4x4 inverseTransformMatrix;
	};
}
#endif