#ifndef _H_NODE_
#define _H_NODE_

#include <vector>

namespace SampleUsage {
	class Mesh;
	class Shader;

	class Node {
	protected:
		float mPosition[3];
		float mRotation[4];
		float mScale[3];
		const Mesh* mMesh;
		const Shader* mShader;
		const Node* mParent;
		std::vector<const Node*> mChildren;
	public:
		void SetParent(const Node* node);
		void SetDisplay(const Mesh& mesh, const Shader& shader);

		void SetLocalPosition(const float* xyz);
		void SetLocalRotation(const float* xyzw);
		void SetLocalScale(const float* xyz);

		void GetLocalPosition(float* xyz);
		void GetLocalRotation(float* xyzw);
		void GetLocalScale(float* xyz);

		void GetGlobalPosition(float* xyz);
		void GetGlobalRotation(float* xyzw);
		void GetGlobalScale(float* xyz);
	};
}

#endif // !_H_NODE_
