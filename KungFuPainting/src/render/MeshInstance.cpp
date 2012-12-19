#include "render/MeshInstance.h"

#include "core/debug.h"
#include "core/Vertex.h"
#include "error/errorcodes.h"

namespace psyko
{

	MeshInstance::MeshInstance(ID3D11Device* device)
		:	device(device), mesh(mesh), material(material), vertexLayout(0), technique(0),
			transform(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)
	{
	}


	MeshInstance::~MeshInstance()
	{
		if (vertexLayout) vertexLayout->Release();
	}

	int MeshInstance::UpdateVertexLayout()
	{
		if (vertexLayout) {
			vertexLayout->Release();
			vertexLayout = 0;
		}

		if (mesh == 0 || material == 0 || technique == 0) return 0;

		D3D11_INPUT_ELEMENT_DESC descriptors[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float)*3, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float)*6, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float)*9, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float)*12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float)*14, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	
		D3DX11_PASS_DESC passDescriptor;
	
		// todo: query number of passes and store descriptors for all
		HRESULT result = technique->GetPassByIndex(0)->GetDesc(&passDescriptor);
		if (FAILED(result)) {
			DEBUG_TRACE("GetDesc result code: 0x" << std::hex << result);
			return D3D_INPUT_LAYOUT_CREATION_FAILED;
		}

		result = device->CreateInputLayout(descriptors, 6, passDescriptor.pIAInputSignature, passDescriptor.IAInputSignatureSize, &vertexLayout);
		if (FAILED(result)) {
			DEBUG_TRACE("CreateInputLayout error result code: 0x" << std::hex << result);
			return D3D_INPUT_LAYOUT_CREATION_FAILED;
		}
		return 0;
	}



	void MeshInstance::Render(ID3D11DeviceContext* deviceContext) const
	{
		deviceContext->IASetPrimitiveTopology(mesh->GetTopology());
		AssignBuffers(deviceContext);
		material->SetMeshInstanceState(this);
	
		D3DX11_TECHNIQUE_DESC descriptor;
		technique->GetDesc(&descriptor);
	
		for (unsigned int i = 0; i < descriptor.Passes; ++i) {
			technique->GetPassByIndex(i)->Apply(0, deviceContext);
			deviceContext->DrawIndexed(mesh->NumIndices(), 0, 0);
		}
	}

	void MeshInstance::AssignBuffers(ID3D11DeviceContext* deviceContext) const
	{
		unsigned int stride = mesh->VertexStride();
		unsigned int offset = 0;

		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh->GetIndexBuffer(), mesh->IndexFormat(), 0);
		deviceContext->IASetInputLayout(vertexLayout);
	}
}