#include "VulkanSystemRendererd.h"


GLFWwindow* window = nullptr;

VK_Test::VulkanSystemRendered_t vulkanRenderedSystem;

bool initializeWindow(std::string nameWindow = "Default", const int Widht = 800, const int Height = 600) {
	//Initialize GLFW 
	if (glfwInit()) {
		//Not to work with OpenGL disabled the context. 
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(Widht, Height, nameWindow.c_str(), nullptr, nullptr);

		return true;
	}
	return false;
	
}

void closeWindow(GLFWwindow* window) {

	glfwDestroyWindow(window);
	glfwTerminate();
	vulkanRenderedSystem.cleanAll();

}
int main() {

	if (initializeWindow()) {

		//Init VulkanRenderdedSystem 
		vulkanRenderedSystem.init(window);

		while (!glfwWindowShouldClose(window)) {

			glfwPollEvents();

		}//end while

		closeWindow(window);
	}//end initializeWindow 


}

