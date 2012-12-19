#ifndef _PSYKO_MD5PARSER_
#define _PSYKO_MD5PARSER_

#include <string>
#include "core/Vertex.h"
#include "render/Mesh.h"
#include "skeleton/Skeleton.h"
#include "skeleton/SkeletonMeshController.h"

namespace psyko
{
	class MD5Parser
	{
	public:
		MD5Parser();
		~MD5Parser();

		void SetRotation(vector quat);

		int ParseFile(std::string filename, ID3D11Device* device);
		int Parse(std::ifstream* stream, ID3D11Device* device);

		int GetLine() { return line; }
		int GetCharLineIndex() { return charLineIndex; }
		std::string GetErrorMessage() const { return errorMessage; }

		Skeleton* GetSkeleton() { return skeleton; }
		const SkeletonMeshController* GetSkeletonMeshController() const { return skeletonMeshController; }
		SkeletonMeshController* GetSkeletonMeshController() { return skeletonMeshController; }
		
	private:
		struct VertexWeightData;
		struct JointData;

		int line;
		int charLineIndex;
		std::string errorMessage;
		ID3D11Device* device;

		std::ifstream* stream;

		unsigned int numVertices;
		unsigned int numIndices;
		unsigned int numWeights;
		unsigned int numJoints;
		unsigned int weightsPerVertex;
		Vertex* vertices;
		unsigned short* indices;
		JointData* jointData;
		VertexWeightData* vertexWeightData;
		float4x4* bindPoses;
		SkeletonMeshController* skeletonMeshController;		
		Skeleton* skeleton;
		vector rotationQuat;

		void ParseMesh();
		void InitMesh();
		void InitJointBindings();
		void ParseVertices();
		void ParseTris();
		void ParseWeights();
		void ParseJoints();
		float3 ParseFloat3();
		float4 ParseFloat4();
		float4 ParseQuaternion();

		char GetNextChar();
		std::string GetNextToken();
		void SkipWhiteSpace();
		void IgnoreLine();
		std::string ParseStringLiteral();

		void UnexpectedEOF();
		void ClearData();
	};
}
#endif