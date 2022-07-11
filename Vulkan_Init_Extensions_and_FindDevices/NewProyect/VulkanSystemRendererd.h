#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Utilites.h"
#include <iostream>
#include <vector>

namespace VK_Test {

	
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
		VectExtensions extensionsAviliable;				//Store the exentsions aviliable
		QueueFamilyIndices indicesQueueFamily;			//Store the indicesQueueFamily its valid and count. 
		MainDevice mainDevice;							//Store the PhysicalDevice, and logical device. 
		VkQueue graphicsQueue;							

		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
	

		//////////////////////////////////////////////////////////////////////////////////////////////
		//Vulkan Functions.
		// -Initialize
		bool createInstance();

		/// <summary>
		/// Create logicalDevice to use the interface with physicalDevice. 
		/// </summary>
		void createLogicalDevice();
		
		//Vulkan get information or devices.

		/// <summary>
		/// Get gpu device reference. 
		/// </summary>
		void getPhysicalDevice();
		QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
		//Vulkan functions supports. 
		bool const chekInstanceExtensionsSupports(VectExtensions* checkExtensions) const;
		VectExtensions setUpExtensionNames_VkCreateInfo(VkInstanceCreateInfo& cinfo);
		
		/// <summary>
		/// checking if its posible to switch on device to other gpu device.
		/// </summary>
		/// <param name="device">Device which will be check.</param>
		/// <returns>true -> The device is suitable , false-> the device isn´t suitable </returns>
		bool checkDeviceSuitable(VkPhysicalDevice device) ;

		// --QueueFamilie Indice 
		
		


	};

}//end Namespace