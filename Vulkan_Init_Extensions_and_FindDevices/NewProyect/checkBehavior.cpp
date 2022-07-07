#define GLFW_INCLUDE_VULKAN  //This sentence include vulkan. 
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <iostream>


void isVulkanSoported() {

	if (glfwVulkanSupported()) {

		std::cout << "Yeah vulkan is supported\n";
	}
	uint32_t nm_properties = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &nm_properties, nullptr);
	std::cout << "Number of instancesVulkanProperties = " << nm_properties << std::endl;

}

int main() {

	glfwInit();

	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(800, 600, "WindowVulkan", nullptr, nullptr);
	
	isVulkanSoported();

	glm::vec4 vec(1.0f);
	glm::mat4 matrix(1.0f);

	auto testResult = vec * matrix;



	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

	}
	glfwDestroyWindow(window);
	glfwTerminate();

}