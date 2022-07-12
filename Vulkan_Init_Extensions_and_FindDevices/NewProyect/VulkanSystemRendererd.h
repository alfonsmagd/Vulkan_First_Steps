#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Utilites.h"
#include <iostream>
#include <Windows.h>


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
		VectExtensions extensionsAvailable;				//Store the exentsions aviliable
		QueueFamilyIndices indicesQueueFamily;			//Store the indicesQueueFamily its valid and count. 
		MainDevice mainDevice;							//Store the PhysicalDevice, and logical device. 
		VkQueue graphicsQueue;							
		VkDebugUtilsMessengerEXT debugMessenger;        //Store the Debug Utils messenger debug. 
		const VectExtensions validationLayers = { "VK_LAYER_KHRONOS_validation" }; 

		 
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
	

		//////////////////////////////////////////////////////////////////////////////////////////////
		/******************STATIC FUNCTIONS**********************************************************/
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 
		


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
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
		//Vulkan functions supports. 
		bool const chekInstanceExtensionsSupports(VectExtensions* checkExtensions) const;
		VectExtensions setUpExtensionNames_VkCreateInfo();
		
		/// <summary>
		/// checking if its posible to switch on device to other gpu device.
		/// </summary>
		/// <param name="device">Device which will be check.</param>
		/// <returns>true -> The device is suitable , false-> the device isn´t suitable </returns>
		bool checkDeviceSuitable(VkPhysicalDevice device) ;

		/// <summary>
		/// Checks if all of the requested layers are available
		/// </summary>
		/// <returns>True --> Validation Layer is available . False--> Validation Layer is not available</returns>
		bool checkValidationLayerSupport();

		void setupDebugMessenger();
		

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
											  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			                                  const VkAllocationCallbacks* pAllocator,
			                                  VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
										   VkDebugUtilsMessengerEXT messenger,
			                               const VkAllocationCallbacks* pAllocator);
			
	};

}//end Namespace