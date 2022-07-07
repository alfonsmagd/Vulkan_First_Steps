#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

namespace VK_Test {

	using VectExtensions = std::vector<const char*>;
	class VulkanSystemRendered_t
	{

	public:
		//Constructor 
		explicit VulkanSystemRendered_t() = default;
		//Functions 
		
		/// <summary>
		/// Init the system VulkansystemRendered_t and createInstance
		/// </summary>
		/// <param name="newWindow">Its the window which will rendered and show all. </param>
		/// <returns>A message in console. about the succes or fail.</returns>
		void init(GLFWwindow* newWindow) noexcept;
		/// <summary>
		/// Destroy vulkan instance. 
		/// </summary>
		/// <returns></returns>
		const void cleanAll();

	private:
		//Variables Ptr
		GLFWwindow* window = nullptr;

		//Vulkan Components 
		VkInstance instance;							//Store the instance component;
		
		
		struct {
			VkPhysicalDevice	physicalDevice;        
			VkDevice			logicalDevice;
		}mainDevice;

		//Vulkan Functions.Initialize
		bool createInstance();
		
		//Vulkan get information or devices.
		
		/// <summary>
		/// Get gpu device reference. 
		/// </summary>
		void getPhysicalDevice();


		//Vulkan functions supports. 
		bool chekInstanceExtensionsSupports(VectExtensions* checkExtensions);
		VectExtensions setUpExtensionNames_VkCreateInfo(VkInstanceCreateInfo* cinfo);

	};

}//end Namespace