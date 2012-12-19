#include "io/MD5Parser.h"

#include <iostream>
#include <fstream>
#include "error/errorcodes.h"
#include "core/debug.h"
#include "skeleton/Skeleton.h"

using namespace DirectX;

namespace psyko
{
	#define VERSION_TOKEN "MD5Version"
	#define COMMAND_LINE_TOKEN "commandline"
	#define NUM_JOINTS_TOKEN "numJoints"
	#define NUM_MESHES_TOKEN "numMeshes"
	#define COMMENT_TOKEN "//"
	#define JOINTS_TOKEN "joints"
	#define MESH_TOKEN "mesh"

	#define MESH_SHADER_TOKEN "shader"
	#define MESH_NUM_VERTS_TOKEN "numverts"
	#define MESH_VERT_TOKEN "vert"
	#define MESH_NUM_TRIS_TOKEN "numtris"
	#define MESH_TRI_TOKEN "tri"
	#define MESH_NUM_WEIGHTS_TOKEN "numweights"
	#define MESH_WEIGHT_TOKEN "weight"

	struct MD5Parser::VertexWeightData
	{
		unsigned int startWeight;
		unsigned int countWeight;
	};

	struct MD5Parser::JointData
	{
		int joint;
		float bias;
		vector pos;
	};

	MD5Parser::MD5Parser()
		: line(0), charLineIndex(0), device(0), stream(0), 
		numVertices(0), numIndices(0), numWeights(0), numJoints(0), weightsPerVertex(0),
		vertices(0), indices(0), jointData(0), vertexWeightData(0), bindPoses(0),
		skeleton(0), skeletonMeshController(0)
	{
		rotationQuat = XMQuaternionRotationAxis(XMVectorSet(1.0, 0.0, 0.0, 0.0), -3.141592*.5);
	}


	MD5Parser::~MD5Parser()
	{
		ClearData();
	}

	void MD5Parser::SetRotation(vector quat)
	{
		rotationQuat = XMQuaternionRotationAxis(XMVectorSet(1.0, 0.0, 0.0, 0.0), -3.141592*.5);
		rotationQuat = XMQuaternionMultiply(rotationQuat, quat);
	}

	int MD5Parser::ParseFile(std::string filename, ID3D11Device* device)
	{
		std::ifstream file;
		file.open(filename);
		
		if (!file) return IO_COULD_NOT_OPEN_FILE;

		return Parse(&file, device);
	}

	int MD5Parser::Parse(std::ifstream* stream, ID3D11Device* device)
	{
		int errorCode = 0;

		weightsPerVertex = 0;

		errorMessage = "";
		this->device = device;
		this->stream = stream;
		
		line = 1;
		charLineIndex = 1;

		while (!stream->eof()) {
			std::string token = GetNextToken();
			if (token == COMMENT_TOKEN) {
				IgnoreLine();
			}
			else if (token == VERSION_TOKEN) {
				int version;
				*stream >> version;
				if (version != 10) {
					errorCode = IO_COULD_NOT_PARSE_FILE;
					errorMessage = "Wrong version number";
				}
			}
			else if (token == COMMAND_LINE_TOKEN) {
				ParseStringLiteral();	// just ignore
			}
			else if (token == NUM_JOINTS_TOKEN) {
				*stream >> numJoints;
				bindPoses = new float4x4[numJoints];
			}
			else if (token == NUM_MESHES_TOKEN) {
				int numMeshes;
				*stream >> numMeshes;	// just support the first mesh
			}
			else if (token == JOINTS_TOKEN) {
				ParseJoints();
			}
			else if (token == MESH_TOKEN) {
				ParseMesh();
			}
			else if (!stream->eof()) {
				errorCode = IO_COULD_NOT_PARSE_FILE;
				errorMessage = "Unexpected token";
			}
			
			if (errorCode) {
				ClearData();
				return errorCode;
			}
		}

		InitMesh();
		ClearData();

		return 0;
	}

