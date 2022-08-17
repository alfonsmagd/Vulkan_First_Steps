#include "MeshPatterns.h"


namespace VK_Test{

	 void 
	MeshPatterns::sierpinski(std::vector<Vertex>* vertices, int depth, glm::vec3 left, glm::vec3 right, glm::vec3 top)
	{
		if (depth <= 0) {

			vertices->push_back({{ left },{1.0f, 0.0f, 0.0f}});
			vertices->push_back({{ right }, { 0.0f, 1.0f, 0.0f}});
			vertices->push_back({{ top }, { 0.0f, 0.0f, 1.0f }});
		}
		else {

			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftright = 0.5f * (left + right);

			sierpinski(vertices, depth - 1, left, leftright, leftTop);
			sierpinski(vertices, depth - 1, leftright, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}


}//end namespace 
