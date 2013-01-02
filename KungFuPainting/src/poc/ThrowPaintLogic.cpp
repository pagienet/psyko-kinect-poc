#include "poc/ThrowPaintLogic.h"

#include <math.h>
#include "core/debug.h"
#include "poc/PlaneMesh.h"
#include "utils/PrecisionTimer.h"

using namespace DirectX;

namespace psyko
{
	ThrowPaintLogic::ThrowPaintLogic(Painting* painting, ID3D11Device* device, ID3D11DeviceContext* context)
		: JointPaintLogic(painting, device, context), splotchInstance(0), splotchMesh(0), splotchMaterial(0), texture(0), blendState(0),
		NUM_PARTICLES_PER_JOINT(200), paintJointAdhesion(15.0f), time(-1.0), dt(0.0), paintParticleMesh(0)
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		device->CreateBlendState(&blendDesc, &blendState);
		
		texture = new Texture2D(device);
		texture->LoadWICFromFile(L"textures/paintSmudge.png", context);
		
		splotchMesh = new PlaneMesh(device, 1.0, 1.0);
		splotchMaterial = new TestMaterial(device);
		splotchMaterial->Init();
		splotchMaterial->SetTexture(texture);
		splotchInstance = new MeshInstance(device);
		splotchInstance->SetMesh(splotchMesh);
		splotchInstance->SetMaterial(splotchMaterial);
		splotchInstance->SetActiveTechniqueByName("TextureTech");

		paintParticleMesh = new PaintParticleMesh(device, NUM_PARTICLES_PER_JOINT);
		particleMaterial = new TestMaterial(device);
		particleMaterial->Init();
		
		for (unsigned int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
			particles[i] = 0;
			meshes[i] = 0;
			previousJointPositions[i] = float3(0, 0, 0);
			previousPositionsInvalid[i] = true;
		}
	}


	ThrowPaintLogic::~ThrowPaintLogic()
	{
		if (blendState) blendState->Release();
		if (splotchInstance) delete splotchInstance;
		if (splotchMaterial) delete splotchMaterial;
		if (splotchMesh) delete splotchMesh;
		if (texture) delete texture;
		if (paintParticleMesh) delete paintParticleMesh;

		for (unsigned int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
			if (particles[i] != 0) delete particles[i];	
			if (meshes[i] != 0) delete meshes[i];
		}
	}

	void ThrowPaintLogic::UpdateSim(matrix4x4& projection) 
	{
		float4x4 proj;
		XMStoreFloat4x4(&proj, projection);
		particleMaterial->SetProjectionMatrix(proj);

		double newTime = GetGlobalTimer();
		dt = time == -1? 0.0f : float(newTime - time)/1000.0f;	// dt in seconds
		time = newTime;

		//splotchMaterial->SetMeshInstanceState(0);

		for (unsigned int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
			if (particles[i] == 0) continue;
			
			for (unsigned int j = 0; j < NUM_PARTICLES_PER_JOINT; ++j) {				
				if (particles[i][j].thrown)
					UpdateParticle(particles[i][j]);									
			}
			
			paintParticleMesh->Update(deviceContext, particles[i]);
			meshes[i]->Render(deviceContext);
		}
		
	}

	void ThrowPaintLogic::UpdateJoint(matrix4x4& projection, const Vector4* jointPositions, int index)
	{
		deviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
			
		if (!particles[index]) 
			particles[index] = new PaintParticle[NUM_PARTICLES_PER_JOINT];
		if (!meshes[index]) {
			meshes[index] = new MeshInstance(device);
			meshes[index]->SetMaterial(particleMaterial);
			meshes[index]->SetMesh(paintParticleMesh);
			meshes[index]->SetTransformMatrix(float4x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0));
			meshes[index]->SetActiveTechniqueByName("ColorTech");			
		}

		float3 currentPos(jointPositions[index].x, jointPositions[index].y, jointPositions[index].z);
		
		if (!previousPositionsInvalid[index]) {
			float3 velocity;
			velocity.x = (currentPos.x - previousJointPositions[index].x)/dt;
			velocity.y = (currentPos.y - previousJointPositions[index].y)/dt;
			velocity.z = (currentPos.z - previousJointPositions[index].z)/dt;
			float len = velocity.x*velocity.x + velocity.y*velocity.y + velocity.z*velocity.z;
			if (len > paintJointAdhesion) {
				float scale = (len - paintJointAdhesion)/len;
				/*velocity.x *= scale;
				velocity.y *= scale;
				velocity.z *= scale;*/
				ThrowParticles(index, velocity, currentPos);
			}			
		}

		float hw = painting->GetWidth()*.5f;
		float hh = painting->GetHeight()*.5f;

		for (unsigned int i = 0; i < NUM_PARTICLES_PER_JOINT; ++i) {
			PaintParticle* particle = particles[index] + i;
			// follow joint
			if (!particle->thrown) {
				particle->position.x = currentPos.x + (rand()/float(RAND_MAX)-.5f)*.05f;
				particle->position.y = currentPos.y + (rand()/float(RAND_MAX)-.5f)*.05f;
				particle->position.z = currentPos.z + (rand()/float(RAND_MAX)-.5f)*.05f;
			}
			else {
				const float splotchSize = .2f;
				
				if (particle->position.z > painting->GetZ())
				{
					float3 pos = particle->position;
			
					pos.x /= hw;
					pos.y /= hh;
					float4x4 transform(
						splotchSize, 0.0f, 0.0f, 0.0f,
						0.0f, splotchSize, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						pos.x, pos.y, 0.0f, 1.0f
					);
					splotchInstance->SetTransformMatrix(transform);
			
					splotchInstance->Render(deviceContext);
			
					particle->thrown = false;
				}
			}
		}

		previousJointPositions[index] = currentPos;
		previousPositionsInvalid[index] = false;

		deviceContext->OMSetBlendState(0, 0, 0xffffffff);
	}

	void ThrowPaintLogic::ThrowParticles(int index, const float3& velocity, const float3& position)
	{
		PaintParticle* currentParticles = particles[index];
		for (unsigned int i = 0; i < NUM_PARTICLES_PER_JOINT; ++i) {
			if (!currentParticles[i].thrown) {
				currentParticles[i].thrown = true;
				float dx = (rand()/float(RAND_MAX)-.5f)*.05f;
				float dy = (rand()/float(RAND_MAX)-.5f)*.05f;
				float dz = (rand()/float(RAND_MAX)-.5f)*.05f;
				currentParticles[i].position.x = position.x + dx;
				currentParticles[i].position.y = position.y + dy;
				currentParticles[i].position.z = position.z + dz;
				currentParticles[i].velocity.x = velocity.x + dx*10.0;
				currentParticles[i].velocity.y = velocity.y + dy*10.0;
				currentParticles[i].velocity.z = velocity.z + dz*10.0;
			}
		}
	}

	void ThrowPaintLogic::UpdateParticle(PaintParticle& particle)
	{
		particle.position.x += particle.velocity.x*dt;
		particle.position.y += particle.velocity.y*dt;
		particle.position.z += particle.velocity.z*dt;

		particle.velocity.y -= 0.981*dt;

		float hw = painting->GetWidth()*.5f;
		float hh = painting->GetHeight()*.5f;
		
		if ((particle.position.x > hw && particle.velocity.x > 0) || 
			(particle.position.x < -hw && particle.velocity.x < 0) || 
			(particle.position.y > hh && particle.velocity.y > 0) || 
			(particle.position.y < -hh && particle.velocity.y < 0)) {
				particle.thrown = false;
				return;
		}
	}
}