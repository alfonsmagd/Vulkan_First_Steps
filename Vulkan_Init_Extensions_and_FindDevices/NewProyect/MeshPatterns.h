#pragma once
#include <GLFW/glfw3.h>

#include "Utilites.h"

namespace VK_Test {

	class MeshPatterns
	{
	public:
		explicit MeshPatterns() = default;

		static void sierpinski(std::vector<Vertex>* vertices,int depth,
			glm::vec3 left,
			glm::vec3 right,
			glm::vec3 top);
	};

}