	void MD5Parser::InitMesh()
	{
		skeletonMeshController = new SkeletonMeshController(device, skeleton, weightsPerVertex);

		for (unsigned int i = 0; i < numVertices; ++i) {
			Vertex* vertex = vertices + i;
			VertexWeightData* weightData = vertexWeightData + i;
			vertex->position.x = 0.0;
			vertex->position.y = 0.0;
			vertex->position.z = 0.0;

			for (unsigned int j = 0; j < weightData->countWeight; ++j) {
				JointData* weight = jointData + weightData->startWeight + j;
				
				if (weight->bias > 0.0) {
					matrix4x4 bindPose = XMLoadFloat4x4(bindPoses + weight->joint);
					float3 rotatedPoint;

					XMStoreFloat3(&rotatedPoint, XMVector4Transform(weight->pos, bindPose));

					vertex->position.x += rotatedPoint.x * weight->bias;
					vertex->position.y += rotatedPoint.y * weight->bias;
					vertex->position.z += rotatedPoint.z * weight->bias;

					vertex->normal.x = 0.0;
					vertex->normal.y = 0.0;
					vertex->normal.z = 0.0;

					vertex->tangent.x = 0.0;
					vertex->tangent.y = 0.0;
					vertex->tangent.z = 0.0;
					
					vertex->bitangent.x = 0.0;
					vertex->bitangent.y = 0.0;
					vertex->bitangent.z = 0.0;

					vertex->color.x = 0.0;
					vertex->color.y = 0.0;
					vertex->color.z = 0.0;
					vertex->color.w = 0.0;				
				}
			}
		}

		skeletonMeshController->InitVertexData(vertices, numVertices);
		skeletonMeshController->InitIndexData(indices, numIndices, DXGI_FORMAT_R16_UINT);

		InitJointBindings();
	}

	void MD5Parser::InitJointBindings()
	{
		VertexJointBinding* jointBindings = skeletonMeshController->GetJointBindings();
		unsigned int l = 0;

		for (unsigned int i = 0; i < numVertices; ++i) {
			VertexWeightData* weightData = vertexWeightData + i;

			unsigned int nonZeroWeights = 0;
			for (unsigned int j = 0; j < weightData->countWeight; ++j) {
				JointData* weight = jointData + weightData->startWeight + j;
				jointBindings[l].jointIndex = weight->joint;
				jointBindings[l++].jointWeight = weight->bias;				
			}

			for (unsigned int j = weightData->countWeight; j < weightsPerVertex; ++j) {
				jointBindings[l].jointIndex = 0;
				jointBindings[l++].jointWeight = 0;
			}
		}

		skeletonMeshController->ClearPose();
	}

	void MD5Parser::ParseMesh()
	{
		std::string token = GetNextToken();

		while (token != "}") {
			token = GetNextToken();

			if (token == COMMENT_TOKEN)
				IgnoreLine();
			else if (token == MESH_SHADER_TOKEN)
				ParseStringLiteral();	// todo: implement
			else if (token == MESH_NUM_VERTS_TOKEN)
				ParseVertices();
			else if (token == MESH_NUM_TRIS_TOKEN)
				ParseTris();
			else if (token == MESH_NUM_WEIGHTS_TOKEN)
				ParseWeights();
		}
	}

	void MD5Parser::ParseVertices()
	{
		std::string token;
		
		*stream >> numVertices;
		vertices = new Vertex[numVertices];
		vertexWeightData = new VertexWeightData[numVertices];				
				

		for (unsigned int i = 0; i < numVertices; ++i) {
			Vertex* v = vertices + i;
			VertexWeightData* weightData = vertexWeightData + i;
			v->position = float3(0.0, 0.0, 0.0);
			v->normal = float3(0.0, 0.0, 0.0);
			v->tangent = float3(0.0, 0.0, 0.0);
			v->bitangent = float3(0.0, 0.0, 0.0);
			v->color = float4(0.0, 0.0, 0.0, 1.0);
			
			*stream >> token >> token;
			*stream >> token >> v->uv.x >> v->uv.y >> token;
			*stream >> weightData->startWeight >> weightData->countWeight;

			if (weightData->countWeight > weightsPerVertex)
				weightsPerVertex = weightData->countWeight;
		}
	}

	void MD5Parser::ParseTris()
	{
		std::string token;
		unsigned int numTris;
		*stream >> numTris;
		numIndices = numTris*3;
		indices = new unsigned short[numIndices];

		for (unsigned int i = 0; i < numTris; ++i) {
			*stream >> token >> token;
			*stream >> indices[i*3];
			*stream >> indices[i*3+1];
			*stream >> indices[i*3+2];
		}
	}

	void MD5Parser::ParseWeights()
	{
		std::string token;

		*stream >> numWeights;
		jointData = new JointData[numWeights];

		for (unsigned int i = 0; i < numWeights; ++i) {
			JointData* joint = jointData + i;
			
			*stream >> token >> token;
			*stream >> joint->joint >> joint->bias;

			float4 pos = ParseFloat4();
			pos.w = 1.0;
			joint->pos = XMLoadFloat4(&pos);
		}
	}

	void MD5Parser::ParseJoints()
	{
		int i = 0;
		std::string token;

		*stream >> token;

		skeleton = new Skeleton(numJoints);

		for (unsigned int i = 0; i < numJoints; ++i) {
			Joint* joint = skeleton->joints + i;
			joint->index = i;
			joint->name = ParseStringLiteral();			
			*stream >> joint->parentIndex;			
			float3 pos = ParseFloat3();
			XMStoreFloat3(&pos, XMVector3Rotate(XMLoadFloat3(&pos), rotationQuat));
			float4 quat = ParseQuaternion();
			vector orientation = XMLoadFloat4(&quat);
			matrix4x4 matrix = XMMatrixRotationQuaternion(orientation);
			matrix = XMMatrixMultiply(matrix, XMMatrixTranslation(pos.x, pos.y, pos.z));
			
			XMStoreFloat4x4(bindPoses + i, matrix);
			matrix = XMMatrixInverse(0, matrix);
			XMStoreFloat4x4(&(joint->inverseBindPose), matrix);

			IgnoreLine();
		}

		*stream >> token;
	}

	float3 MD5Parser::ParseFloat3()
	{
		float3 v;
		std::string token;
		*stream >> token >> v.x >> v.y >> v.z >> token;
		v.x = -v.x;
		return v;
	}

	float4 MD5Parser::ParseFloat4()
	{
		float4 v;
		std::string token;
		*stream >> token >> v.x >> v.y >> v.z >> token;
		v.x = -v.x;	
		v.w = 0.0;		
		return v;
	}

	float4 MD5Parser::ParseQuaternion()
	{
		float4 v;
		std::string token;
		*stream >> token >> v.x >> v.y >> v.z >> token;
		v.y = -v.y;
		v.z = -v.z;
		float t = 1.0f - v.x*v.x - v.y*v.y - v.z*v.z;
		v.w = t < 0.0f? 0.0f : -sqrt(t);

		vector fl = XMLoadFloat4(&v);
		XMStoreFloat4(&v, XMQuaternionMultiply(fl, rotationQuat));
		return v;
	}

	char MD5Parser::GetNextChar()
	{
		char ch = stream->get();

		if (ch == '\n') {
			++line;
			charLineIndex = 0;
		}
		else if (ch != '\r') ++charLineIndex;

		return ch;
	}

	std::string MD5Parser::GetNextToken()
	{
		std::string token;
		char ch;

		while (!stream->eof()) {
			ch = GetNextChar();
			if (ch == -1) return token;
			if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
				if (token != COMMENT_TOKEN)
					SkipWhiteSpace();
				if (token != "")
					return token;
			}
			else token += ch;

			if (token == COMMENT_TOKEN) return token;
		}

		return token;
	}

	std::string MD5Parser::ParseStringLiteral()
	{
		std::string target;
		char ch = 0;
		*stream >> ch;	// skip first "
		
		do {
			ch = GetNextChar();
			
			if (ch == '\"')
				return target;
			
			target += ch;
		} while (true);
	}

	void MD5Parser::SkipWhiteSpace()
	{
		char ch = 0;

		do {			
			ch = GetNextChar();
			if (ch == -1) return;
		} while (ch == '\n' || ch == ' ' || ch == '\r' || ch == '\t');

		stream->putback(ch);
	}

	void MD5Parser::IgnoreLine()
	{
		char ch = 0;
		while (!stream->eof() && ch != '\n')
			ch = GetNextChar();
	}

	void MD5Parser::UnexpectedEOF()
	{
		errorMessage = "Unexpected EOF";
	}

	void MD5Parser::ClearData()
	{
		if (vertices != 0) delete[] vertices;
		if (indices != 0) delete[] indices;
		if (jointData != 0) delete[] jointData;
		if (vertexWeightData != 0) delete[] vertexWeightData;
		if (bindPoses != 0) delete[] bindPoses;
		
		vertices = 0;
		indices = 0;
		jointData = 0;
		vertexWeightData = 0;
		bindPoses = 0;
	}
}